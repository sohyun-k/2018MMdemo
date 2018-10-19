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
		setFlipHorizontal(false) -> ������ �ſ��ó�� ���� (ī�޶� ���� �ٶ�)
		setFlipHorizontal(true) -> ī�޶� ���� ���� �þ� (ī�޶�� ���� ���� ������ �ٶ�)

		setFlip��(@) �Լ��� �޾ƿ��� ����� �ȼ��� ������ų �� ī�޶� ��ǥ���� mesh���� ���� �Һ��� (���� ī�޶� ��ǥ��)
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

		/* �����϶��� �� ��, �� ����� ���� ���� �� �������� ���� ��ġ */
		if (user_parts.size() >= 1) {
						
			// ���߿� �� ���� �� ���ͼ� �� ���� �� ���!
			if (user_parts.size() >= 2) {

				// ���� �ִ� ����� 1�� index ����
				firstRealHandPoint = manager2->depthToProjection(user_parts[0].getLeftHandPoint());
				firstUserHand.set(firstRealHandPoint.x, firstRealHandPoint.y - HAND_OFFSET, 0);

				// �� ��° ����� 0�� index ����
				secondRealHandPoint = manager2->depthToProjection(user_parts[1].getRightHandPoint());
				secondUser.set(secondRealHandPoint.x - HAND_OFFSET, secondRealHandPoint.y - HAND_OFFSET, 0);
			}

			// ó���� �� �� ���� ���!
			else {
				// ó�� �� ���� 0�� index ����
				firstRealHandPoint = manager2->depthToProjection(user_parts[0].getLeftHandPoint());
				firstUserHand.set(firstRealHandPoint.x, firstRealHandPoint.y - HAND_OFFSET, 0);
			}
		}

#endif

	/* Hand Version: ���̷������� �ٷ� ���� ã�� */
	/* Head Version: �Ӹ��� �������� AR Kit ������ �̿��Ͽ� ���� ã�� */
#if HEAD_VERSION

		/* ���̷����� ������ �ʾ��� ���� �߾��� �������� (AR Kit �� ���) */
		if (user_parts.size() == 0) {
			firstUserHand.set(
				1920/2,
				1080/2,
				0
			);
		}

		/* �����϶��� �� ��, �� ����� ���� ���� �� �������� ���� ��ġ */
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

			/* ���̷��� �׸��� */
			if (user_parts.size() == 2) {
				// �� ���� ��� �� �����
				// �ڿ� ���� ������ 0�� �ε���
				user_parts[0].drawBody();
				ofDrawBitmapString("Id: last", user_parts[0].getHeadPoint());
				// ���� ���� ������ 1�� �ε����� �ٲ�
				user_parts[1].drawBody();
				ofDrawBitmapString("Id: first", user_parts[1].getHeadPoint());

			}
			else {
				// �� �� ������ ���� 0�� �ε���
				user_parts[0].drawBody();
				ofDrawBitmapString("Id: first", user_parts[0].getHeadPoint());

			}

			/// ���ϴ� Ķ���극�̼� Ȯ���� �� ������^^
			//ofPushStyle();

			//ofSetColor(0, 255, 0);
			//ofDrawCircle(realHandPoint.x, realHandPoint.y, 100);

			//ofPopStyle();
			/// �̻��� Ķ���극�̼� Ȯ���� �� ������^^

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