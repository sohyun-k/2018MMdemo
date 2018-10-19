#pragma once
#include "ofxXmlSettings.h"
#include "opencv.hpp"
#include "Vertices.h"

class WarpVideo
{
private:
	std::vector<Vertices> mobileVertices;
	std::vector<string> mobileContentsFileName;
	std::vector<string> mobileXmlFileName;
	std::vector<ofVideoPlayer> mobileContentsVideo;
	int mobileNum;
	bool bDrawDragPoints;
	bool xmlExists = false;
	bool paused = true;

	bool testVideoSetup(std::vector<string> fileNames);
	void xmlSetup();
	void drawVideo();
	void drawVertices();
	void setupInitVertices();

public:
	WarpVideo() {};
	void testSetup(std::vector<string> fileNames);
	void mobileImageSetup(std::vector<ofVideoPlayer> mobileVideos);
	void xmlSave();
	void draw();
	void showDragPoints();
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y);
	void mousePressed(int x, int y);
	void mouseReleased(int x, int y);
	void setPaused(bool paused);
};