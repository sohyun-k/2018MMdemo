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
	int touchMinArea;
	int touchMaxArea;

	bool bViewFlip;
	bool bTouchStart;
	bool bTrain;
	bool bDrawTouchDebugView;
	bool bClear;
	bool bDrawTouchPoint;
	bool bUpdate;

	ofVec2f touchPointOffset;
	int minThreshold;
	int maxThreshold;

	VisionDeviceManager* visionDeviceManager;
	//TouchBGScene* currentScene;
	Scene* currentScene;
	draggableCameraVertex	vertices[4];
	vector<Point2f>	originalTouchPoint;
	vector<Point2f> warpedTouchPoint;

	// 디버그 뷰 용 파라미터
	//------------------------------
	ofRectangle	debugViewport;
	cv::Size2f debugViewRatio;

	DepthTouch() : bUpdate(false),  bTrain(false), bTouchStart(false), bDrawTouchDebugView(false), bMultifirst(false), bDrawTouchPoint(false), bClear(false)
	{
		//trainCnt = 0;
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
	}

	void init(VisionDeviceManager* _visionDeviceManager)
	{
		visionDeviceManager = _visionDeviceManager;
		//visionDeviceManager->setFlipVertical(false);
		//visionDeviceManager->setFlipHorizontal(true);
		//bTrain, bTouchStart = false;
		//bDrawTouchDebugView, bDrawTouchPoint = true;
		//warpedTouchPoint.clear();
		//clearDT();
	}
	void clearDT()
	{
		//trainCnt = 0;
		normalizedTouchPoints.clear();
		path.clear();									// 2016-02-20
		originalTouchPoint.clear();
		warpedTouchPoint.clear();
		bTrain = false;
		bTouchStart = false;
		frameCnt = 0;
		touchMat.empty();
		warpMat.empty();
		trainedMat.empty();
		//visionDeviceManager->setFlipVertical(true);
		//visionDeviceManager->setFlipHorizontal(false);
		//warpdepthRGB.clear();
		//cvDiffImg.clear();
		//contourFinder.resetAnchor();
		visionSet(false);
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

	void set()
	{
		minThreshold = 0;
		maxThreshold = 0;
		
		touchMinArea = 500;
		touchMaxArea = 800;

		frameCnt = 0;
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
		//colorMat = visionDeviceManager->getColorMat();

		warpMat = Mat(size.height, size.width, depthShortMat.type());
		touchMat = Mat(size.height, size.width, depthShortMat.type());
		
		//warpDepth = Mat(size.height, size.width, depthShortMat.type());

		//warpColor = Mat(size.height, size.width, colorMat.type());

		//currWarpMat = Mat(size.height, size.width, depthShortMat.type());
		//prevWarpMat = Mat(size.height, size.width, depthShortMat.type());
		//addMat = Mat(size.height, size.width, depthShortMat.type());
		//meanMat = Mat(size.height, size.width, depthShortMat.type());
		cvDiffImg.allocate(size.width, size.height);
		diffMapChar = new unsigned char[size.width * size.height];

		for (int i = 0; i<4; ++i)
			canonicalVertices[i] = vertices[i];

		// 디버그 뷰 용 파라미터
		//------------------------------
		debugViewport.width = ofGetWidth() / 4.0;
		//debugViewport.height = debugViewport.width * (480.0 / 640.0);
		debugViewport.height = debugViewport.width * (424.0 / 512.0);
		//debugViewport.height = debugViewport.width * (visionDeviceManager->getDepthHeight()/visionDeviceManager->getDepthWidth());
		debugViewport.x = ofGetWidth() - debugViewport.width - 20;
		debugViewport.y = 20;

		debugViewRatio.width = ((float)debugViewport.width / (float)visionDeviceManager->getDepthWidth())-0.1;
		debugViewRatio.height = (float)debugViewport.height / (float)visionDeviceManager->getDepthHeight();
		cout << "debugViewRatio.width = " << debugViewRatio.width << "debugViewRatio.height = " << debugViewRatio.height <<endl;
		touchPointOffset.set(0, 0);
		sceneChanged();
	}

	void setCurrentScene(/*TouchBGScene * current*/Scene * current)
	{
		currentScene = current;
		sceneChanged();
	}

	void parameterSetup(float _minThreshold, float _maxThreshold, float _touchPointOffsetx, float _touchPointOffsety, int _touchMinArea, int _touchMaxArea)
	{
		minT = _minThreshold;
		maxT = _maxThreshold;
		touchPointOffset.set(_touchPointOffsetx, _touchPointOffsety);
		touchMinArea = _touchMinArea;
		touchMaxArea = _touchMaxArea;
	}

	void update(/*float _minThreshold, float _maxThreshold, float _touchPointOffsetx, float _touchPointOffsety*/)
	{
		/*minT = _minThreshold;
		maxT = _maxThreshold;
		touchPointOffset.set(_touchPointOffsetx, _touchPointOffsety);*/

		//visionSet(true);
		// 181009 왜 depthShortMat이 제대로 안그려지는지...음..
		// depthMat이랑 Color는 다 잘 그려지는데
		// 다시 되네???? 음.. currentScene에서 문제가 생겼나봄
		//if (bUpdate)
		//{
			/*visionDeviceManager->setFlipVertical(false);
			visionDeviceManager->setFlipHorizontal(true);*/
			// color image 너무 큼
			//colorMat = visionDeviceManager->getColorMat();
			depthShortMat = visionDeviceManager->getDepthShortMat();

			//colorMat.resize(size.width, size.height);
			//Mat colorResize;
			//resize(colorMat, colorResize, Size(size.width, size.height), 0, 0, 1);
			//colorMat.resize(visionDeviceManager->getDepthWidth(), visionDeviceManager->getDepthHeight());

			//depthShortMat.resize(size.width, size.height);
			//warpPerspective(colorResize, warpColor, homography, size, INTER_CUBIC);
			
			// 18-10-19 color image 너무 큼
			//warpPerspective(colorMat, warpColor, homography, size, INTER_CUBIC);

			// DephtShortMat이 제대로 안보일 시에 아래꺼 살리기
			/*if (bViewFlip)
			{
			Mat flipDSM;
			flip(depthShortMat, flipDSM, 1);
			warpPerspective(flipDSM, warpMat, homography, size, INTER_CUBIC);
			warpPerspective(flipDSM, warpDepth, homography, size, INTER_CUBIC);
			}
			else {*/
			warpPerspective(depthShortMat, warpMat, homography, size, INTER_CUBIC);
			//warpPerspective(depthShortMat, warpDepth, homography, size, INTER_CUBIC);
			//warpDepth = warpMat.clone();
			//}

			Mat warpdepth8;
			//depth162depth8Color(warpDepth, warpdepth8, 500, 4500);
			depth162depth8Color(warpMat, warpdepth8, 500, 4500);
			warpdepthRGB.setFromPixels(warpdepth8.data, size.width, size.height, OF_IMAGE_COLOR);
	//	}
			
			if (bTrain) {
				ofLogNotice("KinectTouch: Training...");
				//cvDiffImg.clear();
				//cvDiffImg.allocate(size.width, size.height);
				//contourFinder.resetAnchor();
				trainedMat = warpMat.clone();
				bTouchStart = true;
				bTrain = false;
			}

			if (bTouchStart)
			{
				for (int i = 0; i < size.width * size.height; i++) {
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
				//contourFinder.findContours(cvDiffImg, 500, size.width*size.height*0.5, 1, false, false); // touch되는 영역 contour finder
				contourFinder.findContours(cvDiffImg, touchMinArea, touchMaxArea, 1, false, false); // touch되는 영역 contour finder
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
						//prevX = contourFinder.blobs[0].centroid.x; // multi touch에서 검출되는 포인트가 무작위로 저장되는 벡터의 순서를 정하기 위해서 그 전 포인트의 x,y값 저장
						//prevY = contourFinder.blobs[0].centroid.y;
						bMultifirst = false;
						//firstAnglecnt = 0;
					}
					if (contourFinder.nBlobs == 2) // multi touch
					{
						bMultifirst = true; // 한번이라도 multi touch가 검출 되는 순간 flag on
						for (int i = 0; i < contourFinder.nBlobs; i++)
						{
							p.set(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y - (contourFinder.blobs[i].boundingRect.height / 2) + 20);
							p.x /= (double)size.width;
							p.y /= (double)size.height;
							normalizedTouchPoints.push_back(p);
						}
					}
					//cout << "Coutour Finder AREA = " << contourFinder.blobs[0].area << endl;
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
				if (bDetect&& normalizedTouchPoints.size() != 0)
				{
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
					{
						perspectiveTransform(originalTouchPoint, warpedTouchPoint, currentScene->translate);
						// key press Touch Point offset control
						warpedTouchPoint[0].x = warpedTouchPoint[0].x + touchPointOffset.x;
						warpedTouchPoint[0].y = warpedTouchPoint[0].y + touchPointOffset.y;
					}

					//if (bDetect) 
					if (warpedTouchPoint.size() != 0)
					{
						TouchDecision();
					}
					else {
						frameCnt = 0;
						prevWTP.x = 0;
					}
				}
				if (!bDetect)
				{
					originalTouchPoint.clear();
					warpedTouchPoint.clear();
				}
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
				//cout << "distance : " << distance << endl;
				if (distance < 50 )
				{
					/*warpedTouchPoint[0].x = prevWTP.x;
					warpedTouchPoint[0].y = prevWTP.y;
					prevWTP.x = currWTP.x;
					prevWTP.y = currWTP.y;*/
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
				if (WTP.x == 0) // multi-touch 처음으로 검출 되었을때
				{
					//PrevWTP = warpedTouchPoint[0].x;
					WTP.x = warpedTouchPoint[0].x;
					WTP.y = warpedTouchPoint[0].y;
				}
				//if (abs(PrevWTP - warpedTouchPoint[1].x) < abs(PrevWTP - warpedTouchPoint[0].x)) // 두번째 인식된 포인트가 저번 프레임에서 두번째 인식된 포인트와 다를시 스왑해줘야함
				// 각 포인트 비교 후에 스왑 
				if (sqrt(pow(WTP.x - warpedTouchPoint[1].x, 2) + pow(WTP.y - warpedTouchPoint[1].y, 2))
					< sqrt(pow(WTP.x - warpedTouchPoint[0].x, 2) + pow(WTP.y - warpedTouchPoint[0].y, 2))) // x,y 포인트 각기 비교
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
				if (prevWTP2.x == 0) // 두번째 터치가 검출 되었을때 
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
					// 스왑 완료된 포인트 [0]. [1] 끼리 비교해서 removeShake 해주기......귀찮당...
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
						// 첫번째 touch point distance remove
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
						// 두번째 touch point distance remove
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
		//originalTouchPoint.clear();
		//for (int i = 0; i < normalizedTouchPoints.size(); ++i)
		//{
		//	Point2f OTP(normalizedTouchPoints[i].x, normalizedTouchPoints[i].y);
		//	OTP.x *= currentScene->contentsImage.getWidth();
		//	OTP.y *= currentScene->contentsImage.getHeight();
		//	originalTouchPoint.push_back(OTP);
		//}

//// warping된 view 상에서의 touch point
		//warpedTouchPoint.clear();
		//if (originalTouchPoint.size())
		//{
		//	perspectiveTransform(originalTouchPoint, warpedTouchPoint, currentScene->translate);
		//	// key press Touch Point offset control
		//	warpedTouchPoint[0].x = warpedTouchPoint[0].x + touchPointOffset.x;
		//	warpedTouchPoint[0].y = warpedTouchPoint[0].y + touchPointOffset.y;
		//}

		//if (bDetect) {
		//	TouchDecision();
		//}
		//else {
		//	frameCnt = 0;
		//	prevWTP.x = 0;
		//}
		if (warpedTouchPoint.size() != 0)
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
		if (bDrawTouchDebugView)
		{
			// 터치용 Debug Images
			//==============================
			warpdepthRGB.draw(debugViewport.x - debugViewport.width, debugViewport.y, debugViewport.width, debugViewport.height);
			visionDeviceManager->getColorImage().draw(debugViewport.x, debugViewport.y, debugViewport.width, debugViewport.height);
			if (bTouchStart)
			{
				cvDiffImg.draw(debugViewport.x, debugViewport.y + debugViewport.height, debugViewport.width, debugViewport.height);
				contourFinder.draw(debugViewport.x, debugViewport.y + debugViewport.height, debugViewport.width, debugViewport.height);
			}
			
			//	터치용 Drag Point 그리기
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
	}
	void refresh()
	{
		clearDT();
		update();
		//bTouchStart = false;
		bTrain = true;
	}

	
	//void exit()
	//{
	//	//visionDeviceManager->exit();
	//	//delete visionDeviceManager;
	//	//visionDeviceManager = nullptr;
	//	depthShortMat.~Mat();
	//	colorMat.~Mat();
	//	warpMat.~Mat();
	//	trainedMat.~Mat();
	//	warpdepthRGB.~ofImage_();
	//	viewColor.~ofImage_();
	//	warpDepth.~Mat();
	//	warpColor.~Mat();
	//	clearDT();
	//	//delete diffMapChar, depthShortMat, colorMat, warpMat, trainedMat, touchMat, warpdepthRGB, viewColor, warpDepth, warpColor;
	//}
	// 키보드 제어
	//==============================
	void keyPressed(int key)
	{
		switch (key) {
		/*case OF_KEY_LEFT: touchPointOffset.x -= 20; break;
		case OF_KEY_RIGHT: touchPointOffset.x += 20; break;
		case OF_KEY_UP: touchPointOffset.y -= 20; break;
		case OF_KEY_DOWN: touchPointOffset.y += 20; break;
		case OF_KEY_HOME: minThreshold += 1; break;
		case OF_KEY_END: minThreshold -= 1; break;
		case OF_KEY_PAGE_DOWN: maxThreshold += 1; break;
		case OF_KEY_DEL: maxThreshold -= 1; break;*/

		case OF_KEY_F1: touchPointOffset.x -= 20; break;
		case OF_KEY_F2: touchPointOffset.x += 20; break;
		case OF_KEY_F3: touchPointOffset.y -= 20; break;
		case OF_KEY_F4: touchPointOffset.y += 20; break;
		case OF_KEY_F5: minT -= 1; break;
		case OF_KEY_F6: minT += 1; break;
		case OF_KEY_F7: maxT -= 1; break;
		case OF_KEY_F8: maxT += 1; break;
		case OF_KEY_F9: touchMinArea -= 20; break;
		case OF_KEY_F10: touchMinArea += 20; break;
		case OF_KEY_F11: touchMaxArea -= 20; break;
		case OF_KEY_F12: touchMaxArea += 20; break;
		parameterSetup(minT, maxT, touchPointOffset.x,touchPointOffset.y, touchMinArea, touchMaxArea);

		/*case OF_KEY_HOME: debugViewRatio.height += 0.1; break;
		case OF_KEY_END: debugViewRatio.height -= 0.1; break;
		case OF_KEY_PAGE_DOWN: debugViewRatio.width += 0.1; break;
		case OF_KEY_DEL: debugViewRatio.width -= 0.1; break;*/


		}
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

public:
	unsigned char *diffMapChar;						// min~max 범위 내의 차영상 -> contour 찾는 대상이 됨
	Mat depthShortMat;
	Mat colorMat;

	Mat		warpMat;								// 와핑된 결과 영상
	Mat		trainedMat;							// 학습된 결과 영상
	Mat		touchMat;							// 학습영상과 실시간 와핑영상의 차 영상

	ofImage warpdepthRGB;
	//ofImage viewColor;

	Mat	    warpDepth;
	Mat		warpColor;

	Mat currWarpMat;
	Mat prevWarpMat;
	Mat addMat;
	Mat meanMat;

	// 카메라 영상에서 네 꼭지점 좌표
	Point2f cameraSceneVertices[4];
	Point2f canonicalVertices[4];

	Mat		homography;
	Size		size;



	bool bMultifirst;
	bool bDetect;
	//int trainCnt;
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