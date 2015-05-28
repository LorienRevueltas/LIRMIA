/*
 * @(#)		Remote_monitor.h
 * @version	1.0
 * @autor	C. Rouvière
 */

/**
 * Remote monitor sending data with TCP
 */

#ifndef REMOTE_MONITOR
#define REMOTE_MONITOR

#include "../../core/ComThread.h"
#include "../../interfaces/TCP_server.h"

class Remote_monitor : public ComThread{

public:

	Remote_monitor();
	~Remote_monitor();

private:

	TCP_server tcp_server;

	void On_start();
	void Job();
	void IO();

	float t;
	float xy[2];
	float thz;
	float motor;
	float rudder;
	float bow_thrusters[2];
	int fsm_state;

};

#endif
