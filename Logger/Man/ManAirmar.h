/*
 * ManAirmar.h
 *
 *  Created on: Jul 29, 2019
 *      Author: annuar
 */

#ifndef MAN_MANAIRMAR_H_
#define MAN_MANAIRMAR_H_

#include 			"Arduino.h"
#include 			"han.h"



typedef struct{
	double bp;
	double at;
	double ws;
	double wd;
} airmarReading_t;

class ManAirmar {
public:
	ManAirmar();
	virtual ~ManAirmar();
	static void loop(void* parameter);
};

#endif /* MAN_MANAIRMAR_H_ */
