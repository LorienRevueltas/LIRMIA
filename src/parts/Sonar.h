/*
 * @(#)		Sonar.h
 */

/**
 * 
 */

#ifndef SONAR
#define SONAR

#include "../core/ComThread.h"
#include "../interfaces/Serial.h"

class Sonar : public ComThread{

public:

	Sonar();
	~Sonar();

	void Set_serial_x(Serial* serial);
	void Set_serial_y(Serial* serial);
	void Set_serial_s(Serial* serial);

private:

	Serial *serial_x, *serial_y,*serial_s;

	void On_start();
	void Job();
	void IO();

	float obst_xy[2];
	unsigned char mtSendV;
	int* sData;

	unsigned char request;

	float Get_version(Serial* serial);
	float Get_user(Serial* serial);
	float Config_sonar(Serial* serial);
	float Ask_data(Serial* serial);
	int* Get_data(Serial* serial, int* data);

};

#endif
