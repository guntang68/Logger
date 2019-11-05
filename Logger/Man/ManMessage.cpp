/*
 * ManMessage.cpp
 *
 *  Created on: Jul 19, 2019
 *      Author: annuar
 */

#include <ManMessage.h>

//ais ais;
std::vector<ManData_t> ManData;
ManData_t manData;


extern double LAT, LNG;
extern double BP, AT, WS, WD;
int channel = 1;

char sixChar(char data);

int checksumMsg(String data);




String ManMessage::encode() {

	int x,y;
	long titik;
	int nums, data;

	int base = 0;
	int offset = 0;
	char sixbits;
	char bitfill = 0;
	char last = 0;
	char fill;


	buffer[0] = 0;

	for(int q=0; q < ManData.size(); q++){
		nums = ManData.at(q).size;
		data = ManData.at(q).data;

		titik = pow(2,nums);

		x = strlen(buffer);

		for(y = x; y < x + nums; y++){
			data = data << 1;
			buffer[y] = 0x30;
			if(titik & data){
				buffer[y] |= 1;
			}
		}
		buffer[y] = 0x00;
	}

	if(String(buffer).length()>0){

		while(1){
			sixbits = 0x00;
			for(offset=0; offset < 6; offset++){
				if(buffer[(base*6)+offset] == 0x00) {
					bitfill = 6 - offset;
					last = 1;
					break;    //offset == 4
				}
				sixbits = sixbits << 1;
				if(buffer[(base*6)+offset] == '1'){
					sixbits |= 0x01;
				}
				else{
					sixbits &= 0xFE;
				}

				buffer[(base*6)+offset] = 'x';
			}
			if(last){
				if(bitfill){
					for(fill=0; fill<bitfill; fill++){
						sixbits = sixbits << 1;
					}
				}
			}
			sixbits = sixChar(sixbits);
			buffer[base] = sixbits;
			base ++;
			buffer[base] = 0x00;

			if(last) break;
		}

		if(bitfill == 6) bitfill = 0;
		buffer[base+0] = ',';
		buffer[base+1] = sixChar(bitfill);
		buffer[base+2] = 0x00;
	}

	return String(buffer);


}



String ManMessage::turnel(String load, int total, int page, int id) {
	long dest = 701;

	bool abm=false;

	char hh[12];
	String result;

	if(!abm){
		//BBM
		result = "!AIBBM,"+String(total)+","+String(page)+","+String(id)+",";
		ltoa(channel,hh,10);
		result += String(hh) + ",8,";
		result += load;
		int resultCS = checksumMsg(result.substring(1));
		String calculate = String(resultCS,HEX);
		if(calculate.length()==1){
			calculate = "0" + calculate;
		}
		calculate.toUpperCase();
		calculate.toCharArray(hh, 3);
		result += "*" + String(hh);


	}
	else{
		//ABM
		result = "!AIABM,1,1,3,";
		ltoa(dest,hh,10);
		//	strcat(buffer,hh);
		//	strcat(buffer,",");
		result += String(hh) + ",";

		//	strcat(buffer,hh);
		//	strcat(buffer, ",6,");
		ltoa(channel,hh,10);
		result += String(hh) + ",6,";

		result += load;
		int resultCS = checksumMsg(result.substring(1));
		String calculate = String(resultCS,HEX);
		if(calculate.length()==1){
			calculate = "0" + calculate;
		}
		calculate.toUpperCase();
		calculate.toCharArray(hh, 3);
		result += "*" + String(hh);

	}


	return result;
}

int dummyWG=1;

extern double sonarDistance;

String ManMessage::genMessage08(int DAC, int FI) {
	clearData();
	addData(DAC, 10);	//10
	addData(FI, 6);		//16
	addData(LNG * 1000 * 60, 25);	//Longitude in 1/1,000 min, ±180 degrees as per 2's complement (East = positive, West = negative).181 = not available = default
	addData(LAT * 1000 * 60, 24);	//Latitude in 1/1,000 min, ±90 degrees as per 2's complement (North = positive, South = negative). 91 = not available = default
	addData(1, 1);//1 = high (<10 m; Differential Mode of, e.g., DGNSS receiver) 0 = low (>10 m; Autonomous Mode of, e.g., GNSS receiver or of other electronic position fixing device) default = 0
	addData(day(), 5);//UTC Day 5bit
	addData(hour(), 5);//UTC Hour 5bit
	addData(minute(), 6);//UTC Minute 6bit


	addData(WS, 7);//Average Wind Speed 7bit



	addData(0, 7);//Wind Gust	7
//	addData(dummyWG, 7);//Wind Gust	7
//	dummyWG++;




	addData(WD, 9);//Wind Direction	9
	addData(0, 9);//Wind Gust Direction	9
	addData(AT * 10, 11);//Air Temperature	11
	addData(101, 7);//Relative Humidity	7
	addData(501, 10);//Dew Point	10
	addData((BP - 799), 9);//Air Pressure	9
	addData(0, 2);//Air Pressure Tendency	2
	addData(127, 8);//Horizontal Visibility	8


	double tl = sonarDistance / 10;
	tl += 1000;
//	tl /= 100;
//	tl -= 10;

	addData(tl, 12);//Water level (incl. tide)	12



	addData(3, 2);//Water Level Trend	2
	addData(255, 8);//Surface Current Speed (incl. tide)	8
	addData(360, 9);//Surface Current Direction	9
	addData(255, 8);//Current Speed, #2	8
	addData(360, 9);//Current Direction, #2	9
	addData(31, 5);//Current Measuring level, #2	5
	addData(255, 8);//Current Speed, #3	8
	addData(360, 9);//Current Direction, #3	9
	addData(31, 5);//Current Measuring level, #3	5
	addData(255, 8);//Significant Wave Height	8
	addData(63, 6);//Wave Period	6
	addData(360, 9);//Wave Direction	9
	addData(255, 8);//Swell Height	8
	addData(63, 6);//Swell Period	6
	addData(360, 9);//Swell Direction	9
	addData(13, 4);//Sea State	4
	addData(501, 10);//Water Temperature	10
	addData(7, 3);//Precipitation (type)	3
	addData(511, 9);//Salinity	9
	addData(3, 2);//Ice	2
	addData(0, 10);//Spare	10


	return encode();


}



String ManMessage::genMessage06(int DAC, int FI) {
	clearData();
	addData(DAC, 10);
	addData(FI, 6);
	addData(4500/20, 9);
	addData(1200/20, 9);
	addData(1300/20, 9);
	addData(1, 1);			//main supply
	addData(0, 1);			//half
	addData(1, 1);			//open
	addData(12.4*20, 9);	//battery
	addData(1, 1);			//sonar
	addData(0, 4);			//#hopper
	addData(1, 1);			//hopper 1
	addData(1, 1);			//hopper 2
	return encode();
}


ManMessage::ManMessage() {
	// TODO Auto-generated constructor stub

}

ManMessage::~ManMessage() {
	// TODO Auto-generated destructor stub
}

void ManMessage::addData(int data, int size) {
	manData.data = data;
	manData.size = size;
	ManData.push_back(manData);
}


void ManMessage::clearData() {
	while(true){
		if(ManData.size() > 0){
			auto i = ManData.begin();
			ManData.erase(i);
			ManData.clear();
		}
		else{
			break;
		}
	}
}

inline char sixChar(char data) {
	  if(data < 0x28){
	    data += 0x30;
	  }
	  else{
	    data += 0x38;
	  }
	  return data;
}

inline int checksumMsg(String data) {
	int c = 0;
	char t;

	for(int i= 0; i < data.length(); i++){
		t = data.charAt(i);
		if(t == '!'){
			t = 0;
		}
		if(t == '$'){
			t = 1;
		}
		c ^= t;
	}
    return c;
}
