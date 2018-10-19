#pragma once

//#include "TouchBGScene.hpp"
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

class DepthTouch
{
public:
	float minT;
	float maxT;

	bool bViewFlip;
	bool bTouchStart;
	bool bTrain;
	bool bDrawTouchDebugView;
	bool bClear;
	bool bDrawTouchPoint;
	VisionDeviceManager* visionDeviceManager;
	//TouchBGScene* currentScene;
	Scene* currentScene;
	draggableCameraVertex	vertices[4];
	vector<Point2f>	originalTouchPoint;
	vector<Point2f> warpedTouchPoint;

	// ����� �� �� �Ķ����
	//------------------------------
	ofRectangle	debugViewport;
	cv::Size2f debugViewRatio;

	DepthTouch(): bTrain(false), bTouchStart(false), bDrawTouchDebugView(false), bMultifirst(false), bDrawTouchPoint(false), bClear(false)
	{
		trainCnt = 0;
		normalizedTouchPoints.clear();
		path.clear();									// 2016-02-20
		originalTouchPoint.clear();
		warpedTouchPoint.clear();
		WTP.x = 0;
		WTP.y = 0;
		prevWTP.x = 0;
		prevWTP.y = 0;
		currWTP.x = 0;
		currWTP.y = 0;
		prevWTP2.x = 0;
		prevWTP2.y = 0;
		currWTP2.x = 0;
		currWTP2.y = 0;
		frameCnt = 0;
		currMultiPoint.clear();
		prevMultiPoint.clear();
	}
	
	void init(VisionDeviceManager* _visionDeviceManager)
	{
		visionDeviceManager = _visionDeviceManager;
		//bTrain, bTouchStart = false;
		//bDrawTouchDebugView, bDrawTouchPoint = true;
		//warpedTouchPoint.clear();
		clearDT();
	}
	void clearDT()
	{
		trainCnt = 0;
		normalizedTouchPoints.clear();
		path.clear();									// 2016-02-20
		originalTouchPoint.clear();
		warpedTouchPoint.clear();
		WTP.x = 0;
		WTP.y = 0;
		prevWTP.x = 0;
		prevWTP.y = 0;
		currWTP.x = 0;
		currWTP.y = 0;
		prevWTP2.x = 0;
		prevWTP2.y = 0;
		currWTP2.x = 0;
		currWTP2.y = 0;
		frameCnt = 0;
		currMultiPoint.clear();
		prevMultiPoint.clear();
		bTrain, bTouchStart, bDrawTouchDebugView, bDrawTouchPoint = false;
	}
	void visionSet(bool bTouch)
	{
		if (bTouch)
		{
			visionDeviceManager->setFlipVertical(false);
			visionDeviceManager->setFlipHorizontal(true);
		} 
		else {
			visionDeviceManager->setFlipVertical(true);
			visionDeviceManager->setFlipHorizontal(false);
		}
	}

	void set(float minThreshold, float maxThreshold)
	{
		minT = minThreshold;
		maxT = maxThreshold;

		vertices[0].x = 0;
		vertices[0].y = 0;

		vertices[1].x = visionDeviceManager->getDepthWidth();
		vertices[1].y = 0;

		vertices[2].x = visionDeviceManager->getDepthWidth();
		vertices[2].y = visionDeviceManager->getDepthHeight();

		vertices[3].x = 0;
		vertices[3].y = visionDeviceManager->getDepthHeight();

		bTrain = false;

		size = Size(visionDeviceManager->getDepthWidth(), visionDeviceManager->getDepthHeight());

		depthShortMat = visionDeviceManager->getDepthShortMat();
		colorMat = visionDeviceManager->getColorMat();
		
		warpMat = Mat(size.height, size.width, depthShortMat.type());
		touchMat = Mat(size.height, size.width, depthShortMat.type());

		warpDepth = Mat(size.height, size.width, depthShortMat.type());
		warpColor = Mat(size.height, size.width, colorMat.type());

		currWarpMat = Mat(size.height, size.width, depthShortMat.type());
		prevWarpMat = Mat(size.height, size.width, depthShortMat.type());
		addMat = Mat(size.height, size.width, depthShortMat.type());
		meanMat = Mat(size.height, size.width, depthShortMat.type());
		cvDiffImg.allocate(size.width, size.height);
		diffMapChar = new unsigned char[size.width * size.height];

		for (int i = 0; i<4; ++i)
			canonicalVertices[i] = vertices[i];

		// ����� �� �� �Ķ����
		//------------------------------
		debugViewport.width = ofGetWidth() / 4.0;
		//debugViewport.height = debugViewport.width * (480.0 / 640.0);
		debugViewport.height = debugViewport.width * (424.0 / 512.0);
		//debugViewport.height = debugViewport.width * (visionDeviceManager->getDepthHeight()/visionDeviceManager->getDepthWidth());
		debugViewport.x = ofGetWidth() - debugViewport.width - 20;
		debugViewport.y = 20;

		debugViewRatio.width = (float)debugViewport.width / (float)visionDeviceManager->getDepthWidth();
		debugViewRatio.height = (float)debugViewport.height / (float)visionDeviceManager->getDepthHeight();

		debugViewRatio.width = (float) visionDeviceManager->getDepthHeight()/ (float)visionDeviceManager->getDepthWidth();
		debugViewRatio.width = (float) 0.831;
		debugViewRatio.height = (float) 0.9;

	}

	void setCurrentScene(/*TouchBGScene * current*/Scene * current)
	{
		currentScene = current;
		sceneChanged();
	}

	void update()
	{
		//visionDeviceManager->setFlipVertical(false);
		//visionDeviceManager->setFlipHorizontal(true);
		//visionSet(true);
		// 181009 �� depthShortMat�� ����� �ȱ׷�������...��..
		// depthMat�̶� Color�� �� �� �׷����µ�
		// �ٽ� �ǳ�???? ��.. currentScene���� ������ ���峪��
		colorMat = visionDeviceManager->getColorMat();
		depthShortMat = visionDeviceManager->getDepthShortMat();

		//colorMat.resize(size.width, size.height);
		//Mat colorResize;
		//resize(colorMat, colorResize, Size(size.width, size.height), 0, 0, 1);
		//colorMat.resize(visionDeviceManager->getDepthWidth(), visionDeviceManager->getDepthHeight());
		
		//depthShortMat.resize(size.width, size.height);
		//warpPerspective(colorResize, warpColor, homography, size, INTER_CUBIC);
		warpPerspective(colorMat, warpColor, homography, size, INTER_CUBIC);

		// DephtShortMat�� ����� �Ⱥ��� �ÿ� �Ʒ��� �츮��
		/*if (bViewFlip)
		{
			Mat flipDSM;
			flip(depthShortMat, flipDSM, 1);
			warpPerspective(flipDSM, warpMat, homography, size, INTER_CUBIC);
			warpPerspective(flipDSM, warpDepth, homography, size, INTER_CUBIC);
		}
		else {*/
			warpPerspective(depthShortMat, warpMat, homography, size, INTER_CUBIC);
			warpPerspective(depthShortMat, warpDepth, homography, size, INTER_CUBIC);
		//}
		
		Mat warpdepth8;
		depth162depth8Color(warpDepth, warpdepth8, 500, 1500);
		warpdepthRGB.setFromPixels(warpdepth8.data, size.width, size.height, OF_IMAGE_COLOR);

		if (bTrain) {
			ofLogNotice("KinectTouch: Training...");
			trainedMat = warpMat.clone();
			bTouchStart = true;
			bTrain = false;
		}

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
			contourFinder.findContours(cvDiffImg, 500, size.width*size.height*0.5, 2, false, false); // touch�Ǵ� ���� contour finder
			normalizedTouchPoints.clear(); // ��� ���� Ŭ����

			if (contourFinder.nBlobs > 0) // touch�� �Ǵ� ����
			{
				bDetect = true; // touch flag on
				if (contourFinder.nBlobs == 1) // single touch
				{
					p.set(contourFinder.blobs[0].centroid.x, contourFinder.blobs[0].centroid.y - (contourFinder.blobs[0].boundingRect.height / 2) + 20);
					p.x /= (double)size.width;
					p.y /= (double)size.height;
					normalizedTouchPoints.push_back(p);
					//prevX = contourFinder.blobs[0].centroid.x; // multi touch���� ����Ǵ� ����Ʈ�� �������� ����Ǵ� ������ ������ ���ϱ� ���ؼ� �� �� ����Ʈ�� x,y�� ����
					//prevY = contourFinder.blobs[0].centroid.y;
					bMultifirst = false;
					//firstAnglecnt = 0;
				}
				if (contourFinder.nBlobs == 2) // multi touch
				{
					bMultifirst = true; // �ѹ��̶� multi touch�� ���� �Ǵ� ���� flag on
					for (int i = 0; i<contourFinder.nBlobs; i++)
					{
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
			}

			if (bDetect && normalizedTouchPoints.size() == 0)
			{
				if (path.size() != 0) {
					path.clear();
				}
			}

			/*if (bTrain)
				bTrain = false;*/
		}
	}

	void depth162depth8Color(Mat& src, Mat& dest, double minv, double maxv)
	{
		Mat depthMat;
		src.convertTo(depthMat, CV_8U, 255.0 / maxv, -minv / maxv);
		applyColorMap(depthMat, dest, 2);
	}

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
	float distanceCheck(float currX, float currY, float prevX, float prevY)
	{
		float distance;
		distance = sqrt(pow(currX - prevX, 2) + pow(currY - prevY, 2));

		return distance;
	}

	Point2f removeShake(float currX, float currY, float prevX, float prevY/*, float distanceNumber*/)
	{
		currX = prevX;
		currY = prevY;
		Point2f temp;
		temp.x = currX;
		temp.y = currY;
		return temp;
	}

	void TouchDecision()
	{
		if (frameCnt == 0)
		{
			prevWTP.x = warpedTouchPoint[0].x;
			prevWTP.y = warpedTouchPoint[0].y;
			currWTP.x = warpedTouchPoint[0].x;
			currWTP.y = warpedTouchPoint[0].y;
		}
		else if (frameCnt >= 2)
		{
			// Single touch
			if (warpedTouchPoint.size() == 1)
			{
				currWTP.x = warpedTouchPoint[0].x;
				currWTP.y = warpedTouchPoint[0].y;
				float distance;
				distance = distanceCheck(currWTP.x, currWTP.y, prevWTP.x, prevWTP.y);

				if (distance < 15)
				{
					Point2f temp;
					temp = removeShake(currWTP.x, currWTP.y, prevWTP.x, prevWTP.y/*, 15*/);

					currWTP.x = temp.x;
					currWTP.y = temp.y;
					prevWTP.x = currWTP.x;
					prevWTP.y = currWTP.y;
					warpedTouchPoint[0].x = currWTP.x;
					warpedTouchPoint[0].y = currWTP.y;
				}
			}
			// multi-touch					
			if (warpedTouchPoint.size() == 2)
			{
				if (WTP.x == 0) // multi-touch ó������ ���� �Ǿ�����
				{
					//PrevWTP = warpedTouchPoint[0].x;
					WTP.x = warpedTouchPoint[0].x;
					WTP.y = warpedTouchPoint[0].y;
				}
				//if (abs(PrevWTP - warpedTouchPoint[1].x) < abs(PrevWTP - warpedTouchPoint[0].x)) // �ι�° �νĵ� ����Ʈ�� ���� �����ӿ��� �ι�° �νĵ� ����Ʈ�� �ٸ��� �����������
				// �� ����Ʈ �� �Ŀ� ���� 
				if (sqrt(pow(WTP.x - warpedTouchPoint[1].x, 2) + pow(WTP.y - warpedTouchPoint[1].y, 2))
					< sqrt(pow(WTP.x - warpedTouchPoint[0].x, 2) + pow(WTP.y - warpedTouchPoint[0].y, 2))) // x,y ����Ʈ ���� ��
				{
					float firstx = warpedTouchPoint[0].x;
					float firsty = warpedTouchPoint[0].y;
					warpedTouchPoint[0].x = warpedTouchPoint[1].x;
					warpedTouchPoint[0].y = warpedTouchPoint[1].y;
					warpedTouchPoint[1].x = firstx;
					warpedTouchPoint[1].y = firsty;
					WTP.x = warpedTouchPoint[0].x;
					WTP.y = warpedTouchPoint[0].y;
				}
				if (prevWTP2.x == 0) // �ι�° ��ġ�� ���� �Ǿ����� 
				{
					prevWTP.x = warpedTouchPoint[0].x;
					prevWTP.y = warpedTouchPoint[0].y;
					currWTP.x = warpedTouchPoint[0].x;
					currWTP.y = warpedTouchPoint[0].y;
					prevWTP2.x = warpedTouchPoint[1].x;
					prevWTP2.y = warpedTouchPoint[1].y;
					currWTP2.x = warpedTouchPoint[1].x;
					currWTP2.y = warpedTouchPoint[1].y;
				}
				else
				{
					// ���� �Ϸ�� ����Ʈ [0]. [1] ���� ���ؼ� removeShake ���ֱ�......������...
					currWTP.x = warpedTouchPoint[0].x;
					currWTP.y = warpedTouchPoint[0].y;
					currWTP2.x = warpedTouchPoint[1].x;
					currWTP2.y = warpedTouchPoint[1].y;

					float distance1;
					distance1 = distanceCheck(currWTP.x, currWTP.y, prevWTP.x, prevWTP.y);
					float distance2;
					distance2 = distanceCheck(currWTP2.x, currWTP2.y, prevWTP2.x, prevWTP2.y);

					if (distance1 < 15)
					{
						Point2f temp;
						temp = removeShake(currWTP.x, currWTP.y, prevWTP.x, prevWTP.y/*, 15*/);
						// ù��° touch point distance remove
						currWTP.x = temp.x;
						currWTP.y = temp.y;
						prevWTP.x = currWTP.x;
						prevWTP.y = currWTP.y;
						warpedTouchPoint[0].x = currWTP.x;
						warpedTouchPoint[0].y = currWTP.y;
					}
					if (distance2 < 15)
					{
						Point2f temp2;
						temp2 = removeShake(currWTP2.x, currWTP2.y, prevWTP2.x, prevWTP2.y/*, 15*/);
						// �ι�° touch point distance remove
						currWTP2.x = temp2.x;
						currWTP2.y = temp2.y;
						prevWTP2.x = currWTP2.x;
						prevWTP2.y = currWTP2.y;
						warpedTouchPoint[1].x = currWTP2.x;
						warpedTouchPoint[1].y = currWTP2.y;
					}
				}
			}
		}
		frameCnt++;
	}


	void draw()
	{
		if (bDrawTouchDebugView)
		{
			// ��ġ�� Debug Images
			//==============================
			warpdepthRGB.draw(debugViewport.x - debugViewport.width, debugViewport.y, debugViewport.width, debugViewport.height);
			visionDeviceManager->getColorImage().draw(debugViewport.x, debugViewport.y, debugViewport.width, debugViewport.height);
			cvDiffImg.draw(debugViewport.x, debugViewport.y + debugViewport.height, debugViewport.width, debugViewport.height);
			contourFinder.draw(debugViewport.x, debugViewport.y + debugViewport.height, debugViewport.width, debugViewport.height);

			//	��ġ�� Drag Point �׸���
			//------------------------------
			ofSetColor(ofColor::white);
			ofNoFill();
			ofBeginShape();

			for (int i = 0; i < 4; i++)
				ofVertex(vertices[i].x * debugViewRatio.width + debugViewport.x, vertices[i].y * debugViewRatio.height + debugViewport.y);
			ofEndShape(true);

			for (int i = 0; i < 4; i++) 
			{
				ofNoFill();
				ofCircle(vertices[i].x * debugViewRatio.width + debugViewport.x, vertices[i].y * debugViewRatio.height + debugViewport.y, 10);

				if (vertices[i].bOver == true) 
				{
					ofFill();
					ofCircle(vertices[i].x * debugViewRatio.width + debugViewport.x, vertices[i].y * debugViewRatio.height + debugViewport.y, 7);
				}
			}
		}

		originalTouchPoint.clear();
		for (int i = 0; i < normalizedTouchPoints.size(); ++i)
		{
			Point2f OTP(normalizedTouchPoints[i].x, normalizedTouchPoints[i].y);
			OTP.x *= currentScene->contentsImage.getWidth();
			OTP.y *= currentScene->contentsImage.getHeight();
			originalTouchPoint.push_back(OTP);
		}

		// warping�� view �󿡼��� touch point
		warpedTouchPoint.clear();
		if (originalTouchPoint.size())
			perspectiveTransform(originalTouchPoint, warpedTouchPoint, currentScene->translate);

		if (bDetect) {
			TouchDecision();
		}
		else {
			frameCnt = 0;
			prevWTP.x = 0;
		}


		if (bDrawTouchPoint)
		{
			ofPushStyle();
			ofFill();
			ofSetColor(ofColor::red);
			for (int i = 0; i < warpedTouchPoint.size(); ++i)
			{
				ofSetColor(ofColor::red);
				ofCircle(warpedTouchPoint[0].x, warpedTouchPoint[0].y, 30);					// TODO:
																							//ofCircle(currWTP.x, currWTP.y, 30);
				if (warpedTouchPoint.size() == 2) {
					ofSetColor(ofColor::white);
					ofCircle(warpedTouchPoint[1].x, warpedTouchPoint[1].y, 30);					// TODO:	
				}
			}
			ofPopStyle();
		}


	}
	void refresh()
	{
		clearDT();
		//bTouchStart = false;
		bTrain = true;
	}
	// ���콺 ����
	//==============================
	void mouseMoved(int x, int y)
	{
		x -= debugViewport.x;
		y -= debugViewport.y;

		for (int i = 0; i < 4; i++) {
			float diffx = x - vertices[i].x;
			float diffy = y - vertices[i].y;
			float dist = sqrt(diffx*diffx + diffy * diffy);
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
			float dist = sqrt(diffx*diffx + diffy * diffy);
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
		unsigned char *diffMapChar;						// min~max ���� ���� ������ -> contour ã�� ����� ��
		Mat depthShortMat;
		Mat colorMat;

		Mat		warpMat;								// ���ε� ��� ����
		Mat		trainedMat;							// �н��� ��� ����
		Mat		touchMat;							// �н������ �ǽð� ���ο����� �� ����

		ofImage warpdepthRGB;
		ofImage viewColor;

		Mat	    warpDepth;
		Mat		warpColor;

		Mat currWarpMat;
		Mat prevWarpMat;
		Mat addMat;
		Mat meanMat;

		// ī�޶� ���󿡼� �� ������ ��ǥ
		Point2f cameraSceneVertices[4];
		Point2f canonicalVertices[4];

		Mat		homography;
		Size		size;


		
		bool bMultifirst;
		bool bDetect;
		int trainCnt;
		vector<ofPoint>	normalizedTouchPoints;
		ofPoint p;
		vector<ofPoint> path;
		ofxCvGrayscaleImage	cvDiffImg;
		ofxCvContourFinder 	contourFinder;
		ofxCvColorImage cvColorImg;

		Point2f WTP;

		Point2f prevWTP;
		Point2f currWTP;
		Point2f prevWTP2;
		Point2f currWTP2;

		vector<Point2f> currMultiPoint;
		vector<Point2f> prevMultiPoint;

		//20180822 
		int frameCnt;
};