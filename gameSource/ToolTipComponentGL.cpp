#include "ToolTipComponentGL.h"
#include "ToolTipManager.h"

#include "minorGems/util/TranslationManager.h"
#include "minorGems/util/stringUtils.h"

#include <stdio.h>


ToolTipComponentGL::ToolTipComponentGL( double inAnchorX, double inAnchorY, 
                                    double inWidth, double inHeight )
        : GUIComponentGL( inAnchorX, inAnchorY, inWidth, inHeight ),
          mHover( false ), mTip( NULL ) {
    }


        
ToolTipComponentGL::~ToolTipComponentGL() {
    if( mTip != NULL ) {
        delete [] mTip;
        }
    }


        
void ToolTipComponentGL::setToolTip( const char *inTipTranslationKey ) {
    if( mTip != NULL ) {
        delete [] mTip;
        mTip = NULL;
        }

    if( inTipTranslationKey != NULL ) {
        char *string = 
            (char *)TranslationManager::translate( 
                (char*)inTipTranslationKey );
        
        mTip = stringDuplicate( string );
        }
    }

        

void ToolTipComponentGL::mouseMoved( double inX, double inY ) {
    char oldHover = mHover;
    

    GUIComponentGL::mouseMoved( inX, inY );
    
    if( !isInside( inX, inY ) || !isEnabled() ) {
		// the mouse has been moved outside of us or we're disabled, 
        // so un-hover
		mHover = false;
		}
    else if( isEnabled() ){
        // moved back in, and we're enabled, so re-hover
        mHover = true;
        }

    if( mHover && ( ! oldHover || ToolTipManager::getTip() == NULL ) ) {
        // mouse newly hovering or tip not set
        ToolTipManager::setTip( mTip );
        }
    else if( !mHover && oldHover ) {
        // mouse newly left
        ToolTipManager::setTip( NULL );
        }
    
    }



void ToolTipComponentGL::mouseDragged( double inX, double inY ) {
    char oldHover = mHover;
    
    GUIComponentGL::mouseMoved( inX, inY );

    if( !isInside( inX, inY ) || !isEnabled() ) {
		// the mouse has been dragged outside of us or we're disabled
        mHover = false;
        }
    else if( isEnabled() ){
        // dragged back in, and we're enabled
        mHover = true;
        }

    
    if( mHover && ( ! oldHover || ToolTipManager::getTip() == NULL ) ) {
        // mouse newly hovering, or tip not set
        ToolTipManager::setTip( mTip );
        }
    else if( !mHover && oldHover ) {
        // mouse newly left
        ToolTipManager::setTip( NULL );
        }
    }
