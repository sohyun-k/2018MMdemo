#pragma once

//#include <ofxDepthDeviceKinect1.hpp>
#include "Scene.hpp"
#include <VisionDeviceManager.hpp>
using namespace cv;


class draggableCameraVertex : public Point2f {
public:
	bool 	bBeingDragged;
	bool 	bOver;
	float 	radius;

	draggableCameraVertex() : radius(12), bOver(false), bBeingDragged(false) {}
};

class TouchHandler
{
public:
	Scene* currentScene;

	// Touch Threshold
	float	minT;
	float	maxT;
	bool bTrain;
	bool bTouchStart;
	bool bDrawTouchDebugView;
	bool bClear;
	bool bDetect;
	draggableCameraVertex	vertices[4];
	vector<Point2f>	originalTouchPoint;
	vector<Point2f> warpedTouchPoint;

	// 디버그 뷰 용 파라미터
	//------------------------------
	ofRectangle	debugViewport;
	cv::Size2f debugViewRatio;

	/************************************************************************/
	/*	Constructor																	  */
	/************************************************************************/
	TouchHandler() : bTouchStart(false), bTrain(false), bDrawTouchDebugView(false), bDetect(false), bMultifirst(false), bClear(false)
	{
		prevX, prevY = 0;
		PrevWTP = 0;
		minT = 10.0;
		maxT = 30.0;
		trainCnt = 0;
		normalizedTouchPoints.clear();
		path.clear();									// 2016-02-20
		originalTouchPoint.clear();
		warpedTouchPoint.clear();
	}

	void init() {
		bTouchStart = false;
		bTrain = false;
		bDrawTouchDebugView = false;
		bDetect = false;
		bMultifirst = false;
		bClear = false;
		prevX, prevY = 0;
		PrevWTP = 0;
		trainCnt = 0;
		normalizedTouchPoints.clear();
		path.clear();									// 2016-02-20
		originalTouchPoint.clear();
		warpedTouchPoint.clear();
	}

	void Clear() {
		bClear = true;
	}

	/************************************************************************/
	/*	Setup kinect touch													*/
	/*		- assigns "Kinect" instance
	/*		- calculates canonical vertices and input vertices(from the scene)
	/************************************************************************/

	void setup(/*VisionDeviceManager	*_Device,*/Mat depthMat, Mat colorMat, int width, int height, float minThreshold, float maxThreshold)
	{
		//Device = _Device;
		//init();
		minT = minThreshold; //15
		maxT = maxThreshold; //33

		vertices[0].x = 0;
		vertices[0].y = 0;

		vertices[1].x = width;
		vertices[1].y = 0;

		vertices[2].x = width;
		vertices[2].y = height;

		vertices[3].x = 0;
		vertices[3].y = height;


		size = Size(width, height);

		warpMat = Mat(size.height, size.width, depthMat.type());
		touchMat = Mat(size.height, size.width, depthMat.type());

		warpDepth = Mat(size.height, size.width, depthMat.type());
		warpColor = Mat(size.height, size.width, colorMat.type());

		cvDiffImg.allocate(size.width, size.height);
		diffMapChar = new unsigned char[size.width * size.height];


		for (int i = 0; i<4; ++i)
			canonicalVertices[i] = vertices[i];
		//canonicalVertices[i] = vertices[i].toCv();
		// 디버그 뷰 용 파라미터
		//------------------------------
		debugViewport.width = ofGetWidth() / 4.0;
		debugViewport.height = debugViewport.width * (480.0 / 640.0);
		debugViewport.x = ofGetWidth() - debugViewport.width - 20;
		debugViewport.y = 20;

		debugViewRatio.width = (float)debugViewport.width / (float)width;
		debugViewRatio.height = (float)debugViewport.height / (float)height;

		sceneChanged();

	}

	void setCurrentScene(Scene * current)
	{
		currentScene = current;
		sceneChanged();
	}

	/************************************************************************/
	/*	calculates touch																  */
	/************************************************************************/

	void setTrainPlane(bool tf) {
		bTrain = tf;
	}

	void update(/*VisionDeviceManager	*_Device,*/ Mat depthMat, Mat colorMat)
	{
		//Device = _Device;

		// 170808
		warpPerspective(depthMat, warpMat, homography, size, INTER_CUBIC);
		warpPerspective(depthMat, warpDepth, homography, size, INTER_CUBIC);
		warpPerspective(colorMat, warpColor, homography, size, INTER_CUBIC);
		Mat warpdepth8;
		depth162depth8Color(warpDepth, warpdepth8, 500, 1500);
		warpdepthRGB.setFromPixels(warpdepth8.data, size.width, size.height, OF_IMAGE_COLOR);

		if (bTrain) {
			ofLogNotice("KinectTouch: Training...");
			bClear = true;
			trainedMat = warpMat.clone();
			bTouchStart = true;
			trainCnt = 0;
		}
		// Touch Mode
		//--------------------------------------------------
		if (bTouchStart)
		{
			for (int i = 0; i<size.width * size.height; i++) {
				ushort v2 = trainedMat.at<ushort>(i);
				ushort v1 = warpMat.at<ushort>(i);
				ushort v3 = touchMat.at<ushort>(i);
				touchMat.at<ushort>(i) = abs(warpMat.at<ushort>(i) - trainedMat.at<ushort>(i));

				if (touchMat.at<ushort>(i) >= (ushort)minT && touchMat.at<ushort>(i) <= (ushort)maxT) {
					diffMapChar[i] = (unsigned char)255;
				}
				else
					diffMapChar[i] = (unsigned char)0;
			}

			cvDiffImg.setFromPixels(diffMapChar, size.width, size.height);
			contourFinder.findContours(cvDiffImg, 1000, size.width*size.height*0.5, 1, false, false); // touch되는 영역 contour finder
			normalizedTouchPoints.clear(); // 결과 벡터 클리어

			if (contourFinder.nBlobs > 0) // touch가 되는 순간
			{
				bDetect = true; // touch flag on
				if (contourFinder.nBlobs == 1) // single touch
				{
					p.set(contourFinder.blobs[0].centroid.x, contourFinder.blobs[0].centroid.y - (contourFinder.blobs[0].boundingRect.height / 2) + 20);
					p.x /= (double)size.width;
					p.y /= (double)size.height;
					normalizedTouchPoints.push_back(p);
					prevX = contourFinder.blobs[0].centroid.x; // multi touch에서 검출되는 포인트가 무작위로 저장되는 벡터의 순서를 정하기 위해서 그 전 포인트의 x,y값 저장
					prevY = contourFinder.blobs[0].centroid.y;
					bMultifirst = false;
					//firstAnglecnt = 0;
				}
				if (contourFinder.nBlobs == 2) // multi touch
				{
					bMultifirst = true; // 한번이라도 multi touch가 검출 되는 순간 flag on
					for (int i = 0; i<contourFinder.nBlobs; i++)
					{
						//cout << "SAVE contourFinder [" << i << "] = "<< contourFinder.blobs[i].centroid.x << " , " << contourFinder.blobs[i].centroid.y << endl;
						p.set(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y - (contourFinder.blobs[i].boundingRect.height / 2) + 20);
						p.x /= (double)size.width;
						p.y /= (double)size.height;
						normalizedTouchPoints.push_back(p);
					}
				}
			}
			else {
				bDetect = false;
				bMultifirst = false;
				//firstAnglecnt = 0;
			}

			if (bDetect && normalizedTouchPoints.size() == 0)
			{
				if (path.size() != 0) {
					path.clear();
				}
			}

			if (bClear)
			{
				path.clear();
				bClear = false;
				//init();
			}

			if (bTrain)
				bTrain = false;
		}
	}


	void depth162depth8Color(Mat& src, Mat& dest, double minv, double maxv)
	{
		Mat depthMat;
		src.convertTo(depthMat, CV_8U, 255.0 / maxv, -minv / maxv);
		applyColorMap(depthMat, dest, 2);
	}
	/************************************************************************/
	/*  Scene이 변경되었을 때 호출되어 호모그라피 재계산					  */
	/************************************************************************/
	void sceneChanged()
	{
		cameraSceneVertices[0] = vertices[1];
		cameraSceneVertices[1] = vertices[0];
		cameraSceneVertices[2] = vertices[3];
		cameraSceneVertices[3] = vertices[2];

		homography = getPerspectiveTransform(cameraSceneVertices, canonicalVertices);
	}
	void setVertices(ofPoint p[])
	{
		for (int i = 0; i<4; ++i)
		{
			vertices[i].x = p[i].x;
			vertices[i].y = p[i].y;
		}
		sceneChanged();
	}


	void draw()
	{
		// 디버그 뷰 출력
		//------------------------------
		if (bDrawTouchDebugView)
		{
			// 터치용 Debug Images
			//==============================
			warpdepthRGB.draw(debugViewport.x - debugViewport.width, debugViewport.y, debugViewport.width, debugViewport.height);
			//Device->getColorImageAligned().draw(debugViewport.x, debugViewport.y, debugViewport.width, debugViewport.height);
			cvDiffImg.draw(debugViewport.x, debugViewport.y + debugViewport.height, debugViewport.width, debugViewport.height);
			contourFinder.draw(debugViewport.x, debugViewport.y + debugViewport.height, debugViewport.width, debugViewport.height);

			//	터치용 Drag Point 그리기
			//------------------------------
			ofSetColor(ofColor::white);
			ofNoFill();
			ofBeginShape();


			for (int i = 0; i < 4; i++)
				ofVertex(vertices[i].x * debugViewRatio.width + debugViewport.x, vertices[i].y * debugViewRatio.height + debugViewport.y);
			ofEndShape(true);

			for (int i = 0; i < 4; i++) {
				ofNoFill();
				ofCircle(vertices[i].x * debugViewRatio.width + debugViewport.x, vertices[i].y * debugViewRatio.height + debugViewport.y, 10);

				if (vertices[i].bOver == true) {
					ofFill();
					ofCircle(vertices[i].x * debugViewRatio.width + debugViewport.x, vertices[i].y * debugViewRatio.height + debugViewport.y, 7);
				}
			}
		}

		// 실제로 kinect 상에서의 touch point
		originalTouchPoint.clear();
		for (int i = 0; i < normalizedTouchPoints.size(); ++i)
		{
			Point2f OTP(normalizedTouchPoints[i].x, normalizedTouchPoints[i].y);
			OTP.x *= currentScene->contentsImage.getWidth();
			OTP.y *= currentScene->contentsImage.getHeight();

			originalTouchPoint.push_back(OTP);
		}

		// warping된 view 상에서의 touch point
		warpedTouchPoint.clear();
		if (originalTouchPoint.size())
			perspectiveTransform(originalTouchPoint, warpedTouchPoint, currentScene->translate);

		// single touch
		if (bDetect&&warpedTouchPoint.size() == 1)
		{
			float distance;
			PrevWTP = warpedTouchPoint[0].x;
			path.push_back(ofPoint(warpedTouchPoint[0].x, warpedTouchPoint[0].y));

			if (path.size() > 1) {
				distance = sqrt(pow(path.at(1).x - path.at(0).x, 2) + pow(path.at(1).y - path.at(0).y, 2));
				// 흔들림 제거
				if (distance < 15)
					path.erase(path.begin());
			}

		}
		// multi-touch
		if (bDetect&&warpedTouchPoint.size() == 2)
		{
			if (PrevWTP == 0)
			{
				PrevWTP = warpedTouchPoint[0].x;
			}
			if (abs(PrevWTP - warpedTouchPoint[1].x) < abs(PrevWTP - warpedTouchPoint[0].x)) // 두번째 인식된 포인트가 저번 프레임에서 두번째 인식된 포인트와 다를시 스왑해줘야함
			{

				float firstx = warpedTouchPoint[0].x;
				float firsty = warpedTouchPoint[0].y;
				warpedTouchPoint[0].x = warpedTouchPoint[1].x;
				warpedTouchPoint[0].y = warpedTouchPoint[1].y;
				warpedTouchPoint[1].x = firstx;
				warpedTouchPoint[1].y = firsty;
				PrevWTP = warpedTouchPoint[0].x;
			}
		}


		if (bTouchStart)
		{
			ofPushStyle();
			ofFill();
			ofSetColor(ofColor::red);
			for (int i = 0; i < warpedTouchPoint.size(); ++i)
			{
				ofSetColor(ofColor::red);
				//ofCircle(warpedTouchPoint[0].x, warpedTouchPoint[0].y, 30);					// TODO:
				if (warpedTouchPoint.size() == 2) {
					ofSetColor(ofColor::white);
					//ofCircle(warpedTouchPoint[1].x, warpedTouchPoint[1].y, 30);					// TODO:	
				}
				//#endif
			}
			ofPopStyle();
		}

	}

	vector<ofPoint>& getTouchPoint() {
		vector<ofPoint> TP(warpedTouchPoint.size());
		for (int i = 0; i < warpedTouchPoint.size(); i++)
		{
			TP[i].x = warpedTouchPoint[i].x;
			TP[i].y = warpedTouchPoint[i].y;
		}
		return TP;
	}
	// 마우스 제어
	//==============================
	void mouseMoved(int x, int y)
	{
		x -= debugViewport.x;
		y -= debugViewport.y;

		for (int i = 0; i < 4; i++) {
			float diffx = x - vertices[i].x;
			float diffy = y - vertices[i].y;
			float dist = sqrt(diffx*diffx + diffy*diffy);
			if (dist < vertices[i].radius) {
				vertices[i].bOver = true;
			}
			else {
				vertices[i].bOver = false;
			}
		}


	}

	void mouseDragged(int x, int y, int button) {
		x -= debugViewport.x;
		y -= debugViewport.y;

		x /= debugViewRatio.width;
		y /= debugViewRatio.height;

		for (int i = 0; i < 4; i++) {
			if (vertices[i].bBeingDragged == true) {
				vertices[i].x = x;
				vertices[i].y = y;
			}
		}

	}
	//------------------------------
	void mousePressed(int x, int y, int button) {
		x -= debugViewport.x;
		y -= debugViewport.y;

		x /= debugViewRatio.width;
		y /= debugViewRatio.height;

		for (int i = 0; i < 4; i++) {
			float diffx = x - vertices[i].x;
			float diffy = y - vertices[i].y;
			float dist = sqrt(diffx*diffx + diffy*diffy);
			if (dist < vertices[i].radius) {
				vertices[i].bBeingDragged = true;
			}
			else {
				vertices[i].bBeingDragged = false;
			}
		}
	}
	//------------------------------
	void mouseReleased(int x, int y, int button) {
		for (int i = 0; i < 4; i++) {
			vertices[i].bBeingDragged = false;
		}
		sceneChanged();
	}

private:
	unsigned char *diffMapChar;						// min~max 범위 내의 차영상 -> contour 찾는 대상이 됨

	Mat		warpMat;								// 와핑된 결과 영상
	Mat		trainedMat;							// 학습된 결과 영상
	Mat		touchMat;							// 학습영상과 실시간 와핑영상의 차 영상

	ofImage warpdepthRGB;
	ofImage viewColor;

	Mat	    warpDepth;
	Mat		warpColor;


	// 카메라 영상에서 네 꼭지점 좌표
	Point2f cameraSceneVertices[4];
	Point2f canonicalVertices[4];

	Mat		homography;
	Size		size;


	float prevX, prevY;
	float PrevWTP;
	bool bMultifirst;

	int trainCnt;
	vector<ofPoint>	normalizedTouchPoints;
	ofPoint p;
	vector<ofPoint> path;
	ofxCvGrayscaleImage	cvDiffImg;
	ofxCvContourFinder 	contourFinder;
	ofxCvColorImage cvColorImg;

	//VisionDeviceManager	*Device;
};