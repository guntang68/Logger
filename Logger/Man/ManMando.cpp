/*
 * ManMando.cpp
 *
 *  Created on: Jul 29, 2019
 *      Author: annuar
 */

#include 			<ManMando.h>
#include 			"ManNMEA.h"
#include			<TimeLib.h>
#include 			"Nyamuk.h"


extern Nyamuk *nyamuk;
void processMandoData();
float getDistance(String data);
boolean isNumeric(String str);

String mandoRaw="";
double LAT=0, LNG=0;

bool MandoWake=false;
bool enReadSonar=false;
bool freshSleep=false;

unsigned long sonarReadTime;
String sonarRaw="";

double sonarDistance;

inline void processMandoData() {
	int x;
	String header="";

	ManNMEA *manNmea;

	x = mandoRaw.indexOf('*', 0);
	if(x >= 0){
		String cs = mandoRaw.substring(x+1,x+3);
		int y = mandoRaw.lastIndexOf('!',x);
		int z = mandoRaw.lastIndexOf('$',x);

		if((y>=0) || (z>=0)){
			if(z > y) y = z;
			String word = mandoRaw.substring(y, x+3);

			manNmea = new ManNMEA;
			if(manNmea->ValidCheckSum(mandoRaw)){
				header = manNmea->extractCol(mandoRaw, 0).substring(1);
				if((header == "GPRMC") || (header == "GNRMC")){
					if(!MandoWake){
						//$GNRMC,072014.00,A,0305.37058,N,10132.77695,E,0.003,,260419,,,D*64
						if(manNmea->extractCol(mandoRaw, 2) == "A"){
							LAT = manNmea->toDegree(manNmea->extractCol(mandoRaw, 3));
							if(manNmea->extractCol(mandoRaw, 4) == "S"){
								LAT *= -1;
							}
							LNG = manNmea->toDegree(manNmea->extractCol(mandoRaw, 5));
							if(manNmea->extractCol(mandoRaw, 6) == "W"){
								LNG *= -1;
							}
							String sTime = manNmea->extractCol(mandoRaw, 1);
							String sDate = manNmea->extractCol(mandoRaw, 9);

							setTime(sTime.substring(0,2).toInt(), sTime.substring(2,4).toInt(), sTime.substring(4,6).toInt(), sDate.substring(0,2).toInt(), sDate.substring(2,4).toInt(), sDate.substring(4,6).toInt());
							adjustTime(3600*8);
							MandoWake = true;
						}
					}
				}
				else if(header == "PAMC"){
					String text = manNmea->extractCol(mandoRaw, 3);
					text.lastIndexOf("saving");
					if(text.lastIndexOf("saving") == 20){
						MandoWake = false;

						if(millis() > 60000){

							freshSleep = true;
						}







//						hantu(String(text.lastIndexOf("saving")) + " = " + text);
					}


				}
				else if(header == "AIABK"){
					String abk = manNmea->extractCol(mandoRaw,5).substring(0, 1);
//					nyamuk->hantar("AIABK", abk);
					MandoABK(abk.toInt());

				}

//				hantu(manNmea->extractCol(mandoRaw, 0).substring(1) + "\t" + mandoRaw);

			}
			manNmea->~ManNMEA();
			delete manNmea;
		}
	}


}

ManMando::ManMando() {
	// TODO Auto-generated constructor stub


}




ManMando::~ManMando() {
	// TODO Auto-generated destructor stub
}




boolean isNumeric(String str)
{
    unsigned int stringLength = str.length();
    boolean seenDecimal = false;

    if (stringLength == 0) {
        return false;
    }

    for(unsigned int i = 0; i < stringLength; ++i) {
        if (isDigit(str.charAt(i))) {
            continue;
        }

        if (str.charAt(i) == '.') {
            if (seenDecimal) {
                return false;
            }
            seenDecimal = true;
            continue;
        }
        return false;
    }
    return true;
}

float getDistance(String data)
{
	int Start = 0, Found=0;
	int reading[36], i=0;
	String Temp="";

	int max=0, min=9999;
	long jumlah=0;

	do{
		Found = data.indexOf("R",Start);
		if(Found >= 0){
			Temp = data.substring(Found+1,Found + 5);

			if(Temp.length() == 4 && isNumeric(Temp)){
				reading[i] = Temp.toInt();
				i++;
				reading[i] = 0;
			}
			Start = Found + 3;
		}
	}while(Found >= 0);

	if(i > 4){
		int y;
		for(y = 0; y < i; y ++){

			if(reading[y] == 0) break;
			//get max
			if(reading[y] > max) max = reading[y];

			//get min
			if(reading[y] < min) min = reading[y];

			//get total
			jumlah += reading[y];
		}
		//get average
		jumlah /= y;
//		Serial.println(jumlah);
//		sonarDistance = jumlah;
	}
	return jumlah;
}

void ManMando::loop(void* parameter) {
	char cr;

	while(true){

		if(freshSleep){
			freshSleep = false;
			//=========================================================================================
			pinMode(13, OUTPUT);
			digitalWrite(13, HIGH);			//Sonar POWER on masa nak baca sahaja


			digitalWrite(5, LOW);			//sonar 0x02
			digitalWrite(17, HIGH);
			digitalWrite(16, LOW);

			enReadSonar = true;
			sonarReadTime = millis();

			sonarRaw = "";


			//=========================================================================================


		}


		while(Serial.available() > 0){
			cr = Serial.read();

			if(enReadSonar == false){
				if(cr >= 32 && cr <= 126 ){			//printable char only
					mandoRaw.concat(cr);
				}
				else if(cr >= 9 && cr <= 13){			//control char only
					mandoRaw.concat(cr);
				}

				if(cr == 13){

					mandoRaw.trim();
					processMandoData();

					mandoRaw = "";
				}
			}
			else{

				sonarRaw.concat(cr);

				if((millis()-sonarReadTime)<1000){

					sonarRaw = "";

				}


			}

			if(enReadSonar){
				if((millis()-sonarReadTime)>4500){

					pinMode(13, OUTPUT);
					digitalWrite(13, LOW);			//Sonar POWER on masa nak baca sahaja

					digitalWrite(5, LOW);			//mando 0x04
					digitalWrite(17, LOW);
					digitalWrite(16, HIGH);


					enReadSonar = false;
					//read distance

//					nyamuk->hantar("sonarRaw", "XXXX");

					sonarDistance = getDistance(sonarRaw);
					nyamuk->hantar("Distance", String(sonarDistance));


				}


			}


		}
		delay(10);
	}
}

