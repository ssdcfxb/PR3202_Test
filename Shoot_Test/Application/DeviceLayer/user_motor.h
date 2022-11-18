#ifndef __USER_MOTOR_H
#define __USER_MOTOR_H


#include "motor.h"

typedef enum
{
	FRIC_R = 0,
	FRIC_L,
	DIAL,
	
	MOTOR_LIST,
}motor_list_e;


void motor_all_init(void);

extern motor_t motor[MOTOR_LIST];

#endif

