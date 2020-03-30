//
//  SelectionUI.hpp
//  Ambient Projection
//
//  Created by Yang on 2017. 3. 17..
//
//

#ifndef SelectionUI_h
#define SelectionUI_h

#define IMAGES_COUNT 7

class SelectionUI{
public:
    vector <ofImage> images;
    bool isActive;
    
    float CCW(ofVec2f p1,ofVec2f p2,ofVec2f p3){
        return p1.x*p2.y + p2.x*p3.y + p3.x*p1.y - p1.y*p2.x - p2.y*p3.x - p3.y*p1.x;
    }
    
    void setup(){
        ofImage tem;
        tem.load("images/0.jpg");
        images.push_back(tem);
        isActive = false;
    }
    
    void draw(){
        images.at(0).draw(0,0,ofGetWindowWidth(),ofGetWindowHeight());
    }
    
    int doubleTap(int x, int y){
        /*
        cout<<"ININ"<<endl;
        int ret = -1;
        for(int i = 0; i< images.size();i++){
            bool matched;
            
            for(int j = 0 ; j < 4; j++){
                if(CCW(points[i][j],points[i][(j+1)%4],ofVec2f(x,y))<0){
                    matched = false;
                    break;
                }
                matched = true;
            }
            
            if(matched == true){
                ret = i;
                cout<<"Selected Index : "<<ret<<endl;//Add Gui setup
                
                break;
            }else
                ret = -1;
        }
        */
        return 0;
    }
    
private:
    ofVec2f points[IMAGES_COUNT][4];
    
    
};



#endif /* SelectionUI_h */
