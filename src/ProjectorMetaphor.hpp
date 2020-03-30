#pragma once

#include "ofMain.h"
#include "GyroSensorData.hpp"

#define SINGLE
//#define DUO

class ProjectorMetaphor {

public:
	//ofImage*		object_image;
	int*			object_below_or_above;
	//ofImage			second_image;

	//ofImage			first_neon_image;
	//ofImage			second_neon_image;

	ofImage			bkgd_image;

	//ofImage			arch_background;
	//ofVideoPlayer		background;

	ofVideoPlayer	bkgd_video;

	// ��� ���� ���� �޾ƿ��� �� ��
	GyroSensorData	first_imu_data;
	float			first_fixed_yaw;
	ofVec2f			first_fixed_user_hand;

	GyroSensorData	second_imu_data;
	float			second_fixed_yaw;
	ofVec2f			second_fixed_user_hand;

	float			decision_boundary;
	int				current_image_index;

	struct MM_images {
		ofImage img;
		ofPoint fixed_point;		
		float yaw;
		bool dirty;
	};

	MM_images* for_mm_image;
	
	void setup() {

		glAlphaFunc(GL_GREATER, 0.5);
		glEnable(GL_ALPHA_TEST);

		//object_image = new ofImage[6];
		//object_below_or_above = new int[6];
		//object_image[1].load("revised_mm_asset_1.png"); object_below_or_above[1] = 1;
		//object_image[2].load("revised_mm_asset_2.png"); object_below_or_above[2] = 1;
		//object_image[3].load("revised_mm_asset_3.png"); object_below_or_above[3] = -1;
		//object_image[4].load("revised_mm_asset_4.png"); object_below_or_above[4] = -1;
		//object_image[5].load("revised_mm_asset_5.png");	object_below_or_above[5] = -1;

		for_mm_image = new MM_images[6];
		object_below_or_above = new int[6];
		for_mm_image[1].img.load("revised_mm_asset_1.png"); object_below_or_above[1] = 1;
		for_mm_image[2].img.load("revised_mm_asset_2.png"); object_below_or_above[2] = 1;
		for_mm_image[3].img.load("revised_mm_asset_3.png"); object_below_or_above[3] = -1;
		for_mm_image[4].img.load("revised_mm_asset_4.png"); object_below_or_above[4] = -1;
		for_mm_image[5].img.load("revised_mm_asset_5.png");	object_below_or_above[5] = -1;

		//first_neon_image.load("mm_asset_2.png");
		//second_neon_image.load("asset_2_neon.png");

		//bkgd_image.load("bkgd_image.png");
		//bkgd_image.load("wall_and_floor_image_5.jpg");

		/* ���� ���� */
		//bkgd_video.load("movies/bkgd_video.mp4");
		//bkgd_video.play();
		//bkgd_video.setPaused(true);

		this->first_imu_data.setData(0.0, 0.0, 0.0, -1, -1, 0.0, 0.0, 0.0);
		this->second_imu_data.setData(0.0, 0.0, 0.0, -1, -1, 0.0, 0.0, 0.0);

		decision_boundary = ofGetWindowHeight()/2;
		current_image_index = 1;

		for (int i = 1; i < 6; i++) {
			for_mm_image[i].fixed_point.set(-9999, -9999);
			for_mm_image[i].yaw = 0.0f;
			for_mm_image[i].dirty = false;			
		}
		
	}

	void update(GyroSensorData imu_data) {
		this->first_imu_data = imu_data;
	}

	void update(GyroSensorData first_imu_data, GyroSensorData second_imu_data) {
		this->first_imu_data = first_imu_data;
		this->second_imu_data = second_imu_data;

		current_image_index = first_imu_data.getUserIndex();
		//bkgd_video.update();

	}

	void draw(ofVec3f firstUserHandVec, ofVec3f secondUserHandVec) {		

		if (first_imu_data.getTouchOrder() == 0) {
			//first_image.draw(613, 498);

		}

		// 1�� ������ ù ��° ������
		else if (first_imu_data.getTouchOrder() == 2) {

			/* ��ϵ��� ���� ��ü �׸��� */
			if (!for_mm_image[current_image_index].dirty) {
				drawMovingObjectImage(
					for_mm_image[current_image_index].img,
					object_below_or_above[current_image_index],
					firstUserHandVec);
			}			

			/* �̹� ����� ��ü �׸��� */
			drawVirtualObjects();

		}

		// 1�� ������ �� ��° ������ (�̶��� ��ǥ�� �߰���)
		else if (first_imu_data.getTouchOrder() == 3) {

			/* ���� ����� ��ü ��ǥ �߰� */
			setFixedObjectCoord(for_mm_image);

			/* �̹� ����� ��ü �׸��� */
			drawVirtualObjects();		

		}				

		//bkgd_video.draw(0, 0, 1920, 1080);z

		ofPushStyle();
		
		ofSetColor(255, 0, 0);
		ofSetLineWidth(3);

		//ofDrawLine(
		//	ofPoint(0, decision_boundary), 
		//	ofPoint(ofGetWindowWidth(), decision_boundary)
		//);

		ofPopStyle();

		//bkgd_image.draw(0, 0, 1920, 1080);
	}

	/* set the parameter -1 for below, +1 for above */
	int getAboveOrBelow(int testPoint, int settingPoint) {

		// �츮�� ���� ��躸�� �Ʒ��� �־�� �ϴ� ��ü
		if (settingPoint == -1) {
			return testPoint > decision_boundary ? true : false;
		}
		// �츮�� ���� ��躸�� ���� �־�� �ϴ� ��ü
		else if (settingPoint == +1) {
			return testPoint <= decision_boundary ? true : false;;
		}

		else {
			return -1;
		}
	}

	void drawMovingObjectImage(ofImage first_image, int settingValue, ofVec3f firstUserHandVec) {
		/* �ٴڿ� ��ġ�ؾ� �ϴ� ��ü */
		//if (!getAboveOrBelow(firstUserHandVec.y + first_neon_image.getHeight(), -1)) {
		//	firstUserHandVec.y = decision_boundary - first_neon_image.getHeight();
		//}

		/* ���� ��ġ�ؾ� �ϴ� ��ü */
		if (!getAboveOrBelow(firstUserHandVec.y + first_image.getHeight(), settingValue)) {
			firstUserHandVec.y = decision_boundary - first_image.getHeight();
		}

		ofTranslate(firstUserHandVec.x, firstUserHandVec.y);
		//

		ofPushMatrix(); // >

		ofTranslate(first_image.getWidth() / 2, first_image.getHeight() / 2, 0);//move pivot to centre
		ofRotate(first_imu_data.getYaw(), 0, 0, 1);//rotate from centre
		first_fixed_yaw = first_imu_data.getYaw();

		ofPushMatrix(); // >>

		ofTranslate(-first_image.getWidth() / 2, -first_image.getHeight() / 2, 0);//move back by the centre offset		

		first_image.draw(0, 0);

		ofPopMatrix(); // >>

		ofPopMatrix(); // >

		ofTranslate(-firstUserHandVec.x, -firstUserHandVec.y);

		first_fixed_user_hand.x = firstUserHandVec.x;
		first_fixed_user_hand.y = firstUserHandVec.y;
	}

	void setFixedObjectCoord(MM_images* for_mm_image) {

		for_mm_image[current_image_index].dirty = true;
		for_mm_image[current_image_index].fixed_point.set(first_fixed_user_hand.x, first_fixed_user_hand.y);
		for_mm_image[current_image_index].yaw = first_fixed_yaw;

	}

	void setMovedObjectCoord(int ro_index, ofPoint moved_point) {

		if (!getAboveOrBelow(moved_point.y + for_mm_image[ro_index].img.getHeight(), object_below_or_above[ro_index])) { 
			moved_point.y = decision_boundary - for_mm_image[ro_index].img.getHeight();
		}

		for_mm_image[ro_index].dirty = true;
		for_mm_image[ro_index].fixed_point.set(moved_point.x, moved_point.y);
		//for_mm_image[current_image_index].yaw = first_fixed_yaw;
	}

	void drawVirtualObjects() {

		for (int i = 5; i > 0; i--) {

			/* �̹� ��ϵǾ� �ִ� ��ü�� ��� */
			if (for_mm_image[i].dirty) {

				ofTranslate(for_mm_image[i].fixed_point.x, for_mm_image[i].fixed_point.y);

				ofPushMatrix(); // >

				ofTranslate(for_mm_image[i].img.getWidth() / 2, for_mm_image[i].img.getHeight() / 2, 0);//move pivot to centre
				ofRotate(for_mm_image[i].yaw, 0, 0, 1);//rotate from centre

				ofPushMatrix(); // >> 

				ofTranslate(-for_mm_image[i].img.getWidth() / 2, -for_mm_image[i].img.getHeight() / 2, 0);//move back by the centre offset
				for_mm_image[i].img.draw(0, 0);

				ofPopMatrix(); // >>

				ofPopMatrix(); // >

				ofTranslate(-for_mm_image[i].fixed_point.x, -for_mm_image[i].fixed_point.y);
				
			}

		}

	}

	void keyPressed(int key) {
		switch (key) {
		//case ' ': 
		//	if (bkgd_video.isPlaying())
		//		bkgd_video.setPaused(true);
		//	else
		//		bkgd_video.setPaused(false);
		//	break;
		case OF_KEY_HOME: decision_boundary -= 10; break;
		case OF_KEY_END: decision_boundary += 10; break;
		case '1': current_image_index = 1; break;
		case '2': current_image_index = 2; break;
		case '3': current_image_index = 3; break;
		case '4': current_image_index = 4; break;
		case '5': current_image_index = 5; break;
		}
	}

	void exit() {
		delete object_below_or_above;
		delete for_mm_image;
	}
};