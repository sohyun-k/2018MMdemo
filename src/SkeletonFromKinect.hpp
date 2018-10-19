#pragma once

#define USE_DEVICE_ONE false
#define USE_DEVICE_TWO true

#define DRAW_USERS_BONES false

#define HAND_VERSION false
#define HEAD_VERSION true

#include "ofMain.h"
//#include "VisionDeviceManager.hpp"
//#include "VisionDeviceAstra.hpp"
#include "VisionDeviceManager.hpp"
//#include "VisionDeviceKinect1.hpp"
#include "VisionDeviceKinect2.hpp"
//#include "VisionDeviceOpenNi.hpp"
//#include "VisionDeviceR200.hpp"

class SkeletonFromKinect {

public:

	ofCamera					viewer;

	//VisionDeviceManager*		manager1;
	VisionDeviceManager*		manager2;

	vector<ofxKinect2::Body>	user_parts;

	ofVec3f						firstUserHand;
	ofVec3f						secondUser;
	
	ofVec3f						firstUserDepthPos;

	ofPoint						firstRealHandPoint;
	ofPoint						secondRealHandPoint;

	ofVec2f						firstUserHandOffset;
	
	void setup(VisionDeviceManager* manager2) {

		//ofEnableDepthTest();
		viewer.setNearClip(0.1);
		viewer.setFarClip(20000);

		/*
		setFlipHorizontal(false) -> 영상이 거울상처럼 보임 (카메라가 나를 바라봄)
		setFlipHorizontal(true) -> 카메라가 실제 보는 시야 (카메라와 내가 같은 방향을 바라봄)

		setFlip※(@) 함수는 받아오는 영상과 픽셀을 반전시킬 뿐 카메라 좌표계의 mesh들은 방향 불변임 (로컬 카메라 좌표계)
		*/

#if USE_DEVICE_ONE
		//manager1 = new VisionDeviceOpenNi();
		manager1 = new VisionDeviceKinect1();
		cout << "You are calling " << manager1->getDeviceType() << endl;

		manager1->setup();
		manager1->setMapCameraToCloud(true);
		manager1->setFlipHorizontal(true);
		manager1->setFlipVertical(false);

		cout << endl;
#endif

#if USE_DEVICE_TWO
		//manager2 = new VisionDeviceKinect2();
		////cout << "You are calling " << manager2->getDeviceType() << endl;
		//
		//manager2->setup();
		manager2->setMapCameraToCloud(true);
		manager2->setFlipHorizontal(false);
		manager2->setFlipVertical(true);
#endif
		firstUserHand.set(-1, -1, -1);
		secondUser.set(-1, -1, -1);

		firstUserDepthPos.set(-1, -1, -1);

		firstUserHandOffset.set(200, 0);
	}

	void update(VisionDeviceManager* manager2) {
		//ofSetWindowTitle(ofToString(ofGetFrameRate()));

#if USE_DEVICE_ONE
		manager1->update();
#endif
#if USE_DEVICE_TWO
		//manager2->update();

		user_parts.clear();
		user_parts = ((VisionDeviceKinect2*)manager2)->getSkeletons();		
		
#if HAND_VERSION

		/* 정면일때는 왼 손, 뒷 모습일 때는 오른 손 기준으로 사진 배치 */
		if (user_parts.size() >= 1) {
						
			// 나중에 한 명이 더 들어와서 두 명이 된 경우!
			if (user_parts.size() >= 2) {

				// 원래 있던 사람은 1번 index 가짐
				firstRealHandPoint = manager2->depthToProjection(user_parts[0].getLeftHandPoint());
				firstUserHand.set(firstRealHandPoint.x, firstRealHandPoint.y - HAND_OFFSET, 0);

				// 두 번째 사람은 0번 index 가짐
				secondRealHandPoint = manager2->depthToProjection(user_parts[1].getRightHandPoint());
				secondUser.set(secondRealHandPoint.x - HAND_OFFSET, secondRealHandPoint.y - HAND_OFFSET, 0);
			}

			// 처음에 한 명만 들어온 경우!
			else {
				// 처음 한 명은 0번 index 가짐
				firstRealHandPoint = manager2->depthToProjection(user_parts[0].getLeftHandPoint());
				firstUserHand.set(firstRealHandPoint.x, firstRealHandPoint.y - HAND_OFFSET, 0);
			}
		}

#endif

	/* Hand Version: 스켈레톤으로 바로 손을 찾음 */
	/* Head Version: 머리를 기준으로 AR Kit 정보를 이용하여 손을 찾음 */
#if HEAD_VERSION

		/* 스켈레톤이 잡히지 않았을 때는 중앙을 기준으로 (AR Kit 만 사용) */
		if (user_parts.size() == 0) {
			firstUserHand.set(
				1920/2,
				1080/2,
				0
			);
		}

		/* 정면일때는 왼 손, 뒷 모습일 때는 오른 손 기준으로 사진 배치 */
		if (user_parts.size() >= 1) {
			
			firstRealHandPoint = manager2->depthToProjection(user_parts[0].getHeadPoint());
			
			firstUserDepthPos.set(
				user_parts[0].getHeadPoint().x, 
				user_parts[0].getHeadPoint().y, 
				user_parts[0].getHeadPoint().z
			);

			firstUserHand.set(
				firstRealHandPoint.x + firstUserHandOffset.x, 
				firstRealHandPoint.y + firstUserHandOffset.y, 
				0
			);

		}

#endif
		

#endif
	}

	void draw() {

#if DRAW_USERS_BONES

		if (user_parts.size() >= 1) {

			/* 스켈레톤 그리기 */
			if (user_parts.size() == 2) {
				// 두 명이 들어 온 경우임
				// 뒤에 들어온 유저가 0번 인덱스
				user_parts[0].drawBody();
				ofDrawBitmapString("Id: last", user_parts[0].getHeadPoint());
				// 먼저 들어온 유저는 1번 인덱스로 바뀜
				user_parts[1].drawBody();
				ofDrawBitmapString("Id: first", user_parts[1].getHeadPoint());

			}
			else {
				// 한 명만 들어왔을 때는 0번 인덱스
				user_parts[0].drawBody();
				ofDrawBitmapString("Id: first", user_parts[0].getHeadPoint());

			}

			/// 이하는 캘리브레이션 확인할 때 쓰세요^^
			//ofPushStyle();

			//ofSetColor(0, 255, 0);
			//ofDrawCircle(realHandPoint.x, realHandPoint.y, 100);

			//ofPopStyle();
			/// 이상은 캘리브레이션 확인할 때 쓰세요^^

		}

#endif

	}

	void exit() {
		//if (manager1) {
		//	manager1->exit();
		//	delete manager1;
		//	manager1 = nullptr;
		//}

		if (manager2) {
			manager2->exit();
			delete manager2;
			manager2 = nullptr;
		}
	}

	ofVec3f getFirstLeftHandPoint() {
		return firstUserHand;
	}

	ofVec3f getSecondLeftHandPoint() {
		return secondUser;
	}

	/* for AR Kit */
	ofVec3f getFirstUserHandPos(GyroSensorData firstUserData) {

		ofPoint iPhoneCamPos;
		iPhoneCamPos.set(firstUserData.getCamPosX(), firstUserData.getCamPosY());

		firstUserHand.set(
			firstUserHand.x + iPhoneCamPos.y * 1000,
			firstUserHand.y - iPhoneCamPos.x * 1000
		); // *1000 = meter to millimeter

		if (firstUserHand.x > 1920) {
			firstUserHand.x = 1920;
		} 

		if (firstUserHand.x < 0) {
			firstUserHand.x = 0;
		}

		if (firstUserHand.y > 1080) {
			firstUserHand.y = 1080;
		}

		if (firstUserHand.y < 0) {
			firstUserHand.y = 0;
		}		

		return firstUserHand;
	}

	ofVec3f getFirstUserHeadDepthPos() {
		return firstUserDepthPos;
	}

	void keyPressed(int key) {
		switch (key) {
		case OF_KEY_LEFT: firstUserHandOffset.x -= 20; break;
		case OF_KEY_RIGHT: firstUserHandOffset.x += 20; break;
		case OF_KEY_UP: firstUserHandOffset.y -= 20; break;
		case OF_KEY_DOWN: firstUserHandOffset.y += 20; break;
		}
	}

};