#pragma once

#include "Scene.hpp"
#include "PanTiltSystem.hpp"
#include "DepthTouch.hpp"

class SceneManager
{
public:
	const static int TOTAL_SCENE_NUMBER = 12;
	Scene	scenes[TOTAL_SCENE_NUMBER];

	int		currentSceneIndex;
	Scene	*currentScene;

	double TimerCount;

	PanTiltSystem	*ptSystem;
	//ofxUICanvas *gui;
	DepthTouch *touch;

	/************************************************************************/
	/*	Constructor                                                         */
	/************************************************************************/
	SceneManager() : currentSceneIndex(0), currentScene(&scenes[0]) {}
	
	/************************************************************************/
	/*  Gets the pointer of current scene									*/
	/************************************************************************/
	Scene*	getCurrentScene() {	return currentScene; }

	/************************************************************************/
	/*	Changes the current scene to input scene index						*/
	/************************************************************************/
	void	changeCurrentScene(int sceneIndex)
	{
		touch->bTouchStart = false;

		ofLogNotice("Scene is changed from " + ofToString(currentSceneIndex) + " to " + ofToString(sceneIndex));

		// 이전 Scene의 Touchable 속성은 꺼줌
		//currentScene->isTouchable = false;

		// 이전 Scene이 Touchable이라면 Touch Vertex에 저장
		if(currentScene->isTouchable)
		{
			for(int i=0; i<4; ++i)
			{
				currentScene->touchVertices[i].x = touch->vertices[i].x;
				currentScene->touchVertices[i].y = touch->vertices[i].y;
			}
			currentScene->min = touch->minT;
			currentScene->max = touch->maxT;
			currentScene->offsetx = touch->touchPointOffset.x;
			currentScene->offsety = touch->touchPointOffset.y;
		}

		// 이전 Scene의 PanTilt 정보 저장
		ptSystem->prePan = scenes[currentSceneIndex].panAngle;
		ptSystem->preTilt = scenes[currentSceneIndex].tiltAngle;

		// 지정된 Scene으로 변경
		currentSceneIndex = sceneIndex;
		currentScene = &(scenes[sceneIndex]);

		//// GUI 수정
		//ofxUITextInput *ptInput;
		//ptInput = (ofxUITextInput *)gui->getWidget("PAN");
		//ptInput->setTextString(ofToString(currentScene->panAngle));
		//ptInput = (ofxUITextInput *)gui->getWidget("TILT");
		//ptInput->setTextString(ofToString(currentScene->tiltAngle));

		//ofxUIToggle* touchable = (ofxUIToggle*)gui->getWidget("TOUCHABLE");
		//touchable->setValue(currentScene->isTouchable);
		//
		//ptSystem->bSceneChange = true;	
		//TimerCount = ofGetElapsedTimeMillis();

		// KinectTouch setup
		if(currentScene->isTouchable) {
			for(int i=0; i<4; ++i)
			{
				touch->vertices[i].x = currentScene->touchVertices[i].x;
				touch->vertices[i].y = currentScene->touchVertices[i].y;
			}
			touch->sceneChanged();
			touch->minT = currentScene->min;
			touch->maxT = currentScene->max;
			touch->touchPointOffset.set(currentScene->offsetx, currentScene->offsety);
		}
	}




	/************************************************************************/
	/*	Loads/Saves scene information from/to the configuration file		*/
	/************************************************************************/
	bool	load(string fileName = "scenes.xml")
	{
		ofxXmlSettings settings;

		if(!settings.loadFile(fileName)){
			ofLogError("SceneManager: Scene configuration file is missing.");
			return false;
		}

		settings.pushTag("Scenes");
		for(int i=0; i< TOTAL_SCENE_NUMBER; ++i)
		{
			settings.pushTag("Scene", i);

			scenes[i].panAngle = settings.getValue("pan", 0);
			scenes[i].tiltAngle = settings.getValue("tilt", 0);
			scenes[i].isTouchable = settings.getValue("touchable", 0);

			for(int j=0; j<4; ++j){
				scenes[i].vertices[j].x =  settings.getValue("x", 0, j);
				scenes[i].vertices[j].y =  settings.getValue("y", 0, j);
			}

			if(scenes[i].isTouchable){
				for(int j=0; j<4; ++j){
					scenes[i].touchVertices[j].x = settings.getValue("touch_x", 0, j);
					scenes[i].touchVertices[j].y = settings.getValue("touch_y", 0, j);
				}
				scenes[i].min = settings.getValue("min", 0);
				scenes[i].max = settings.getValue("max", 0);
				scenes[i].offsetx = settings.getValue("offsetX", 0);
				scenes[i].offsety = settings.getValue("offsetY", 0);
			}

			scenes[i].contentsFileName = settings.getValue("contents","");

			if(!scenes[i].loadContents())
				ofLogError("SceneManager: "+ofToString(i) + "th Image is missing.\n");
			settings.popTag();
		}
		settings.popTag();

		// KinectTouch setup
		if(scenes[0].isTouchable) {
			for(int i=0; i<4; ++i)
			{
				touch->vertices[i].x = scenes[0].touchVertices[i].x;
				touch->vertices[i].y = scenes[0].touchVertices[i].y;
				touch->sceneChanged();
			}
			touch->minT = scenes[0].min;
			touch->maxT = scenes[0].max;
			touch->touchPointOffset.set(scenes[0].offsetx, scenes[0].offsety);
		}
		
		return true;
	}
	void	save(string fileName = "scenes.xml")
	{
		// 이전 Scene이 Touchable이라면 Touch Vertex에 저장
		if(currentScene->isTouchable)
		{
			for(int i=0; i<4; ++i)
			{
				currentScene->touchVertices[i].x = touch->vertices[i].x;
				currentScene->touchVertices[i].y = touch->vertices[i].y;
			}
			currentScene->min = touch->minT;
			currentScene->max = touch->maxT;
			currentScene->offsetx = touch->touchPointOffset.x;
			currentScene->offsety = touch->touchPointOffset.y;
		}

		ofxXmlSettings settings;

		settings.addTag("Scenes");
		settings.pushTag("Scenes");

		for(int i=0; i < TOTAL_SCENE_NUMBER; ++i)
		{
			// Scene을 Serialize하여 저장하면 편하겠는데 0.7.4버전에는 Serialize 기능 없음
			settings.addTag("Scene");
			settings.pushTag("Scene", i);
			
			settings.addValue("pan", scenes[i].panAngle);			
			settings.addValue("tilt", scenes[i].tiltAngle);			
			//settings.addValue("pan", 90);			
			//settings.addValue("tilt", 0);			
			settings.addValue("touchable", scenes[i].isTouchable);

			for(int j=0; j<4; ++j){
				settings.addValue("x", scenes[i].vertices[j].x);
				settings.addValue("y", scenes[i].vertices[j].y);
			}

			if(scenes[i].isTouchable){
				for(int j=0; j<4; ++j){
					settings.addValue("touch_x", scenes[i].touchVertices[j].x);
					settings.addValue("touch_y", scenes[i].touchVertices[j].y);
				}
				settings.addValue("min", scenes[i].min);
				settings.addValue("max", scenes[i].max);
				settings.addValue("offsetX", scenes[i].offsetx);
				settings.addValue("offsetY", scenes[i].offsety);
			}
			settings.addValue("contents", scenes[i].contentsFileName);
			settings.popTag();
		}
		settings.popTag();

		settings.saveFile(fileName);
	}




	/************************************************************************/
	/*	Sets a flag to draw drag-points										*/
	/************************************************************************/
	void setDrawDragPoints( bool bDrawDragPoints ) 
	{
		if(bDrawDragPoints)
			enableDrawDragPoints();
		else
			disableDrawDragPoints();
	}
	void enableDrawDragPoints( )
	{
		for(int i=0; i<TOTAL_SCENE_NUMBER; ++i)
			scenes[i].bDrawDragPoints = true;
	}
	void disableDrawDragPoints( )
	{
		for(int i=0; i<TOTAL_SCENE_NUMBER; ++i)
			scenes[i].bDrawDragPoints = false;
	}



	/************************************************************************/
	/*	Sets a flag to draw contents										*/
	/************************************************************************/
	void setDrawContents( bool bDrawContents ) 
	{
		if(bDrawContents)
			enableDrawContents();
		else
			disableDrawContents();
	}
	void enableDrawContents( )
	{
		for(int i=0; i<TOTAL_SCENE_NUMBER; ++i)
			scenes[i].bDrawContents = true;
		ofLogNotice("SceneManager: Drawing contents is on.");
	}
	void disableDrawContents( )
	{
		for(int i=0; i<TOTAL_SCENE_NUMBER; ++i)
			scenes[i].bDrawContents = false;
		ofLogNotice("SceneManager: Drawing contents is off.");
	}



	void resetScene(int sceneIndex)
	{
		scenes[sceneIndex].tiltAngle = TILT_DEFAULT;
		scenes[sceneIndex].panAngle = PAN_DEFAULT;
		// TODO: PTsystem에서 이동


		float w = scenes[sceneIndex].contentsImage.getWidth();
		float h = scenes[sceneIndex].contentsImage.getHeight();
		scenes[sceneIndex].vertices[0].x = (ofGetWidth() - w) * 0.5;
		scenes[sceneIndex].vertices[0].y = (ofGetHeight() - h) * 0.5;

		scenes[sceneIndex].vertices[1].x = (ofGetWidth() + w) * 0.5;
		scenes[sceneIndex].vertices[1].y = (ofGetHeight() - h) * 0.5;

		scenes[sceneIndex].vertices[2].x = (ofGetWidth() + w) * 0.5;
		scenes[sceneIndex].vertices[2].y = (ofGetHeight() + h) * 0.5;

		scenes[sceneIndex].vertices[3].x = (ofGetWidth() - w) * 0.5;;
		scenes[sceneIndex].vertices[3].y = (ofGetHeight() + h) * 0.5;

		scenes[sceneIndex].sceneChanged();
	}
private:

};