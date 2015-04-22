/*
 * @(#)		Remote.h
 * @version	1.0
 * @autor	C. Rouvière
 */

/**
 * Remote control with TCP
 */

#ifndef REMOTE
#define REMOTE

#include "../core/ComThread.h"
#include "../interfaces/TCP_server.h"

class Remote : public ComThread{

public:

	Remote();
	~Remote();

	void Job_and_wait_quit();

private:

	TCP_server tcp_server;

	void On_start();
	void Job();
	void IO();
	
	float remote;
	float remote_forward;
	float remote_turn;
	float remote_deeper;

	bool alive;

};

#endif
