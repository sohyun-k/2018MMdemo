#pragma once
class Vertex : public cv::Point2f
{
private:
	bool bBeingDragged;
	bool bOver;
	float radius = 12;
public:
	Vertex() { this->bBeingDragged = false; this->bOver = false; }
	void setDraggable() { this->bBeingDragged = true; }
	void setNotDraggable() { this->bBeingDragged = false; }
	void setOverTrue() { this->bOver = true; }
	void setOverFalse() { this->bOver = false; }
	float getRadius() { return radius; }
	bool isDraggable() { return this->bBeingDragged; }
	bool isOver() { return this->bOver; }
};

class Vertices
{
public:
	Vertex vertices[4];
};