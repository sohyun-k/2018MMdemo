#pragma once

#include "ofMain.h"
#include "VisionDeviceKinect2.hpp"

#include "MapScanning.h"
#include "PanTiltSystem.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
//#include "MobileConnectionServer.hpp"
//#include "ofxNetwork\src\ofxNetwork.h"
//#include "ofxNetwork\src\ofxTCPServer.h"
//#include "ofxTCPServer.h"
//#include "MobileConnectionManager.hpp"
//#include "MobileConnectionServer.hpp"
#include "PhoneToDesktop.hpp"
#include "ProjectorMetaphor.hpp"
#include "SkeletonFromKinect.hpp"
#include "DepthTouch.hpp"
#include "WarpImg.h"
#include "WarpVideo.h"

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
		void showPanTiltAngle();
		void showVirutalWindowInfo();
		void showSUIInfo();
		
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
		char recv[50];
				
		bool	bDrawDragPoints;
		bool	bDrawContents;	
		bool	bTouchMode;
		bool	bMappingMode;
		bool	b_Mapping;
		bool	bVirtualMode;
		bool	bDisplayMode; 
		bool	bUIMode;

		/* Spatial Touch */
		DepthTouch touch;
		int touch_determine;
		int touch_determine_cnt;
		ofRectangle UI_region[4];
		int UI_touch_determine;
		int UI_touch_determine_cnt;
		
		/* Image viewer */
		int display_img_num;

		vector<string> img_test_path = {
			"/images/dt/sui_idea_sharing"
			,"/images/dt/sui_image_viewer"
			//,"/images/numberimg/3"
			//,"/images/numberimg/4"
			//,			"/images/background"
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
		bool b_warpImgDisplay;
		bool imgWarpingStart = false;
		/*WarpVideo videoWarpManager;
		bool b_warpVideoDisplay = false;
		bool videoWarpingStart = false;*/

		
		/* virtual window */
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
};
