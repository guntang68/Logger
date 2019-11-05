/*
 * ManDecode.h
 *
 *  Created on: Jul 31, 2019
 *      Author: annuar
 */

#ifndef MAN_MANDECODE_H_
#define MAN_MANDECODE_H_
#include 			"Arduino.h"
#include 			"han.h"


class ManDecode {
public:
	ManDecode();
	virtual ~ManDecode();
	std::vector<double> explodeData(String load, int arr[], int size);
	String compactData(std::vector<double> datas,int arr[]);
};

#endif /* MAN_MANDECODE_H_ */
