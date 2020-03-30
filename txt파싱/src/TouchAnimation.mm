
/*
 *  TouchAnimation.mm
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 28/03/12.
 *  Copyright 2012 uri.cat. All rights reserved.
 *
 */

#include "TouchAnimation.h"


void TouchAnimation::update(float dt){

	//to keep it simple, walk the vector backwards and delete dead touches easily
	for (int i = touches.size() - 1; i >= 0; i--){
		touch t = touches[i];
		t.time -= dt;
		touches[i] = t;
		if( touches[i].time <= 0){
			touches.erase( touches.begin() + i);
		}
	}
}


void TouchAnimation::addTouch(float x, float y, bool highlight){
	
	touch t;
	t.pos = ofVec2f(x, y);
	t.time = TOUCH_ANIM_DURATION;	
	t.highlight = highlight;
	touches.push_back(t);	
}


void TouchAnimation::draw(){
    
    ofPushStyle();
	for (int i = 0; i < touches.size(); i++){
		touch t = touches[i];
		ofVec2f p = touches[i].pos;
		float lifePercent = (TOUCH_ANIM_DURATION - t.time) / TOUCH_ANIM_DURATION; //[0..1]
		float radius = TOUCH_ANIM_RADIUS * lifePercent;
		float alpha = 1.0f;
		if ( lifePercent < 0.5f){
			alpha = 1.0f;
		}else {
			alpha = 0.5 + 0.5 * cosf( -M_PI + 2.0f * M_PI * lifePercent );
		}

		ofPushStyle();
			ofNoFill();
			ofSetLineWidth(1);
			ofSetColor(255,255,255, alpha * 255);
			ofCircle(p.x, p.y, radius);
			ofFill();
			if (touches[i].highlight) ofSetColor(255,0,0, alpha * 128);
			else ofSetColor(255,255,255, alpha * 128);
			ofCircle(p.x, p.y, radius);
		ofPopStyle();
		
		ofSetColor(255,255,255, alpha * 255);
		ofDrawBitmapString( "["+ofToString(p.x, 1) + ", " +  ofToString(p.y, 1) + "]", p.x, p.y);
    }
    ofPopStyle();
}

float TouchAnimation::moveTouch(float x, float y){
    int minNum = 0;
    ofVec2f p1 = touches[0].pos;

    float min = abs(sqrt((p1.x - x)*(p1.x - x) + (p1.y - y) * (p1.y - y)));
    for(int i = 1;i<touches.size();i++){
        p1 = touches[1].pos;
        float distance = abs(sqrt((p1.x - x)*(p1.x - x) + (p1.y - y) * (p1.y - y)));

        if(min > distance){
            min = distance;
            minNum = i;
        }
    }
    
    touches[minNum].pos = ofVec2f(x,y);
    float ret =  pinch();
    
    return ret;
}

void TouchAnimation::deleteTouch(float x, float y){
    int minNum = 0;
    ofVec2f p1 = touches[0].pos;
    
    float min = abs(sqrt((p1.x - x)*(p1.x - x) + (p1.y - y) * (p1.y - y)));
    for(int i = 1;i<touches.size();i++){
        p1 = touches[1].pos;
        float distance = abs(sqrt((p1.x - x)*(p1.x - x) + (p1.y - y) * (p1.y - y)));
        
        if(min > distance){
            min = distance;
            minNum = i;
        }
    }
    vector<touch> touches_temp;
    for(int i = 0 ; i < touches.size(); i++){
        if(i != minNum)
            touches_temp.push_back(touches[i]);
    }
    touches = touches_temp;
    
}



float TouchAnimation::pinch(){
    if(touches.size() > 1){
        ofVec2f p1 = touches[0].pos;
        ofVec2f p2 = touches[1].pos;
        
        float distance = abs(sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)));
        
        if(preDistance == -9999){
            preDistance = distance;
            return 0;
        }else{
            float ret = preDistance - distance;
            
            preDistance = distance;
            
            if(ret > 0){
                return 1;
            }else if (ret == 0){
                return 0;
                
            }else
                return -1;
        }
    }
    else{
        preDistance = -9999;
    }
}
