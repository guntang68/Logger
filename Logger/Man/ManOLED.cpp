/*
 * ManOLED.cpp
 *
 *  Created on: Jul 1, 2019
 *      Author: annuar
 */

#include <ManOLED.h>
//#include "images.h"
#include "images.h"
#include <WiFi.h>
#include "ManAirmar.h"
#include "Print.h"

const int screenW = 128;
const int screenH = 64;

const int clockCenterX = screenW/2;
const int clockCenterY = ((screenH-16)/2)+16;   // top yellow part is 16 px height
const int clockRadius = 23;

const int spacing = 12;

extern double BP, AT, WS, WD;


extern std::vector<airmarReading_t> readingList;
extern bool serial1Rx;

extern bool MandoWake;
extern double sonarDistance;


void displayIcons(OLEDDisplay* display);
String makeTwoDigits(int digits);
void tTitle(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void tMeteorological(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void tProgress(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void tGraph(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);


void tPlainType1(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void tPlainType2(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
String PlainType1Text="PlainType1Text";
String PlainType2Text="PlainType2Text";



SSD1306Spi display(pOutDRes, pOutDDC, pOutDCS);
OLEDDisplayUi ui ( &display );

FrameCallback frames1[] = {tTitle, tPlainType1, tPlainType2};
FrameCallback frames2[] = {tMeteorological};
FrameCallback frames3[] = {tProgress};
FrameCallback frames4[] = {tTitle, tGraph};

int frameCount = 3;
int8_t setFrame=1;

bool frameScroll = false;
bool freshDone = false;
bool done = false;
int8_t currentSet=0;
int8_t newSet=0;

double dProgress=0;

void ManOLED::setPlainType1Text(String text) {
	PlainType1Text = text;
}


void ManOLED::setPlainType2Text(String text) {
	PlainType2Text = text;
}
inline void tPlainType1(OLEDDisplay* display, OLEDDisplayUiState* state,
		int16_t x, int16_t y) {

	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->setFont(ArialMT_Plain_10);

	display->drawString(0 ,  12, PlainType1Text );

	displayIcons(display);
	String timenow = makeTwoDigits(hour())+":"+makeTwoDigits(minute())+":"+makeTwoDigits(second());
	display->drawString(0 , 0, timenow );
}





void ManOLED::setProgress(double value) {
	dProgress = value;
}



void ManOLED::setFrameScroll(bool value) {
	frameScroll = value;
}


void ManOLED::setSet(int8_t set) {
	setFrameScroll(true);
	newSet = set;

	ui.enableAutoTransition();

}


ManOLED::ManOLED() {
	ui = &display;

	ui.setTargetFPS(20);
	ui.setFrameAnimation(SLIDE_LEFT);
	ui.setFrames(frames4, 2);
	ui.enableAutoTransition();
	ui.disableAllIndicators();


	display.init();


	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.flipScreenVertically();



	frameScroll = true;

}


unsigned long lastRx;
bool mandoMissing=false;


void ManOLED::loop(void* parameter) {

	ui.disableAllIndicators();

	while(1){

		if(serial1Rx){
			serial1Rx = false;
			lastRx = millis();
			mandoMissing = false;

		}
		if((millis() - lastRx) > 12000){
			mandoMissing = true;
		}


		if(frameScroll){
			int remainingTimeBudget = ui.update();
			if(remainingTimeBudget > 0){
				delay(remainingTimeBudget);
			}

			if(ui.getUiState()->frameState == IN_TRANSITION){
				freshDone = false;

			}
			else{
				if(currentSet != newSet){
					currentSet = newSet;

					switch (currentSet) {
						case 1:
							frameCount = 3;
							ui.setFrames(frames1, frameCount);
							break;
						case 2:
							frameCount = 1;
							ui.setFrames(frames2, frameCount);
							ui.disableAutoTransition();
							break;
						case 3:
							frameCount = 1;
							ui.setFrames(frames3, frameCount);
							ui.disableAutoTransition();
							break;
						case 4:

//							hantu("SINI");
//
//							frameCount = 2;
//							ui.setFrames(frames4, frameCount);

//							ui.disableAutoTransition();
							break;
						default:
							break;
					}
				}
			}
		}



		delay(1);
	}
}


inline void displayIcons(OLEDDisplay* display) {





	switch (WiFi.getMode()) {
		case 1:
			display->drawFastImage(48, 0, 8, 8, sta);
			break;
		case 2:
			display->drawFastImage(48, 0, 8, 8, ap);
			break;
		case 3:
			display->drawFastImage(48, 0, 8, 8, both);
			break;
		default:
			display->drawFastImage(48, 0, 8, 8, n);
			break;
	}

	display->drawFastImage(58, 0, 8, 8, nOpen);

//		if(sDoorOpen){
//			display->drawFastImage(58, 0, 8, 8, nOpen);
//		}
//		else{
//			display->drawFastImage(58, 0, 8, 8, nClose);
//		}

	if(MandoWake){
		display->drawFastImage(68, 0, 8, 8, aton);
	}
	else{
		display->drawFastImage(68, 0, 8, 8, atonsleep);
	}

	if(mandoMissing){
		display->drawFastImage(78, 0, 8, 8, offline);

	}
	else{
		display->drawFastImage(78, 0, 8, 8, online);
	}



	//	if(gfSensor->MainPower()){
	//		display->drawFastImage(78, 0, 12, 8, nMain);
	//	}
	//	else{
	//		if(gfSensor->getVoltage() > 4.15){
	//			display->drawFastImage(78, 0, 12, 8, nBatteryFull);
	//		}
	//		else if(gfSensor->getVoltage() > 3.91){
	//			display->drawFastImage(78, 0, 12, 8, nBattery34);
	//		}
	//		else if(gfSensor->getVoltage() > 3.77){
	//			display->drawFastImage(78, 0, 12, 8, nBattery12);
	//		}
	//		else if(gfSensor->getVoltage() > 3.5){
	//			display->drawFastImage(78, 0, 12, 8, nBattery14);
	//		}
	//		else if(gfSensor->getVoltage() > 3.65){
	//			display->drawFastImage(78, 0, 12, 8, nBattery18);
	//		}
	//		else{
	//			display->drawFastImage(78, 0, 12, 8, nBatteryAlarm);
	//		}
	//
	//	}

	//	if(fixedPosition){
	//		display->drawFastImage(93, 0, 8, 8, nGPS);
	//	}
	//	else{
	//		display->drawFastImage(93, 0, 8, 8, nXGPS);
	//	}
}

inline String makeTwoDigits(int digits)
{
	if(digits < 10) {
		String i = '0'+String(digits);
		return i;
	}
	else {
		return String(digits);
	}
}


inline void tTitle(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->setFont(ArialMT_Plain_10);

	String timenow = makeTwoDigits(hour())+":"+makeTwoDigits(minute())+":"+makeTwoDigits(second());
	display->drawString(0 , 0, timenow );

//	timenow = "SPMS V5.0";
//	timenow = "GFLOGER";
	timenow = "EARTLOG";
	display->setTextAlignment(TEXT_ALIGN_CENTER);
	display->setFont(ArialMT_Plain_24);
	display->drawString(clockCenterX + x , clockCenterY/2 + y - 2, timenow );

//	timenow = "GreenFinder";
	timenow = "GeoAIS-Comm";
	display->setFont(ArialMT_Plain_16);
	display->drawString(clockCenterX + x , clockCenterY + y + 4, timenow );

	displayIcons(display);
}

inline void tPlainType2(OLEDDisplay* display, OLEDDisplayUiState* state,
		int16_t x, int16_t y) {
	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->setFont(ArialMT_Plain_10);

	display->drawString(0 ,  12, PlainType2Text );

	displayIcons(display);
	String timenow = makeTwoDigits(hour())+":"+makeTwoDigits(minute())+":"+makeTwoDigits(second());
	display->drawString(0 , 0, timenow );
}

inline void tGraph(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

//	display.clearDisplay();
	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->setFont(ArialMT_Plain_10);


//	display->drawRect(0, 13, 128, 40);
	display->drawRect(0, 13, 62, 40);

	int xp=0, yp=0, yt=0;

	if(readingList.size()>0){
		airmarReading_t v = readingList.at(0);
		yp = 60 - (v.ws*2 +15);

		for(int x = 1; x< readingList.size(); x++){
			v = readingList.at(x);

			yt = 60 - (v.ws*2 +15);
			if(yt < 5) yt = 5;

			display->drawLine(xp, yp, x,yt);
			xp = x;
			yp = yt;
		}

	}

	display->drawString(72 , 50, String(sonarDistance,0) + "mm");
	display->drawCircle(94, 25, 13);

//	display->fillRect(20, 50, 45, 15);
	double rad = WD*3.14/180;
	double dX = 15 * sin(rad);
	double dY = 15 * cos(rad) * -1;

	display->drawLine(94, 25, 94+dX, 25+dY);


//	display->display();



	String timenow = makeTwoDigits(hour())+":"+makeTwoDigits(minute())+":"+makeTwoDigits(second());
	display->drawString(0 , 0, timenow );

	displayIcons(display);

}

inline void tMeteorological(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
	String text = "";
	int offY = 0;

	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->setFont(ArialMT_Plain_10);

	text = "AT : ";// + String(AT);
	offY += spacing;
	display->drawString(x ,  y + offY , text );

	text = "BP : ";// + String(BP);
	offY += spacing;
	display->drawString(x ,  y + offY , text );


	text = "WS : ";// + String(WS);
	offY += spacing;
	display->drawString(x ,  y + offY , text );

	text = "WD : ";// + String(WD);
	offY += spacing;
	display->drawString(x ,  y + offY , text );


	displayIcons(display);
	String timenow = makeTwoDigits(hour())+":"+makeTwoDigits(minute())+":"+makeTwoDigits(second());
	display->drawString(0 , 0, timenow );
}



ManOLED::~ManOLED() {
}

inline void tProgress(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->setFont(ArialMT_Plain_10);

	int progress = (int) dProgress % 101;
	// draw the progress bar
	display->drawProgressBar(0, 32, 120, 10, progress);

	// draw the percentage as String
	display->setTextAlignment(TEXT_ALIGN_CENTER);
	display->drawString(64, 15, String(dProgress) + "%");

	display->drawString(64 , 50 , "New firmware ..." );
	String timenow = makeTwoDigits(hour())+":"+makeTwoDigits(minute())+":"+makeTwoDigits(second());

	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->drawString(0 , 0, timenow );
}

