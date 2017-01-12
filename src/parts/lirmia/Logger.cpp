#include "Logger.h"

using namespace std;

Logger::Logger() : ComThread(){
	time_t t_now = time(0);
	struct tm* now = localtime(&t_now);
	stringstream log_name;
	log_name	<< "./test/"
			<< (now->tm_year + 1900) << "-"
			<< setfill('0') << setw(2) << (now->tm_mon + 1) << "-"
			<< setfill('0') << setw(2) << now->tm_mday << "--"
			<< setfill('0') << setw(2) << now->tm_hour << ":"
			<< setfill('0') << setw(2) << now->tm_min << ":"
			<< setfill('0') << setw(2) << now->tm_sec << "--LIRMIA.log";
	log_file.open(log_name.str());
	string header = "t\tstate\tyaw\tvyaw\tyawref\tuw\tz\tvz\tzref\tuz\tmotor1\tmotor2\tmotor3\tmotor4\tang1\tang2\tdist1\tdist2\thor1\thor2\tvert1\tvert2\tew1\tew2\talfabw1\talfabw2\tez1\tez2\talfabz1\talfabz2\tthzd0\tthzd1";
	log_file << header << "\n";
	last_t_save = -1.;
}

Logger::~Logger(){
	log_file.close();
}

void Logger::On_start(){}

void Logger::IO(){
	Link_input("fsm_state",			COMINT,		1, &fsm_state);
	Link_input("t",				COMFLOAT,	1, &t);
	Link_input("xyz",			COMFLOAT,	3, xyz);
	Link_input("vxyz",			COMFLOAT,	3, vxyz);
	Link_input("zref",			COMFLOAT,	1, &zref);
	Link_input("uz",			COMFLOAT,	1, &uz);
	Link_input("thxyz",			COMFLOAT,	3, thxyz);
	Link_input("vthxyz",			COMFLOAT,	3, vthxyz);
	Link_input("yawref",			COMFLOAT,	1, &yawref);
	Link_input("uw",			COMFLOAT,	1, &uw);
	Link_input("zref",			COMFLOAT,       1, &zref);
	Link_input("uz",			COMFLOAT,	1, &uz);
	Link_input("motor",			COMFLOAT,	4, motor);
	Link_input("cam_pipeline_angle",	COMFLOAT,	2, cam_pipeline_angle);
	Link_input("cam_pipeline_distance",	COMFLOAT,	2, cam_pipeline_distance);
	Link_input("cam_detect_horizontal",	COMFLOAT,	2, cam_detect_horizontal);
	Link_input("cam_detect_vertical",	COMFLOAT,	2, cam_detect_vertical);
	Link_input("ew1",			COMFLOAT,	1, &ew1);
	Link_input("ew2",			COMFLOAT,	1, &ew2);
	Link_input("alfabw1",			COMFLOAT,	1, &alfabw1);
	Link_input("alfabw2",			COMFLOAT,	1, &alfabw2);
	Link_input("ez1",			COMFLOAT,	1, &ez1);
	Link_input("ez2",			COMFLOAT,	1, &ez2);
	Link_input("alfabz1",			COMFLOAT,	1, &alfabz1);
	Link_input("alfabz2",			COMFLOAT,	1, &alfabz2);
	Link_input("thzd",			COMFLOAT,	2, thzd);
}

void Logger::Job(){
	Critical_receive();
	if(t < 1.){return;}
	stringstream new_line;
	new_line.setf(ios::fixed, ios::floatfield);
	new_line.setf(ios::showpoint);
	new_line.precision(2);
	new_line	<< t						<< "\t"
			<< State_machine::Decode_state_str(fsm_state)	<< "\t"
			<< thxyz[2]	<< "\t"	<<	vthxyz[2]	<< "\t"	<<	yawref	<< "\t"	<<	uw	<< "\t"
			<< xyz[2]	<< "\t"	<<	vxyz[2]		<< "\t" <<	zref	<< "\t" <<	uz	<< "\t"
			<< motor[0]	<< "\t"	<<	motor[1]	<< "\t"	<< 	motor[2]<< "\t" << 	motor[3]<< "\t"
			<< cam_pipeline_angle[0]<< "\t"	<< cam_pipeline_angle[1]<< "\t"	<< cam_pipeline_distance[0]<< "\t"<< cam_pipeline_distance[1]<< "\t"
			<< cam_detect_horizontal[0]<< "\t"<< cam_detect_horizontal[1]<< "\t"<< cam_detect_vertical[0]<< "\t"<< cam_detect_vertical[1]<< "\t"
			<< ew1		<< "\t" <<	ew2		<< "\t" <<	alfabw1	<< "\t"	<<	alfabw2	<< "\t"
			<< ez1		<< "\t"	<<	ez2		<< "\t"	<<	alfabz1	<< "\t"	<<	alfabz2	<< "\t"
			<< thzd[0]	<< "\t" << 	thzd[1] 	<< "\t";
	log_file << new_line.str() << "\n";
	if(t - last_t_save > 5){
		log_file.flush();
		last_t_save = t;
	}
}
