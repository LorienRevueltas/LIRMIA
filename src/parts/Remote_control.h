/*
 * @(#)		Remote_control.h
 * @version	1.0
 * @autor	C. Rouvière
 */

/**
 * Remote control receiving orders from TCP
 */

#ifndef REMOTE_CONTROL
#define REMOTE_CONTROL

#include "../core/ComThread.h"
#include "../interfaces/TCP_server.h"
#include "../interfaces/vision/Blobs.h"
#include "../interfaces/vision/hsv_params.h"

class Remote_control : public ComThread{

public:

	Remote_control();
	~Remote_control();

	void Job_and_wait_quit();
	void Set_blobs_obj(Blobs* blobs);

private:

	TCP_server tcp_server;
	hsv_params hsv;
	Blobs* blobs;

	void On_start();
	void Job();
	void IO();

	float remote;
	float remote_forward;
	float remote_turn;
	float remote_deeper;
	float fsm_unlocked;

	bool alive;

};

#endif
