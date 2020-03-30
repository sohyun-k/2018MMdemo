//
//  MyGuiView.m
//  iPhone Empty Example
//
//  Created by theo on 26/01/2010.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MyGuiView.h"
#include "ofxiOSExtras.h"


@implementation MyGuiView

// called automatically after the view is loaded, can be treated like the constructor or setup() of this class
-(void)viewDidLoad {
	myApp = (ofApp*)ofGetAppPtr();
}

//----------------------------------------------------------------
-(void)setStatusString:(NSString *)trackStr{
	displayText.text = trackStr;
}



//----------------------------------------------------------------
-(IBAction)hide:(id)sender{
	self.view.hidden = YES;
}

//----------------------------------------------------------------
-(IBAction)adjustPoints:(id)sender{
	
	UISlider * slider = sender;
	cout << "slider value is - "  << [slider value] << "\n";
	
	//myApp->numPoints = 3 + [slider value] * 28;
	
	//string statusStr = " Status: numPoints is " + ofToString(myApp->numPoints);
	//[self setStatusString:ofxStringToNSString(statusStr)];
	
}

//----------------------------------------------------------------
-(IBAction)fillSwitch:(id)sender{
	
	UISwitch * toggle = sender;
	printf("switch value is - %i\n", [toggle isOn]);
	
	//myApp->bFill = [toggle isOn];
	
	//string statusStr = " Status: fill is " + ofToString(myApp->bFill);
	//[self setStatusString:ofxStringToNSString(statusStr)];
}

//----------------------------------------------------------------

-(IBAction)Modify:(id)sender{
    myApp->b_modify = YES;
    [self hide:0];
}
-(IBAction)Delete:(id)sender{
    
    myApp->b_delete = YES;
    [self hide:0];
}
-(IBAction)SelectInformation:(id)sender{
    myApp->b_select = YES;
    [self hide:0];
    
}
-(IBAction)Projection:(id)sender{
    myApp->b_projection = YES;
    [self hide:0];
}

@end
