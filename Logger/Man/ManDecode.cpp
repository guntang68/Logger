/*
 * ManDecode.cpp
 *
 *  Created on: Jul 31, 2019
 *      Author: annuar
 */

#include <ManDecode.h>
#include <iostream>
#include <iterator>
#include <cmath>

String Char2Bin(char z, char size);

ManDecode::ManDecode() {
	// TODO Auto-generated constructor stub

}

ManDecode::~ManDecode() {
	// TODO Auto-generated destructor stub
}




std::vector<double> ManDecode::explodeData(String load, int arr[], int size) {
	char r;
	String panjang="";
	int start=0;
	String temp="";

	std::vector<double> jawapan;

	double decimal;

	for(int x=0; x < load.length(); x++){
		r = load.charAt(x) - 48;
		if(r > 40){
			r-=8;
		}
		panjang += Char2Bin(r,6);
	}

//	Serial.println(panjang);
	for(int y = 0; y < size; y++){
//		Serial.print(String(arr[y]));
		temp = panjang.substring(start,start+arr[y]);
		start += arr[y];
		temp.trim();
		decimal = 0;

	    int base = 1;
	    int len = temp.length();
	    for (int i = len - 1; i >= 0; i--) {
	        if (temp[i] == '1')
	        	decimal += base;
	        base = base * 2;
	    }

	    jawapan.push_back(decimal);
//		Serial.println(temp + " " + String(decimal,0));
	}
	return jawapan;
}

inline String Char2Bin(char z, char size) {

	String res="";

	for(int x=0; x < size; x++){
		(z % 2)?res = '1' + res:res = '0' + res;
		z/=2;


	}
	return res;
}

String ManDecode::compactData(std::vector<double> datas, int arr[]) {
	int nums, data;
	long titik;
	String implode="";

	for(int x=0; x < datas.size(); x++){
//		Serial.print(String(x) + "\t");
//		Serial.println(datas.at(x),0);
		nums = arr[x];
		data = datas.at(x);
		titik = pow(2,nums);
		for(int y=0; y<nums; y++){
			data = data << 1;
			if(titik & data){
				implode+="1";
			}
			else{
				implode+="0";
			}
		}
	}

	String sixbits;
	int start=0;
	int val;
	String out="";
	int bitfill = 0;
	if(implode.length()>0){

		while(true){
			sixbits = implode.substring(start,start+6);
			if(sixbits.length()==6){
				start+=6;

				val = std::strtol(sixbits.c_str(),nullptr,2);
				if(val < 0x28){
					val += 0x30;
				}
				else{
					val += 0x38;
				}

				char c = static_cast<char>(val);
				out += c;

//				Serial.print(c );
//				Serial.println(sixbits);

			}
			else{
				bitfill = 6 - sixbits.length();

//				Serial.println(String(bitfill));

				for(int u = 0; u < bitfill; u++){
					sixbits+= "0";
				}

				val = std::strtol(sixbits.c_str(),nullptr,2);
				if(val < 0x28){
					val += 0x30;
				}
				else{
					val += 0x38;
				}

				char c = static_cast<char>(val);
				out += c;

//				Serial.println("c======");
//
//				Serial.print(c );
//				Serial.println(sixbits);
				break;
			}
		}

		out += ",";
		out += String(bitfill);

//		Serial.println(out);

	}

	return out;
}
