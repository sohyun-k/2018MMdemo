#pragma once

#include "ofApp.h"
#define IMAGE_SIZE 750000
#define IMAGE_WIDTH 200
#define IMAGE_HEIGHT 200

//--------------------------------------------------------------
void ofApp::setup() {

	font.load("verdana.ttf", 30);

	/* 초기 메뉴 */
	home = true;
	vWindow = false;
	//map = false;
	bTouchMode = false;
	bMappingMode = false;
	bDisplayMode = false;
	bUIMode = false;
	bDrawDragPoints = true;
	bDrawContents = true;
	//this->b_warpImgDisplay = false;

	/* tcp network setup */
	tcpServer.setup(19132, true);
	tcpFile.setup(19133, true);
	tcpImage.setup(19134, true);
	tcpText.setup(19135, true);

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
	this->videoWarpManager.testSetup(this->video_test_path);

	display_img_num = 0;
	touch_determine_cnt = 0;

	/* new touch setup */
	touch.init(kinect);
	touch.set();
	touch.parameterSetup(touch.minT, touch.maxT, touch.touchPointOffset.x, touch.touchPointOffset.y);
	
	cout << "bMappingMode = " << bMappingMode << ", bDisplayMode = " << bDisplayMode << ", bTouchMode = " << bTouchMode << ", bVirtualWinodw = " << vWindow << endl;
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
	if (vWindow)
	{
		/*if (sceneManager.currentScene->isTouchable && touch.bTouchStart)
		{
			touch.bTouchStart = false;
		}*/

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
	if (vWindow)
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

}

//--------------------------------------------------------------
void ofApp::draw() {
	if (bMappingMode || home)
	{
		ofBackground(0);
		//sceneManager.currentScene->drawBackground();
	}
	/* test Virtual Current scene Draw */
	/*if (testVirtual)
	{
		sceneManager.currentScene->draw(true);
	}*/
	/* Warping Draw */
	if (this->b_warpImgDisplay)
	{
		this->imgWarpManager.draw();
	}
	if (this->b_warpVideoDisplay)
	{
		this->videoWarpManager.draw();
	}

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
	}
	/* Virtual window mode */
	if (vWindow)
	{
		sceneManager.currentScene->draw(true);
		iPhone->draw();
		skeletonData->draw();

		if (bkgd_flag)
			projMeta->draw(headPos, secondUserHandVec);
	}

	/* Map scanning Draw */
	if (bMappingMode)
	{
		mapScanning.Draw();
	}
	/* image viewer Draw */
	if (bDisplayMode)
	{
		if (display_img_num == 1) {
			sceneManager.currentScene->drawVideo(true);
		}
		else {
			sceneManager.currentScene->draw(true);
		}
	}

	/* text 전송 및 이미지 전송 부분 */
	/* 이미지 전송 후 draw하는 부분은 one, two, three, four 부분 */
	for (int i = 0; i < tcpText.getNumClients(); i++)
	{
		if (tcpText.isClientConnected(i))
		{
			mobileCommand = tcpText.receive(i);
			/*if (bReadyToReceive) {
				mobileCommand = tcpText.receive(i);
				//receiveImage(recvImage, IMAGE_SIZE);
				bReadyToReceive = false;
			}*/
		}
		if (mobileCommand == "d")
		{
			KeyDown();
			bReadyToReceive = !bReadyToReceive;
		}
		else if (mobileCommand == "u")
		{
			KeyUp();
			bReadyToReceive = !bReadyToReceive;
		}
		else if (mobileCommand == "r")
		{
			KeyRight();
			bReadyToReceive = !bReadyToReceive;
		}
		else if (mobileCommand == "l")
		{
			KeyLeft();
			bReadyToReceive = !bReadyToReceive;
		}
		else if (mobileCommand == "m")
		{
			keyPressed('m');
		}
		else if (mobileCommand == "mapping")
		{
			b_Mapping = true;

		}
		/*else if (mobileCommand == "sendme")
		{
			char* fileSize;
			tcpText.send(tcpText.getLastID(), itoa(sendingFile.getSize(), fileSize, 10));
			sendFile(sendingFile, sendingFile.getSize());
			cout << "Send point cloud data complete!" << endl;
			bReadyToReceive = !bReadyToReceive;
		}*/
		else if (mobileCommand == "one")
		{
			backgroundA = recvImage;
			backgroundA.saveImage("scene1_ver2.jpg", OF_IMAGE_QUALITY_HIGH);
			//backgroundA.draw(10, 50, IMAGE_WIDTH, IMAGE_HEIGHT);
			keyPressed('1');
			bReadyToReceive = true;
		}
		else if (mobileCommand == "two")
		{
			keyPressed('2');
			bReadyToReceive = true;
		}
		else if (mobileCommand == "three")
		{
			backgroundA = recvImage;
			backgroundA.saveImage("scene_2.jpg", OF_IMAGE_QUALITY_HIGH);
			//backgroundA.draw(10, 50, IMAGE_WIDTH, IMAGE_HEIGHT);
			keyPressed('3');
			bReadyToReceive = true;
		}
		else if (mobileCommand == "four")
		{
			backgroundA = recvImage;
			backgroundA.saveImage("scene_3.jpg", OF_IMAGE_QUALITY_HIGH);
			//backgroundA.draw(10, 50, IMAGE_WIDTH, IMAGE_HEIGHT);
			keyPressed('4');
			bReadyToReceive = true;
		}
		else if (mobileCommand == "v")
		{
			keyPressed('v');
			bReadyToReceive = true;
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
	/* Depth touch Draw */
	if (bTouchMode)
	{
		touch.draw();
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
	
	if (key == 't' || key == 'T')
	{
		touch.clearDT();
		bTouchMode = !bTouchMode;
		if (!bTouchMode)
		{
			touch.bDrawTouchDebugView = false;
			sceneManager.save();
			//touch.bTouchStart = false;
		}
		else {
			touch.parameterSetup(touch.minT, touch.maxT, touch.touchPointOffset.x, touch.touchPointOffset.y);
		}
	}
	
	if (bTouchMode)
	{
		touch.keyPressed(key);
		
		if (key == 'o' || key == 'O')
		{
			touch.bDrawTouchDebugView = !touch.bDrawTouchDebugView;
		}
		if (key == '5') {
			touch.refresh();
		}
	}
	if (vWindow)
	{
		skeletonData->keyPressed(key);
		projMeta->keyPressed(key);
	}
	if (bMappingMode)
	{
		//bDrawContents = false;
		//bMappingMode = !bMappingMode;
		/*
		bTouchMode = false;
		bDisplayMode = false;
		vWindow = false;
		bUIMode = false;
		this->b_warpImgDisplay = false;
		this->imgWarpingStart = false;
		this->b_warpVideoDisplay = false;
		this->videoWarpingStart = false;*/
		//kinect->setFlipVertical(true);
		//kinect->setFlipHorizontal(false);
		if (key == ' ') {
			ptSystem.update(PAN_DEFAULT, 127);
			b_Mapping = !b_Mapping;
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
		vWindow = false;
		//testVirtual = false;
		bUIMode = !bUIMode;
		touch.bDrawTouchDebugView = false;
		touch.clearDT();
		display_img_num = 4;
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
		//bTouchMode = false;
		bUIMode = false;
		bDisplayMode = false;
		vWindow = true;
		this->b_warpImgDisplay = false;
		this->imgWarpingStart = false;
		this->b_warpVideoDisplay = false;
		this->videoWarpingStart = false;
		//touch.init(kinect);
		display_img_num = 5;
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
			this->imgWarpManager.xmlSave();
		if (this->videoWarpingStart)
			this->videoWarpManager.xmlSave();
	}
	if (key == 'f' || key == 'F') {
		ofToggleFullscreen();
	}
	if (key == 'd' || key == 'D') {
		bDrawDragPoints = !bDrawDragPoints;
		sceneManager.setDrawDragPoints(bDrawDragPoints);
	}
	if (key == 'm' || key == 'M') {
		bMappingMode = !bMappingMode;
		bDisplayMode = false;
		bUIMode = false;
		vWindow = false;
		bTouchMode = false;
		touch.clearDT();
		touch.bDrawTouchDebugView = false;
		this->b_warpImgDisplay = false;
		this->imgWarpingStart = false;
		this->b_warpVideoDisplay = false;
		this->videoWarpingStart = false;
	}

	/* image viwer key press */
	if (key >= '1' && key <= '4')
	{
		bMappingMode = false;
		bTouchMode = false;
		touch.clearDT();
		touch.bDrawTouchDebugView = false;
		bDisplayMode = true;
		vWindow = false;
		this->b_warpImgDisplay = false;
		this->imgWarpingStart = false;
		this->b_warpVideoDisplay = false;
		this->videoWarpingStart = false;
		display_img_num = key - '1';
		sceneManager.changeCurrentScene(display_img_num);
		if (display_img_num==1)
		{
			sceneManager.currentScene->loadVideoContents();
		}
		ptSystem.bSceneChange = true;
	}

	/*warping key*/
	if (key == 'w' || key == 'W')
	{
		bTouchMode = false;
		touch.clearDT();
		touch.bDrawTouchDebugView = false;
		bMappingMode = false;
		bUIMode = false;
		bDisplayMode = false;
		vWindow = false;
		this->imgWarpingStart = !this->imgWarpingStart;
		this->b_warpImgDisplay = !this->b_warpImgDisplay;
		this->imgWarpManager.showDragPoints();
		this->imgWarpManager.draw();
	}
	if (key == 'x' || key == 'X')
	{
		bTouchMode = false;
		touch.clearDT();
		touch.bDrawTouchDebugView = false;
		bMappingMode = false;
		bUIMode = false;
		bDisplayMode = false;
		vWindow = false;
		this->videoWarpingStart = !this->videoWarpingStart;
		this->b_warpVideoDisplay = !this->b_warpVideoDisplay;
		this->videoWarpManager.showDragPoints();
		this->videoWarpManager.draw();
	}

	if (key == OF_KEY_UP)			KeyUp();
	if (key == OF_KEY_DOWN)			KeyDown();
	if (key == OF_KEY_RIGHT)		KeyRight();
	if (key == OF_KEY_LEFT)			KeyLeft();

	cout << "bMappingMode = " << bMappingMode << ", bDisplayMode = " << bDisplayMode << ", bTouchMode = " << bTouchMode << ", bVirtualWinodw = " << vWindow << endl;
	cout << "touch.bDrawTouchPoint = " << touch.bDrawTouchPoint << ", touch.bDrawTouchDebugView = " << touch.bDrawTouchDebugView << endl;
	cout << "isTouchable = " << sceneManager.currentScene->isTouchable << endl;

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	// drag point 출력 중이면 포인트 조절
	if (sceneManager.currentScene->bDrawDragPoints)
		sceneManager.currentScene->mouseMoved(x, y);

	// 터치 모드에서 터치 영역 디버그 뷰에 drag point 조절
	if (sceneManager.currentScene->isTouchable && touch.bDrawTouchDebugView)
		touch.mouseMoved(x, y);

	// warping
	if (this->imgWarpingStart)
		this->imgWarpManager.mouseMoved(x, y);
	if (this->videoWarpingStart)
		this->videoWarpManager.mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	// drag point 출력 중이면 포인트 조절
	if (sceneManager.currentScene->bDrawDragPoints)
		sceneManager.currentScene->mouseDragged(x, y, button);

	// 터치 모드에서 터치 영역 디버그 뷰에 drag point 조절
	if (sceneManager.currentScene->isTouchable && touch.bDrawTouchDebugView)
		touch.mouseDragged(x, y, button);

	// warping
	if (this->imgWarpingStart)
		this->imgWarpManager.mouseDragged(x, y);
	if (this->videoWarpingStart)
		this->videoWarpManager.mouseDragged(x, y);

	/* 뎁스 터치를 위해서 */
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
	if ((vWindow && !bDrawDragPoints) || testVirtual)
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
	// drag point 출력 중이면 포인트 조절
	if (sceneManager.currentScene->bDrawDragPoints)
		sceneManager.currentScene->mousePressed(x, y, button);

	// 터치 모드에서 터치 영역 디버그 뷰에 drag point 조절
	if (sceneManager.currentScene->isTouchable && touch.bDrawTouchDebugView)
		touch.mousePressed(x, y, button);

	// warping
	if (this->imgWarpingStart)
		this->imgWarpManager.mousePressed(x, y);
	if (this->videoWarpingStart)
		this->videoWarpManager.mousePressed(x, y);

	/* 뎁스 터치를 위해서 */
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
	if ((vWindow && !bDrawDragPoints) || testVirtual)
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
	// drag point 출력 중이면 포인트 조절
	if (sceneManager.currentScene->bDrawDragPoints)
		sceneManager.currentScene->mouseReleased(x, y, button);

	// 터치 모드에서 터치 영역 디버그 뷰에 drag point 조절
	if (sceneManager.currentScene->isTouchable) {
		touch.mouseReleased(x, y, button);
	}

	// warping
	if (this->imgWarpingStart)
		this->imgWarpManager.mouseReleased(x, y);
	if (this->videoWarpingStart)
		this->videoWarpManager.mouseReleased(x, y);

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
	if (bTouchMode || bDisplayMode || vWindow)
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
	if (bTouchMode || bDisplayMode || vWindow)
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
	if (bTouchMode || bDisplayMode || vWindow)
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
	if (bTouchMode || bDisplayMode || vWindow)
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