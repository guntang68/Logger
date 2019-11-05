/*
 * ManOLED.h
 *
 *  Created on: Jul 1, 2019
 *      Author: annuar
 */

#ifndef MAN_MANOLED_H_
#define MAN_MANOLED_H_

#include "Arduino.h"
#include <Wire.h>
#include <SSD1306Spi.h>
#include <TimeLib.h>
#include <OLEDDisplayUi.h>

#include <Print.h>
#include "han.h"

//OLED Display pin
#define pOutDRes 25
#define pOutDDC 26
#define pOutDCS 27



class ManOLED {
public:
	ManOLED();
	virtual ~ManOLED();
	static void loop(void * parameter);
	static void setFrameScroll(bool value);
	static void setSet(int8_t set);
	static void setProgress(double value);

	static void setPlainType1Text(String text);
	static void setPlainType2Text(String text);

};

#endif /* MAN_MANOLED_H_ */
