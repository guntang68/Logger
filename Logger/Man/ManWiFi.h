/*
 * ManWiFi.h
 *
 *  Created on: Jun 28, 2019
 *      Author: annuar
 */

#ifndef MAN_MANWIFI_H_
#define MAN_MANWIFI_H_

#include <WiFi.h>
#include <WiFiMulti.h>
#include <vector>
#include "esp_task_wdt.h"
#include "han.h"
#include <ESPmDNS.h>

enum asMode{
	as_ap=30,
	as_sta,
	as_apsta,
	as_none
};

enum{
	manWiFi_connected,
	manWiFi_disconnected,
	manWiFi_APconnected
};

typedef struct {
    char * ssid;
    char * passphrase;
} Router_t;

typedef struct{
	int8_t 	needModeAs;
	bool	connected;
	bool	routerGone;
	String	routerSSID;
	String	ip;				//STA
	String	ipAP;			//AP



} Connection_t;




class ManWiFi {
private:

//	static void setupWiFi();
//	static void connectToAPs();
//	static void setConnectionNow();
	static void checkConnection();
	static int getChannel();
public:
	ManWiFi();
	virtual ~ManWiFi();
	static void addRouterSTA(const char* ssid, const char *passphrase);
	static void addRouterAP(const char* ssid, const char *passphrase);
	static void clearRouterListP();
	static void loopScan(void * parameter);
	static void setConnectionMode(int8_t mode, bool fresh);	// set connection mode
	static Connection_t getInfo();

};

#endif /* MAN_MANWIFI_H_ */
