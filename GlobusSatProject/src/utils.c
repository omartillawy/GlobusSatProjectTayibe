/*
 * utils.c
 *
 *  Created on: 25 במאי 2024
 *      Author: 2022
 */

#include "utils.h"

int logError(int error ,char* msg){
	if(error != E_NO_SS_ERR)
	    printf("Got %d from , %s",error , msg);
	return error;
}

