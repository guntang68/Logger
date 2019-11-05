/*
 * spiff.h
 *
 *  Created on: Jun 13, 2019
 *      Author: annuar
 */

#ifndef SPIFF_SPIFF_H_
#define SPIFF_SPIFF_H_

#include "Arduino.h"
#include "SPIFFS.h"
#include <Print.h>



class Spiff {

public:
	String readFromSpiffs(String whatToRead);
	bool writeToSpiffs(String whatToSave, String value);
};

#endif /* SPIFF_SPIFF_H_ */
