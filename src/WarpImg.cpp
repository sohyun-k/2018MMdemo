#include "WarpImg.h"

bool WarpImg::testImageSetup(std::vector<string> fileNames)
{
	for (int index = 0; index < this->mobileNum; ++index) {

		ofImage ofImg;
		ofImg.loadImage(this->mobileContentsFileName.at(index) + ".jpg");
		this->mobileContentsImage.push_back(ofImg);

		// vertice조절
		if (this->xmlExists == false)
		{
			this->mobileVertices.at(index).vertices[1].x += this->mobileContentsImage.at(index).getWidth();
			this->mobileVertices.at(index).vertices[2].x += this->mobileContentsImage.at(index).getWidth();
			this->mobileVertices.at(index).vertices[2].y += this->mobileContentsImage.at(index).getHeight();
			this->mobileVertices.at(index).vertices[3].y += this->mobileContentsImage.at(index).getHeight();
		}
	}
	this->draw();
	return true;
}

void WarpImg::xmlSetup()
{

	int xmlNum = 0;
	for (int index = 0; index < this->mobileNum; ++index) {
		ofxXmlSettings xmlSettings;
		if (xmlSettings.loadFile(this->mobileContentsFileName.at(index) + ".xml")) {
			xmlSettings.addTag("Scenes");
			xmlSettings.pushTag("Scenes");
			xmlNum++;
			for (int verticeIdx = 0; verticeIdx < 4; ++verticeIdx)
			{
				this->mobileVertices.at(index).vertices[verticeIdx].x = xmlSettings.getValue("x", 0, verticeIdx);
				this->mobileVertices.at(index).vertices[verticeIdx].y = xmlSettings.getValue("y", 0, verticeIdx);
			}
			xmlSettings.popTag();
		}

	}
	if (xmlNum == this->mobileNum){
		this->xmlExists = true;
	}
	else {
		this->xmlExists = false;
	}

}

void WarpImg::drawImage()
{
	for (int index = 0; index < this->mobileNum; ++index) {
		ofPoint point[4];
		point[0].x = this->mobileVertices.at(index).vertices[0].x;
		point[0].y = this->mobileVertices.at(index).vertices[0].y;
		point[1].x = this->mobileVertices.at(index).vertices[1].x;
		point[1].y = this->mobileVertices.at(index).vertices[1].y;
		point[2].x = this->mobileVertices.at(index).vertices[2].x;
		point[2].y = this->mobileVertices.at(index).vertices[2].y;
		point[3].x = this->mobileVertices.at(index).vertices[3].x;
		point[3].y = this->mobileVertices.at(index).vertices[3].y;
		ofImage ofImg;
		ofImg.allocate(this->mobileContentsImage.at(index).getWidth(), this->mobileContentsImage.at(index).getHeight(), OF_IMAGE_COLOR);
		ofImg.setFromPixels(this->mobileContentsImage.at(index).getPixels());
		ofImg.getTexture().draw(point[0], point[1], point[2], point[3]);
	}
}

void WarpImg::drawVertices()
{
	for (int index = 0; index < this->mobileNum; ++index)
	{
		ofPushStyle();
		ofSetColor((255 / this->mobileNum) *index, (255 / this->mobileNum) *(this->mobileNum - index - 1), 255);
		if (bDrawDragPoints) {
			ofNoFill();
			ofBeginShape();
			for (int verticeIdx = 0; verticeIdx < 4; ++verticeIdx)
			{
				ofVertex(this->mobileVertices.at(index).vertices[verticeIdx].x, this->mobileVertices.at(index).vertices[verticeIdx].y);
			}
			ofEndShape(true);

			for (int verticeIdx = 0; verticeIdx < 4; ++verticeIdx)
			{
				ofNoFill();
				ofCircle(this->mobileVertices.at(index).vertices[verticeIdx].x, this->mobileVertices.at(index).vertices[verticeIdx].y, 10);
				if (this->mobileVertices.at(index).vertices[verticeIdx].isOver())
				{
					ofFill();
					ofCircle(this->mobileVertices.at(index).vertices[verticeIdx].x, this->mobileVertices.at(index).vertices[verticeIdx].y, 7);
				}
			}
		}
		ofPopStyle();
	}
}

void WarpImg::setupInitVertices()
{
	std::vector<ofPoint> initialPoints;
	ofPoint point;
	point.x = 0; point.y = 0;
	initialPoints.push_back(point);
	point.x = ofGetWidth() / 4; point.y = 0;
	initialPoints.push_back(point);
	point.x = ofGetWidth() / 2; point.y = 0;
	initialPoints.push_back(point);
	point.x = ofGetWidth() * 3 / 4; point.y = 0;
	initialPoints.push_back(point);
	point.x = 0; point.y = ofGetHeight() / 2;
	initialPoints.push_back(point);
	point.x = ofGetWidth() / 4; point.y = ofGetHeight() / 2;
	initialPoints.push_back(point);
	point.x = ofGetWidth() / 2; point.y = ofGetHeight() / 2;
	initialPoints.push_back(point);
	point.x = ofGetWidth() * 3 / 4; point.y = ofGetHeight() / 2;
	initialPoints.push_back(point);

	for (int index = 0; index < this->mobileNum; ++index) {
		Vertices tempVertex;
		this->mobileVertices.push_back(tempVertex);
		this->mobileVertices.at(index).vertices[0].x = initialPoints.at(index).x;
		this->mobileVertices.at(index).vertices[0].y = initialPoints.at(index).y;
		this->mobileVertices.at(index).vertices[1].x = initialPoints.at(index).x;
		this->mobileVertices.at(index).vertices[1].y = initialPoints.at(index).y;
		this->mobileVertices.at(index).vertices[2].x = initialPoints.at(index).x;
		this->mobileVertices.at(index).vertices[2].y = initialPoints.at(index).y;
		this->mobileVertices.at(index).vertices[3].x = initialPoints.at(index).x;
		this->mobileVertices.at(index).vertices[3].y = initialPoints.at(index).y;
	}
}

void WarpImg::testSetup(std::vector<string> fileNames)
{
	this->mobileNum = fileNames.size();
	this->mobileContentsFileName = fileNames;
	this->setupInitVertices();
	this->xmlSetup();
	this->testImageSetup(fileNames);
}

void WarpImg::mobileImageSetup(std::vector<ofImage> mobileImages)
{
	this->mobileNum = mobileImages.size();
	this->mobileContentsImage = mobileImages;
	this->setupInitVertices();
	this->xmlSetup();
	for (int index = 0; index < this->mobileNum; ++index) {
		// vertice조절
		if (this->xmlExists == false)
		{
			this->mobileVertices.at(index).vertices[1].x += this->mobileContentsImage.at(index).getWidth();
			this->mobileVertices.at(index).vertices[2].x += this->mobileContentsImage.at(index).getWidth();
			this->mobileVertices.at(index).vertices[2].y += this->mobileContentsImage.at(index).getHeight();
			this->mobileVertices.at(index).vertices[3].y += this->mobileContentsImage.at(index).getHeight();
		}
	}
	this->draw();
}

void WarpImg::xmlSave()
{
	for (int index = 0; index < this->mobileNum; ++index) {
		ofxXmlSettings xmlSettings;
		xmlSettings.addTag("Scenes");
		xmlSettings.pushTag("Scenes");
		for (int verticeIdx = 0; verticeIdx < 4; ++verticeIdx) {
			xmlSettings.addValue("x", this->mobileVertices.at(index).vertices[verticeIdx].x);
			xmlSettings.addValue("y", this->mobileVertices.at(index).vertices[verticeIdx].y);
		}
		xmlSettings.popTag();

		xmlSettings.saveFile(this->mobileContentsFileName.at(index) + ".xml");
	}
}

void WarpImg::draw()
{
	this->drawImage();
	this->drawVertices();
}

void WarpImg::showDragPoints()
{
	if (bDrawDragPoints) {
		bDrawDragPoints = false;
	}
	else
		bDrawDragPoints = true;
}

void WarpImg::mouseMoved(int x, int y)
{
	if (bDrawDragPoints) {
		for (int index = 0; index < this->mobileNum; ++index) {
			for (int verticeIdx = 0; verticeIdx < 4; ++verticeIdx)
			{
				float diffx = x - this->mobileVertices.at(index).vertices[verticeIdx].x;
				float diffy = y - this->mobileVertices.at(index).vertices[verticeIdx].y;
				float dist = sqrt(diffx*diffx + diffy*diffy);
				if (dist < this->mobileVertices.at(index).vertices[verticeIdx].getRadius())
				{
					this->mobileVertices.at(index).vertices[verticeIdx].setOverTrue();
				}
				else
				{
					this->mobileVertices.at(index).vertices[verticeIdx].setOverFalse();
				}
			}
		}
	}
}

void WarpImg::mouseDragged(int x, int y)
{
	if (bDrawDragPoints)
	{
		for (int index = 0; index < this->mobileNum; ++index) {
			for (int verticeIdx = 0; verticeIdx < 4; ++verticeIdx) {
				if (this->mobileVertices.at(index).vertices[verticeIdx].isDraggable()) {
					this->mobileVertices.at(index).vertices[verticeIdx].x = x;
					this->mobileVertices.at(index).vertices[verticeIdx].y = y;
				}
			}
		}
	}
}

void WarpImg::mousePressed(int x, int y)
{
	if (bDrawDragPoints)
	{
		for (int index = 0; index < this->mobileNum; ++index) {
			for (int verticeIdx = 0; verticeIdx < 4; ++verticeIdx) {
				float diffx = x - this->mobileVertices.at(index).vertices[verticeIdx].x;
				float diffy = y - this->mobileVertices.at(index).vertices[verticeIdx].y;
				float dist = sqrt(diffx*diffx + diffy*diffy);
				if (dist < this->mobileVertices.at(index).vertices[verticeIdx].getRadius()) {
					this->mobileVertices.at(index).vertices[verticeIdx].setDraggable();
				}
				else {
					this->mobileVertices.at(index).vertices[verticeIdx].setNotDraggable();
				}
			}
		}
	}
}

void WarpImg::mouseReleased(int x, int y)
{
	for (int index = 0; index < this->mobileNum; ++index) {
		for (int verticeIdx = 0; verticeIdx < 4; ++verticeIdx) {
			this->mobileVertices.at(index).vertices[verticeIdx].setNotDraggable();
		}
	}
	this->draw();
}
