#include 			"Arduino.h"
#include 			"ManWiFi.h"
#include			"ManOTA.h"
#include 			"ManMando.h"
#include			"ManOLED.h"
#include 			"ManAirmar.h"
#include 			"Nyamuk.h"
#include 			"ManSmartGuard.h"
#include 			"ManMessage.h"
#include 			"ManDecode.h"
#include 			"ManNMEA.h"

ManWiFi 			*manWiFi;
ManOTA				*manOta;
ManMando			*manMando;
ManOLED				*manOLED;
ManAirmar			*manAirmar;
Nyamuk 				*nyamuk;
ManSmartGuard		*manSmartguard;
ManMessage			*manMessage;
ManDecode			*manDecode;

TaskHandle_t 		TaskManWiFi;
TaskHandle_t 		TaskManOTA;
TaskHandle_t 		TaskManMando;
TaskHandle_t 		TaskManOLED;
TaskHandle_t 		TaskManAirmar;
TaskHandle_t 		TaskManSmartguard;

//BBM

bool mqttEnabled=false;
String machineID;
unsigned long tickNyamukTime = 0;
extern bool MandoWake;
bool fWake=false;
int identifier=0;

void sendOutBuffer();

std::vector<String> MsgBuffer;


String xx="";
int AbkValue=99;

void MandoABK(int ack) {
	AbkValue = ack;
	nyamuk->hantar("A B K", String(AbkValue));
}


int sendOutFlag=0;	//idle
unsigned long masaHantar;
inline void sendOutBuffer() {

	String currentMessage;

	if((MsgBuffer.size()>0) && MandoWake){
		switch (sendOutFlag) {
			case 0:
				sendOutFlag = 1;
				break;
			case 1:
				MsgBuffer.shrink_to_fit();
				currentMessage = MsgBuffer.at(0);

				masaHantar = millis();
				Serial.println(currentMessage);
				Serial.flush();
				nyamuk->hantar("sendOutBuffer", currentMessage);
				sendOutFlag = 2;
				break;
			case 2:
				if((millis()-masaHantar) < 20000){
					if(AbkValue == 3){
						MsgBuffer.erase(MsgBuffer.begin()); //delete first buffer
						sendOutFlag = 1;	//send another if any
						AbkValue = 99;
						nyamuk->hantar("ABK","3");
					}
					else if(AbkValue == 2){
						sendOutFlag = 1;	//send the same
						AbkValue = 99;
						nyamuk->hantar("ABK","2");
					}
				}
				else{
					sendOutFlag = 1;	//send the same
				}


				break;
			default:
				break;
		}
	}
	else{
		sendOutFlag = 0;
	}

}

int WaterLevel=0;

void ManSmartGuard_events(String data) {


	//data => "!WIBBM,1,1,,0,8,05tfNwB1JPjvMKwwe5`P1UOGwswu3wu`wsAwwe7wwvlOwu`muOwt00,2*37"

	nyamuk->hantar("ManSmartGuard_events", data);

	xx = data;

	Serial.println(data);
	Serial.flush();


//	//====================================================
//	ManNMEA *manNmea;
//
//
//	manNmea = new ManNMEA;
//	String load = manNmea->extractCol(data, 6);
//	delete manNmea;
//
//
//	manDecode = new ManDecode;
//
////	setTime(12,3,2,2,12,2019);
//
//	int type8[40] = {10,6,25,24,1,5,5,6,7,7,9,9,11,7,10,9,2,8,12,2,8,9,8,9,5,8,9,5,8,6,9,8,6,9,4,10,3,9,2,10};
//
//	std::vector<double>  jj = manDecode->explodeData(load, type8, 40);
//	//											      05tfNwB1JPl4h?wwe5`P1UOGwswu3wu`wsAwwe7wwvlOwu`muOwt0
//
//	jj.at(5) = day();
//	jj.at(6) = hour();
//	jj.at(7) = minute();
//	jj.at(18) = WaterLevel;	//WaterLevel
//
//	WaterLevel++;
//
////	jj.at(39) = 1;
//
//	String jawapan = manDecode->compactData(jj, type8);
////	Serial.println(jawapan);
//	delete manDecode;
//
//	xx = manMessage->turnel(jawapan,1,1, identifier);
//	Serial.println(xx);
//	Serial.flush();
//	//====================================================





}

int cnt=0;


void loop()
{

//	char cr;

	nyamuk->update();

	cnt++;

	if((cnt % 50)==0){
		digitalWrite(0, HIGH);
	}
	if((cnt % 80)==0){
		digitalWrite(0, LOW);
	}



	delay(20);

	if(millis() - tickNyamukTime > 60000){
		tickNyamukTime = millis();
		nyamuk->hantar(machineID, "tick");
	}

	if(fWake != MandoWake){
		fWake = MandoWake;

		if(MandoWake){
//			sendOutFlag = 1;

			digitalWrite(0, HIGH);


			delay(200);

			if(xx.length()>20){

				Serial.println(xx);
				Serial.flush();
				nyamuk->hantar("SmartGuard", xx);
			}


			String dataMsg8 = manMessage->genMessage08(1, 31);
			xx = manMessage->turnel(dataMsg8,1,1, identifier);

			Serial.println(xx);
			Serial.flush();
			nyamuk->hantar("GFLoger", xx);

			xx = "";

			digitalWrite(0, LOW);


			identifier++;
			if(identifier>9) identifier=0;

		}

		nyamuk->hantar(machineID + "/wake", fWake?"1":"0");
	}


//	sendOutBuffer();



}


void ManWiFi_events(int8_t state)
{
	switch(state){
		case manWiFi_connected:
			hantu("Connected " + manWiFi->getInfo().ip);
			hantu(manWiFi->getInfo().routerSSID);
			mqttEnabled = true;
			break;
		case manWiFi_disconnected:
			mqttEnabled = false;
			hantu("Disconnected");
			break;
		case manWiFi_APconnected:
			hantu("AP Connected");
			break;
	}
	hantu("-------------------------");
	hantu("WiFi.mode=" + String(WiFi.getMode()));

	if((WiFi.getMode() == WIFI_MODE_AP) || (WiFi.getMode() == WIFI_MODE_APSTA)){
		hantu("WiFi.softAPIP()=" + WiFi.softAPIP().toString());
	}

	hantu("WiFi.localIP =" + WiFi.localIP().toString() + "\n\n\n");

}

bool ota_busy = false;
unsigned long manOTAlapTime=0;
bool manOTAStart=false;
int32_t otaFileSize=0, otaProgress=0;

void ManOTA_events(int8_t status)
{
	switch (status) {
		case manOTA_StartDownload:
			manOTAStart = true;
			manOTAlapTime = millis();
			esp_task_wdt_init(200,true);
//			hantu("Start download ...");

			manOLED->setSet(3);	//+++++++++++++++++++++++++++

			break;
		case manOTA_FailDownload:
			manOTAStart = false;
			manOTAlapTime = 0;
			esp_task_wdt_init(10,true);

//			hantu("Fail Download");
			manOLED->setSet(1);	//+++++++++++++++++++++++++++
			break;
		case manOTA_EndDownload:
			manOTAStart = false;
			manOTAlapTime = 0;
			esp_task_wdt_init(10,true);

			manOLED->setSet(1);	//+++++++++++++++++++++++++++
			break;
		default:
			if(manOTAStart){
				unsigned long lap = millis()-manOTAlapTime;
				if(lap > 150000){
//					hantu("OTA LAP TOO LONG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
				}
			}

			break;
	}
}

void ManOTA_size(int32_t value)
{
	otaFileSize = value;
}

void ManOTA_progress(int32_t value)
{
	double progress = ((double)value / (double)otaFileSize) * 100;
	Serial.println(progress);
	manOLED->setProgress(progress);

}


//	Serial.println(String(WiFi.getTxPower()));
//	if(Serial.available()>0){
//		Serial.println("\n\n++++++++++\n\n");
//	}
//	while(Serial.available() > 0){
//		cr = Serial.read();
//		Serial.print(cr);
//	}
//	if(Serial1.available()>0){
//		Serial.println("\n\n==========\n\n");
//	}
//	while(Serial1.available() > 0){
//		cr = Serial1.read();
//		Serial.print(cr);
//	}
//
//	if(Serial2.available()>0){
//		Serial.println("\n\n~~~~~~~~~~~~~\n\n");
//	}
//	while(Serial2.available() > 0){
//		cr = Serial2.read();
//		Serial.print(cr);
//	}
//	delay(1000);
//	Serial.println("Logger");
//	Serial.flush();
//	Serial1.println("Hahahaha");
//	Serial1.flush();




void setup()
{
	Serial.begin(9600);			//Mando
	Serial1.begin(9600);		//RMS; 4800=SmartGuard
	Serial2.begin(4800);		//Airmar
	delay(100);
	Serial.println("\n\n\n\n\nSalam Dunia Logger");

//	//=======================================================================================
//	manDecode = new ManDecode;
//
////	setTime(12,3,2,2,12,2019);
//
//	int type8[40] = {10,6,25,24,1,5,5,6,7,7,9,9,11,7,10,9,2,8,12,2,8,9,8,9,5,8,9,5,8,6,9,8,6,9,4,10,3,9,2,10};
//
//	std::vector<double>  jj = manDecode->explodeData("05tfNwB1JPluB3wwe5`P1UOGwswu3wu`wsAwwe7wwvlOwu`muOwt00", type8, 40);
//	//											      05tfNwB1JPl4h?wwe5`P1UOGwswu3wu`wsAwwe7wwvlOwu`muOwt0
//
//	jj.at(5) = day();
//	jj.at(6) = hour();
//	jj.at(7) = minute();
////	jj.at(39) = 1;
//
//	String jawapan = manDecode->compactData(jj, type8);
//	Serial.println(jawapan);
//
//	hantu("Done");
//
//	while(1){
//		delay(100);
//	}
//	//=======================================================================================


//	//=======================================================================================
//
//	MsgBuffer.push_back("data0");
//	MsgBuffer.push_back("data1");
//	MsgBuffer.push_back("data2");
//	MsgBuffer.push_back("data3");
//	MsgBuffer.push_back("data4");
//	MsgBuffer.push_back("data5");
//	MsgBuffer.push_back("data6");
//	MsgBuffer.push_back("data7");
//	MsgBuffer.push_back("data8");
//	MsgBuffer.push_back("data9");
//	MsgBuffer.push_back("data10");
//
//	hantu(String(MsgBuffer.size()));
//
//
//	MandoWake = true;
//	while(1){
//		AbkValue = 3;
//		sendOutBuffer();
//	}
//	hantu("Done");
//
//	while(1){
//		delay(100);
//	}
//	//=======================================================================================

	manMessage = new ManMessage;

	pinMode(0, OUTPUT);
	digitalWrite(0, LOW);


	pinMode(12, OUTPUT);
	digitalWrite(12, HIGH);			//Airmar POWER

	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);			//Sonar POWER on masa nak baca sahaja

	pinMode(5, OUTPUT);
	pinMode(17, OUTPUT);
	pinMode(16, OUTPUT);

	digitalWrite(5, LOW);			//mando 0x04
	digitalWrite(17, LOW);
	digitalWrite(16, HIGH);


//	digitalWrite(5, LOW);			//sonar 0x02
//	digitalWrite(17, HIGH);
//	digitalWrite(16, LOW);

	manOLED = new ManOLED;
	manOLED->setSet(4);
	xTaskCreatePinnedToCore(manOLED->loop,  "task4", 4000, NULL, 2,  &TaskManOLED, 1);
	manOLED->setPlainType1Text("Status");
	manOLED->setPlainType2Text("Sensors");


	manWiFi = new ManWiFi;
	manWiFi->addRouterSTA("ideapad", "sawabatik1");
	manWiFi->addRouterSTA("AndroidAP", "sawabatik");
	manWiFi->addRouterSTA("GF_Wifi_2.4GHz", "Gr33nF1nd3r2018");
	manWiFi->addRouterAP ("NiNe", "AsamBoiqqq");
	manWiFi->setConnectionMode(as_apsta,true);

	xTaskCreatePinnedToCore(manWiFi->loopScan, "task1", 8000, NULL, 1, &TaskManWiFi, 0);

	WiFi.setTxPower(WIFI_POWER_19_5dBm);

	int8_t typeConnection =	manWiFi->getInfo().needModeAs;
	if((typeConnection == as_sta) || (typeConnection == as_apsta)){
		manOta = new ManOTA;
		delay(2000);

		manOta->setFileLookUp("Logger.bin");
		xTaskCreatePinnedToCore(manOta->serverOTA, "task2", 10000, NULL, 1, &TaskManOTA, 0);
		esp_task_wdt_add(TaskManOTA);
		manOta->setLoopDelay(10000);

		nyamuk = new Nyamuk;

	}

	manMando = new ManMando;
	manAirmar = new ManAirmar;
	manSmartguard = new ManSmartGuard;

	xTaskCreatePinnedToCore(manMando->loop, "task3", 2000, NULL, 1, &TaskManMando, 0);
	xTaskCreatePinnedToCore(manAirmar->loop, "task5", 2000, NULL, 1, &TaskManAirmar, 0);
	xTaskCreatePinnedToCore(manSmartguard->loop, "task6", 2000, NULL, 1, &TaskManSmartguard, 0);




}
