#include "launcher.h"

#include "rp_math.h"
#include "rm_protocol.h"

//临时函数
extern void CAN1_Send_With_int16_to_uint8(uint32_t stdId, int16_t *dat);

int16_t    launcher_out[3];

void Launcher_Init(void);
void Launcher_Ctrl(void);
void Launcher_SelfProtect(void);

// 发射机构设备
launcher_dev_t		launcher_dev = {
	.user_motor = motor,
	.rc_sensor = &rc_sensor,
};

// 发射机构信息
launcher_info_t 	launcher_info = {
	.remote_mode = RC,
};

launcher_work_info_t  launcher_work_info = {
	.fric_status = WaitCommond_Fric,
	.dial_status = WaitCommond_Dial,
	.launcher_commond = WaitCommond_L,
	.lock_cnt = 0,
	.shoot_cnt = 0,
};

launcher_conf_t   launcher_conf = {
	.fric_speed = Fric_15,
	.dial_speed = -2500.0f,
	.lock_angle_check = 1.5f,
	.lock_cnt = 50,
	.Back_Angle = 45.0f,
	.Load_Angle = -45.0f,
	.wait_time = 2000,  //发射间隔时间，单位ms
};

launcher_t launcher = {
	.dev = &launcher_dev,
	.info = &launcher_info,
	.work_info = &launcher_work_info,
	.conf = &launcher_conf,
	.init = Launcher_Init,
	.ctrl = Launcher_Ctrl,
	.self_protect = Launcher_SelfProtect,
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @brief  发射机构初始化
  * @param  
  * @retval 
  */
void Launcher_Init(void)
{
	if (rc_sensor.work_state == DEV_ONLINE)
	{
		launcher.info->init_s2 = rc_sensor.info->s2;
		launcher.info->last_s2 = rc_sensor.info->s2;
	}
	
	launcher.info->rc_work_state = rc_sensor.work_state;
}



void Launcher_GetInfo(void);
void Launcher_MotorCtrl(void);
void Launcher_SendOut(void);
/**
  * @brief  发射机构控制
  * @param  
  * @retval 
  */
void Launcher_Ctrl(void)
{
	Launcher_GetInfo();
	
	Launcher_MotorCtrl();
	
	Launcher_SendOut();
}



void Launcher_GetBaseInfo(void);
void Launcher_GetCtrlInfo(void);
void Get_LauncherStatus(void);
/**
  * @brief  读取发射机构相关信息
  * @param  
  * @retval 
  */
void Launcher_GetInfo(void)
{
	Launcher_GetBaseInfo();
	
	Launcher_GetCtrlInfo();
	
	Get_LauncherStatus();
}



void Launcher_GetBaseInfo(void)
{
	launcher.info->measure_left_speed = motor[FRIC_L].rx_info.speed;
	launcher.info->measure_right_speed = motor[FRIC_R].rx_info.speed;
	launcher.info->measure_dial_speed = motor[DIAL].rx_info.speed;
	launcher.info->measure_dial_angle = motor[DIAL].rx_info.angle * M2006_ECD_TO_ANGLE;
	
}


void Launcher_GetRcState(void);

void Launcher_GetCtrlInfo(void)
{
	launcher.info->remote_mode = RC;
	
	Launcher_GetRcState();
}

void Launcher_GetRcState(void)
{
	if ((launcher.info->rc_work_state == DEV_OFFLINE)
		&& rc_sensor.work_state == DEV_ONLINE)
	{
		launcher.info->init_s2 = rc_sensor.info->s2;
		launcher.info->last_s2 = rc_sensor.info->s2;
	}
	
	if (rc_sensor.work_state == DEV_ONLINE)
	{
		if (launcher.info->init_s2 == rc_sensor.info->s2)
		{
			launcher.work_info->launcher_commond = WaitCommond_L;
		}
		else
		{
			launcher.info->init_s2 = 0;
			if (rc_sensor.info->s1 == RC_SW_UP)
			{
				if (rc_sensor.info->s2 == RC_SW_MID)
				{
					launcher.work_info->launcher_commond = Func_Reset;
				}
				else if (rc_sensor.info->s2 == RC_SW_DOWN)
				{
					launcher.work_info->launcher_commond = Fric_Toggle;
				}
				else if (rc_sensor.info->s2 == RC_SW_UP)
				{
					launcher.work_info->launcher_commond = Keep_Shoot;
					if (launcher.info->last_s2 != rc_sensor.info->s2)
					{
						launcher.work_info->dial_status = Reload_Dial;
						launcher.info->target_dial_angle = launcher.conf->Load_Angle + launcher.info->measure_dial_angle;
					}
				}
			}
			else if (rc_sensor.info->s2 == RC_SW_DOWN)
			{
				if (rc_sensor.info->s2 == RC_SW_MID)
				{
					launcher.work_info->launcher_commond = Func_Reset;
				}
				else if (rc_sensor.info->s2 == RC_SW_DOWN)
				{
					launcher.work_info->launcher_commond = Bubble_Shoot;
				}
				else if (rc_sensor.info->s2 == RC_SW_UP)
				{
					launcher.work_info->launcher_commond = Single_Shoot;
					if (launcher.info->last_s2 != rc_sensor.info->s2)
					{
						launcher.work_info->dial_status = Reload_Dial;
						launcher.info->target_dial_angle = launcher.conf->Load_Angle + launcher.info->measure_dial_angle;
					}
				}
			}
		}
	}
	else 
	{
		launcher.work_info->launcher_commond = WaitCommond_L;
		launcher.work_info->fric_status = WaitCommond_Fric;
		launcher.work_info->dial_status = WaitCommond_Dial;
	}
	
}




void Fric_StepCheck(void);
void Dial_StatusCheck(void);

void Get_LauncherStatus(void)
{
	Fric_StepCheck();
	
	Dial_StatusCheck();
	
	launcher.info->rc_work_state = rc_sensor.work_state;
	launcher.info->last_s2 = rc_sensor.info->s2;
}

// 摩擦轮跳变检测
void Fric_StepCheck(void)
{
	if ((launcher.work_info->launcher_commond == Fric_Toggle)
		&& (launcher.info->last_s2 != rc_sensor.info->s2))
	{
		if (launcher.work_info->fric_status == WaitCommond_Fric)
		{
			launcher.work_info->fric_status = On_Fric;
		}
		else if (launcher.work_info->fric_status == On_Fric)
		{
			launcher.work_info->fric_status = Off_Fric;
		}
		else if (launcher.work_info->fric_status == Off_Fric)
		{
			launcher.work_info->fric_status = On_Fric;
		}
	}
}

// 拨盘堵转检测
void Dial_StatusCheck(void)
{
	if (launcher.work_info->dial_status == Reload_Dial)
	{
		if (launcher.info->measure_dial_angle < launcher.info->target_dial_angle + launcher.conf->lock_angle_check)
		{
			if (launcher.work_info->launcher_commond == Keep_Shoot)
			{
				launcher.info->target_dial_angle = launcher.conf->Load_Angle + launcher.info->measure_dial_angle;
			}
			else if (launcher.work_info->launcher_commond == Single_Shoot)
			{
				launcher.work_info->dial_status = WaitCommond_Dial;
			}
			else if (launcher.work_info->launcher_commond == Bubble_Shoot)
			{
				if (++launcher.work_info->shoot_cnt == 60000)
				{
					launcher.work_info->shoot_cnt = 0;
				}
				
				if (launcher.work_info->shoot_cnt % launcher.conf->wait_time == 0)
				{
					launcher.info->target_dial_angle = launcher.conf->Load_Angle + launcher.info->measure_dial_angle;
				}
			}
		}
		else 
		{
			if (abs(launcher.info->measure_dial_speed) < 5)
			{
				launcher.work_info->lock_cnt ++;
				if (launcher.work_info->lock_cnt > launcher.conf->lock_cnt)
				{
					launcher.work_info->dial_status = F_Lock_Dial;
					launcher.info->target_dial_angle = launcher.conf->Back_Angle + launcher.info->measure_dial_angle;
					launcher.work_info->lock_cnt = 0;
				}
			}
			else 
			{
				launcher.work_info->lock_cnt = 0;
			}
		}
	}
	else if (launcher.work_info->dial_status == F_Lock_Dial)
	{
		if (launcher.info->measure_dial_angle > launcher.info->target_dial_angle - launcher.conf->lock_angle_check)
		{
			launcher.info->target_dial_angle = launcher.conf->Load_Angle + launcher.info->measure_dial_angle;
			launcher.work_info->dial_status = Reload_Dial;
		}
		else 
		{
			if (abs(launcher.info->measure_dial_speed) < 5)
			{
				launcher.work_info->lock_cnt ++;
				if (launcher.work_info->lock_cnt > launcher.conf->lock_cnt)
				{
					launcher.work_info->dial_status = Reload_Dial;
					launcher.info->target_dial_angle = -launcher.conf->Back_Angle + launcher.info->measure_dial_angle;
					launcher.work_info->lock_cnt = 0;
				}
			}
			else 
			{
				launcher.work_info->lock_cnt = 0;
			}
		}
	}
	else
	{
		launcher.work_info->dial_status = WaitCommond_Dial;
	}
}



void Fric_Ctrl(void);
void Dial_Ctrl(void);
/**
  * @brief  发射机构电机控制
  * @param  
  * @retval 
  */
void Launcher_MotorCtrl(void)
{
	Fric_Ctrl();
	Dial_Ctrl();
}


void Fric_Ctrl(void)
{
	if (launcher.work_info->fric_status == On_Fric)
	{
		launcher.info->target_left_speed = -launcher.conf->fric_speed;
		launcher.info->target_right_speed = launcher.conf->fric_speed;
	
		launcher_out[motor[FRIC_L].id.buff_p] = motor[FRIC_L].c_speed(&motor[FRIC_L], launcher.info->target_left_speed);
		launcher_out[motor[FRIC_R].id.buff_p] = motor[FRIC_R].c_speed(&motor[FRIC_R], launcher.info->target_left_speed);
		
	}
	else if (launcher.work_info->fric_status == Off_Fric || launcher.work_info->fric_status == WaitCommond_Fric)
	{
		launcher.info->target_left_speed = 0.0f;
		launcher.info->target_right_speed = 0.0f;
		
		launcher_out[motor[FRIC_L].id.buff_p] = motor[FRIC_L].c_speed(&motor[FRIC_L], launcher.info->target_left_speed);
		launcher_out[motor[FRIC_R].id.buff_p] = motor[FRIC_R].c_speed(&motor[FRIC_R], launcher.info->target_left_speed);
		
	}
}

void Dial_Ctrl(void)
{
	if (launcher.work_info->dial_status != WaitCommond_Dial)
	{
		// 过零点处理
		if (launcher.info->target_dial_angle < -360.0f)
		{
			launcher.info->target_dial_angle += 360.0f;
		}
		else if (launcher.info->target_dial_angle > 360.0f)
		{
			launcher.info->target_dial_angle -= 360.0f;
		}
		
		if (launcher.info->target_dial_angle - launcher.info->measure_dial_angle > 180.0f)
		{
			launcher.info->target_dial_angle -= 360.0f;
		}
		else if (launcher.info->target_dial_angle - launcher.info->measure_dial_angle < -180.0f)
		{
			launcher.info->target_dial_angle += 360.0f;
		}
		
		launcher_out[motor[DIAL].id.buff_p] = motor[DIAL].c_angle(&motor[DIAL], launcher.info->target_dial_angle);
		
	}
	else 
	{
		launcher_out[motor[DIAL].id.buff_p] = motor[DIAL].c_angle(&motor[DIAL], launcher.info->measure_dial_angle);
	}
	
}




/**
  * @brief  发送发射机构电机控制数据
  * @param  
  * @retval 
  */
void Launcher_SendOut(void)
{
	CAN1_Send_With_int16_to_uint8(motor[FRIC_L].id.tx_id,launcher_out);
	CAN1_Send_With_int16_to_uint8(motor[FRIC_R].id.tx_id,launcher_out);
	CAN1_Send_With_int16_to_uint8(motor[DIAL].id.tx_id,launcher_out);
	
}



void Launcher_Stop(void);
/**
  * @brief  发射机构控制离线保护
  * @param  
  * @retval 
  */
void Launcher_SelfProtect(void)
{
	Launcher_Stop();
	Launcher_GetInfo();
}

// 遥控器失联保护
void Launcher_Stop(void)
{
	launcher_out[0] = 0;
	launcher_out[1] = 0;
	launcher_out[2] = 0;
	
	CAN1_Send_With_int16_to_uint8(motor[FRIC_L].id.tx_id,launcher_out);
	CAN1_Send_With_int16_to_uint8(motor[FRIC_R].id.tx_id,launcher_out);
	CAN1_Send_With_int16_to_uint8(motor[DIAL].id.tx_id,launcher_out);
}
