#include "DragAndDropManager.h"
#include "ToolTipManager.h"



DragAndDropManager::DragAndDropManager( double inAnchorX, double inAnchorY, 
                                        double inWidth, double inHeight )
        : GUIComponentGL( inAnchorX, inAnchorY, inWidth, inHeight ),
          mNumSprites( 0 ),
          mSprites( NULL ),
          mOffsets( NULL ),
          mTrans( NULL ),
          mGlows( NULL ),
          mDragging( false ) {

    }



DragAndDropManager::~DragAndDropManager() {
    clear();
    }



void DragAndDropManager::clear() {
    if( mSprites != NULL ) {
        for( int i=0; i<mNumSprites; i++ ) {
            delete mSprites[i];
            }
        delete [] mSprites;
        delete [] mOffsets;
        delete [] mTrans;
        delete [] mGlows;
        }
    mSprites = NULL;
    mOffsets = NULL;
    mTrans = NULL;
    mGlows = NULL;
    mNumSprites = 0;
    }



void DragAndDropManager::setSprite( Sprite *inSprite, double inZoom ) {
    clear();
    
    mNumSprites = 1;
    mSprites = new Sprite*[1];
    mOffsets = new intPair[1];
    mTrans = new float[1];
    mGlows = new char[1];

    mSprites[0] = inSprite;
    mOffsets[0].x = 0;
    mOffsets[0].y = 0;
    mTrans[0] = 1.0f;
    mGlows[0] = false;
    
    mZoom = inZoom;
    }



void DragAndDropManager::setSprites( int inNumSprites,
                                     Sprite **inSprites, intPair *inOffsets,
                                     float *inTrans,
                                     char *inGlows,
                                     double inZoom ) {
    clear();
    mNumSprites = inNumSprites;
    mSprites = inSprites;
    mOffsets = inOffsets;
    mTrans = inTrans;
    mGlows = inGlows;
    mZoom = inZoom;
    }



char DragAndDropManager::isDragging() {
    return mDragging && ( mSprites != NULL );
    }



void DragAndDropManager::mousePressed( double inX, double inY ) {
    mX = inX;
    mY = inY;
    mDragging = false;
    }



void DragAndDropManager::mouseDragged( double inX, double inY ) {
    mX = inX;
    mY = inY;
    mDragging = true;
    }



void DragAndDropManager::mouseReleased( double inX, double inY ) {
    clear();

    ToolTipManager::freeze( false );
    ToolTipManager::setTip( NULL );
    

    mDragging = false;
    }



void DragAndDropManager::fireRedraw() {
    if( mSprites != NULL && mDragging ) {

        double extraX = 0;
        double extraY = 0;
        
        if( mZoom == 3 ) {
            extraY = -1;
            extraX = 1;
            }
        
        // stick to nearest zoom grid
        double pixelX = floor( (mX + extraX) / mZoom ) * mZoom;
        double pixelY = floor( (mY + extraY) / mZoom ) * mZoom + mZoom;
        
        if( mZoom == 3 ) {
            pixelX --;
            pixelY ++;
            }
        
        for( int i=0; i<mNumSprites; i++ ) {
            double thisSpriteX = pixelX + mOffsets[i].x * mZoom;
            double thisSpriteY = pixelY + mOffsets[i].y * mZoom;
            
            Vector3D pos( thisSpriteX, thisSpriteY, 0 );
        
            if( mGlows[i] ) {
                // brighten only
                glBlendFunc( GL_SRC_ALPHA, GL_ONE );
                }

            mSprites[i]->draw( 0, 0, &pos, mZoom, mTrans[i] );
            
            
            if( mGlows[i] ) {
                // back to normal blend
                glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
                }
            }
        }
    
    }

