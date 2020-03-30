#include "ofApp.h"


#include "MyGuiView.h"

//  Note:
//      If the app is not compiling, try removing the MyGuiView.xib reference
//      from the xcode project and adding it back to the project.
//      Set the deployment target in project / General / Deployment Target >= 5.1

MyGuiView * myGuiViewController;


float CCW(ofVec2f p1,ofVec2f p2,ofVec2f p3){
    return p1.x*p2.y + p2.x*p3.y + p3.x*p1.y - p1.y*p2.x - p2.y*p3.x - p3.y*p1.x;
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetOrientation(ofOrientation::OF_ORIENTATION_DEFAULT);
    ofBackground(0,0,0);
    scale = 100.0;
    cam.setDistance(scale);
    
    ofFile is("total/rotation.txt", ofFile::ReadOnly);
    
    for(int i = 0 ; i < CLOUD_COUNT;i++){
        char name[100];
        ofMesh tempMesh;
        tempMesh.setMode(OF_PRIMITIVE_POINTS);
        tempMesh.enableColors();
        
        sprintf(name,"total/%d.txt",i);
        tempMesh.load(name);
        
        ofMesh tempMesh1;
        for (int j = 0; j < tempMesh.getVertices().size(); j++) {
            tempMesh1.setMode(OF_PRIMITIVE_POINTS);
            tempMesh1.enableColors();
            tempMesh1.addColor(ofColor(tempMesh.getColors()[j].r, tempMesh.getColors()[j].g, tempMesh.getColors()[j].b, tempMesh.getColors()[j].a));
            
            tempMesh.getVertices()[j].x /= 10; tempMesh.getVertices()[j].y /= 10; tempMesh.getVertices()[j].z /= 10;
            tempMesh1.addVertex(ofVec3f(tempMesh.getVertices()[j]));
        }
        
        mesh.push_back(tempMesh1);
        
        ofPoint rot;
        ofPoint tran;
       
        is>> rot.x >> rot.y >> rot.z >> tran.x>> tran.y>> tran.z;
        cout <<rot.x <<" "<< rot.y <<" "<< rot.z <<" "<<  tran.x<<" "<< tran.y<<" "<<  tran.z<<endl;;
        cameraPosition.push_back(tran);
        cameraRotation.push_back(rot);
    }
    is.close();
    
    
    //Projection region
    for(int i = 0 ; i < PROJECTION_SIZE; i++){
        Projection p;
        p.b_texture = false;
        projection.push_back(p);
    }
    
    ///0000////////////////////////////////////////////////////////////////////////
    
    projection[0].position.enableIndices();
    projection[0].position.enableTextures();
    projection[0].position.enableColors();
    projection[0].position.enableNormals();
    projection[0].position.setupIndicesAuto();
    
    projection[0].position.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    ofVec3f t3(-97.0901, 309.311, 84.911);
    ofVec3f t2(-3.0888, 217.611, 87.511);
    ofVec3f t1(-57.5898, 165.411, 87.911);
    ofVec3f t0(-152.489, 255.811, 87.011);
    projection[0].position.addVertex(t0);    projection[0].position.addVertex(t1);    projection[0].position.addVertex(t2);    projection[0].position.addVertex(t3);
    projection[0].position.addTexCoord(ofVec2f(0, 0)); projection[0].position.addTexCoord(ofVec2f(1, 0));projection[0].position.addTexCoord(ofVec2f(1, 1)); projection[0].position.addTexCoord(ofVec2f(0, 1));
    
    projection[0].position.addTriangle(0, 1, 2); projection[0].position.addTriangle(2, 3, 0);
    projection[0].position.addTriangle(0, 2, 1); projection[0].position.addTriangle(2, 0, 3);
    projection[0].position.addTriangle(1, 2, 3); projection[0].position.addTriangle(1, 3, 2);
    
    
    ///1111////////////////////////////////////////////////////////////////////////
    projection[1].position.enableIndices();
    projection[1].position.enableTextures();
    projection[1].position.enableColors();
    projection[1].position.enableNormals();
    projection[1].position.setupIndicesAuto();
    
    projection[1].position.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    ofVec3f c3(-272.489, 147.811, 61.011);
    ofVec3f c2(-167.59,44.411,54.911);
    ofVec3f c1(-172.089,50.611,-35.489);
    ofVec3f c0(-274.09,152.311,-24.089);
    
    projection[1].position.addVertex(c0);	projection[1].position.addVertex(c1);	projection[1].position.addVertex(c2);	projection[1].position.addVertex(c3);
    
    projection[1].position.addTexCoord(ofVec2f(0, 0)); projection[1].position.addTexCoord(ofVec2f(1, 0));projection[1].position.addTexCoord(ofVec2f(1, 1)); projection[1].position.addTexCoord(ofVec2f(0, 1));
    
    projection[1].position.addTriangle(0, 1, 2); projection[1].position.addTriangle(2, 3, 0);
    projection[1].position.addTriangle(0, 2, 1); projection[1].position.addTriangle(2, 0, 3);
    projection[1].position.addTriangle(1, 2, 3); projection[1].position.addTriangle(1, 3, 2);
    
    ///2222////////////////////////////////////////////////////////////////////////
    
    projection[2].position.enableIndices();
    projection[2].position.enableTextures();
    projection[2].position.enableColors();
    projection[2].position.enableNormals();
    projection[2].position.setupIndicesAuto();
    
    projection[2].position.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
   
    ofVec3f in0(-379.59,373.411,66.911);
    ofVec3f in1(-249.489,510.811,68.011);
    ofVec3f in2(-247.09,511.311,-58.089);
    ofVec3f in3(-383.089,370.611,-53.489);
    projection[2].position.addVertex(in0);	projection[2].position.addVertex(in1);	projection[2].position.addVertex(in2);	projection[2].position.addVertex(in3);
    projection[2].position.addTexCoord(ofVec2f(1, 1)); projection[2].position.addTexCoord(ofVec2f(0, 1));projection[2].position.addTexCoord(ofVec2f(0, 0)); projection[2].position.addTexCoord(ofVec2f(1, 0));
    
    projection[2].position.addTriangle(0, 1, 2); projection[2].position.addTriangle(2, 3, 0);
    projection[2].position.addTriangle(0, 2, 1); projection[2].position.addTriangle(2, 0, 3);
    projection[2].position.addTriangle(1, 2, 3); projection[2].position.addTriangle(1, 3, 2);
    
    ///3333////////////////////////////////////////////////////////////////////////
    
    projection[3].position.enableIndices();
    projection[3].position.enableTextures();
    projection[3].position.enableColors();
    projection[3].position.enableNormals();
    projection[3].position.setupIndicesAuto();
    
    projection[3].position.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    ofVec3f r12(-195.59, 576.411,64.911);
    ofVec3f r13(-54.489,449.811,69.011);
    ofVec3f r10(-46.09,440.311,-76.089);
    ofVec3f r11(-186.089,568.611,-72.489);
    projection[3].position.addVertex(r10);	projection[3].position.addVertex(r11);	projection[3].position.addVertex(r12);	projection[3].position.addVertex(r13);
    projection[3].position.addTexCoord(ofVec2f(0, 0)); projection[3].position.addTexCoord(ofVec2f(1, 0));projection[3].position.addTexCoord(ofVec2f(1, 1)); projection[3].position.addTexCoord(ofVec2f(0, 1));
    projection[3].position.addTriangle(0, 1, 2); projection[3].position.addTriangle(2, 3, 0);
    projection[3].position.addTriangle(0, 2, 1); projection[3].position.addTriangle(2, 0, 3);
    projection[3].position.addTriangle(1, 2, 3); projection[3].position.addTriangle(1, 3, 2);
    
    
    //Texture setup
    //image.load("1.png");
    //texture = image.getTexture();
    
    //selection setup
    selectionUI.setup();
    
    //update modifyscene
    modifyScene.contentsFileName = "projection_image/0.jpg";
    modifyScene.bDrawContents = false;
    modifyScene.bDrawDragPoints = false;
    ofRectangle rect(1240,1050,440,250);
    modifyScene.loadContents(rect);
    
    selectedIndex = -1;
    
    //Our Gui setup
    myGuiViewController	= [[MyGuiView alloc] initWithNibName:@"MyGuiView" bundle:nil];
    [ofxiOSGetGLParentView() addSubview:myGuiViewController.view];
    myGuiViewController.view.hidden = YES;
    image_background.load("background.png");
}

//--------------------------------------------------------------
void ofApp::update(){
    touchAnim.update(0.0015f);
    
    if(selectedIndex != -1){
        if(b_modify){
            modifyScene.bDrawContents = true;
            modifyScene.bDrawDragPoints = true;
            
            b_modify = NO;
            selectedIndex = -1;
        }else if(b_select){
            /*TODO*/
            selectionUI.isActive = YES;
            
            b_select = NO;
        }else if(b_delete){
            vector<Projection> projection_temp;
            for(int i = 0; i<projection.size();i++){
                if(i != selectedIndex){
                    projection_temp.push_back(projection[i]);
                }
            }
            projection = projection_temp;
            b_delete = NO;
            selectedIndex = -1;
        }else if(b_projection){
            /*TODO*/
            
            ofSystemAlertDialog("Waiting for projection");
            
            b_projection = NO;
            selectedIndex = -1;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(modifyScene.bDrawContents){
        image_background.draw(0,0,ofGetWidth(),ofGetHeight());
        glDisable(GL_DEPTH_TEST);
        ofPushMatrix();
        ofPushStyle();
        //modifyScene.draw();
        ofPopStyle();
        ofPopMatrix();
        //return;
    }else if(selectionUI.isActive){
        selectionUI.draw();
        return;
    }
    
    ofBackground(ofColor::black);
    glEnable(GL_DEPTH_TEST);
    
    cam.begin();
    cam.setDistance(scale);
    
    ofPushMatrix();
    ofRotateY(-90);
    //ofDrawGridPlane(20);
    ofPopMatrix();
    
    ofPushMatrix();
    ofPushStyle();
    ofRotateX(-90);
    
    ofFill();
    glPointSize(2);
    ofDrawAxis(10);
    
    glEnable(GL_DEPTH_TEST);
    double a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
    
    for (int i = 0; i < mesh.size(); i++) {
        double translateX = cameraPosition.at(i).x;
        double translateY = cameraPosition.at(i).y;
        double translateZ = cameraPosition.at(i).z;
        
        double rotateX = cameraRotation.at(i).x;
        double rotateY = cameraRotation.at(i).y;
        double rotateZ = cameraRotation.at(i).z;
        
        ofPushMatrix();
        ofTranslate(translateX + a, translateY + b, translateZ + c);
        ofRotateX(rotateX + d);
        ofRotateY(rotateY + e);
        ofRotateZ(rotateZ + f);
        //mesh.at(i).enableColors();
        
        mesh.at(i).draw();
        ofPopMatrix();
        
        ofPushMatrix();
        ofTranslate(translateX + a, translateY + b, translateZ + c);
        ofRotateX(rotateX + d);
        ofRotateY(rotateY + e);
        ofRotateZ(rotateZ + f);
        //ofDrawAxis(8);
        ofPopMatrix();
        
        a += cameraPosition.at(i).x;
        b += cameraPosition.at(i).y;
        c += cameraPosition.at(i).z;
        
        d += cameraRotation.at(i).x;
        e += cameraRotation.at(i).y;
        f += cameraRotation.at(i).z;
    }
    ofPopMatrix();
    ofPopStyle();
    
    glDisable(GL_DEPTH_TEST);
    
    ofPushMatrix();
    ofPushStyle();
    
    for (int i = 0; i < projection.size(); i++) {
        if(projection.at(i).b_texture){
            bindMyTexture(projection.at(i).texture);
            projection.at(i).position.draw();
            unbindMyTexture(projection.at(i).texture);
        }
        
        ofPushStyle();
        ofSetColor(255, 0, 0);
        ofFill();
        ofSetLineWidth(8);
        projection.at(i).position.drawWireframe();
        ofPopStyle();

    }
    
    ofVec3f t0(-97.0901, 309.311, 84.911);
    ofVec3f t1(-3.0888, 217.611, 87.511);
    ofVec3f t2(-57.5898, 165.411, 87.911);
    ofVec3f t3(-152.489, 255.811, 87.011);
    
    ofVec3f c0(-272.489, 147.811, 61.011);
    ofVec3f c1(-167.59,44.411,54.911);
    ofVec3f c2(-172.089,50.611,-35.489);
    ofVec3f c3(-274.09,152.311,-24.089);
    
    ofVec3f in0(-379.59,373.411,66.911);
    ofVec3f in1(-249.489,510.811,68.011);
    ofVec3f in2(-247.09,511.311,-58.089);
    ofVec3f in3(-383.089,370.611,-53.489);
    
    ofVec3f r12(-195.59, 576.411,64.911);
    ofVec3f r13(-54.489,449.811,69.011);
    ofVec3f r10(-46.09,440.311,-76.089);
    ofVec3f r11(-186.089,568.611,-72.489);
    
    ofPushStyle();
    ofSetColor(255, 0, 0);
    ofFill();
    ofSetLineWidth(8);
    
    ofDrawLine(t0, t1);
    ofDrawLine(t1, t2);
    ofDrawLine(t2, t3);
    ofDrawLine(t3, t0);
    ofDrawLine(t0, t2);
    ofDrawLine(t1, t3);
    
    ofDrawLine(c0, c1);
    ofDrawLine(c1, c2);
    ofDrawLine(c2, c3);
    ofDrawLine(c3, c0);
    ofDrawLine(c0, c2);
    ofDrawLine(c1, c3);

    ofDrawLine(in0, in1);
    ofDrawLine(in1, in2);
    ofDrawLine(in2, in3);
    ofDrawLine(in3, in0);
    ofDrawLine(in0, in2);
    ofDrawLine(in1, in3);
    
    
    
    ofDrawLine(r12, r13);
    ofDrawLine(r13, r10);
    ofDrawLine(r10, r11);
    ofDrawLine(r11, r12);
    ofDrawLine(r12, r10);
    ofDrawLine(r13, r11);
    
    ofPopStyle();

    ofPopStyle();
    ofPopMatrix();
    
    cam.end();
    
    
    //ofPushStyle();
    //if(touchAnim.touches.size()>0)
    //    touchAnim.draw();
    //ofPopStyle();
    
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){
    if(modifyScene.bDrawContents){
        modifyScene.mousePressed(touch.x, touch.y, 0);
        return;
    }
    
    cam.enableMouseInput();
    touchAnim.addTouch( touch.x, touch.y );
}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch){
    if(modifyScene.bDrawContents){
        modifyScene.mouseDragged(touch.x, touch.y, 0);
        return;
    }
    
    if(touchAnim.touches.size() > 1){
        cam.disableMouseInput();
        float ret = (touchAnim.moveTouch( touch.x, touch.y ));
        if(scale > -200){
            if(ret > 0 && ret != -9999){
                scale += 5;
            }else{
                scale -= 5;
            }
        }
        else
            scale = -199;
        //cam.enableMouseInput();
    }else
        cam.enableMouseInput();
}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){
    if(modifyScene.bDrawContents){
        modifyScene.mouseReleased(touch.x, touch.y, 0);
        return;
    }
    touchAnim.deleteTouch(touch.x, touch.y);
}

int te=0;
//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){
    if(selectionUI.isActive){
        //int ret = selectionUI.doubleTap(touch.x, touch.y);
        //if(ret != -1){
        int ret = selectedIndex;
            
            projection.at(selectedIndex).b_texture = true;
            char name[100];
            sprintf(name, "projection_image/%d.png",ret);
            
            ofImage t_image;
            t_image.load(name);
            projection.at(selectedIndex).texture = t_image.getTexture();
            
            selectionUI.isActive = false;
            selectedIndex = -1;
        //}
        return;
    }
    
    for(int i = 0; i< projection.size();i++){
        bool matched;
        ofVec2f points[4];
        for(int j = 0 ; j <4; j++)
            points[j] = cam.worldToScreen(projection.at(i).position.getVertices().at(j));
        
        for(int j = 0 ; j < 4; j++){
            if(CCW(points[j],points[(j+1)%4],ofVec2f(touch.x,touch.y))<0){
                matched = false;
                break;
            }
            matched = true;
        }
        
        if(matched == true){
            selectedIndex = i;
            cout<<"Selected Index : "<<selectedIndex<<endl;//Add Gui setup
            
            b_select = NO;
            b_delete = NO;
            b_modify = NO;
            b_projection = NO;
            
            myGuiViewController.view.hidden = NO;
            
            break;
        }else
            selectedIndex = -1;
    }
    
}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::lostFocus(){

}

//--------------------------------------------------------------
void ofApp::gotFocus(){
    
}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){
}

void ofApp::bindMyTexture(ofTexture texture) {
    ofTexture &tex = texture;
    tex.bind();
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    ofTextureData texData = tex.getTextureData();
        glScalef(tex.getWidth() / texData.tex_w, tex.getHeight() / texData.tex_h, 1.0f);
    
    glMatrixMode(GL_MODELVIEW);
}

void ofApp::unbindMyTexture(ofTexture texture) {
    texture.unbind();
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

