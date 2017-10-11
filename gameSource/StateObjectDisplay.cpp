#include "StateObjectDisplay.h"

#include "common.h"
#include "SpriteResource.h"
#include "TilePicker.h"

#include "minorGems/graphics/openGL/gui/TextGL.h"
#include "minorGems/util/stringUtils.h"



extern TilePicker *mainTilePicker;



static clickMask blankMask;


StateObjectDisplay::StateObjectDisplay( int inAnchorX, int inAnchorY )
        : GUIComponentGL( inAnchorX, inAnchorY,
                          G * P, G * P ),
          mLastPixelClickX( 0 ), mLastPixelClickY( 0 ),
          mLastGridClickX( 0 ), mLastGridClickY( 0 ),
          mLastActionRelease( false ),
          mLastActionPress( false ),
          mMouseHover( false ),
          mLastHoverX( 0 ), mLastHoverY( 0 ),
          mShowSelected( true ),
          mShowGrid( true ),
          mUseTrans( true ),
          mLastSelected( -2 ),
          mBlinkCycle( 0 ) {
    
    for( int y=0; y<P; y++ ) {
        for( int x=0; x<P; x++ ) {
            blankMask.opaque[y][x] = false;
            }
        }
    

    mZoomFactor = 1.0;
    }



StateObjectDisplay::~StateObjectDisplay() {
    
    int numSprites = mObjectSprites.size();
    for( int s=0; s<numSprites; s++ ) {
        delete *( mObjectSprites.getElement( s ) );
        delete *( mObjectSpriteHighlights.getElement( s ) );
        }
    }

        

void StateObjectDisplay::setStateObject( StateObject inStateObject,
                                         char inUseTrans ) {
    mUseTrans = inUseTrans;
    
    mStateObject = inStateObject;
    
    
    // clear old
    int numSprites = mObjectSprites.size();
    int i;
    for( i=0; i<numSprites; i++ ) {
        delete *( mObjectSprites.getElement( i ) );
        delete *( mObjectSpriteHighlights.getElement( i ) );
        }
    mObjectSprites.deleteAll();
    mObjectSpriteHighlights.deleteAll();
    mObjectSpritePositions.deleteAll();
    mObjectSpriteFades.deleteAll();
    mObjectSpriteGlows.deleteAll();
    mObjectMasks.deleteAll();
    

    int centerScreenPosX = (int)( P * (G/2) / mZoomFactor );
    int centerScreenPosY = (int)( P * (G/2 - mZoomFactor) / mZoomFactor );

    
    // add new
    numSprites = mStateObject.getNumLayers();
    for( int s=0; s<numSprites; s++ ) {
        
        SpriteResource resource( mStateObject.getLayerSprite( s ) );
        
        mObjectSprites.push_back( resource.getSprite( mUseTrans ) );
        mObjectSpriteHighlights.push_back( resource.getHighlightSprite() );
            
            
        intPair p = mStateObject.getLayerOffset( s );
        
        p.x += centerScreenPosX;
        p.y += centerScreenPosY;

        mObjectSpritePositions.push_back( p );

        mObjectSpriteFades.push_back( mStateObject.getLayerTrans( s ) /
                                      255.0f );

        mObjectSpriteGlows.push_back( mStateObject.getLayerGlow( s ) );
        
        clickMask mask;
        
        for( int y=0; y<P; y++ ) {
            for( int x=0; x<P; x++ ) {
                mask.opaque[y][x] = ! resource.getTrans( x, y );
                }
            }
        mObjectMasks.push_back( mask );
        }

    // last, add anchor sprite for this object, on top
    mObjectSprites.push_back( new Sprite( "anchor.tga", true ) );
    // no special highlight for anchor
    mObjectSpriteHighlights.push_back( new Sprite( "anchor.tga", true ) );
    
    intPair p = {0,0};
    p.x += centerScreenPosX;
    p.y += centerScreenPosY;
        
    mObjectSpritePositions.push_back( p );
    mObjectSpriteFades.push_back( 1.0f );
    mObjectSpriteGlows.push_back( false );
    mObjectMasks.push_back( blankMask );
    }




void StateObjectDisplay::updateSpritePositions( StateObject inStateObject ) {
    
    // don't need to touch masks


    mStateObject = inStateObject;

    int centerScreenPosX = (int)( P * (G/2) / mZoomFactor );
    int centerScreenPosY = (int)( P * (G/2 - mZoomFactor) / mZoomFactor );
    
    
    int numSprites = mStateObject.getNumLayers();    
    for( int s=0; s<numSprites; s++ ) {
        intPair p = mStateObject.getLayerOffset( s );
        
        p.x += centerScreenPosX;
        p.y += centerScreenPosY;

        *( mObjectSpritePositions.getElement( s ) ) = p;
        *( mObjectSpriteFades.getElement( s ) ) = 
            mStateObject.getLayerTrans( s ) /  255.0f;
        *( mObjectSpriteGlows.getElement( s ) ) = 
            mStateObject.getLayerGlow( s );
        }


    // anchor
    intPair p = {0,0};
    p.x += centerScreenPosX;
    p.y += centerScreenPosY;
        
    *( mObjectSpritePositions.getElement( numSprites ) ) = p;
    }



void StateObjectDisplay::showGrid( char inShow ) {
    mShowGrid = inShow;
    }


double StateObjectDisplay::getZoom() {
    return mZoomFactor;
    }

void StateObjectDisplay::setZoom( double inZoom ) {
    mZoomFactor = inZoom;
    updateSpritePositions( mStateObject );
    }

    

      

void StateObjectDisplay::fireRedraw() {

    int gridLimit = (int)( G / mZoomFactor );
         

    int selected = mStateObject.mSelectedLayer;
    
    if( mLastSelected != selected  || ! mShowGrid ) {
        // reset
        mBlinkCycle = 0;
        mLastSelected = selected;
        }
    else {
        mBlinkCycle ++;
        }

    
        
    if( mUseTrans ) {
        // background of selected tile
        
        Tile tile = (Tile)( mainTilePicker->getBackgroundTile() );
        
        Sprite *tileSprite = tile.getSprite();
        

        for( int y=0; y<gridLimit; y++ ) {
            for( int x=0; x<gridLimit; x++ ) {
                
                Vector3D pos( mAnchorX + ( x * P + P/2 ) * mZoomFactor, 
                              mAnchorY + (y * P + P/2) * mZoomFactor,
                              0 );
            
                tileSprite->draw( 0, 0, &pos, mZoomFactor );
                }
            }
        delete tileSprite;
        }


    // now draw all sprites
    int numSprites = mObjectSprites.size();
    
    //printf( "Drawing %d sprites, %d selected\n", numSprites, selected );
    
    for( int s=0; s<numSprites; s++ ) {
        Sprite *sprite = *( mObjectSprites.getElement( s ) );
        intPair pixelPos = *( mObjectSpritePositions.getElement( s ) );

        float fadeFactor = *( mObjectSpriteFades.getElement( s ) );

        
        
        Vector3D pos( mAnchorX + mZoomFactor * (pixelPos.x + P/2), 
                      mAnchorY + mZoomFactor * (pixelPos.y + P/2), 0 );
        
        char drawIt = true;
        
        Color c( 1, 1, 1, 1 );
        
        
        char isAnchor = false;
        
        if( s == numSprites - 1 ) {
            // an anchor
            isAnchor = true;
            fadeFactor *= 0.25;
            
            if( !mShowGrid ) {
                // hide anchors
                drawIt = false;
                }
            
            }

        
        /*
        if( ! isAnchor &&
            mShowGrid && 
            s == selected ) {
            
        
            // make it blink
            double blinkFadeFactor = 0.35 * cos( mBlinkCycle / 3.0 ) + 0.65;

            fadeFactor *= blinkFadeFactor;
            }
        */

            
        // skip sprites that are way out of bounds (due to zooming)
        if( pos.mX < mAnchorX || pos.mX > mAnchorX + mWidth 
            ||
            pos.mY < mAnchorY || pos.mY > mAnchorY + mHeight ) {
            drawIt = false;
            }
        
        if( drawIt ) {

            char glow = *( mObjectSpriteGlows.getElement( s ) );
            if( glow ) {
                // brighten only
                glBlendFunc( GL_SRC_ALPHA, GL_ONE );
                }

            sprite->draw( 0, 0, &pos, mZoomFactor, fadeFactor, &c );

            if( glow ) {
                // back to normal blend
                glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
                }
            
            
            if( ! isAnchor &&
                mShowGrid && 
                mShowSelected &&
                s == selected ) {
                
        
                // blinking highlight on top
                double blinkFadeFactor = 
                    0.25 * cos( mBlinkCycle / 3.0 ) + 0.25;
                
                Sprite *highlight = 
                    *( mObjectSpriteHighlights.getElement( s ) );
                
                highlight->draw( 0, 0, &pos, 
                                 mZoomFactor, 
                                 blinkFadeFactor, &c );
                }


            }
        }
    
    

    if( mShowGrid ) {
        //grid lines
        
        glColor4f( 1, 1, 1, 0.25f ); 
        glBegin( GL_LINES ); {
            

            for( int x=0; x<gridLimit+1; x++ ) {
                glVertex2d( mAnchorX + x * P * mZoomFactor, mAnchorY );
                glVertex2d( mAnchorX + x * P * mZoomFactor, 
                            mAnchorY + mHeight );
                }
            for( int y=0; y<gridLimit+1; y++ ) {
                glVertex2d( mAnchorX, mAnchorY + y * P * mZoomFactor );
                glVertex2d( mAnchorX + mWidth, 
                            mAnchorY + y * P * mZoomFactor );
                }
            }
        glEnd();
        }
    
        
    /*
    // thin white border, partly transparent
    glColor4f( 1, 1, 1, 0.25f ); 
    
    glBegin( GL_LINE_LOOP ); {		
        glVertex2d( mAnchorX, mAnchorY );
        glVertex2d( mAnchorX + mWidth, mAnchorY ); 
        glVertex2d( mAnchorX + mWidth, mAnchorY + mHeight );
            glVertex2d( mAnchorX, mAnchorY + mHeight );
        }
    glEnd();
    */
    
    }



void StateObjectDisplay::setLastMouse( double inX, double inY ) {
    
    double tempX = ( inX - mAnchorX ) - P * (G/2);
    double tempY = ( inY - mAnchorY ) - P * (G/2 - mZoomFactor);

    mLastPixelClickX = (int)( floor( tempX / mZoomFactor ) );
    mLastPixelClickY = (int)( floor( tempY / mZoomFactor ) );
    
    mLastPixelClickX -= P/2;
    mLastPixelClickY -= P/2;

    mLastHoverX = mLastPixelClickX;
    mLastHoverY = mLastPixelClickY;

    mLastGridClickX = (int)( mLastPixelClickX / P );
    mLastGridClickY = (int)( mLastPixelClickY / P );
    }



void StateObjectDisplay::mouseDragged( double inX, double inY ) {
    mLastActionRelease = false;
    mLastActionPress = false;
    
    if( isEnabled() && isInside( inX, inY ) ) {
        setLastMouse( inX, inY );
        fireActionPerformed( this );
        }
    }



void StateObjectDisplay::mousePressed( double inX, double inY ) {
    mLastActionRelease = false;
    mLastActionPress = true;

    if( isEnabled() && isInside( inX, inY ) ) {
        setLastMouse( inX, inY );
        fireActionPerformed( this );
        }
    }



void StateObjectDisplay::mouseReleased( double inX, double inY ) {
    mShowSelected = true;
    mLastActionRelease = true;
    mLastActionPress = false;
    
    if( isEnabled() && isInside( inX, inY ) ) {
        setLastMouse( inX, inY );
        fireActionPerformed( this );
        }
    }


void StateObjectDisplay::mouseMoved( double inX, double inY ) {
        
    if( isEnabled() && isInside( inX, inY ) ) {
        mMouseHover = true;
        setLastMouse( inX, inY );
        }
    else {
        mMouseHover = false;
        }
    }



int StateObjectDisplay::getClickedLayer( int inPixelClickX, 
                                         int inPixelClickY,
                                         int *outClickOffsetX, 
                                         int *outClickOffsetY ) {
    

    //printf( "Checking clicked layer for %d, %d\n", 
    //        inPixelClickX, inPixelClickY );
    
    int centerScreenPosX = (int)( P * (G/2) / mZoomFactor ) + P / 2;
    int centerScreenPosY = 
        (int)( P * (G/2 - mZoomFactor) / mZoomFactor ) + P / 2;
    

    int numLayers = mObjectMasks.size();
    
    // skip anchor, start at top, stop when a hit is found
    for( int i=numLayers-2; i>=0; i-- ) {
        intPair pos = *( mObjectSpritePositions.getElement( i ) );

        int posX = pos.x - centerScreenPosX;
        int posY = pos.y - centerScreenPosY;
        
        //printf( "  Layer %d at (%d,%d)\n", i, posX, posY );
    
        // first, do bounding box
        if( inPixelClickX >= posX && inPixelClickX < posX + P
            &&
            inPixelClickY >= posY && inPixelClickY < posY + P ) {
            
            clickMask *mask = mObjectMasks.getElement( i );
            
            int offsetY = P - ( inPixelClickY - posY ) - 1;
            int offsetX = inPixelClickX - posX;

            if( mask->opaque[ offsetY ][ offsetX ] ) {
                // hit!
                
                *outClickOffsetX = offsetX - P/2;
                *outClickOffsetY = offsetY - P/2;
                return i;
                }
            }
        }

    return -1;
    }

