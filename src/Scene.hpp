#pragma once

#include <ofxOpenCv.h>
#include "ofxXmlSettings.h"

using namespace cv;

class draggableVertex : public Point2f
{
public:
	bool 	bBeingDragged;
	bool 	bOver;
	float 	radius;

	draggableVertex() : radius(12), bOver(false), bBeingDragged(false) {}
};

class Scene
{
public:
	/************************************************************************/
	/* Scene 속성들 - pan/tilt 각도, Touchable, vertice 좌표					*/
	/************************************************************************/
	int panAngle;
	int tiltAngle;
	bool isTouchable;

	draggableVertex vertices[4];
	Point2f touchVertices[4];
	float min;
	float max;
	float offsetx;
	float offsety;
	/************************************************************************/
	/* Contents 이미지 파일명과 와핑될 이미지								*/
	/************************************************************************/
	string contentsFileName;
	string xmlFileName;
	ofImage	contentsImage;

	ofVideoPlayer video;
	/************************************************************************/
	/*	Homography를 계산할 자료형들										*/
	/************************************************************************/
	ofxCvColorImage		cvContentsImage;
	ofxCvColorImage		warpImg;

	CvPoint2D32f cvsrc[4];
	CvPoint2D32f cvdst[4];
	CvMat*	cvTranslate;

	Point2f src[4];
	Point2f dst[4];
	Mat		translate;

	// Drag-points를 그릴지 flag
	bool bDrawDragPoints;
	bool bDrawContents;
	bool bChanged;

	/************************************************************************/
	/* Constructor															*/
	/************************************************************************/
	Scene() : isTouchable(false), bDrawDragPoints(false), bDrawContents(true)
	{
		warpImg.allocate(ofGetWidth(), ofGetHeight());
		cvTranslate = cvCreateMat(3, 3, CV_32FC1);
		translate = Mat(3, 3, CV_32FC1);
		vertices[0].x = 0;
		vertices[0].y = 0;
		vertices[1].x = 1300;
		vertices[1].y = 0;
		vertices[2].x = 1300;
		vertices[2].y = 500;
		vertices[3].x = 0;
		vertices[3].y = 500;
	}

	void setupVideo(/*string xml_path*/) {
		
		
		//xmlFileName = xml_path;

		//video.loadMovie("images\\video_1.mp4");
		video.loadMovie("images\\memory_1.mp4");
	}

	/************************************************************************/
	/* Loads a image from the given file name								*/
	/************************************************************************/
	bool loadContents(/*string contentsFileName*/)
	{
		if (!contentsImage.loadImage(contentsFileName))
		{
			ofLogError("[Scene]: Contents image file is missing.");
			return false;
		}

		// 미리 Homography 계산을 위한 source 배열 계산
		cvsrc[0].x = 0;
		cvsrc[0].y = 0;
		cvsrc[1].x = contentsImage.getWidth();
		cvsrc[1].y = 0;
		cvsrc[2].x = contentsImage.getWidth();
		cvsrc[2].y = contentsImage.getHeight();
		cvsrc[3].x = 0;
		cvsrc[3].y = contentsImage.getHeight();

		src[0].x = 0;
		src[0].y = 0;
		src[1].x = contentsImage.getWidth();
		src[1].y = 0;
		src[2].x = contentsImage.getWidth();
		src[2].y = contentsImage.getHeight();
		src[3].x = 0;
		src[3].y = contentsImage.getHeight();

		cvContentsImage.allocate(contentsImage.getWidth(), contentsImage.getHeight());
		cvContentsImage.setFromPixels(contentsImage.getPixels()/*, contentsImage.getWidth(), contentsImage.getHeight()*/);

		sceneChanged();

		return true;
	}

	bool loadVideoContents()
	{
		//cvContentsImage.allocate(contentsImage.getWidth(), contentsImage.getHeight());
		//cvContentsImage.setFromPixels(contentsImage.getPixels());

		//video.loadMovie("images\\video_1.mp4");
		//video.loadMovie("images\\video_1.mp4");
		setupVideo();
		//video.loadMovie("images\\memory_1.mp4");
		video.play();
		video.update();
		//ofImage image;
		//image.setFromPixels(video.getPixelsRef());

		//cvContentsImage.allocate(image.getWidth(), image.getHeight());
		//cvContentsImage.setFromPixels(image.getPixels());

		//// 미리 Homography 계산을 위한 source 배열 계산
		//cvsrc[0].x = 0;
		//cvsrc[0].y = 0;
		//cvsrc[1].x = image.getWidth();
		//cvsrc[1].y = 0;
		//cvsrc[2].x = image.getWidth();
		//cvsrc[2].y = image.getHeight();
		//cvsrc[3].x = 0;
		//cvsrc[3].y = image.getHeight();

		//src[0].x = 0;
		//src[0].y = 0;
		//src[1].x = image.getWidth();
		//src[1].y = 0;
		//src[2].x = image.getWidth();
		//src[2].y = image.getHeight();
		//src[3].x = 0;
		//src[3].y = image.getHeight();

		//sceneChanged();

		return true;
	}

	//bool	load()
	//{
	//	ofxXmlSettings settings;
	//	if (!settings.loadFile(xmlFileName)) {
	//		ofLogError("SceneManager: Scene configuration file is missing.");
	//		return false;
	//	}
	//	settings.addTag("Scenes");
	//	settings.pushTag("Scenes");
	//	for (int j = 0; j < 4; ++j) {
	//		vertices[j].x = settings.getValue("x", 0, j);
	//		vertices[j].y = settings.getValue("y", 0, j);
	//	}
	//	settings.popTag();
	//	//for (int i = 0; i< TOTAL_SCENE_NUMBER; ++i)
	//	//{
	//	//	settings.pushTag("Scene", i);
	//	//	scenes[i].isTouchable = settings.getValue("touchable", 0);
	//	//	for (int j = 0; j<4; ++j) {
	//	//		scenes[i].vertices[j].x = settings.getValue("x", 0, j);
	//	//		scenes[i].vertices[j].y = settings.getValue("y", 0, j);
	//	//	}
	//	//	/*if (scenes[i].isTouchable) {
	//	//		for (int j = 0; j<4; ++j) {
	//	//			scenes[i].touchVertices[j].x = settings.getValue("touch_x", 0, j);
	//	//			scenes[i].touchVertices[j].y = settings.getValue("touch_y", 0, j);
	//	//			cout << "touchVectices[" << i << "] x = " << scenes[i].touchVertices[j].x << ", y =" << scenes[i].touchVertices[j].y << endl;
	//	//		}
	//	//		scenes[i].min = settings.getValue("min", 0);
	//	//		scenes[i].max = settings.getValue("max", 0);
	//	//	}*/
	//	//	scenes[i].contentsFileName = settings.getValue("contents", "");
	//	//	if (!scenes[i].loadContents())
	//	//		ofLogError("SceneManager: " + ofToString(i) + "th Image is missing.\n");
	//	//	settings.popTag();
	//	//}
	//	//settings.popTag();
	//	//// KinectTouch setup
	//	//if (scenes[0].isTouchable) {
	//	//	for (int i = 0; i<4; ++i)
	//	//	{
	//	//		touch->vertices[i].x = scenes[0].touchVertices[i].x;
	//	//		touch->vertices[i].y = scenes[0].touchVertices[i].y;
	//	//		touch->sceneChanged();
	//	//	}
	//	//	touch->minT = scenes[0].min;
	//	//	touch->maxT = scenes[0].max;
	//	//}
	//	return true;
	//}

	//void	save()
	//{
	//	ofxXmlSettings settings;
	//	settings.addTag("Scenes");
	//	settings.pushTag("Scenes");
	//	for (int j = 0; j < 4; ++j) {
	//		settings.addValue("x", vertices[j].x);
	//		settings.addValue("y", vertices[j].y);
	//	}
	//	settings.popTag();
	//	settings.saveFile(xmlFileName);
	//}


	/************************************************************************/
	/* Draw this node														*/
	/*	- draw contents														*/
	/*	- draw drag points													*/
	/************************************************************************/
	void draw(bool b_draw)
	{
		// 이미지 그리기
		if (bDrawContents) {
			ofEnableAlphaBlending();
			warpImg.draw(0, 0, warpImg.width, warpImg.height);
		}
		if (b_draw) {
			ofPushStyle();
			ofSetColor(255, 255, 255);
			if (bDrawDragPoints) {
				ofNoFill();
				ofBeginShape();
				for (int i = 0; i < 4; i++) {
					ofVertex(vertices[i].x, vertices[i].y);
				}
				ofEndShape(true);

				for (int i = 0; i < 4; i++) {
					ofNoFill();
					ofCircle(vertices[i].x, vertices[i].y, 10);

					if (isTouchable || vertices[i].bOver == true) {
						ofFill();
						ofCircle(vertices[i].x, vertices[i].y, 7);
					}
				}
			}
			ofPopStyle();
		}

		//////////////////////////////////////////////////////////////////////////
		//	Drag Point 그리기

	}

	void drawVideo(bool b_draw)
	{
		//ofEnableAlphaBlending();

		video.play();
		video.update();

		ofImage image;
		image.setFromPixels(video.getPixelsRef());

		cvContentsImage.allocate(image.getWidth(), image.getHeight());
		cvContentsImage.setFromPixels(image.getPixels());

		// 미리 Homography 계산을 위한 source 배열 계산
		cvsrc[0].x = 0;
		cvsrc[0].y = 0;
		cvsrc[1].x = image.getWidth();
		cvsrc[1].y = 0;
		cvsrc[2].x = image.getWidth();
		cvsrc[2].y = image.getHeight();
		cvsrc[3].x = 0;
		cvsrc[3].y = image.getHeight();

		src[0].x = 0;
		src[0].y = 0;
		src[1].x = image.getWidth();
		src[1].y = 0;
		src[2].x = image.getWidth();
		src[2].y = image.getHeight();
		src[3].x = 0;
		src[3].y = image.getHeight();

		sceneChanged();

		warpImg.draw(0, 0, warpImg.width, warpImg.height);

		if (b_draw) {
			ofPushStyle();
			ofSetColor(255, 255, 255);
			if (bDrawDragPoints) {
				ofNoFill();
				ofBeginShape();
				for (int i = 0; i < 4; i++) {
					ofVertex(vertices[i].x, vertices[i].y);
				}
				ofEndShape(true);

				for (int i = 0; i < 4; i++) {
					ofNoFill();
					ofCircle(vertices[i].x, vertices[i].y, 10);

					if (isTouchable || vertices[i].bOver == true) {
						ofFill();
						ofCircle(vertices[i].x, vertices[i].y, 7);
					}
				}
			}
			ofPopStyle();
		}

	}
		//////////////////////////////////////////////////////////////////////////
		//	Drag Point 그리기

	/************************************************************************/
	/*	calculate a new homography											*/
	/************************************************************************/
	void sceneChanged()
	{
		for (int i = 0; i < 4; ++i)
		{
			cvdst[i].x = vertices[i].x;
			cvdst[i].y = vertices[i].y;

			dst[i].x = vertices[i].x;
			dst[i].y = vertices[i].y;
		}

		cvGetPerspectiveTransform(cvsrc, cvdst, cvTranslate);
		//WarpPerspectiveQMatrix( cvsrc, cvdst, cvTranslate );  // calculate 변환행렬 Transform 변환

		translate = getPerspectiveTransform(src, dst); // Translate 회전행렬 3 by 3

		// 이미지 Warp
		cvWarpPerspective(cvContentsImage.getCvImage(), warpImg.getCvImage(), cvTranslate);

		Mat cvContentsImageMat = cvarrToMat(cvContentsImage.getCvImage());
		Mat warpImgMat = cvarrToMat(warpImg.getCvImage());
		warpPerspective(cvContentsImageMat, warpImgMat, translate, Size(warpImg.getCvImage()->width, warpImg.getCvImage()->height));
		// 0808 Mat -> ofxCvColorImage로 어떻게 conversion?
		warpImg.flagImageChanged();
	}

	void showDragPoints() {
		if (bDrawDragPoints) {
			bDrawDragPoints = false;
		}
		else
			bDrawDragPoints = true;
	}

	/************************************************************************/
	/* Drag Point 마우스 컨트롤												*/
	/************************************************************************/
	void mouseMoved(int x, int y ){
		if(bDrawDragPoints){
			for (int i = 0; i < 4; i++){
				float diffx = x - vertices[i].x;
				float diffy = y - vertices[i].y;
				float dist = sqrt(diffx*diffx + diffy*diffy);
				if (dist < vertices[i].radius){
					vertices[i].bOver = true;
				} else {
					vertices[i].bOver = false;
				}	
			}
		}
	}
	//--------------------------------------------------------------
	void mouseDragged(int x, int y, int button){
		if(bDrawDragPoints)
		{
			for (int i = 0; i < 4; i++){
				if (vertices[i].bBeingDragged == true){
					vertices[i].x = x;
					vertices[i].y = y;
				}
			}
		}
	}
	//--------------------------------------------------------------
	void mousePressed(int x, int y, int button)
	{
		if(bDrawDragPoints)
		{
			for (int i = 0; i < 4; i++){
				float diffx = x - vertices[i].x;
				float diffy = y - vertices[i].y;
				float dist = sqrt(diffx*diffx + diffy*diffy);
				if (dist < vertices[i].radius){
					vertices[i].bBeingDragged = true;
				} else {
					vertices[i].bBeingDragged = false;
				}	
			}
		}
	}
	//--------------------------------------------------------------
	void mouseReleased(int x, int y, int button)
	{
		for (int i = 0; i < 4; i++){
			vertices[i].bBeingDragged = false;	
		}
		sceneChanged();
	}

private:
};