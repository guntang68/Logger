/*
 * spiff.cpp
 *
 *  Created on: Jun 13, 2019
 *      Author: annuar
 */

#include <Spiff.h>

String Spiff::readFromSpiffs(String whatToRead) {
	bool err = false;
	String res = "";
	if (!SPIFFS.begin(true)) {
		Serial.println("An Error has occurred while mounting SPIFFS");
		err = true;
	}

	if (!err) {
		File file;
		String fileName = "/";
		whatToRead.toLowerCase();
		fileName += whatToRead;
		fileName += ".txt";

		file = SPIFFS.open(fileName);

		if(!file){
			Serial.println("Failed to open file for reading");
			err = true;
		}

		if (!err) {
//			Serial.println("File Content::");

			while(file.available()){

				res += file.readString();
			}
//			Serial.print(whatToRead + "::");
//			Serial.println(res);
			file.close();
		}
	}
	SPIFFS.end();

	return res;

}

bool Spiff::writeToSpiffs(String whatToSave, String value) {
	bool err = false;
	if (!SPIFFS.begin(true)) {
		Serial.println("An Error has occurred while mounting SPIFFS");
		err = true;
	}

	if (!err) {
		File file;
		String fileName = "/";
		whatToSave.toLowerCase();
		fileName += whatToSave;
		fileName += ".txt";
		file = SPIFFS.open(fileName, FILE_WRITE);

		if (!file) {
			Serial.println("There was an error opening the file for writing");
			err = true;
		}

		if (!err && file.print(value)) {
			Serial.println("File was written");

//			if(whatToSave == "min"){
//				Ais.min = value.toInt();
//			}
//			else{
//				Ais.max = value.toInt();
//			}
		} else {
			Serial.println("File write failed :(   huuuuuu");
			err = true;
		}

		file.close();
	}
	SPIFFS.end();
	return !err;
}
