/*
 * han.h
 *
 *  Created on: Jun 28, 2019
 *      Author: annuar
 */

#ifndef HAN_H_
#define HAN_H_



void hantu(String str);
void hantu(String *str);

void ManWiFi_events(int8_t state);
void ManOTA_events(int8_t status = 0);

//void ManSerial_events(String data);

void ManOTA_size(int32_t value);
void ManOTA_progress(int32_t value);

void ManSmartGuard_events(String data);

void MandoABK(int ack);

//void ManTimer_events(int8_t stateNext);

//void ManCodex_events(int8_t requestDevice);
//void SendAIS();


//void Processing_events(int8_t DoneDevice);
//void ManNmea_changePort(int8_t port);

//void Sengat(String topic, String message);



#endif /* HAN_H_ */
