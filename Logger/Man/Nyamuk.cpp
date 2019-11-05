/*
 * Nyamuk.cpp
 *
 *  Created on: Jun 23, 2019
 *      Author: annuar
 */

#include "Nyamuk.h"
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <TimeLib.h>

void reconnect();
void callback(char* topic, byte* message, unsigned int length);

unsigned long tika=0;
//extern double WS, WD, BP, AT;
char beat='1';

//const char* mqtt_server = "103.75.190.251";
const char* mqtt_server = "172.105.117.29";



String message="";
String topic="";
char tempString[120];
char tempTopic[36];

int8_t tumpul=0;


WiFiClient espClient;
PubSubClient client(espClient);

void Nyamuk::hantar(String t, String m) {
	topic = t;
	message = m;

	if(client.connected() && m.length()>0){
		m.toCharArray(tempString, m.length()+1);
		t.toCharArray(tempTopic, t.length()+1);
		client.publish(tempTopic, tempString);

	}

}

Nyamuk::Nyamuk() {
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);
//	BP = 800;


}

Nyamuk::~Nyamuk() {
	// TODO Auto-generated destructor stub
}

void Nyamuk::update() {

	if(tumpul > 10){
		ESP.restart();

	}
	if (!client.connected()) {
		hantu("reconnect" + String(tumpul));
		delay(1000);
		reconnect();
		tumpul++;
	}
	else{
		client.loop();
		tumpul=0;
	}

//	if(client.connected() && (now()>tika)){
//
//
////		dtostrf(AT, 1, 2, tempString);
////		client.publish("ayamhutan/temp", tempString);
////
////		dtostrf(BP, 1, 2, tempString);
////		client.publish("ayamhutan/baro", tempString);
////
////		dtostrf(WD, 1, 2, tempString);
////		client.publish("ayamhutan/wd", tempString);
////
////		dtostrf(WS, 1, 2, tempString);
////		client.publish("ayamhutan/ws", tempString);
//
//
//
//		tempString[1]=0;
//		if(beat == '1'){
//			beat = '0';
//			tempString[0] = '0';
//		}
//		else{
//			beat = '1';
//			tempString[0] = '1';
//		}
//		client.publish("ayamhutan/beat", tempString);
//
//		tika = now() + 30;
//
//	}

//	if(client.connected() && message.length()>0){
//		message.toCharArray(tempString, message.length()+1);
//		topic.toCharArray(tempTopic, topic.length()+1);
//		client.publish("ayamhutan/msg6", tempString);
//
//		message=  "";
//	}
}


void callback(char* topic, byte* message, unsigned int length)
{
	String messageTemp;

	for (int i = 0; i < length; i++) {
		messageTemp += (char)message[i];
	}
	if (String(topic) == "esp32/output") {
		if(messageTemp == "on"){

		}
		else if(messageTemp == "off"){

		}
	}
}

void reconnect()
{
	while (!client.connected() && WiFi.isConnected()) {
		yield();
		if (client.connect("ESP8266Client")) {
			client.subscribe("esp32/output");
		} else {
			delay(5000);
//			hantu("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH");
		}
	}
}


