#pragma once

#include "ofApp.h"
#define IMAGE_SIZE 750000
#define IMAGE_WIDTH 200
#define IMAGE_HEIGHT 200

//--------------------------------------------------------------
void ofApp::setup() {

	font.load("verdana.ttf", 30);

	/* �ʱ� �޴� */
	home = true;
	//bVirtualMode = false;
	bVirtualMode = false;
	//map = false;
	bTouchMode = false;
	bMappingMode = false;
	bDisplayMode = false;
	bUIMode = false;
	bDrawDragPoints = true;
	bDrawContents = true;
	this->b_warpImgDisplay = false;

	/* tcp network setup */
	tcpServer.setup(19132, true);
	tcpFile.setup(19133, true);
	tcpImage.setup(19134, true);
	tcpText.setup(19135, true);
	bReadyToReceive = true;

	/* Kinect 2 */
	kinect = new VisionDeviceKinect2();
	kinect->setup();
	kinect->setFlipVertical(true);
	//kinect->setFlipVertical(false);
	kinect->setFlipHorizontal(false);
	kinect->setRefineDepthData(false);

	/* Virtual window */
	iPhone = new PhoneToDesktop();
	projMeta = new ProjectorMetaphor();
	skeletonData = new SkeletonFromKinect();
	bkgd_flag = true;

	/* Virtual window setup */
	/*viewer.setNearClip(0.1);
	viewer.setFarClip(20000);*/
	iPhone->setup();
	projMeta->setup();
	skeletonData->setup(kinect);

	/* 3D Map Reconstruction */
	b_Mapping = false;
	mapScanning.Init(kinect);

	/* 3D Map file transfer */
	sendingFile.open(ofToDataPath("rotation.txt"), ofFile::ReadWrite, false);
	fileBuffer = sendingFile.readToBuffer();

	/* Pan-tilt */
	ptSystem.setup();
	ofSleepMillis(3000);
	ptSystem.initializePose();
	ptSystem.bSceneChange = true;

	/* Scene Manager */
	sceneManager.touch = &touch;
	sceneManager.load();
	sceneManager.ptSystem = &ptSystem;
	sceneManager.enableDrawDragPoints();

	/* Image Warping */
	this->imgWarpManager.testSetup(this->img_test_path);
	//this->videoWarpManager.testSetup(this->video_test_path);
	this->imgWarpManager.bDrawDragPoints = true;
	this->imgViewer.testSetup(this->img_viewer_path);
	this->imgViewer.bDrawDragPoints = true;
	this->imgVirtual.testSetup(this->img_virtual_path);
	this->imgVirtual.bDrawDragPoints = true;

	display_img_num = 0;
	touch_determine_cnt = 0;
	UI_touch_determine = -1;
	UI_touch_determine_cnt = 0;

	/* new touch setup */
	touch.init(kinect);
	touch.set();
	touch.parameterSetup(touch.minT, touch.maxT, touch.touchPointOffset.x, touch.touchPointOffset.y, touch.touchMinArea, touch.touchMaxArea);
	
	cout << "bMappingMode = " << bMappingMode << ", bDisplayMode = " << bDisplayMode << ", bTouchMode = " << bTouchMode << ", bVirtualWinodw = " << bVirtualMode << ", bUIMode = " << bUIMode << ", bWarpMode = " << this->b_warpImgDisplay << endl;
	cout << "touch.bDrawTouchPoint = " << touch.bDrawTouchPoint << ", touch.bDrawTouchDebugView = " << touch.bDrawTouchDebugView << endl;

}

//--------------------------------------------------------------
void ofApp::update() {

	char *recvText;

	kinect->update();
	
	if (bMappingMode)
	{
		if (b_Mapping)
		{
			mapScanning.Update();

			KeyRight();
			ofSleepMillis(500);
			if (ptSystem.panAngle >= ptSystem.Pan_max)
			{
				b_Mapping = false;
				ptSystem.bSceneChange = false;

				bReadyToReceive = true;
			}
		}
	}

	/* Pan Tilt update */
	if (ptSystem.bSceneChange)
	{
		if (!bMappingMode)
		{
			ptSystem.panAngle = sceneManager.scenes[sceneManager.currentSceneIndex].panAngle;
			ptSystem.tiltAngle = sceneManager.scenes[sceneManager.currentSceneIndex].tiltAngle;
		}
		ptSystem.update(ptSystem.panAngle, ptSystem.tiltAngle);
		showPanTiltAngle();
	}
	
	/* Virtual Window */
	if (bVirtualMode)
	{
		iPhone->update();

		first_imu_data = iPhone->getFirstSensorData();
		second_imu_data = iPhone->getSecondSensorData();

		projMeta->update(first_imu_data, second_imu_data);

		skeletonData->update(kinect);
		headDepthPos = skeletonData->getFirstUserHeadDepthPos();
		headPos = skeletonData->getFirstUserHandPos(first_imu_data);
	}

	/* new touch update */
	if (bTouchMode)
	{
		touch.update();	
	}
	/* virtual window object moving using the spatial touch */
	if (bVirtualMode)
	{
		if (touch.warpedTouchPoint.size() != 0)
		{
			ofRectangle object_region[6];
			for (int i = 1; i < 6; i++)
			{
				if (projMeta->for_mm_image[i].dirty)
				{
					object_region[i].set(
						projMeta->for_mm_image[i].fixed_point.x,
						projMeta->for_mm_image[i].fixed_point.y,
						projMeta->for_mm_image[i].img.getWidth(),
						projMeta->for_mm_image[i].img.getHeight());
					if (object_region[i].inside(touch.warpedTouchPoint[0].x, touch.warpedTouchPoint[0].y))
					{

						touch_determine_cnt++;
						if (touch_determine_cnt >= 5)
						{
							touch_determine_cnt = 5;
							touch_determine = i;
						}
					}
				}
			}
		}
		else {
			touch_determine_cnt -= 1;
			if (touch_determine_cnt < 0)
			{
				touch_determine_cnt = 0;
				touch_determine = -3;
			}
		}
		if (touch_determine > 0)
		{
				projMeta->setMovedObjectCoord(
				touch_determine,
				ofPoint(
					touch.warpedTouchPoint[0].x - projMeta->for_mm_image[touch_determine].img.getWidth() / 2,
					touch.warpedTouchPoint[0].y - projMeta->for_mm_image[touch_determine].img.getHeight() / 2
				)
			);
		}
	}
	/* Spatial UI touch using the spatial touch */
	if (bUIMode)
	{
		if (touch.warpedTouchPoint.size() != 0)
		{
			for (int i = 0; i < (this->imgWarpManager.mobileNum-1); i++)
			{
				if (this->b_warpImgDisplay)
				{		
					UI_region[i].set(
						this->imgWarpManager.mobileVertices.at(i).vertices[0].x,
						this->imgWarpManager.mobileVertices.at(i).vertices[0].y,
						abs(this->imgWarpManager.mobileVertices.at(i).vertices[2].x - this->imgWarpManager.mobileVertices.at(i).vertices[0].x),
						abs(this->imgWarpManager.mobileVertices.at(i).vertices[2].y - this->imgWarpManager.mobileVertices.at(i).vertices[0].y));
					if (UI_region[i].inside(touch.warpedTouchPoint[0].x, touch.warpedTouchPoint[0].y))
					{

						UI_touch_determine_cnt++;
						if (UI_touch_determine_cnt >= 10)
						{
							UI_touch_determine_cnt = 10;
							UI_touch_determine = i;
						}
					}
				}
			}
		}
		else {
			UI_touch_determine_cnt -= 1;
			if (UI_touch_determine_cnt < 0)
			{
				UI_touch_determine_cnt = 0;
				UI_touch_determine = -1;
			}
		}
		if (UI_touch_determine >= 0)
		{
			bUIMode = false;
			this->b_warpImgDisplay = false;
			int caseNum = UI_touch_determine;
			UI_touch_determine_cnt = 0;
			UI_touch_determine = -1;
			switch (caseNum +1) {
			case 1: keyPressed('v'); break;
			case 2: keyPressed('1'); break;
			//case 3: keyPressed('4'); break;
			//case 4:  keyPressed('m'); break;
			}	
		}
	}

}

//--------------------------------------------------------------
void ofApp::draw() {
	if (/*bMappingMode ||*/ home)
	{
		ofBackground(0);
		//sceneManager.currentScene->drawBackground();
	}
	/* test Virtual Current scene Draw */
	/*if (testVirtual)
	{
		sceneManager.currentScene->draw(true);
	}*/

	/* Map mode : mobile commend */
	if (map)
	{
		ofSetColor(255, 255, 255);
		mConstruct.setRec(ofPoint(235, 750), 100, 100);
		ofDrawRectangle(mConstruct.getRec());

		mSelect.setRec(ofPoint(435, 750), 100, 100);
		ofDrawRectangle(mSelect.getRec());

		if (mobileCommand == "mapping")
			b_Mapping = !b_Mapping;
		else
			cout << "Wrong command" << endl;
	}
	/* Table top GUI Draw */
	if (bUIMode) 
	{
		sceneManager.currentScene->draw(true);
		/* Warping Draw */
		if (this->b_warpImgDisplay)
		{
			this->imgWarpManager.draw();
		}
	}
	/* Virtual window mode */
	if (bVirtualMode)
	{
		sceneManager.currentScene->draw(true);
		iPhone->draw();
		skeletonData->draw();
		if (bkgd_flag)
			projMeta->draw(headPos, secondUserHandVec);
		
		if (this->b_warpImgDisplay)
		{
			this->imgVirtual.draw();
		}
	}
	
	/* Map scanning Draw */
	if (bMappingMode)
	{
		mapScanning.Draw();
	}
	/* image viewer Draw */
	if (bDisplayMode)
	{
		sceneManager.currentScene->draw(true);
		if (this->b_warpImgDisplay)
		{
			this->imgViewer.draw();
		}
	}

	/* Mobile commend one, two, three, four */
	if (bReadyToReceive)
	{
		for (int i = 0; i < tcpText.getNumClients(); i++)
		{
			if (tcpText.isClientConnected(i))
			{
				mobileCommand = tcpText.receive(i);
			}
			if (mobileCommand == "d")
			{
				KeyDown();
			}
			else if (mobileCommand == "u")
			{
				KeyUp();
			}
			else if (mobileCommand == "r")
			{
				KeyRight();
			}
			else if (mobileCommand == "l")
			{
				KeyLeft();
			}
			else if (mobileCommand == "m")
			{
				keyPressed('m');
			}
			else if (mobileCommand == "mapping")
			{
				b_Mapping = true;
				bReadyToReceive = false;
			}
			else if (mobileCommand == "one")
			{
				keyPressed('1');
			}
			else if (mobileCommand == "two")
			{
				keyPressed('2');
			}
			else if (mobileCommand == "three")
			{
				keyPressed('3');
			}
			else if (mobileCommand == "four")
			{
				keyPressed('4');
			}
			else if (mobileCommand == "five")
			{
				keyPressed('5');
			}
			else if (mobileCommand == "six")
			{
				keyPressed('6');
			}
			else if (mobileCommand == "v")
			{
				keyPressed('v');
			}
			else if (mobileCommand == "touch")
			{
				keyPressed('t');
			}
			else if (mobileCommand == "train")
			{
				keyPressed('r');
			}

			else if (mobileCommand == "debug") 
			{
				keyPressed('o');
			}

			else if (mobileCommand == "home")
			{
				keyPressed('x');
				//keyPressed('o');
			}			

			else if (mobileCommand == "uimode")
			{
				keyPressed('u');
			}


		}
	}

	/* Virtual window touch test */
	/*if (testVirtual)
	{
		iPhone->draw();
		skeletonData->draw();

		if (bkgd_flag)
			projMeta->draw(headPos, secondUserHandVec);

		projMeta->drawVirtualObjects();
	}*/
	/* Sptial touch Draw */
	if (bTouchMode)
	{
		touch.draw();
		showSUIInfo();
	}
}

void ofApp::exit() {
	if (iPhone)
	{
		iPhone->exit();
		delete iPhone;
		iPhone = nullptr;
	}

	if (projMeta)
	{
		delete projMeta;
		projMeta = nullptr;
	}

	if (skeletonData)
	{
		skeletonData->exit();
		delete skeletonData;
		skeletonData = nullptr;
	}
	else
		delete kinect;

	/* new touch exit */
	mapScanning.Exit();
	ptSystem.exit();
	sceneManager.save();

	//delete xtion;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
	if (sceneManager.currentScene->isTouchable)
	{
		if (key == 't' || key == 'T')
		{
			touch.clearDT();
			bTouchMode = !bTouchMode;
			touch.bTouchStart = false;
			if (!bTouchMode)
			{
				touch.bDrawTouchDebugView = false;
				sceneManager.save();
				touch.clearDT();
			}
			else {
				touch.parameterSetup(touch.minT, touch.maxT, touch.touchPointOffset.x, touch.touchPointOffset.y, touch.touchMinArea, touch.touchMaxArea);
			}
		}
	} 
	if (bTouchMode)
	{
		touch.keyPressed(key);
		
		if (key == 'o' || key == 'O')
		{
			touch.bDrawTouchDebugView = !touch.bDrawTouchDebugView;
		}
		if (key == 'r' || key == 'R') {
			//touch.clearDT();
			touch.refresh();
		}
	}
	if (bVirtualMode)
	{
		skeletonData->keyPressed(key);
		projMeta->keyPressed(key);
	}
	if (bMappingMode)
	{
		if (key == ' ') {
			mapScanning.Refresh();
			ptSystem.update(PAN_DEFAULT, 127);
			//b_Mapping = !b_Mapping;
			b_Mapping = true;
		}
	}
	if (key == 'z' || key == 'Z')
	{
		sceneManager.currentScene->isTouchable = !sceneManager.currentScene->isTouchable;
		if (!sceneManager.currentScene->isTouchable)
		{
			touch.clearDT();
			touch.bDrawTouchDebugView = false;
		}
	}
	if (key == 'u' || key == 'U')
	{
		bMappingMode = false;
		bDisplayMode = false;
		bVirtualMode = false;
		//testVirtual = false;
		bUIMode = !bUIMode;
		this->b_warpImgDisplay = bUIMode;
		this->imgWarpingStart = bUIMode;
		touch.bDrawTouchDebugView = false;
		touch.clearDT();
		display_img_num = 6;
		sceneManager.changeCurrentScene(display_img_num);
		ptSystem.bSceneChange = true;
		if (sceneManager.currentScene->isTouchable)
		{
			touch.setCurrentScene(sceneManager.currentScene);
		}
	}
	//if (key == 'c' || key == 'C')
	//{
	//	ofVec3f v1((float)(ofGetWindowWidth() / 2 - projMeta->for_mm_image[1].img.getWidth() / 2), (float)(ofGetWindowHeight() / 2 - projMeta->for_mm_image[1].img.getWidth() / 2), 0);
	//	ofVec3f v2((float)(300), (float)(500), 0);
	//	bMappingMode = false;
	//	//bTouchMode = false;
	//	bDisplayMode = false;
	//	this->b_warpImgDisplay = false;
	//	this->imgWarpingStart = false;
	//	this->b_warpVideoDisplay = false;
	//	this->videoWarpingStart = false;
	//	testVirtual = !testVirtual;
	//	//touch.init(kinect);
	//	projMeta->for_mm_image[1].dirty = true;
	//	projMeta->for_mm_image[1].fixed_point.set(v1);
	//	projMeta->for_mm_image[1].yaw = 0;
	//	projMeta->for_mm_image[2].dirty = true;
	//	projMeta->for_mm_image[2].fixed_point.set(v2);
	//	projMeta->for_mm_image[2].yaw = 0;
	//	display_img_num = 5;
	//	sceneManager.changeCurrentScene(display_img_num);
	//	if (sceneManager.currentScene->isTouchable)
	//	{
	//		touch.setCurrentScene(sceneManager.currentScene);
	//	}
	//	//cout << "projMeta->for_mm_image[1].fixed_point = " << projMeta->for_mm_image[1].fixed_point.x << " , " << projMeta->for_mm_image[1].fixed_point.y << endl;
	//	//ptSystem.bSceneChange = true;
	//	//ptSystem.panAngle = sceneManager.scenes[sceneManager.currentSceneIndex].panAngle;
	//	//ptSystem.tiltAngle = sceneManager.scenes[sceneManager.currentSceneIndex].tiltAngle;
	//	//cout << "Pan, Tilt : " << ptSystem.panAngle << " , " << ptSystem.tiltAngle << endl;
	//}
	
	if (key == 'v' || key == 'V')
	{
		touch.clearDT();
		touch.bDrawTouchDebugView = false;
		bMappingMode = false;
		bUIMode = false;
		bDisplayMode = false;
		bVirtualMode = true;
		this->b_warpImgDisplay = bVirtualMode;
		this->imgWarpingStart = bVirtualMode;
		display_img_num = 7;
		sceneManager.changeCurrentScene(display_img_num);
		if (sceneManager.currentScene->isTouchable)
		{
			touch.setCurrentScene(sceneManager.currentScene);
		}
		ptSystem.bSceneChange = true;
	}
	if (key == 's' || key == 'S') {
		mapScanning.SaveFile("");
		sceneManager.save();
		if (this->imgWarpingStart)
		{
			this->imgWarpManager.xmlSave();
			this->imgViewer.xmlSave();
			this->imgVirtual.xmlSave();
		}
	}
	if (key == 'f' || key == 'F') {
		ofToggleFullscreen();
	}
	if (key == 'd' || key == 'D') {
		bDrawDragPoints = !bDrawDragPoints;
		sceneManager.setDrawDragPoints(bDrawDragPoints);
		//this->imgWarpManager.showDragPoints();
		this->imgWarpManager.bDrawDragPoints = !this->imgWarpManager.bDrawDragPoints;
		this->imgViewer.bDrawDragPoints = !this->imgViewer.bDrawDragPoints;
		this->imgVirtual.bDrawDragPoints = !this->imgVirtual.bDrawDragPoints;
	}
	if (key == 'm') {
		//bMappingMode = !bMappingMode;
		bMappingMode = true;
		bDisplayMode = false;
		bUIMode = false;
		bVirtualMode = false;
		bTouchMode = false;
		touch.clearDT();
		touch.bDrawTouchDebugView = false;
		this->b_warpImgDisplay = false;
		this->imgWarpingStart = false;
	}
	if (key == 'M')
	{
		bMappingMode = false;
	}
	/* image viwer key press */
	if (key >= '1' && key <= '6')
	{
		bMappingMode = false;
		bTouchMode = false;
		bUIMode = false;
		touch.clearDT();
		touch.bDrawTouchDebugView = false;
		bDisplayMode = true;
		bVirtualMode = false;
		this->b_warpImgDisplay = bDisplayMode;
		this->imgWarpingStart = this->b_warpImgDisplay;
		//keyPressed('w');
		display_img_num = key - '1';
		sceneManager.changeCurrentScene(display_img_num);
		ptSystem.bSceneChange = true;
	}

	/*warping key*/
	if (key == 'w' || key == 'W')
	{
		touch.clearDT();
		touch.bDrawTouchDebugView = false;
		//bMappingMode = false;
		//bDisplayMode = false;
		//bVirtualMode = false;
		this->imgWarpingStart = !this->imgWarpingStart;
		this->b_warpImgDisplay = !this->b_warpImgDisplay;
	}

	if (key == 'x' || key == 'X') {
		bMappingMode = false;
		bDisplayMode = false;
		bUIMode = false;
		bVirtualMode = false;
		bTouchMode = false;
		touch.clearDT();
		touch.bDrawTouchDebugView = false;
		this->b_warpImgDisplay = false;
		this->imgWarpingStart = false;
	}
	//if (key == 'x' || key == 'X')
	//{
	//	bTouchMode = false;
	//	touch.clearDT();
	//	touch.bDrawTouchDebugView = false;
	//	bMappingMode = false;
	//	bUIMode = false;
	//	bDisplayMode = false;
	//	bVirtualMode = false;
	//	this->videoWarpingStart = !this->videoWarpingStart;
	//	this->b_warpVideoDisplay = !this->b_warpVideoDisplay;
	////	this->videoWarpManager.showDragPoints();
	////	this->videoWarpManager.draw();
	//}

	if (key == OF_KEY_UP)			KeyUp();
	if (key == OF_KEY_DOWN)			KeyDown();
	if (key == OF_KEY_RIGHT)		KeyRight();
	if (key == OF_KEY_LEFT)			KeyLeft();

	cout << "bMappingMode = " << bMappingMode << ", bDisplayMode = " << bDisplayMode << ", bTouchMode = " << bTouchMode << ", bVirtualWinodw = " << bVirtualMode << ", bUIMode = " << bUIMode << ", bWarpMode = " << this->b_warpImgDisplay << endl;
	cout << "touch.bDrawTouchPoint = " << touch.bDrawTouchPoint << ", touch.bDrawTouchDebugView = " << touch.bDrawTouchDebugView << endl;
	cout << "isTouchable = " << sceneManager.currentScene->isTouchable << endl;

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	// drag point ���?���̸� ����Ʈ ����
	if (sceneManager.currentScene->bDrawDragPoints)
		sceneManager.currentScene->mouseMoved(x, y);

	// ��ġ ��忡��?��ġ ���� �����?�信 drag point ����
	if (sceneManager.currentScene->isTouchable && touch.bDrawTouchDebugView)
		touch.mouseMoved(x, y);

	// warping
	if (this->imgWarpingStart)
	{
		this->imgWarpManager.mouseMoved(x, y);
		this->imgViewer.mouseMoved(x,y);
		this->imgVirtual.mouseMoved(x, y);
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	// drag point ���?���̸� ����Ʈ ����
	if (sceneManager.currentScene->bDrawDragPoints)
		sceneManager.currentScene->mouseDragged(x, y, button);

	// ��ġ ��忡��?��ġ ���� �����?�信 drag point ����
	if (sceneManager.currentScene->isTouchable && touch.bDrawTouchDebugView)
		touch.mouseDragged(x, y, button);

	// warping
	if (this->imgWarpingStart)
	{
		this->imgWarpManager.mouseDragged(x, y);
		this->imgViewer.mouseDragged(x, y);
		this->imgVirtual.mouseDragged(x, y);
	}
	/* ���� ��ġ�� ���ؼ� */
	if (touch_determine > 0) {
		projMeta->setMovedObjectCoord(
			touch_determine,
			ofPoint(
				x - projMeta->for_mm_image[touch_determine].img.getWidth() / 2,
				y - projMeta->for_mm_image[touch_determine].img.getHeight() / 2
			)
		);
	}

	/* Virtual window */
	if ((bVirtualMode && !bDrawDragPoints) || testVirtual)
	{
		int x_offset = x - previous_x;
		int y_offset = y - previous_y;

		if (button == 0)
		{
			float x_scale = (0.75f * 360) / ofGetWidth();
			float y_scale = (0.75f * 360) / ofGetHeight();

			float panVal = x_offset * x_scale;
			float tiltVal = -y_offset * y_scale;

			viewer.rotate(panVal, ofVec3f(0, -1 * abs(viewer.getPosition().y), 0));
			viewer.rotate(tiltVal, viewer.getXAxis());
		}
		else if (button == 2)
		{
			float x_scale = ((float)1 * 360 / ofGetWidth());
			float y_scale = ((float)1000 / ofGetHeight());

			float elevationVal = -y_offset * y_scale;
			viewer.move(0, elevationVal, 0);
		}
		previous_x = x;
		previous_y = y;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	// drag point ���?���̸� ����Ʈ ����
	if (sceneManager.currentScene->bDrawDragPoints)
		sceneManager.currentScene->mousePressed(x, y, button);

	// ��ġ ��忡��?��ġ ���� �����?�信 drag point ����
	if (sceneManager.currentScene->isTouchable && touch.bDrawTouchDebugView)
		touch.mousePressed(x, y, button);

	// warping
	if (this->imgWarpingStart)
	{
		this->imgWarpManager.mousePressed(x, y);
		this->imgViewer.mousePressed(x, y);
		this->imgVirtual.mousePressed(x, y);
	}
	/* ���� ��ġ�� ���ؼ� */
	ofRectangle object_region[6];

	for (int i = 1; i < 6; i++) {

		if (projMeta->for_mm_image[i].dirty) {

			object_region[i].set(
				projMeta->for_mm_image[i].fixed_point.x,
				projMeta->for_mm_image[i].fixed_point.y,
				projMeta->for_mm_image[i].img.getWidth(),
				projMeta->for_mm_image[i].img.getHeight());

			if (object_region[i].inside(x, y)) {
				touch_determine = i;
			}
		}
	}

	/* Virtual window */
	if ((bVirtualMode && !bDrawDragPoints) || testVirtual)
	{
		static auto last = ofGetElapsedTimeMillis();
		auto now = ofGetElapsedTimeMillis();
		if (button == 0)
		{
			if (now - last < 500)
			{
				viewer.rotate(-viewer.getOrientationEuler().x, viewer.getXAxis());
			}
			last = now;
		}
		previous_x = x;
		previous_y = y;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	// drag point ���?���̸� ����Ʈ ����
	if (sceneManager.currentScene->bDrawDragPoints)
		sceneManager.currentScene->mouseReleased(x, y, button);

	// ��ġ ��忡��?��ġ ���� �����?�信 drag point ����
	if (sceneManager.currentScene->isTouchable) {
		touch.mouseReleased(x, y, button);
	}

	// warping
	if (this->imgWarpingStart)
	{
		this->imgWarpManager.mouseReleased(x, y);
		this->imgViewer.mouseReleased(x, y);
		this->imgVirtual.mouseReleased(x, y);
	}
	touch_determine = -1;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

void ofApp::KeyUp()
{
	ptSystem.tiltAngle += 1;

	ptSystem.tiltAngle = ptSystem.tiltAngle > ptSystem.Tilt_max ? ptSystem.Tilt_max : ptSystem.tiltAngle;
	if (bUIMode || bDisplayMode || bVirtualMode)
	{
		sceneManager.scenes[sceneManager.currentSceneIndex].tiltAngle = ptSystem.tiltAngle;
	}
	ptSystem.bSceneChange = true;
	showPanTiltAngle();
}

void ofApp::KeyDown()
{
	ptSystem.tiltAngle -= 1;

	ptSystem.tiltAngle = ptSystem.tiltAngle < ptSystem.Tilt_min ? ptSystem.Tilt_min : ptSystem.tiltAngle;
	if (bUIMode || bDisplayMode || bVirtualMode)
	{
		sceneManager.scenes[sceneManager.currentSceneIndex].tiltAngle = ptSystem.tiltAngle;
	}
	ptSystem.bSceneChange = true;
	showPanTiltAngle();
}

void ofApp::KeyRight()
{
	ptSystem.panAngle += 2;

	ptSystem.panAngle = ptSystem.panAngle > ptSystem.Pan_max ? ptSystem.Pan_max : ptSystem.panAngle;
	if (bUIMode || bDisplayMode || bVirtualMode)
	{
		sceneManager.scenes[sceneManager.currentSceneIndex].panAngle = ptSystem.panAngle;
	}

	ptSystem.bSceneChange = true;
	showPanTiltAngle();
}

void ofApp::KeyLeft()
{
	ptSystem.panAngle -= 2;

	ptSystem.panAngle = ptSystem.panAngle < ptSystem.Pan_min ? ptSystem.Pan_min : ptSystem.panAngle;
	if (bUIMode || bDisplayMode || bVirtualMode)
	{
		sceneManager.scenes[sceneManager.currentSceneIndex].panAngle = ptSystem.panAngle;
	}

	ptSystem.bSceneChange = true;
	showPanTiltAngle();
}

void ofApp::receiveCommand(string command)
{
	int key;
	for (int i = 0; i < commandMobile.size(); i++)
	{
		if (command == commandMobile[i])
			keyPressed(key);
	}
}

void ofApp::receiveImage(ofImage &image, int numBytes)
{
	unsigned char* buffer = new unsigned char[numBytes];
	int recd = numBytes;
	int totalReceived = 0;

	while (totalReceived != numBytes)
	{
		int ret = tcpImage.receiveRawBytes(tcpImage.getLastID() - 1, (char*)&buffer[totalReceived], recd);
		if (ret < 0)
		{
			continue;
		}
		recd -= ret;
		totalReceived += ret;
		cout << totalReceived << endl;
	}
	image.setFromPixels(buffer, 500, 500, OF_IMAGE_COLOR);

	delete[] buffer;
}

void ofApp::sendFile(ofFile file, int fileBytesToSend)
{
	char* fileSize;
	int totalBytesSent = 0;
	tcpFile.sendRawBytes(tcpFile.getLastID() - 1, (char*)&file.getFileBuffer()[totalBytesSent], fileBytesToSend);
}

void ofApp::showPanTiltAngle() {
	cout << "Pan Angle: " << ptSystem.panAngle << endl;
	cout << "Tilt Angle: " << ptSystem.tiltAngle << endl << endl;
}

void ofApp::showVirutalWindowInfo() {

	ofDrawBitmapString("Camera Position X: " + ofToString(first_imu_data.getCamPosX()), 200, 200);
	ofDrawBitmapString("Camera Position Y: " + ofToString(first_imu_data.getCamPosY()), 200, 220);
	ofDrawBitmapString("Camera Position Z: " + ofToString(first_imu_data.getCamPosZ()), 200, 240);

	ofDrawBitmapString("Pitch: " + ofToString(first_imu_data.getPitch()), 200, 280);
	ofDrawBitmapString("Roll: " + ofToString(first_imu_data.getRoll()), 200, 300);
	ofDrawBitmapString("Yaw: " + ofToString(first_imu_data.getYaw()), 200, 320);

	ofDrawBitmapString("User Head Depth Position X: " + ofToString(headDepthPos.x), 200, 360);
	ofDrawBitmapString("User Head Depth Position Y: " + ofToString(headDepthPos.y), 200, 380);

	ofDrawBitmapString("User Head Projection Position X: " + ofToString(headPos.x), 200, 420);
	ofDrawBitmapString("User Head Projection Position Y: " + ofToString(headPos.y), 200, 440);

	ofDrawBitmapString("Spatial Touch interface : " + ofToString(touch.bTouchStart), 200, 180);

	if (touch.bTouchStart)
	{
		ofDrawBitmapString("Selected object: " + ofToString(touch_determine), 200, 460);
		ofDrawBitmapString("Select counting: " + ofToString(touch_determine_cnt), 200, 480);
	}
}

void ofApp::showSUIInfo() {

	ofDrawBitmapString("Spatial Touch Start : " + ofToString(touch.bTouchStart), 200, 800);
	if (touch.warpedTouchPoint.size() != 0)
	{
		ofDrawBitmapString("User Hand Depth Position X: " + ofToString(touch.warpedTouchPoint[0].x), 200,820);
		ofDrawBitmapString("User Hand Depth Position Y: " + ofToString(touch.warpedTouchPoint[0].y), 200, 840);
	}
	ofDrawBitmapString("User Hand Depth Position X offset: " + ofToString(touch.touchPointOffset.x),200, 860);
	ofDrawBitmapString("User Hand Depth Position Y offset: " + ofToString(touch.touchPointOffset.y),200, 880);
	ofDrawBitmapString("Depth touch Min: " + ofToString(touch.minT), 200,900);
	ofDrawBitmapString("Depth touch Max: " + ofToString(touch.maxT), 200, 920);
	ofDrawBitmapString("Depth touch Area Min: " + ofToString(touch.touchMinArea), 200, 940);
	ofDrawBitmapString("Depth touch Area Max: " + ofToString(touch.touchMaxArea), 200, 960);
	if (touch.bTouchStart && bUIMode)
	{
		ofDrawBitmapString("Selected object: " + ofToString(UI_touch_determine), 200, 980);
		ofDrawBitmapString("Select counting: " + ofToString(UI_touch_determine_cnt), 200, 1000);
	}
	if (touch.bTouchStart && bVirtualMode)
	{
		ofDrawBitmapString("Selected object: " + ofToString(touch_determine), 200,980);
		ofDrawBitmapString("Select counting: " + ofToString(touch_determine_cnt), 200, 1000);
	}
}