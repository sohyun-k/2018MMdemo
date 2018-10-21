#pragma once
#include "string.h"
#include "ofMain.h"
#include "ofxNetwork.h"
#include "GyroSensorData.hpp"

#define PACKET_SIZE 10000
#define HOST "165.132.48.154"
#define CLIENT "165.132.48.164" 
#define PORT 7979
#define FONT_SIZE 24
#define _MAX_CLIENT_ 10000

#define HOW_MANY_USERS 2

class PhoneToDesktop : public ofThread {

public :
	int nClient;
	int	listenPORT;
	int nGyroClient;

	double _roll;
	double _pitch;
	double _yaw;
	int _user_number;
	int _touch_order;

	// AR Kit (Cam position)
	float _cam_pos_x;
	float _cam_pos_y;
	float _cam_pos_z;

	string clientIP[_MAX_CLIENT_];
	string clientPORT[_MAX_CLIENT_];

	ofxUDPManager udpReceiver;
	ofxUDPManager udpSender;

	ofxTCPServer tcpReceiver;
	ofxTCPServer tcpSender;

	string trackMessage;

	//GyroSensorData fromPhoneData;
	GyroSensorData firstPhoneData;
	GyroSensorData secondPhoneData;

	// 아래 미구현임
	//vector<GyroSensorData> multiUserPhoneData;

	void setup() {

		startThread(true);

		_user_number = -1;
		
		ofSetVerticalSync(true);
		ofSetFrameRate(60);

		tcpReceiver.setup(19137);
		tcpSender.setup(19136);

		/*udpReceiver.Create();
		udpReceiver.Bind(PORT);
		udpReceiver.SetNonBlocking(true);

		udpSender.Create();
		udpSender.Connect(CLIENT, 2476); 
		udpSender.SetNonBlocking(true);*/	
	}

	/* Data parsing */
	void update(){
		
		/*char udpMessage[PACKET_SIZE];
		
		udpReceiver.Receive(udpMessage, PACKET_SIZE);
		string packet = udpMessage;*/

		string tcpMessage;
		for (int i = 0; i < tcpReceiver.getNumClients(); i++)
		{
			if (tcpReceiver.isClientConnected(i))
			{
				tcpMessage = tcpReceiver.receive(tcpReceiver.getLastID() - 1);
			}
		}
		
		if (tcpMessage != "") {

			vector<string> message = ofSplitString(tcpMessage, ":");
				
			if (message[0] == "ROLL") {
				_roll = ::atof(message[1].c_str());
				_roll = 0 - _roll * (90 / 1.5);					
			}
			if (message[2] == "PITCH") {
				_pitch = ::atof(message[3].c_str());
				_pitch = 0 - _pitch * (90 / 1.5);
			}
			if (message[4] == "YAW") {
				_yaw = ::atof(message[5].c_str());
				_yaw = 0 - _yaw * (90 / 1.5);
			}
			if (message[6] == "USER_NUMBER") {
				_user_number = ::atoi(message[7].c_str());
			}
			if (message[8] == "TOUCH_ORDER") {
				_touch_order = ::atoi(message[9].c_str());
			}

			// iPhone Camera Pose with AR Kit (meter 단위)
			if (message[10] == "CAM_POS_X") {
				_cam_pos_x = ::atof(message[11].c_str());
			}
			if (message[12] == "CAM_POS_Y") {
				_cam_pos_y = ::atof(message[13].c_str());
			}
			if (message[14] == "CAM_POS_Z") {
				_cam_pos_z = ::atof(message[15].c_str());
			}
			
			/* 1P */
			//if (_user_number == 0) {
				firstPhoneData.setData(_pitch, _roll, _yaw, _user_number, _touch_order, _cam_pos_x, _cam_pos_y, _cam_pos_z);
			//}

			/* 2P */
			//else if (_user_number == 1) {
			//	secondPhoneData.setData(_pitch, _roll, _yaw, _user_number, _touch_order, _cam_pos_x, _cam_pos_y, _cam_pos_z);
			//}
			
		}

	}

	void draw() {
		stopThread();
	}

	void exit() {

	}
	
	float getRoll() {
		return _roll;
	}

	float getPitch() {
		return _pitch;
	}

	float getYaw() {
		return _yaw;
	}

	//GyroSensorData getSensorData() {		
	//	return fromPhoneData.getData();
	//}

	GyroSensorData getFirstSensorData() {		
		return firstPhoneData.getData();
	}

	GyroSensorData getSecondSensorData() {
		return secondPhoneData.getData();
	}
	
};