/*
 * @(#)		Imu_razor.h
 * @version	1.0
 * @autor	C. Rouvière
 */

/**
 * Inertial Measurement Unit with serial
 */

#ifndef IMU_RAZOR
#define IMU_RAZOR

#include <cmath>
#include <vector>
#include <algorithm>
#include "../core/ComThread.h"
#include "./Subscriber.h"

class Imu_razor : public ComThread{

public:

	Imu_razor();
	~Imu_razor();

	void Subscribe(Subscriber* subscriber);

private:

	Serial* serial;

	void On_start();
	void Job();
	void IO();

	static void Process_serial_data(void* object, char* input_msg);

	float imu_thxyz[3];

};

#endif
