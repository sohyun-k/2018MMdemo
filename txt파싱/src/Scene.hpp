#pragma once

#include <ofBaseTypes.h>

#include "ofMain.h"
#include "ofxOpenCv.h"


using namespace cv;

class draggableVertex : public ofVec3f
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

	float min;
	float max;

	/************************************************************************/
	/* Contents 이미지 파일명과 와핑될 이미지								*/
	/************************************************************************/
	string contentsFileName;
	ofImage	contentsImage;

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
		cvTranslate = cvCreateMat(3, 3, CV_32FC1);
		translate = Mat(3, 3, CV_32FC1);
	}

	/************************************************************************/
	/* Loads a image from the given file name								*/
	/************************************************************************/
	bool loadContents(ofRectangle rect)
	{
		warpImg.allocate(ofGetWidth(), ofGetHeight());

		if (!contentsImage.loadImage(contentsFileName))
		{
			ofLogError("[Scene]: Contents image file is missing.");
			return false;
		}

		// 미리 Homography 계산을 위한 source 배열 계산
		cvsrc[0].x = 0;
		cvsrc[0].y = 0;
		cvsrc[1].x = rect.getWidth();
		cvsrc[1].y = 0;
		cvsrc[2].x = rect.getWidth();
		cvsrc[2].y = rect.getHeight();
		cvsrc[3].x = 0;
		cvsrc[3].y = rect.getHeight();

		src[0].x = 0;
		src[0].y = 0;
		src[1].x = rect.getWidth();
		src[1].y = 0;
		src[2].x = rect.getWidth();
		src[2].y = rect.getHeight();
		src[3].x = 0;
		src[3].y = rect.getHeight();
        
        ofImage tempImage;
        tempImage.load(contentsFileName);

		cvContentsImage.allocate(contentsImage.getWidth(), contentsImage.getHeight());
		cvContentsImage.setFromPixels(tempImage.getPixels().getData(), contentsImage.getWidth(), contentsImage.getHeight());
		
		cvContentsImage.resize(rect.getWidth(), rect.getHeight());


		vertices[0].x = rect.x;
		vertices[0].y = rect.y;
		vertices[1].x = rect.x + contentsImage.getWidth();
		vertices[1].y = rect.y;
		vertices[2].x = rect.x + contentsImage.getWidth();
		vertices[2].y = rect.y + contentsImage.getHeight();
		vertices[3].x = rect.x;
		vertices[3].y = rect.y + contentsImage.getHeight();

		
		vertices[0].x = rect.x;
		vertices[0].y = rect.y;
		vertices[1].x = rect.x + rect.getWidth();
		vertices[1].y = rect.y;
		vertices[2].x = rect.x + rect.getWidth();
		vertices[2].y = rect.y + rect.getHeight();
		vertices[3].x = rect.x;
		vertices[3].y = rect.y + rect.getHeight();
		

		sceneChanged();

		return true;
	}


	/************************************************************************/
	/* Draw this node														*/
	/*	- draw contents														*/
	/*	- draw drag points													*/
	/************************************************************************/
	virtual void draw()
	{
		//if (bDrawContents)
			// 이미지 그리기
			//warpImg.draw(0, 0, warpImg.width, warpImg.height);

		//////////////////////////////////////////////////////////////////////////
		//	Drag Point 그리기
		ofSetColor(ofColor::red);
		if (bDrawDragPoints) {
			ofNoFill();
            ofSetLineWidth(10);
			ofBeginShape();
			for (int i = 0; i < 4; i++)
			{
				ofVertex(vertices[i].x, vertices[i].y);
			}
			ofEndShape(true);

			for (int i = 0; i < 4; i++)
			{
				ofNoFill();
				ofCircle(vertices[i].x, vertices[i].y, 30);
				if (isTouchable || vertices[i].bOver == true) {
					ofFill();
					ofCircle(vertices[i].x, vertices[i].y, 15);
				}
			}

		}
	}

	/************************************************************************/
	/*	calculate a new homography											*/
	/************************************************************************/
	void sceneChanged()
	{
		for (int i = 0; i<4; ++i)
		{
			cvdst[i].x = vertices[i].x;
			cvdst[i].y = vertices[i].y;

			dst[i].x = vertices[i].x;
			dst[i].y = vertices[i].y;
		}

		cvWarpPerspectiveQMatrix(cvsrc, cvdst, cvTranslate);  // calculate 
		translate = getPerspectiveTransform(src, dst);

		// 이미지 Warp
		cvWarpPerspective(cvContentsImage.getCvImage(), warpImg.getCvImage(), cvTranslate);

		warpImg.flagImageChanged();
	}



	/************************************************************************/
	/* Drag Point 마우스 컨트롤												*/
	/************************************************************************/
	void mouseMoved(int x, int y) {
		if (bDrawDragPoints) {
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
	}
	//--------------------------------------------------------------
	void mouseDragged(int x, int y, int button) {
		if (bDrawDragPoints)
		{
			for (int i = 0; i < 4; i++) {
				if (vertices[i].bBeingDragged == true) {
					vertices[i].x = x;
					vertices[i].y = y;
				}
			}
		}
	}
	//--------------------------------------------------------------
	void mousePressed(int x, int y, int button)
	{
		if (bDrawDragPoints)
		{
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
	}
	//--------------------------------------------------------------
	void mouseReleased(int x, int y, int button)
	{
		for (int i = 0; i < 4; i++) {
			vertices[i].bBeingDragged = false;
		}
		sceneChanged();
	}


	/************************************************************************/
	/* Transform matrix 저장												*/
	/************************************************************************/
	void saveInfo(char* fileName) {
		FileStorage fs(fileName, FileStorage::WRITE);
		fs.open(fileName, FileStorage::WRITE);

		fs << "translate" << translate;

		for (int i = 0; i < 4; i++) {
			char name[100];
			sprintf(name, "vertices_X_%d", i);
			fs << name << vertices[i].x;
			sprintf(name, "vertices_Y_%d", i);
			fs << name << vertices[i].y;
			
			sprintf(name, "cvsrc_X_%d", i);
			fs << name << cvsrc[i].x;
			sprintf(name, "cvsrc_Y_%d", i);
			fs << name << cvsrc[i].y;

			sprintf(name, "cvdst_X_%d", i);
			fs << name << cvdst[i].x;
			sprintf(name, "cvdst_Y_%d", i);
			fs << name << cvdst[i].y;

			sprintf(name, "src_X_%d", i);
			fs << name << src[i].x;
			sprintf(name, "src_Y_%d", i);
			fs << name << src[i].y;

			sprintf(name, "dst_X_%d", i);
			fs << name << dst[i].x;
			sprintf(name, "dst_Y_%d", i);
			fs << name << dst[i].y;
		}


		fs.release();
	}
	
	/************************************************************************/
	/* Transform matrix 로드												*/
	/************************************************************************/
	void loadInfo(char* fileName, ofRectangle rect) {

		FileStorage fs(fileName, FileStorage::READ);
		fs.open(fileName, FileStorage::READ);
		
		fs["translate"] >> translate;

		for (int i = 0; i < 4; i++) {
			char name[100];
			sprintf(name, "vertices_X_%d", i);
			vertices[i].x = fs[name];
			sprintf(name, "vertices_Y_%d", i);
			vertices[i].y = fs[name];

			sprintf(name, "cvsrc_X_%d", i);
			cvsrc[i].x = fs[name];
			sprintf(name, "cvsrc_Y_%d", i);
			cvsrc[i].y = fs[name];

			sprintf(name, "cvdst_X_%d", i);
			cvdst[i].x = fs[name];
			sprintf(name, "cvdst_Y_%d", i);
			cvdst[i].y = fs[name];

			sprintf(name, "src_X_%d", i);
			src[i].x = fs[name];
			sprintf(name, "src_Y_%d", i);
			src[i].y = fs[name];

			sprintf(name, "dst_X_%d", i);
			dst[i].x = fs[name];
			sprintf(name, "dst_Y_%d", i);
			dst[i].y = fs[name];
		}

		fs.release();
	}

	/************************************************************************/
	/* Point 배열 변환														*/
	/************************************************************************/
	vector<cv::Point2f> transformPoints(vector<cv::Point2f> inputPoints) {
		std::vector<cv::Point2f> ret;

		for (int i = 0; i < inputPoints.size(); i++) {
			inputPoints[i].x /= 640;
			inputPoints[i].y /= 480;

			inputPoints[i].x *= 287.5;
			inputPoints[i].y *= 225;
		}
		
		perspectiveTransform(inputPoints, ret, translate);

		//포인트 변환 x:300 y:200 부터 시작
		for (int i = 0; i < ret.size(); i++) {
			ret[i].x -= 300;
			ret[i].y -= 200;

			//Normalization 진행
			ret[i].x /= 1150;
			ret[i].y /= 450;
		}

		return ret;
	}
		

private:
};
