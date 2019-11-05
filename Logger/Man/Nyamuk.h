/*
 * Nyamuk.h
 *
 *  Created on: Jun 23, 2019
 *      Author: annuar
 */

#ifndef MQTT_NYAMUK_H_
#define MQTT_NYAMUK_H_

#include "Arduino.h"
#include "han.h"
#include "WiFi.h"




class Nyamuk {
public:
	Nyamuk();
	virtual ~Nyamuk();
	void update();
	void hantar(String t, String m);
};

#endif /* MQTT_NYAMUK_H_ */
