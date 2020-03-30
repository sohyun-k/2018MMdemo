#pragma once
#import <AssetsLibrary/AssetsLibrary.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>
#include "ofxiOS.h"
#include "TouchAnimation.h"
#include "Scene.hpp"
#include "SelectionUI.hpp"
#include <opencv.hpp>


#define CLOUD_COUNT 7
#define PROJECTION_SIZE 4

typedef struct projection{
    ofMesh position;
    ofTexture texture;
    bool b_texture;
}Projection;

class ofApp : public ofxiOSApp{
	
    public:
        void setup();
        void update();
        void draw();
        void exit();
	
        void touchDown(ofTouchEventArgs & touch);
        void touchMoved(ofTouchEventArgs & touch);
        void touchUp(ofTouchEventArgs & touch);
        void touchDoubleTap(ofTouchEventArgs & touch);
        void touchCancelled(ofTouchEventArgs & touch);

        void lostFocus();
        void gotFocus();
        void gotMemoryWarning();
        void deviceOrientationChanged(int newOrientation);
    
    void bindMyTexture(ofTexture texture);
    void unbindMyTexture(ofTexture texture);
    
    
    ofEasyCam cam;
    float scale;
    TouchAnimation touchAnim;
    
    vector<ofMesh> mesh;
    vector<ofPoint> cameraPosition;
    vector<ofPoint> cameraRotation;
    
    
    vector<Projection> projection;

    ofVec3f doubleRet;
    int selectedIndex;
    
    UIViewController* myViewController;
    
    bool b_select;
    bool b_delete;
    bool b_modify;
    bool b_projection;
    
    Scene modifyScene;
    SelectionUI selectionUI;
    
    
    ofImage image_background;
};


