#pragma once
#include "ofxXmlSettings.h"
#include "opencv.hpp"
#include "Vertices.h"


class WarpImg
{
private:
	std::vector<string> mobileContentsFileName;
	std::vector<string> mobileXmlFileName;
	
	int mobileNum;
	bool bDrawDragPoints;
	bool xmlExists = false;

	bool testImageSetup(std::vector<string> fileNames);
	void xmlSetup();
	void drawImage();
	void drawVertices();
	void setupInitVertices();

public:
	WarpImg() {};
	std::vector<ofImage> mobileContentsImage;
	std::vector<Vertices> mobileVertices;
	void testSetup(std::vector<string> fileNames);
	void mobileImageSetup(std::vector<ofImage> mobileImages);
	void xmlSave();
	void draw();
	void showDragPoints();
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y);
	void mousePressed(int x, int y);
	void mouseReleased(int x, int y);
};