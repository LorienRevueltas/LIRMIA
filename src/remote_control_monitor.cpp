#include "interfaces/TCP_client.h"
#include "interfaces/Joystick.h"
#include "interfaces/vision/hsv_params.h"
#include "parts/State_machine.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define MONITOR_SIZE	500
#define MONITOR_BORDER	50

typedef struct{
	TCP_client* tcp_client;
	bool go_on;
	bool remote_mode;
} struct_callback;

typedef struct{
	float x, y, z, thz;
	int motor1, motor2, motor3, motor4;
	string state;
	float min_coord, max_coord;
	vector <vector <float> > path;
} struct_monitor;
	

static void send_move_order(void* obj, string msg){
	struct_callback* obj_callback = (struct_callback*) obj;
	obj_callback->tcp_client->Send(msg);
}

static void listen_key_up	(void* obj, bool down){send_move_order(obj, "ra" + to_string(down));}
static void listen_key_down	(void* obj, bool down){send_move_order(obj, "rb" + to_string(down));}
static void listen_key_left	(void* obj, bool down){send_move_order(obj, "rl" + to_string(down));}
static void listen_key_right	(void* obj, bool down){send_move_order(obj, "rr" + to_string(down));}
static void listen_key_plus	(void* obj, bool down){send_move_order(obj, "ru" + to_string(down));}
static void listen_key_minus	(void* obj, bool down){send_move_order(obj, "rd" + to_string(down));}

static void listen_key_a(void* obj, bool down){
	if(down){
		struct_callback* obj_callback = (struct_callback*) obj;
		obj_callback->remote_mode = !(obj_callback->remote_mode);
		obj_callback->tcp_client->Send(obj_callback->remote_mode ? "r1" : "r0");
	}
}

static void listen_key_q(void* obj, bool down){
	struct_callback* obj_callback = (struct_callback*) obj;
	obj_callback->go_on = false;
	obj_callback->tcp_client->Send("bye");
}

static void verify_HSV_params(hsv_params* hsv){
	if(hsv->H_min > hsv->H_max){cv::setTrackbarPos(hsv->name_H_max, hsv->winname, hsv->H_min);};
	if(hsv->S_min > hsv->S_max){cv::setTrackbarPos(hsv->name_S_max, hsv->winname, hsv->S_min);};
	if(hsv->V_min > hsv->V_max){cv::setTrackbarPos(hsv->name_V_max, hsv->winname, hsv->V_min);};
	cv::Mat img_color = cv::Mat::zeros(100, 300, CV_8UC3);
	cv::rectangle(img_color, cv::Point(0, 0), cv::Point(150, 100), cv::Scalar(hsv->H_min, hsv->S_min, hsv->V_min), CV_FILLED);
	cv::rectangle(img_color, cv::Point(150, 0), cv::Point(300, 100), cv::Scalar(hsv->H_max, hsv->S_max, hsv->V_max), CV_FILLED);
	cv::cvtColor(img_color, img_color, CV_HSV2RGB, 3);
	cv::imshow((char*) hsv->winname, img_color);
}

static void send_HSV_param(void* object, string header, int* param){
	hsv_params* hsv = (hsv_params*) object;
	verify_HSV_params(hsv);
	hsv->tcp->Send(header + to_string(*param));
}

static void callback_H_min	(int value, void* object){send_HSV_param(object, "hsv_h0_", &(((hsv_params*) object)->H_min));}
static void callback_H_max	(int value, void* object){send_HSV_param(object, "hsv_h1_", &(((hsv_params*) object)->H_max));}
static void callback_S_min	(int value, void* object){send_HSV_param(object, "hsv_s0_", &(((hsv_params*) object)->S_min));}
static void callback_S_max	(int value, void* object){send_HSV_param(object, "hsv_s1_", &(((hsv_params*) object)->S_max));}
static void callback_V_min	(int value, void* object){send_HSV_param(object, "hsv_v0_", &(((hsv_params*) object)->V_min));}
static void callback_V_max	(int value, void* object){send_HSV_param(object, "hsv_v1_", &(((hsv_params*) object)->V_max));}
static void callback_nb_dilate	(int value, void* object){send_HSV_param(object, "hsv_m0_", &(((hsv_params*) object)->nb_dilate));}
static void callback_nb_erode	(int value, void* object){send_HSV_param(object, "hsv_m1_", &(((hsv_params*) object)->nb_erode));}
static void callback_seuil	(int value, void* object){send_HSV_param(object, "hsv_t0_", &(((hsv_params*) object)->seuil));}

hsv_params create_HSV_params(){
	struct_callback obj_callback;
	hsv_params hsv;
	ifstream in_file("./test/hsv_params.txt");
	if(in_file.is_open()){
		char line_char[256];
		in_file.getline(line_char, 256);
		in_file.getline(line_char, 256);
		string line_str = string(line_char);
		size_t next;
		vector <string> tokens;
		for(size_t current = 0; tokens.size() < 9; current = next + 1){
			next = line_str.find_first_of(",", current);
			tokens.push_back(line_str.substr(current, next - current));
		}
		hsv.H_min = stoi(tokens[0]);	hsv.S_min = stoi(tokens[2]);	hsv.V_min = stoi(tokens[4]);
		hsv.H_max = stoi(tokens[1]);	hsv.S_max = stoi(tokens[3]);	hsv.V_max = stoi(tokens[5]);
		hsv.nb_dilate = stoi(tokens[6]);hsv.nb_erode = stoi(tokens[7]);	hsv.seuil = stoi(tokens[8]);
		in_file.close();
	}
	else{
		hsv.H_min = 80;		hsv.S_min = 100;	hsv.V_min = 100;
		hsv.H_max = 130;	hsv.S_max = 255;	hsv.V_max = 255;
		hsv.nb_dilate = 5;	hsv.nb_erode = 5;	hsv.seuil = 100;
	}
	strcpy(hsv.name_H_min, "H_min"); strcpy(hsv.name_H_max, "H_max");
	strcpy(hsv.name_S_min, "S_min"); strcpy(hsv.name_S_max, "S_max");
	strcpy(hsv.name_V_min, "V_min"); strcpy(hsv.name_V_max, "V_max");
	strcpy(hsv.name_nb_dilate, "nb_dilate");
	strcpy(hsv.name_nb_erode, "nb_erode");
	strcpy(hsv.name_seuil, "seuil");
	strcpy(hsv.winname, "HSV parameters");
	return hsv;
}

void write_HSV_params(hsv_params* hsv){
	ofstream out_file("./test/hsv_params.txt");
	string out_text = "H_min, H_max, S_min, S_max, V_min, V_max, nb_dilate, nb_erode, seuil,\n";
	int tab[] = {hsv->H_min, hsv->H_max, hsv->S_min, hsv->S_max, hsv->V_min, hsv->V_max, hsv->nb_dilate, hsv->nb_erode, hsv->seuil};
	for(int i = 0; i < 9; i++){out_text += to_string(tab[i]) + (i < 8 ? "," : ",\n");}
	out_file << out_text;
	out_file.close();
}

void create_HSV_trackbars(hsv_params* hsv){
	cv::namedWindow(hsv->winname, CV_WINDOW_AUTOSIZE);
	cv::createTrackbar(hsv->name_H_min,	hsv->winname,	&(hsv->H_min),		360,	callback_H_min,		hsv);
	cv::createTrackbar(hsv->name_H_max,	hsv->winname,	&(hsv->H_max),		360,	callback_H_max,		hsv);
	cv::createTrackbar(hsv->name_S_min,	hsv->winname,	&(hsv->S_min),		255,	callback_S_min,		hsv);
	cv::createTrackbar(hsv->name_S_max,	hsv->winname,	&(hsv->S_max),		255,	callback_S_max,		hsv);
	cv::createTrackbar(hsv->name_V_min,	hsv->winname,	&(hsv->V_min),		255,	callback_V_min,		hsv);
	cv::createTrackbar(hsv->name_V_max,	hsv->winname,	&(hsv->V_max),		255,	callback_V_max,		hsv);
	cv::createTrackbar(hsv->name_nb_dilate,	hsv->winname,	&(hsv->nb_dilate),	20,	callback_nb_dilate,	hsv);
	cv::createTrackbar(hsv->name_nb_erode,	hsv->winname,	&(hsv->nb_erode),	20,	callback_nb_erode,	hsv);
	cv::createTrackbar(hsv->name_seuil,	hsv->winname,	&(hsv->seuil),		1000,	callback_seuil,		hsv);
}

void send_first_HSV_params(hsv_params* hsv){
	callback_H_min(hsv->H_min, hsv);
	callback_H_max(hsv->H_max, hsv);
	callback_S_min(hsv->S_min, hsv);
	callback_S_max(hsv->S_max, hsv);
	callback_V_min(hsv->V_min, hsv);
	callback_V_max(hsv->V_max, hsv);
	callback_nb_dilate(hsv->nb_dilate, hsv);
	callback_nb_erode(hsv->nb_erode, hsv);
	callback_seuil(hsv->seuil, hsv);
}

void init_joystick_listeners(Joystick* joystick, struct_callback* obj_callback){
	#ifdef ENABLE_SDL
		joystick->Connect_keyboard(SDLK_UP,		&listen_key_up,		obj_callback);
		joystick->Connect_keyboard(SDLK_DOWN,		&listen_key_down,	obj_callback);
		joystick->Connect_keyboard(SDLK_LEFT,		&listen_key_left,	obj_callback);
		joystick->Connect_keyboard(SDLK_RIGHT,		&listen_key_right,	obj_callback);
		joystick->Connect_keyboard(SDLK_KP_PLUS,	&listen_key_plus,	obj_callback);
		joystick->Connect_keyboard(SDLK_KP_MINUS,	&listen_key_minus,	obj_callback);
		joystick->Connect_keyboard(SDLK_a,		&listen_key_a,		obj_callback);
		joystick->Connect_keyboard(SDLK_q,		&listen_key_q,		obj_callback);
	#endif
}

cv::Mat Draw_monitor(struct_monitor* monitor){
	cv::Mat img_monitor = cv::Mat::zeros(500, 500, CV_8UC3);
	cv::Scalar red(0, 0, 255);
	vector <float> xy; xy.push_back(monitor->x); xy.push_back(monitor->y); 
	monitor->path.push_back(xy);
	if	(monitor->x < monitor->min_coord){monitor->min_coord = monitor->x;}
	else if	(monitor->x > monitor->max_coord){monitor->max_coord = monitor->x;}
	if	(monitor->y < monitor->min_coord){monitor->min_coord = monitor->y;}
	else if	(monitor->y > monitor->max_coord){monitor->max_coord = monitor->y;}
	cv::Point pt_draw_prev;
	for(size_t i = 0; i < monitor->path.size(); i++){
		float delta_min_max = monitor->max_coord - monitor->min_coord;
		float delta_size_border = (float) (MONITOR_SIZE - MONITOR_BORDER);
		int draw_x = MONITOR_BORDER / 2 + (int) (delta_size_border * (monitor->path[i][0] - monitor->min_coord) / delta_min_max);
		int draw_y = MONITOR_BORDER / 2 + (int) (delta_size_border * (monitor->path[i][1] - monitor->min_coord) / delta_min_max);
		cv::Point pt_draw = cv::Point(draw_x, draw_y);
		if(i != 0){
			cv::line(img_monitor, pt_draw_prev, pt_draw, cv::Scalar(255, 0, 0));
		}
		pt_draw_prev = pt_draw;
		if(i == monitor->path.size() - 1){
			cv::circle(img_monitor, pt_draw, 10, red, 3);
			cv::Point pt_arrow = pt_draw + cv::Point(40 * cos(monitor->thz), 40 * sin(monitor->thz));
			cv::Point pt_arrow_l = pt_arrow + cv::Point(10 * cos(monitor->thz + 2.5), 10 * sin(monitor->thz + 2.5));
			cv::Point pt_arrow_r = pt_arrow + cv::Point(10 * cos(monitor->thz - 2.5), 10 * sin(monitor->thz - 2.5));
			cv::line(img_monitor, pt_draw, pt_arrow, red, 3);
			cv::line(img_monitor, pt_arrow, pt_arrow_l, red, 3);
			cv::line(img_monitor, pt_arrow, pt_arrow_r, red, 3);
		}
	}
	string text_x = "x = " + to_string(monitor->x);
	string text_y = "y = " + to_string(monitor->y);
	string text_z = "z = " + to_string(monitor->z);
	string text_motor1 = "motor1 = " + to_string(monitor->motor1) + "%";
	string text_motor2 = "motor2 = " + to_string(monitor->motor2) + "%";
	string text_motor3 = "motor3 = " + to_string(monitor->motor3) + "%";
	string text_motor4 = "motor4 = " + to_string(monitor->motor4) + "%";
	cv::putText(img_monitor, monitor->state, cv::Point(10, 20), CV_FONT_HERSHEY_SIMPLEX, 0.5, red);
	cv::putText(img_monitor, text_x, cv::Point(10, 40), CV_FONT_HERSHEY_SIMPLEX, 0.5, red);
	cv::putText(img_monitor, text_y, cv::Point(10, 60), CV_FONT_HERSHEY_SIMPLEX, 0.5, red);
	cv::putText(img_monitor, text_z, cv::Point(10, 80), CV_FONT_HERSHEY_SIMPLEX, 0.5, red);
	cv::putText(img_monitor, text_motor1, cv::Point(10, 100), CV_FONT_HERSHEY_SIMPLEX, 0.5, red);
	cv::putText(img_monitor, text_motor2, cv::Point(10, 120), CV_FONT_HERSHEY_SIMPLEX, 0.5, red);
	cv::putText(img_monitor, text_motor3, cv::Point(10, 140), CV_FONT_HERSHEY_SIMPLEX, 0.5, red);
	cv::putText(img_monitor, text_motor4, cv::Point(10, 160), CV_FONT_HERSHEY_SIMPLEX, 0.5, red);
	return img_monitor;
}

int main(int argc, char* argv[]){
	#if defined(ENABLE_SDL) and defined(ENABLE_TCP)

		// Manipulated objects
		Joystick joystick;
		TCP_client tcp_client_remote, tcp_client_monitor;
		struct_callback obj_callback;
		struct_monitor monitor;
		hsv_params hsv = create_HSV_params();

		// Init remote controller
		hsv.tcp = &tcp_client_remote;
		create_HSV_trackbars(&hsv);
		init_joystick_listeners(&joystick, &obj_callback);
		tcp_client_remote.Configure(argv[1], 4242);
		obj_callback.tcp_client = &tcp_client_remote;
		obj_callback.go_on = true;
		obj_callback.remote_mode = true;

		// Init remote monitor
		usleep(1000000);
		tcp_client_monitor.Configure(argv[1], 4241);
		monitor.min_coord = -1.;
		monitor.max_coord = +1.;
		monitor.path.clear();

		// Job
		send_first_HSV_params(&hsv);
		while(obj_callback.go_on){
			string msg_monitor = string(tcp_client_monitor.Receive());
			size_t next;
			if(count(msg_monitor.begin(), msg_monitor.end(), '|') == 9){
				vector <string> tokens;
				for(size_t current = 0; tokens.size() < 9; current = next + 1){
					next = msg_monitor.find_first_of("|", current);
					tokens.push_back(msg_monitor.substr(current, next - current));
				}
				float t = stof(tokens[0]);
				string fsm = State_machine::Decode_state_str(stof(tokens[1]));
				float thx = stof(tokens[2]);
				float thy = stof(tokens[3]);
				float thz = stof(tokens[4]);
				monitor.motor1 = (int) stof(tokens[5]);
				monitor.motor2 = (int) stof(tokens[6]);
				monitor.motor3 = (int) stof(tokens[7]);
				monitor.motor4 = (int) stof(tokens[8]);
				monitor.x = 0.;
				monitor.y = 0.;
				monitor.z = 42.;
				monitor.thz = thz / 57.3;
				monitor.state = fsm;
			}
			cv::imshow("Monitor", Draw_monitor(&monitor));
			cv::waitKey(10);
			joystick.Update_event();
		}

		// Goodbye
		tcp_client_remote.Send("bye");
		usleep(1000000);
		tcp_client_remote.Close();
		tcp_client_monitor.Close();
		write_HSV_params(&hsv);

	#else
		cout << "[Error] You need to activate SDL and TCP to use remote controller" << endl;
	#endif
}
