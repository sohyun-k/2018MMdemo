#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include <time.h>

// 데이터 전송시 안전하게 보내기 위해 2^16 byte 보내고 수신 확인을 받음
// 크기 수정 가능
#define SEGMENT_SIZE 65536

enum DataType {DATATYPE_TEXT, 
	DATATYPE_IMAGE, 
	DATATYPE_MESH,
	DATATYPE_BYTE, 
	DATATYPE_USHORT, 
	DATATYPE_INT, 
	DATATYPE_INT64, 
	DATATYPE_FLOAT, 
	DATATYPE_DOUBLE};

struct ofQuad {
	ofPoint p1, p2, p3, p4;
	float getWidth() {
		return (p2 - p1).length();
	}
	float getHeight() {
		return (p3 - p2).length();
	}
	float getArea() {
		return getWidth() * getHeight();
	}
	ofPoint getCenter() {
		return (p1 + p2 + p3 + p4) / 4.f;
	}

	ofVec3f getToGaroVec() {
		return p2 - p1;
	}
	ofVec3f getToSeroVec() {
		return p4 - p1;
	}

	ofPoint& operator[] (int i) {
		return ((ofPoint*)this)[i];
	}
	ofQuad operator+(ofPoint move) {
		ofQuad ret = *this;
		ret.p1 += move;
		ret.p2 += move;
		ret.p3 += move;
		ret.p4 += move;
		return ret;
	}
	ofQuad& operator=(ofQuad quad) {
		this->p1 = quad.p1;
		this->p2 = quad.p2;
		this->p3 = quad.p3;
		this->p4 = quad.p4;
		return *(ofQuad*)this;
	}
};

class ofWindow : public ofQuad {
protected:
	ofTexture* tex;

public:
	ofWindow() {
		tex = nullptr;
	}
	ofWindow(ofQuad& quad) {
		(*this) = quad;
	}

	void bind(ofTexture* _tex) {
		tex = _tex;
	}
	void unbind() {
		tex = nullptr;
	}
	void draw() {
		if (tex) {
			tex->draw(p1, p2, p3, p4);
		}
	}
	void draw(ofTexture* _tex) {
		bind(_tex);
		draw();
	}

	ofQuad& getVertices() {
		return *(ofQuad*)this;
	}
	ofWindow& operator=(ofQuad quad) {
		ofQuad::operator=(quad);
		return *this;
	}
};

typedef std::vector<ofWindow> ofMultiWindow;

inline vector<string> split(string str, char delimiter) {
	vector<string> internal;
	stringstream ss(str); // Turn the string into a stream.
	string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	return internal;
}

struct ClientInfo {
	bool isConnected = false;
	int clientID = -1;
	string clientIP = "000.000.000.000";
	int clientPort = -1;
	int numClients = 0;

	void init() {
		isConnected = false;
		clientID = -1;
		clientIP = "000.000.000.000";
		clientPort = -1;
		numClients = 0;
	}

	void set(int id, int port, string ip) {
		clientID = id;
		clientIP = ip;
		clientPort = port;
		isConnected = true;
	}
};