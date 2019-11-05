/*
 * ManAirmar.cpp
 *
 *  Created on: Jul 29, 2019
 *      Author: annuar
 */

#include			<ManAirmar.h>
#include 			"ManNMEA.h"

void processAirmarData();


double BP=0, AT=0, WS=0, WD=0;


std::vector<airmarReading_t> readingList;
airmarReading_t reading;


String airmarRaw="";


inline void processAirmarData() {
	int x;
	String header="";

	ManNMEA *manNmea;

	x = airmarRaw.indexOf('*', 0);
	if(x >= 0){
		String cs = airmarRaw.substring(x+1,x+3);
		int y = airmarRaw.lastIndexOf('!',x);
		int z = airmarRaw.lastIndexOf('$',x);

		if((y>=0) || (z>=0)){
			if(z > y) y = z;
			String word = airmarRaw.substring(y, x+3);


			manNmea = new ManNMEA;
			if(manNmea->ValidCheckSum(airmarRaw)){
				header = manNmea->extractCol(airmarRaw, 0).substring(1);

				if(header == "WIMDA"){
					BP = manNmea->extractCol(airmarRaw,3).toDouble() * 1000;
					AT = manNmea->extractCol(airmarRaw, 5).toDouble();
					reading.bp = BP;
					reading.at = AT;
				}
				else if(header == "WIMWV"){
					WS = manNmea->extractCol(airmarRaw,3).toDouble();
					WD = manNmea->extractCol(airmarRaw, 1).toDouble();
					reading.ws = WS;
					reading.wd = WD;
				}
				else if(header == "TIROT"){
//					hantu(String(BP) + "\t" + String(AT) + "\t" + String(WS) + "\t" + String(WD) );
					if(readingList.size() > 60){
						readingList.erase(readingList.begin());
					}
					readingList.push_back(reading);
				}

			}
			manNmea->~ManNMEA();
			delete manNmea;
		}
	}


}

ManAirmar::ManAirmar() {
	// TODO Auto-generated constructor stub

}

ManAirmar::~ManAirmar() {
	// TODO Auto-generated destructor stub
}

void ManAirmar::loop(void* parameter) {
	char cr;

	while(true){


		while(Serial2.available() > 0){
			cr = Serial2.read();
			if(cr >= 32 && cr <= 126 ){			//printable char only
				airmarRaw.concat(cr);
			}
			else if(cr >= 9 && cr <= 13){			//control char only
				airmarRaw.concat(cr);
			}

			if(cr == 13){

				airmarRaw.trim();


				processAirmarData();

				airmarRaw = "";
			}


		}

		delay(10);
	}
}
