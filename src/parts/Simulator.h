/*
 * @(#)		Simulator.h
 * @version	1.0
 * @autor	C. Rouvière
 */

/**
 * Generate fake data for sensor from motor orders
 */

#ifndef SIMULATOR
#define SIMULATOR

#include "../core/ComThread.h"
#include <cmath>

class Simulator : public ComThread{

public:

	Simulator();
	~Simulator();

private:

	void On_start();
	void Job();
	void IO();

	float t;
	float simu_xyz[3];
	float simu_thxyz[3];
	float motor[4];

	float last_t;
	float acceleration[6];
	float velocity[6];

	void Saturate(float* number, float maximum);

};

#endif
