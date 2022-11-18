/*
* EXAMPLE
*	RP MOTOR_BAG
*	2022.11.6 
* AUTHOR CCB HXZP
*
*ʹ�õ��ǰ�������úõ�������͡�id�š�������ʽ���Լ�initָ�뺯����ʼ��
*ʹ��pidǰҲ��Ҫ����pid��ʼ���������޷�ʹ��
*����ֱ��ʹ�����ú�������pid���� Ҳ����ֱ��ʹ�ÿ��ڶ��庯�������Լ�����ĺ���
*ktϵ�е����ʹ��ktϵ�е��ǰ��Ҫ��ʵ����motor_9025_info_t������motor�ڵ�ָ����ϵ�����ಽ������
*/

#include "motor.h"
#include "user_motor.h"


/*
* ʹ�õ��ǰ�������úõ�������͡�id�š�������ʽ
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
	����һ��������Ϊ�����������ʹ�ýṹ������Ϊ�����ʼ��չ��Ϊһά�Ŀ������Ƚϼ�Լ������Ҫ��ס���ǵ�λ�úͺ���
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
 *	@brief	����ܳ�ʼ��
 */
void motor_all_init(void)
{
	//�����ʼ��
	motor[FRIC_R].init(&motor[FRIC_R]);
	motor[FRIC_L].init(&motor[FRIC_L]);
	motor[DIAL].init(&motor[DIAL]);
	//����ٶ�pid��ʼ��
	motor[FRIC_R].pid_init(&motor[FRIC_R].pid.speed,fric_r_speed_pid_param);
	motor[FRIC_L].pid_init(&motor[FRIC_L].pid.speed,fric_l_speed_pid_param);
	motor[DIAL].pid_init(&motor[DIAL].pid.angle_in,dial_angle_in_pid_param);
	motor[DIAL].pid_init(&motor[DIAL].pid.angle,dial_angle_pid_param);
	
	
}

