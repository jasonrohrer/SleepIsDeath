#include "ToolTipSliderGL.h"
#include "ToolTipManager.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/util/TranslationManager.h"



ToolTipSliderGL::ToolTipSliderGL(
    double inAnchorX, double inAnchorY, double inWidth,
    double inHeight, Image *inIconImage,
    double inIconWidthFraction,
    Color *inBarStartColor, Color *inBarEndColor,
    Color *inThumbColor, Color *inBorderColor,
    double inBorderWidth,
    double inThumbWidth,
    double inMinThumbIncrement )
        : SliderGL( 
             inAnchorX, inAnchorY, inWidth,
             inHeight, inIconImage,
             inIconWidthFraction,
             inBarStartColor, inBarEndColor,
             inThumbColor, inBorderColor,
             inBorderWidth,
             inThumbWidth,
             inMinThumbIncrement),
          mHover( false ),
          mTip( NULL ) {
    }


        
ToolTipSliderGL::~ToolTipSliderGL() {
    if( mTip != NULL ) {
        delete [] mTip;
        }
    }


        
void ToolTipSliderGL::setToolTip( const char *inTipTranslationKey,
                                  char inUseTranslationManager ) {
    if( mTip != NULL ) {
        delete [] mTip;
        mTip = NULL;
        }

    if( inTipTranslationKey != NULL ) {
        char *string = (char *)inTipTranslationKey;
        
        if( inUseTranslationManager ) {    
            string = (char *)TranslationManager::translate( 
                (char*)inTipTranslationKey );
            }
        
        mTip = stringDuplicate( string );
        }
    }

        

void ToolTipSliderGL::mouseMoved( double inX, double inY ) {
    char oldHover = mHover;

    SliderGL::mouseMoved( inX, inY );
    
    mHover = isInside( inX, inY );    
    
    if( mHover && ( ! oldHover || ToolTipManager::getTip() == NULL ) ) {
        // mouse newly hovering or tip not set
        ToolTipManager::setTip( mTip );
        }
    else if( !mHover && oldHover ) {
        // mouse newly left
        ToolTipManager::setTip( NULL );
        }
    }



void ToolTipSliderGL::mouseDragged( double inX, double inY ) {
    char oldHover = mHover;
    
    SliderGL::mouseDragged( inX, inY );

    mHover = isInside( inX, inY );    
    
    if( mHover && ( ! oldHover || ToolTipManager::getTip() == NULL ) ) {
        // mouse newly hovering, or tip not set
        ToolTipManager::setTip( mTip );
        }
    else if( !mHover && oldHover ) {
        // mouse newly left
        ToolTipManager::setTip( NULL );
        }
    }



void ToolTipSliderGL::mouseReleased( double inX, double inY ) {
    SliderGL::mouseReleased( inX, inY );
    
    // always turn tip off after release inside button
    // this ensures that tip disappears even if button is completely removed
    // (not just disabled) as a result of the button push
    
    // if user moves mouse around in button again, tip will reappear anyway,
    // so it's okay.
    if( isInside( inX, inY ) ) {
        // hide tip
        ToolTipManager::setTip( NULL );
        }
    }
