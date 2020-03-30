#include "ofApp.h"

int main() {
    
    //  here are the most commonly used iOS window settings.
    //------------------------------------------------------
    ofiOSWindowSettings settings;
    settings.enableRetina = true; // enables retina resolution if the device supports it.
    settings.enableDepth = true; // enables depth buffer for 3d drawing.
    settings.enableAntiAliasing = true; // enables anti-aliasing which smooths out graphics on the screen.
    settings.numOfAntiAliasingSamples = 4; // number of samples used for anti-aliasing.
    settings.enableHardwareOrientation = true; // enables native view orientation.
    settings.enableHardwareOrientationAnimation = true; // enables native orientation changes to be animated.
    settings.enableSetupScreen = true;
    settings.glesVersion = OFXIOS_RENDERER_ES1; // type of renderer to use, ES1, ES2, etc.
    settings.windowMode = ofWindowMode::OF_FULLSCREEN ;
    
    ofCreateWindow(settings);
    
	return ofRunApp(new ofApp);
}
