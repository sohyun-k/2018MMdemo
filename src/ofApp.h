#pragma once

#include "ofMain.h"
#include "VisionDeviceKinect2.hpp"

#include "MapScanning.h"
#include "PanTiltSystem.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
#include "PhoneToDesktop.hpp"
#include "ProjectorMetaphor.hpp"
#include "SkeletonFromKinect.hpp"
#include "DepthTouch.hpp"
#include "WarpImg.h"
#include "WarpVideo.h"
//#include "MobileConnectionServer.hpp"
//#include "ofxNetwork\src\ofxNetwork.h"
//#include "ofxNetwork\src\ofxTCPServer.h"
//#include "ofxTCPServer.h"
//#include "MobileConnectionManager.hpp"
//#include "MobileConnectionServer.hpp"

class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void KeyUp();
		void KeyDown();
		void KeyRight();
		void KeyLeft();
		void gotMessage(ofMessage msg);

		void receiveCommand(string command);
		void receiveImage(ofImage &image, int numBytes);

		void sendFile(ofFile file, int fileBytesToSend);
		void showVwInfo();
		void showPanTiltInfo();
		
		VisionDeviceKinect2* kinect;
				
		PanTiltSystem ptSystem;
				
		MapScanning mapScanning;

		SceneManager sceneManager;

		/* TCP network */
		//MobileConnectionServer tcp;
		ofxTCPServer tcpFile; //tcp file manager
		ofxTCPServer tcpImage; //tcp image manager
		ofxTCPServer tcpText; // tcp text manager
		ofxTCPServer tcpServer;	//tcp server

		//ClientInfo clientInfo;
		vector <string> storeText;
		//vector <ofImage> storeImage;
		
		bool b_touch;
		bool frame_touch;
		//Mat touchDepth;
		//Mat touchColor;
		int touch_cnt = 0;
		int touch_cnt2 = 0;		
		bool bDrawColorMat;

		bool	bDrawDragPoints;
		bool	bDrawContents;		

		char recv[50];

		bool b_Mapping;
		//bool mapping_mode;
		
		bool bTouchMode, bMappingMode, bDisplayMode, bUIMode;
		
		//Scene backGroundImg;
		int num_img = 0;
		//vector<string> img_path = {
		//	//"/images/scene_1.jpg",
		//	"/images/scene1_ver2.jpg",
		//	"/images/scene_2.jpg",
		//	"/images/scene_3.jpg",
		//	"/images/scene_4.jpg",
		//	"/images/background.jpg"
		//};

		vector<string> img_test_path = {
			"/images/scene1_ver2",
			"/images/scene_2",
			"/images/scene_3",
			"/images/scene_4",
			"/images/background"
		};

		vector<string> video_test_path = {
			"/images/memory_1",
			"/images/memory_2",
			"/images/memory_3"
		};

		vector<string> commandMobile = {
			"up",
			"down",
			"right",
			"left",
			"map"
		};

		WarpImg imgWarpManager;
		bool b_warpImgDisplay = false;
		bool imgWarpingStart = false;
		WarpVideo videoWarpManager;
		bool b_warpVideoDisplay = false;
		bool videoWarpingStart = false;

		int display_img_num;

		int touch_determine;
		int touch_determine_cnt;

		/* virtual window 변수들 시작 */
		PhoneToDesktop* iPhone;
		ProjectorMetaphor* projMeta;
		SkeletonFromKinect* skeletonData;
		bool testVirtual = false;

		GyroSensorData first_imu_data;
		GyroSensorData second_imu_data;

		ofVec3f firstUserHandVec;
		ofVec3f secondUserHandVec;
		
		ofVec3f headDepthPos;
		ofVec3f headPos;

		bool bkgd_flag;

		ofCamera viewer;
		int previous_x;
		int previous_y;
		/* virtual window 변수들 끝 */

		ofTrueTypeFont font;

		string imgText;
		string mobileCommand;
		string command;

		ofImage recvImage;
		ofImage backgroundA;
		ofImage backgroundB;
		ofImage backgroundC;

		ofFile sendingFile;
		ofBuffer fileBuffer;

		class MenuMaker
		{
		private:
			ofRectangle rec;
			bool mainHome;
			bool map;
			bool vWindow;
			bool pantilt;
			bool imageSend;
			string src;
		public:
			MenuMaker()
			{

			};
			MenuMaker(string str, ofRectangle rec)
			{
				src = str;
				this->rec = rec;
			}

			void setRec(float px, float py, float w, float h)
			{
				this->rec.set(px, py, w, h);
			}
			void setRec(const ofPoint& p, float w, float h)
			{
				this->rec.set(p, w, h);
			}
			ofRectangle getRec()
			{
				return this->rec;
			}
			void setRecColor()
			{
				ofFill();
				ofDrawRectangle(getRec());
				ofNoFill();
				ofSetColor(ofColor::white);
			}
			void changeDisplay(bool menu, bool changeMenu)
			{
				menu = false;
				changeMenu = true;
			}
		};

		bool home, map, vWindow, pantilt, imgReceive;
		MenuMaker mHome, mMap, mWindow, mPantilt, mimgReceive, mReceiving, mConstruct, mSelect;

		bool bReadyToReceive = true;

		private:
			DepthTouch touch;
			//TouchBGManager touchBackImgManager;
};
