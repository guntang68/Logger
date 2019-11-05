/*
 * ManWiFi.cpp
 *
 *  Created on: Jun 28, 2019
 *      Author: annuar
 */

#include "ManWiFi.h"
#include "Print.h"
#include <stdint.h>
#include <stddef.h>

#include <esp_pm.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>

#include "WString.h"
#include "Printable.h"

WiFiMulti *wifiMultiM;
Router_t Router;
Router_t LocalAP;
static Connection_t connection;


std::vector<Router_t> Routerlist;
String ssid_scan;
int32_t rssi_scan;
uint8_t sec_scan;
uint8_t* BSSID_scan;
int32_t chan_scan;
bool APConnected = false;


void hantu(String str)
{
//	int32_t sebelum = Serial.baudRate();


//	Serial.flush();
//	if(sebelum < 900000){
//		Serial.updateBaudRate(921600);
//	}

	Serial.println("> " + str );
//	Serial.flush();
//
//	Serial.updateBaudRate(sebelum);
}



void hantu(String* str) {
//	int32_t sebelum = Serial.baudRate();
//
//	Serial.flush();
//	if(sebelum < 900000){
//		Serial.updateBaudRate(921600);
//	}
//

	Serial.print("< ");
	Serial.println(*str);
	Serial.flush();

//	if(sebelum < 900000){
//		Serial.updateBaudRate(sebelum);
//	}


}

//kenaTukar
void ManWiFi::setConnectionMode(int8_t mode, bool fresh) {
//	hantu("ManWiFi::setConnectionMode");

	if(mode < 30){
		log_e("i must use correct enum");
		ESP.restart();
	}
	else{
		connection.needModeAs = mode;

		if(fresh){
			if(mode == as_sta){
				WiFi.mode(WIFI_MODE_AP);
				WiFi.softAPdisconnect(true);
				WiFi.enableAP(false);
				WiFi.disconnect(true);


			}
		}



		if((mode == as_ap) || (mode == as_apsta)){
			if(!APConnected){
				WiFi.mode(WIFI_MODE_AP);
				WiFi.softAPdisconnect(true);

				APConnected = true;
				if(mode == as_apsta){
					WiFi.mode(WIFI_MODE_APSTA);
				}
				else{
					WiFi.mode(WIFI_MODE_AP);
				}


				int c = getChannel();

				Serial.println("Channel = " + String(c));

				WiFi.mode(WIFI_MODE_AP);
				WiFi.softAP(LocalAP.ssid, LocalAP.passphrase, c);
				MDNS.begin("nine");

				connection.ipAP = WiFi.softAPIP().toString();
				ManWiFi_events(manWiFi_APconnected);

			}
		}


		if((mode == as_sta) || (mode == as_apsta)){
			if(connection.connected == true){
				Serial.println("ting....");
				connection.connected = false;
				connection.routerSSID = "";		//+++++++++++++++++++++++++++++
				connection.ip = "";	//+++++++++++++++++++++++++++++
				ManWiFi_events(manWiFi_disconnected);
			}

			if(mode == as_apsta){
				WiFi.mode(WIFI_MODE_APSTA);
			}
			else{
				WiFi.mode(WIFI_MODE_STA);
			}

//			WiFi.disconnect(true,true);
			wifiMultiM->~WiFiMulti();
			free(wifiMultiM);

			wifiMultiM = new WiFiMulti;

		    for(uint32_t i = 0; i < Routerlist.size(); i++) {
		    	Router_t entry = Routerlist[i];
		    	wifiMultiM->addAP(entry.ssid, entry.passphrase);
		    }

//			if(mode == as_apsta){
//				WiFi.mode(WIFI_MODE_APSTA);
//			}
//			else{
//				WiFi.mode(WIFI_MODE_STA);
//			}
		    wifiMultiM->run(5000);
		    esp_wifi_set_ps (WIFI_PS_NONE);






		}
	}

}


void ManWiFi::loopScan(void* parameter) {


	while(true){

		if((connection.needModeAs == as_sta) || (connection.needModeAs == as_apsta)){
			checkConnection(); //of if APSTA or STA

		}
		delay(2000);
	}
}


void ManWiFi::checkConnection() {
	int8_t scanResult;
	String currentSSID;
	bool kenaTukar=false;
	bool ssidStillValid=false;
	int16_t currentRSSI=-99;
	int8_t knownAP=0;


	if(!WiFi.isConnected()){
		setConnectionMode(connection.needModeAs,false);
	}
	else{
		scanResult = WiFi.scanNetworks(false);												//----Tempat tanya
		currentSSID = WiFi.SSID();
		for(int8_t i = 0; i < scanResult; ++i) {

			WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan);	//----Tempat tanya

			if(ssid_scan == currentSSID){
				currentRSSI = rssi_scan;
				ssidStillValid = true;
			}
			for(uint32_t x = 0; x < wifiMultiM->APlist.size(); x++){
				WifiAPlist_t entry = wifiMultiM->APlist[x];
				if(ssid_scan == entry.ssid){
					knownAP ++;
					if(currentRSSI < rssi_scan){
						kenaTukar = true;
					}
				}
			}
		}

		if(knownAP>0){			//"Kena tukarrrrrr"
			if(kenaTukar){
//				hantu("Kena tukar");
			}
		}
		if(!ssidStillValid){	//"Router Hilang la pulak"
			kenaTukar = true;
//			hantu("Router hilang");
			connection.routerGone = true;
		}

		if(kenaTukar){
			WiFi.disconnect(true,true);
			setConnectionMode(connection.needModeAs,false);
		}
		else{
			if(connection.connected == false){
				connection.connected = true;
				connection.routerGone = false;
				connection.routerSSID = currentSSID;		//+++++++++++++++++++++++++++++
				connection.ip = WiFi.localIP().toString();	//+++++++++++++++++++++++++++++
				ManWiFi_events(manWiFi_connected);
			}
		}

	}
}



void ManWiFi::addRouterSTA(const char* ssid, const char* passphrase) {
	Router.ssid = strdup(ssid);
	Router.passphrase = strdup(passphrase);
	Routerlist.push_back(Router);
}

void ManWiFi::addRouterAP(const char* ssid, const char* passphrase) {
	LocalAP.ssid =strdup(ssid);
	LocalAP.passphrase = strdup(passphrase);
}
void ManWiFi::clearRouterListP() {
    for(uint32_t i = 0; i < Routerlist.size(); i++) {
    	Router_t entry = Routerlist[i];
    	if(entry.ssid){
    		free(entry.ssid);
    	}
    	if(entry.passphrase){
    		free(entry.passphrase);
    	}

    }
    Routerlist.clear();
}


//==================================================
ManWiFi::ManWiFi() {
	wifiMultiM = new WiFiMulti;
	clearRouterListP();
	APConnected = false;

	WiFi.mode(WIFI_MODE_AP);
	WiFi.softAPdisconnect(true);
	delay(2000);
	WiFi.mode(WIFI_MODE_NULL);

	WiFi.disconnect(0, 0);
	while(WiFi.status() == WL_CONNECTED);


}

ManWiFi::~ManWiFi() {
}

Connection_t ManWiFi::getInfo() {
	return connection;
}

int ManWiFi::getChannel() {
	bool found = false;
	int c = 0;

	int n = WiFi.scanNetworks();
	for (int i = 0; i < n; i++) {
		if(WiFi.channel() > 12){
			break;
		}

		if( c != WiFi.channel()){
			c++;
			if( c != WiFi.channel()){
				found = true;
				break;
			}
		}
	}

	if(!found){
		c = 1;
	}
	else{
		if(c == 0) c = 1;

	}
	return c;
}

//void ManWiFi::loopScan(void* parameter) {
//	//init
//
//	if(connection.mode == WIFI_MODE_APSTA){
//		WiFi.mode(WIFI_MODE_APSTA);
//    	int c = getChannel();
//    	WiFi.softAP(LocalAP.ssid, LocalAP.passphrase, c);    //Password length minimum 8 char
//
//    	MDNS.begin("nine");
//	}
//	else if(connection.mode == WIFI_MODE_AP){
//		WiFi.mode(WIFI_MODE_STA);
//		WiFi.softAPdisconnect(true);
//		WiFi.disconnect(0, 0);
//
//
//
//
////		while(WiFi.softAPdisconnect(true) != ESP_OK){
////			hantu("AAAA");
////			delay(1000);
////			if(WiFi.getMode() == 0){
////				hantu("A-" + String(WiFi.getMode()));
////				break;
////			}
////		}
////
////		WiFi.disconnect(true, true);
//
//
//
//
////		if(!WiFi.mode(WIFI_MODE_AP)){
////			hantu("BBBB");
////			delay(1000);
////		}
//
//		hantu("X-" + String(WiFi.getMode()));
//
//
//
////    	int c = getChannel();
////    	WiFi.softAP(LocalAP.ssid, LocalAP.passphrase, c);    //Password length minimum 8 char
////
////    	hantu("A-" + String(WiFi.getMode()));
////    	MDNS.begin("nine");
////    	hantu("B-" + String(WiFi.getMode()));
//	}
//	else if(connection.mode == WIFI_MODE_STA){
//		WiFi.mode(WIFI_MODE_STA);
//	}
//
//	while(true){
//		switch(connection.mode){
//		case WIFI_MODE_STA:
//			setConnectionNow();
//			break;
//		case WIFI_MODE_APSTA:
//			setConnectionNow();
//			break;
//		}
//
//		hantu("C-" + String(WiFi.getMode()));
//
//		delay(3000);
//	}
//}





//connectToAPs tukar dan masukkan ke dalam setConnection
//void ManWiFi::connectToAPs() {
////	if(connection.connected == true){
////		connection.connected = false;
////		connection.routerSSID = "";		//+++++++++++++++++++++++++++++
////		connection.ip = "";	//+++++++++++++++++++++++++++++
////		ManWiFi_events(manWiFi_disconnected);
////	}
//
//
////	hantu("Connect to AP");
//
//	WiFi.disconnect();
//	while(WiFi.isConnected()){
//		delay(50);
//	}
//	wifiMultiM->~WiFiMulti();
//	free(wifiMultiM);
//
//	wifiMultiM = new WiFiMulti;
//	WiFi.softAPdisconnect(true);
//	WiFi.enableAP(false);
//	WiFi.disconnect(0, 0);
//	WiFi.mode(WIFI_OFF);
//	while(WiFi.status() == WL_CONNECTED);
//
////	setupWiFi();
//}
