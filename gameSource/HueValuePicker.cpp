#include "HueValuePicker.h"

#include "common.h"


HueValuePicker::HueValuePicker( double inAnchorX, double inAnchorY,
                                    double inWidth, double inHeight )
        : BorderPanel( inAnchorX - 2, inAnchorY - 2, 
                       inWidth + 4, inHeight + 4,
                       new Color( 0, 0, 0, 1 ),
                       new Color( 0.35, 0.35, 0.35, 1 ),
                       1 ),
          mPressed( false ) {
    
    mH = 0;
    mV = 1;


    double innerW = mWidth -2;
    double innerH = mHeight -2;

    mFieldImage = new Image( (int)innerW, (int)innerH, 3, false );
    
    double *channels[3] = 
        { mFieldImage->getChannel( 0 ),
          mFieldImage->getChannel( 1 ),
          mFieldImage->getChannel( 2 ) };
    
    for( int y=0; y<innerH; y++ ) {
        float V = (innerH - y - 1) / innerH;

        for( int x=0; x<innerW; x++ ) {
            int index = y * (int)innerW + x;
            
            float H = x / innerW;
            
            Color *c = Color::makeColorFromHSV( H, 1, V );
                        
            channels[0][index] = c->r;
            channels[1][index] = c->g;
            channels[2][index] = c->b;
            
            delete c;
            }
        }

    //writeTGA( mFieldImage, "testHV.tga" );
    
    mFieldSprite = new Sprite( mFieldImage );
    //mFieldSprite = new Sprite( "../testHV.tga" );

    mColorSpot = new Sprite( "colorSpot.tga", true );
    }




HueValuePicker::~HueValuePicker() {
    delete mFieldImage;
    delete mFieldSprite;
    delete mColorSpot;
    }



float HueValuePicker::getSelectedHue() {
    return mH;
    }

float HueValuePicker::getSelectedValue() {
    return mV;
    }



void HueValuePicker::setValues( float inHue, float inValue ) {
    mH = inHue;
    mV = inValue;
    }



void HueValuePicker::mouseActivity( double inX, double inY ) { 
    if( isInside( inX, inY ) && mPressed ) {

        inY --;
        
        double innerW = mWidth -3;
        double innerH = mHeight -3;
        if( inX < mAnchorX + 1 ) {
            inX = mAnchorX + 1;
            }
        if( inY < mAnchorY + 1 ) {
            inY = mAnchorY + 1;
            }
        
        if( inX > mAnchorX + 1 + innerW ) {
            inX = mAnchorX + 1 + innerW;
            }
        if( inY > mAnchorY + 1 + innerH ) {
            inY = mAnchorY + 1 + innerH;
            }
        
        
        mH = (float)(inX - (mAnchorX + 1)) / innerW;
        mV = (float)(inY - (mAnchorY + 1)) / innerH;

        fireActionPerformed( this );
        }
    }

                
void HueValuePicker::mousePressed( double inX, double inY ) {
    mPressed = true;
    mouseActivity( inX, inY );
    }

void HueValuePicker::mouseDragged( double inX, double inY ) {
    mouseActivity( inX, inY );
    }

void HueValuePicker::mouseReleased( double inX, double inY ) {
    mouseActivity( inX, inY );
    mPressed = false;
    }



void HueValuePicker::fireRedraw() {
    BorderPanel::fireRedraw();

    Vector3D pos( mAnchorX + mWidth / 2, mAnchorY + mHeight / 2, 0 );
    
    mFieldSprite->draw( 0, 0, &pos );

    double innerW = mWidth -3;
    double innerH = mHeight -3;

    Vector3D spotPos( mAnchorX + 1 + innerW * mH, 
                      mAnchorY + 2 + innerH * mV, 0 );

    mColorSpot->draw( 0, 0, &spotPos );
    }



