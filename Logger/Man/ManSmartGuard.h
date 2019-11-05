/*
 * ManSmartGuard.h
 *
 *  Created on: Jul 29, 2019
 *      Author: annuar
 */

#ifndef MAN_MANSMARTGUARD_H_
#define MAN_MANSMARTGUARD_H_

#include 			"Arduino.h"
#include 			"han.h"


class ManSmartGuard {
public:
	ManSmartGuard();
	virtual ~ManSmartGuard();
	static void loop(void* parameter);

};

#endif /* MAN_MANSMARTGUARD_H_ */
