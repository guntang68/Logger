/*
 * ManSmartGuard.cpp
 *
 *  Created on: Jul 29, 2019
 *      Author: annuar
 */

#include 			<ManSmartGuard.h>
#include 			"ManNMEA.h"
#include			<TimeLib.h>
#include 			"Nyamuk.h"

extern Nyamuk *nyamuk;
extern String machineID;


void processSmartguardData();

String smartguardRaw="";

inline void processSmartguardData() {
	int x;
	String header="";

	ManNMEA *manNmea;

	x = smartguardRaw.indexOf('*', 0);
	if(x >= 0){
		String cs = smartguardRaw.substring(x+1,x+3);
		int y = smartguardRaw.lastIndexOf('!',x);
		int z = smartguardRaw.lastIndexOf('$',x);

//		nyamuk->hantar("/katun", smartguardRaw);

		if((y>=0) || (z>=0)){
			if(z > y) y = z;
			String word = smartguardRaw.substring(y, x+3);

			manNmea = new ManNMEA;
			if(manNmea->ValidCheckSum(smartguardRaw)){
				header = manNmea->extractCol(smartguardRaw, 0).substring(1);



//				if(header == "WIBBM"){
				if(header == "AIABM"){

//					Serial.println(smartguardRaw);
//					Serial.flush();

//					String load = manNmea->extractCol(smartguardRaw, 6);
//					String empty = manNmea->extractCol(smartguardRaw, 7).substring(0,1);
					//ManSmartGuard_events(load + "," + empty);
					ManSmartGuard_events(smartguardRaw);
//					nyamuk->hantar(machineID + "/SmartGuard", load);
				}
				else{

					Serial.println(smartguardRaw);
				}


			}
			manNmea->~ManNMEA();
			delete manNmea;
		}
	}
}

bool serial1Rx=false;


void ManSmartGuard::loop(void* parameter) {
	char cr;

	while(true){


		while(Serial1.available() > 0){

			cr = Serial1.read();
			if(cr >= 32 && cr <= 126 ){			//printable char only
				serial1Rx = true;
				smartguardRaw.concat(cr);
			}
			else if(cr >= 9 && cr <= 13){			//control char only
				serial1Rx = true;
				smartguardRaw.concat(cr);
			}

			if(cr == 13){

				smartguardRaw.trim();
				processSmartguardData();

				smartguardRaw = "";
			}
		}
		delay(10);
	}
}

ManSmartGuard::ManSmartGuard() {
	// TODO Auto-generated constructor stub

}

ManSmartGuard::~ManSmartGuard() {
	// TODO Auto-generated destructor stub
}
