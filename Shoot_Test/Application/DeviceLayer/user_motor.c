/*
* EXAMPLE
*	RP MOTOR_BAG
*	2022.11.6 
* AUTHOR CCB HXZP
*
*使用电机前必须配置好电机的类型、id号、驱动方式，以及init指针函数初始化
*使用pid前也需要进行pid初始化，否则无法使用
*可以直接使用内置函数进行pid控制 也可以直接使用库内定义函数或者自己定义的函数
*kt系列电机：使用kt系列电机前需要先实例化motor_9025_info_t，再与motor内的指针联系，其余步骤无异
*/

#include "motor.h"
#include "user_motor.h"


/*
* 使用电机前必须配置好电机的类型、id号、驱动方式
*/
motor_t motor[MOTOR_LIST] =
{
	[FRIC_R] = {
	
		.id.drive_type = M_CAN1,
		.id.motor_type = RM3508,
		.id.rx_id = 0x201,
		
		.init = motor_class_init,
	},
	[FRIC_L] = {
	
		.id.drive_type = M_CAN1,
		.id.motor_type = RM3508,
		.id.rx_id = 0x202,
		
		.init = motor_class_init,
	},
	
	[DIAL] = {
	
		.id.drive_type = M_CAN1,
		.id.motor_type = RM2006,
		.id.rx_id = 0x203,
		
		.init = motor_class_init,
	},
};



/*
	定义一个数组作为传入参数，不使用结构体是因为数组初始化展开为一维的看起来比较简约，但是要记住他们的位置和含义
	float	  kp;
	float 	ki;
	float 	kd;
	
	float   blind_err;	
	float 	integral_max;	
	float   iout_max;
	float 	out_max;
*/
float fric_r_speed_pid_param[7] = {15,0.5,0,0,6000,6000,12000};
float fric_l_speed_pid_param[7] = {15,0.5,0,0,6000,6000,12000};
float dial_angle_in_pid_param[7] = {17,1.2,0,0,6000,6000,12000};
float dial_angle_pid_param[7] = {200,0,0,0,0,0,10000};



/**
 *	@brief	电机总初始化
 */
void motor_all_init(void)
{
	//电机初始化
	motor[FRIC_R].init(&motor[FRIC_R]);
	motor[FRIC_L].init(&motor[FRIC_L]);
	motor[DIAL].init(&motor[DIAL]);
	//电机速度pid初始化
	motor[FRIC_R].pid_init(&motor[FRIC_R].pid.speed,fric_r_speed_pid_param);
	motor[FRIC_L].pid_init(&motor[FRIC_L].pid.speed,fric_l_speed_pid_param);
	motor[DIAL].pid_init(&motor[DIAL].pid.angle_in,dial_angle_in_pid_param);
	motor[DIAL].pid_init(&motor[DIAL].pid.angle,dial_angle_pid_param);
	
	
}

