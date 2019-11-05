/*
 * ManMando.h
 *
 *  Created on: Jul 29, 2019
 *      Author: annuar
 */

#ifndef MAN_MANMANDO_H_
#define MAN_MANMANDO_H_

#include 			"Arduino.h"
#include 			"han.h"

class ManMando {
public:
	ManMando();
	virtual ~ManMando();
	static void loop(void* parameter);
};

#endif /* MAN_MANMANDO_H_ */
