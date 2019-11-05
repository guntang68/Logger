/*
 * ManMessage.h
 *
 *  Created on: Jul 19, 2019
 *      Author: annuar
 */

#ifndef MAN_MANMESSAGE_H_
#define MAN_MANMESSAGE_H_

#include "Arduino.h"
//#include "Hardware.h"
#include "TimeLib.h"
//#include "ais.h"

typedef struct{
	int data;
	int size;
} ManData_t;

class ManMessage {
public:
	char buffer[1024];

	ManMessage();
	virtual ~ManMessage();
	String genMessage08(int DAC, int FI);
	String genMessage06(int DAC, int FI);
	void addData(int data, int size);
	void clearData();
	String encode();
	String turnel(String load, int total, int page, int id);
};

#endif /* MAN_MANMESSAGE_H_ */
