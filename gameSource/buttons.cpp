#include "buttons.h"
#include "ToolTipManager.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/util/TranslationManager.h"
#include "minorGems/util/log/AppLog.h"

#include <GL/gl.h>



ToolTipButtonGL::ToolTipButtonGL( double inAnchorX, double inAnchorY, 
                                    double inWidth, double inHeight )
        : ButtonGL( inAnchorX, inAnchorY, inWidth, inHeight ),
          mTip( NULL ) {
    }


        
ToolTipButtonGL::~ToolTipButtonGL() {
    if( mTip != NULL ) {
        delete [] mTip;
        }
    }


        
void ToolTipButtonGL::setToolTip( const char *inTipTranslationKey,
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

        

void ToolTipButtonGL::mouseMoved( double inX, double inY ) {
    char oldHover = mHover;
    
    ButtonGL::mouseMoved( inX, inY );
    
    if( mHover && ( ! oldHover || ToolTipManager::getTip() == NULL ) ) {
        // mouse newly hovering or tip not set
        ToolTipManager::setTip( mTip );
        }
    else if( !mHover && oldHover ) {
        // mouse newly left
        ToolTipManager::setTip( NULL );
        }
    }



void ToolTipButtonGL::mouseDragged( double inX, double inY ) {
    char oldHover = mHover;
    
    ButtonGL::mouseDragged( inX, inY );
    
    if( mHover && ( ! oldHover || ToolTipManager::getTip() == NULL ) ) {
        // mouse newly hovering, or tip not set
        ToolTipManager::setTip( mTip );
        }
    else if( !mHover && oldHover ) {
        // mouse newly left
        ToolTipManager::setTip( NULL );
        }
    }



void ToolTipButtonGL::mouseReleased( double inX, double inY ) {
    ButtonGL::mouseReleased( inX, inY );
    
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




//static rgbaColor white = {{255,255,255,255}};
//static rgbaColor gray = {{128,128,128,255}};
static rgbaColor gray = {{89,89,89,255}};
static rgbaColor black = {{0,0,0,255}};


BorderButtonGL::BorderButtonGL( double inAnchorX, double inAnchorY, 
                                double inWidth, double inHeight )
        : ToolTipButtonGL( inAnchorX, inAnchorY, inWidth, inHeight ),
          mBorderColor( gray ), mFillColor( black ) {
    }


void BorderButtonGL::setFillColor( rgbaColor inColor ) {
    mFillColor = inColor;
    }

void BorderButtonGL::setBorderColor( rgbaColor inColor ) {
    mBorderColor = inColor;
    }


void BorderButtonGL::drawBorder() {
    
    // 1 pixel wide
    glColor4f( mBorderColor.comp.r / 255.0f, 
               mBorderColor.comp.g / 255.0f, mBorderColor.comp.b / 255.0f, 1); 

    glBegin( GL_QUADS ); {		
        glVertex2d( mAnchorX, mAnchorY );
        glVertex2d( mAnchorX + mWidth, mAnchorY ); 
        glVertex2d( mAnchorX + mWidth, mAnchorY + mHeight );
        glVertex2d( mAnchorX, mAnchorY + mHeight );
        }
    glEnd();

    // fill
    glColor4f( mFillColor.comp.r / 255.0f, 
               mFillColor.comp.g / 255.0f, mFillColor.comp.b / 255.0f, 1 ); 

    glBegin( GL_QUADS ); {		
        glVertex2d( mAnchorX + 1, mAnchorY + 1 );
        glVertex2d( mAnchorX + mWidth - 1, mAnchorY + 1 ); 
        glVertex2d( mAnchorX + mWidth - 1, mAnchorY + mHeight - 1 );
        glVertex2d( mAnchorX + 1 , mAnchorY + mHeight - 1 );
        }
    glEnd();
    }



IconButtonGL::IconButtonGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight )
        : BorderButtonGL( inAnchorX, inAnchorY, inWidth, inHeight ) {
    }



void IconButtonGL::drawPressed() {
    drawBorder();

    glColor4f( 1, 1, 1, 1 );
    
    drawIcon();
    }


void IconButtonGL::drawUnpressed() {
    drawBorder();
    
    glColor4f( 1, 1, 1, 1 );
    
    drawIcon();
    }







AddButtonGL::AddButtonGL( double inAnchorX, double inAnchorY, 
                          double inWidth, double inHeight )
        : IconButtonGL( inAnchorX, inAnchorY, inWidth, inHeight ) {
    }


void AddButtonGL::drawIcon() {
    // a plus
    double edgeOffset = mWidth;
    if( mHeight > mWidth ) {
        edgeOffset = mHeight;
        }
    edgeOffset *= 0.25;
    
    // round to a whole number of pixels
    edgeOffset = (int)( edgeOffset );
    

    glBegin( GL_QUADS ); {		
        // 1 pixel wide
        glVertex2d( mAnchorX + mWidth/2 - 0.5, mAnchorY + edgeOffset );
        glVertex2d( mAnchorX + mWidth/2 + 0.5, mAnchorY + edgeOffset );
        glVertex2d( mAnchorX + mWidth/2 + 0.5, 
                    mAnchorY + mHeight - edgeOffset ); 
        glVertex2d( mAnchorX + mWidth/2 - 0.5, 
                    mAnchorY + mHeight - edgeOffset ); 

        
        glVertex2d( mAnchorX + edgeOffset, mAnchorY + mHeight/2 - 0.5 );
        glVertex2d( mAnchorX + edgeOffset, mAnchorY + mHeight/2 + 0.5 );
        glVertex2d( mAnchorX + mWidth - edgeOffset, 
                    mAnchorY + mHeight/2 + 0.5 );
        glVertex2d( mAnchorX + mWidth - edgeOffset, 
                    mAnchorY + mHeight/2 - 0.5 );
        }
    glEnd();
    }










SpriteButtonGL::SpriteButtonGL( Sprite *inSprite,
                                double inSpriteDrawScale,
                                double inAnchorX, double inAnchorY, 
                                double inWidth, double inHeight )
        : BorderButtonGL( inAnchorX, inAnchorY, inWidth, inHeight ),
          mSprite( inSprite ),
          mSpriteDrawScale( inSpriteDrawScale ),
          mColor( NULL ) {

    }


        
SpriteButtonGL::~SpriteButtonGL() {
    setSprite( NULL );
    
    if( mColor != NULL ) {
        delete mColor;
        }
    }


void SpriteButtonGL::setColor( Color *inColor ) {
    if( mColor != NULL ) {
        delete mColor;
        }
    mColor = inColor;
    }




void SpriteButtonGL::setSprite( Sprite *inSprite ) {
    if( mSprite != NULL ) {
        delete mSprite;
        }
    mSprite = inSprite;
    }

        
void SpriteButtonGL::drawPressed() {
    if( mEnabled ) {
        drawBorder();

        if( mSprite != NULL ) {
            
            Vector3D pos( mAnchorX + mWidth / 2, mAnchorY + mHeight/2, 0 );
        
            // a bit darker when pressed
            Color c( .75, .75, .75, 1 );
            
            if( mColor != NULL ) {
                c.r *= mColor->r;
                c.g *= mColor->g;
                c.b *= mColor->b;
                }
            
            mSprite->draw( 0, 0, &pos, mSpriteDrawScale, 1, &c );
            }
        }
    }

    

void SpriteButtonGL::drawUnpressed() {
    if( mEnabled ) {
        drawBorder();

        if( mSprite != NULL ) {
            
            Vector3D pos( mAnchorX + mWidth / 2, mAnchorY + mHeight/2, 0 );

            mSprite->draw( 0, 0, &pos, mSpriteDrawScale, 1, mColor );
            }
        }
    }





ColorButtonGL::ColorButtonGL( rgbaColor inColor,
                                   double inAnchorX, double inAnchorY, 
                                   double inWidth, double inHeight ) 
        : BorderButtonGL( inAnchorX, inAnchorY, inWidth, inHeight ),
          mLastActionHover( false ) {

    setColor( inColor );
    }

                        
        
void ColorButtonGL::setColor( rgbaColor inColor, char inForceDarkBorder ) {
    setFillColor( inColor );

    // border now identical to fill
    setBorderColor( inColor );
    
    /*

    // pick a border color that is guaranteed to be seen
    int maxComponent = 0;

    int i;
    for( i=0; i<3; i++ ) {
        if( maxComponent < inColor.bytes[i] ) {
            maxComponent = inColor.bytes[i];
            }
        }
    
    int borderAddition = -50;
    
    if( maxComponent < 128 && ! inForceDarkBorder) {
        borderAddition = 50;
        }
    
    for( i=0; i<3; i++ ) {
        int v = (int)( inColor.bytes[i] + borderAddition );
        if( v > 255 ) {
            v = 255;
            }
        if( v < 0 ) {
            v = 0;
            }
        inColor.bytes[i] = (unsigned char)v;
        }
    
    setBorderColor( inColor );
    */
    }



void ColorButtonGL::drawPressed() {
    drawBorder();
    }



void ColorButtonGL::drawUnpressed() {
    drawBorder();
    }


char ColorButtonGL::wasLastActionHover() {
    return mLastActionHover;
    }



void ColorButtonGL::mousePressed( double inX, double inY ) {
    BorderButtonGL::mousePressed( inX, inY );
    
    if( isEnabled() ) {
        // event
        mLastActionHover = false;
        fireActionPerformed( this );
        }
    }



void ColorButtonGL::mouseDragged( double inX, double inY ) {
    BorderButtonGL::mouseDragged( inX, inY );
    
    if( isEnabled() && isInside( inX, inY ) ) {
        // override pressed behavior, even if press didn't start on us
        mPressed = true;

		// fire an event
        mLastActionHover = false;
		fireActionPerformed( this );
		}
    }



void ColorButtonGL::mouseReleased( double inX, double inY ) {
    mLastActionHover = false;
    //BorderButtonGL::mouseReleased( inX, inY );

    // always unpress on a release
	mPressed = false;
	
	if( isEnabled() && isInside( inX, inY ) ) {
		// fire an event, even if press didn't start on us
		fireActionPerformed( this );
		}

    // reset for next time
    mPressStartedOnUs = false;
    }


void ColorButtonGL::mouseMoved( double inX, double inY ) {
    BorderButtonGL::mouseMoved( inX, inY );
    
    if( isEnabled() && isInside( inX, inY ) ) {
		// fire an event
        mLastActionHover = true;
		fireActionPerformed( this );
		}
    }



LeftButtonGL::LeftButtonGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight ) 
        : SpriteButtonGL( new Sprite( "left.tga", true ),
                          inWidth / 8,
                          inAnchorX, inAnchorY, inWidth, inHeight ) {
    setBorderColor( black );

    setToolTip( "tip_prev" );
    }



RightButtonGL::RightButtonGL( double inAnchorX, double inAnchorY, 
                              double inWidth, double inHeight ) 
        : SpriteButtonGL( new Sprite( "right.tga", true ),
                          inWidth / 8,
                          inAnchorX, inAnchorY, inWidth, inHeight ) {
    setBorderColor( black );
    setToolTip( "tip_next" );
    }




EditButtonGL::EditButtonGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight ) 
        : SpriteButtonGL( new Sprite( "edit.tga", true ),
                          inWidth / 8,
                          inAnchorX, inAnchorY, inWidth, inHeight ) {
    }



CloseButtonGL::CloseButtonGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight ) 
        : KeyEquivButtonGL( new Sprite( "close.tga", true ),
                            inWidth / 8,
                            inAnchorX, inAnchorY, inWidth, inHeight,
                            'w', 'W') {
    }



char CloseButtonGL::isInside( double inX, double inY ) {
    // slightly bigger to left and above
	if( inX >= mAnchorX - 1 && inX < mAnchorX + mWidth
		&& inY >= mAnchorY && inY < mAnchorY + mHeight + 1 ) {

		return true;
		}
	else {
		return false;
		}
	}



NoKeyCloseButtonGL::NoKeyCloseButtonGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight ) 
        : SpriteButtonGL( new Sprite( "close.tga", true ),
                          inWidth / 8,
                          inAnchorX, inAnchorY, inWidth, inHeight ) {
    }



char NoKeyCloseButtonGL::isInside( double inX, double inY ) {
    // slightly bigger to left and above
	if( inX >= mAnchorX - 1 && inX < mAnchorX + mWidth
		&& inY >= mAnchorY && inY < mAnchorY + mHeight + 1 ) {

		return true;
		}
	else {
		return false;
		}
	}



SmallAddButtonGL::SmallAddButtonGL( double inAnchorX, double inAnchorY, 
                                    double inWidth, double inHeight ) 
        : SpriteButtonGL( new Sprite( "smallAdd.tga", true ),
                          inWidth / 8,
                          inAnchorX, inAnchorY, inWidth, inHeight ) {
    }



QuickDeleteButtonGL::QuickDeleteButtonGL( double inAnchorX, double inAnchorY, 
                                          double inWidth, double inHeight ) 
        : SpriteButtonGL( new Sprite( "delete.tga", true ),
                          inWidth / 8,
                          inAnchorX, inAnchorY, inWidth, inHeight ) {
    }




KeyEquivButtonGL::KeyEquivButtonGL( Sprite *inSprite,
                                    double inSpriteDrawScale,
                                    double inAnchorX, double inAnchorY, 
                                    double inWidth, double inHeight,
                                    char inKeyA, char inKeyB ) 
        : SpriteButtonGL( inSprite,
                          inSpriteDrawScale,
                          inAnchorX, inAnchorY, inWidth, inHeight ),
          mKeyA( inKeyA ), mKeyB( inKeyB ) {
    }



char KeyEquivButtonGL::isFocused() {
    if( !isEnabled() ) {
        return false;
        }
    
    return true;
    }


#include <SDL/SDL_keyboard.h>
SDLMod SDLCALL SDL_GetModState(void);

void KeyEquivButtonGL::keyPressed( unsigned char inKey, 
                                   double inX, double inY ) {
    if( !isEnabled() ) {
        return;
        }
    
    char controlDown = false;
    

    SDLMod modState = SDL_GetModState();
    

    if( ( modState & KMOD_CTRL )
        ||
        ( modState & KMOD_ALT )
        ||
        ( modState & KMOD_META ) ) {
        
        controlDown = true;
        }
    

    // 26 is "SUB" which is ctrl-z on some platforms
    // 24 is "CAN" which is ctrl-x on some platforms
    // 23 is "ETB" which is ctrl-w on some platforms
    //  5 is "ENQ" which is ctrl-e on some platforms
    //  1 is "SOH" which is ctrl-a on some platforms
    // but if alt-z or meta-z held, SUB won't be passed through
    if( ( (mKeyA == 'z' ) && inKey == 26 ) ||
        ( (mKeyA == 'x' ) && inKey == 24 ) ||
        ( (mKeyA == 'w' ) && inKey == 23 ) ||
        ( (mKeyA == 'e' ) && inKey ==  5 ) ||
        ( (mKeyA == 'a' ) && inKey ==  1 ) ||
        ( controlDown && ( inKey == mKeyA || inKey == mKeyB ) ) ) {
        
        fireActionPerformed( this );
        }
    }




UndoButtonGL::UndoButtonGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight ) 
        : KeyEquivButtonGL( new Sprite( "undo.tga", true ),
                            inWidth / 16,
                            inAnchorX, inAnchorY, inWidth, inHeight,
                            'z', 'Z' ) {
    setToolTip( "tip_undo" );
    }




RedoButtonGL::RedoButtonGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight ) 
        : SpriteButtonGL( new Sprite( "redo.tga", true ),
                          inWidth / 16,
                          inAnchorX, inAnchorY, inWidth, inHeight ) {
    setToolTip( "tip_redo" );
    }




FlipHButtonGL::FlipHButtonGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight ) 
        : SpriteButtonGL( new Sprite( "flipH.tga", true ),
                          inWidth / 16,
                          inAnchorX, inAnchorY, inWidth, inHeight ) {
    setToolTip( "tip_flipH" );
    }


FlipVButtonGL::FlipVButtonGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight ) 
        : SpriteButtonGL( new Sprite( "flipV.tga", true ),
                          inWidth / 16,
                          inAnchorX, inAnchorY, inWidth, inHeight ) {
    setToolTip( "tip_flipV" );
    }


RotateCCWButtonGL::RotateCCWButtonGL( double inAnchorX, double inAnchorY, 
                                      double inWidth, double inHeight ) 
        : SpriteButtonGL( new Sprite( "rotateCCW.tga", true ),
                          inWidth / 16,
                          inAnchorX, inAnchorY, inWidth, inHeight ) {
    setToolTip( "tip_rotateCCW" );
    }

RotateCWButtonGL::RotateCWButtonGL( double inAnchorX, double inAnchorY, 
                                    double inWidth, double inHeight ) 
        : SpriteButtonGL( new Sprite( "rotateCW.tga", true ),
                          inWidth / 16,
                          inAnchorX, inAnchorY, inWidth, inHeight ) {
    setToolTip( "tip_rotateCW" );
    }



ClearButtonGL::ClearButtonGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight ) 
        : SpriteButtonGL( new Sprite( "clear.tga", true ),
                          inWidth / 16,
                          inAnchorX, inAnchorY, inWidth, inHeight ) {
    setToolTip( "tip_clear" );
    }


ColorizeButtonGL::ColorizeButtonGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight ) 
        : SpriteButtonGL( new Sprite( "colorize.tga", true ),
                          inWidth / 16,
                          inAnchorX, inAnchorY, inWidth, inHeight ) {
    setToolTip( "tip_colorize" );
    }



void SelectableButtonGL::setSelected( char inSelected ) {
    mSelected = inSelected;
    }

char SelectableButtonGL::getSelected() {
    return mSelected;
    }



SelectableButtonGL::SelectableButtonGL( Sprite *inSprite,
                                        double inSpriteDrawScale,
                                        double inAnchorX, double inAnchorY, 
                                        double inWidth, double inHeight )
    // leave room for external border
        : SpriteButtonGL( inSprite, inSpriteDrawScale,
                          inAnchorX + 2, inAnchorY + 2, 
                          inWidth - 4, inHeight - 4 ),
          mSelected( false ),
          mOuterAnchorX( inAnchorX ), mOuterAnchorY( inAnchorY ),
          mOuterWidth( inWidth ), mOuterHeight( inHeight ) {

    }


        
// overrides from BorderButtonGL
void SelectableButtonGL::drawBorder() {
    
    
    if( mSelected ) {
            
        // 1 pixel wide
        glColor4f( mBorderColor.comp.r / 255.0f, 
                   mBorderColor.comp.g / 255.0f, 
                   mBorderColor.comp.b / 255.0f,
                   1 ); 

        glBegin( GL_QUADS ); {		
            glVertex2d( mOuterAnchorX, mOuterAnchorY );
            glVertex2d( mOuterAnchorX + mOuterWidth, mOuterAnchorY ); 
            glVertex2d( mOuterAnchorX + mOuterWidth, 
                        mOuterAnchorY + mOuterHeight );
            glVertex2d( mOuterAnchorX, mOuterAnchorY + mOuterHeight );
            }
        glEnd();

        // fill
        glColor4f( mFillColor.comp.r / 255.0f, 
                   mFillColor.comp.g / 255.0f, 
                   mFillColor.comp.b / 255.0f,
                   1 ); 

        glBegin( GL_QUADS ); {		
            glVertex2d( mOuterAnchorX + 1, mOuterAnchorY + 1 );
            glVertex2d( mOuterAnchorX + mOuterWidth - 1, mOuterAnchorY + 1 ); 
            glVertex2d( mOuterAnchorX + mOuterWidth - 1, 
                        mOuterAnchorY + mOuterHeight - 1 );
            glVertex2d( mOuterAnchorX + 1 , mOuterAnchorY + mOuterHeight - 1 );
            }
        glEnd();
        }
    
    // draw internal border
    BorderButtonGL::drawBorder();
    }





DoublePressSpriteButtonGL::DoublePressSpriteButtonGL( 
    Sprite *inSprite, Sprite *inConfirmSprite,
    double inSpriteDrawScale,
    double inAnchorX, double inAnchorY, 
    double inWidth, double inHeight )
        : SpriteButtonGL( inSprite, inSpriteDrawScale, inAnchorX, inAnchorY,
                          inWidth, inHeight ),
          mOtherSprite( inConfirmSprite ),
          mPressedOnce( false ),
          mFirstTip( NULL ),
          mConfirmTip( NULL ) {

    }



DoublePressSpriteButtonGL::~DoublePressSpriteButtonGL() {
    delete mOtherSprite;
    
    if( mFirstTip != NULL ) {
        delete [] mFirstTip;
        }
    if( mConfirmTip != NULL ) {
        delete [] mConfirmTip;
        }
    }


/*
void DoublePressSpriteButtonGL::resetPressState() {
    if( mPressedOnce ) {

        // swap sprites
        Sprite *temp = mSprite;
        mSprite = mOtherSprite;
        mOtherSprite = temp;

        mPressedOnce = false;

        if( mTip != NULL ) {
            delete [] mTip;
            if( mFirstTip != NULL ) {
                mTip = stringDuplicate( mFirstTip );
                }
            }
        }
    }
*/


void DoublePressSpriteButtonGL::setEnabled( char inEnabled ) {
    SpriteButtonGL::setEnabled( inEnabled );
    
    if( !inEnabled && mPressedOnce ) {
        
        // swap sprites
        Sprite *temp = mSprite;
        mSprite = mOtherSprite;
        mOtherSprite = temp;

        mPressedOnce = false;

        if( mTip != NULL ) {
            delete [] mTip;
            if( mFirstTip != NULL ) {
                mTip = stringDuplicate( mFirstTip );
                }
            }
        }
    }


        
        
void DoublePressSpriteButtonGL::setSprite( Sprite *inSprite ) {
    AppLog::warning( "unsupported setSprite called on "
                     "DoublePressSpriteButtonGL" );
    }

        
// overrides
void DoublePressSpriteButtonGL::setToolTip( const char *inTipTranslationKey ) {
    ToolTipButtonGL::setToolTip( inTipTranslationKey );
    
    const char *confirmSuffix = 
        TranslationManager::translate( "tip_confirm" );

    if( mFirstTip != NULL ) {
        delete [] mFirstTip;
        mFirstTip = NULL;
        }
    if( mConfirmTip != NULL ) {
        delete [] mConfirmTip;
        mConfirmTip = NULL;
        }
    
    if( mTip != NULL ) {
        mFirstTip = stringDuplicate( mTip );
        mConfirmTip = autoSprintf( "%s%s", mTip, confirmSuffix );
        }
    }


void DoublePressSpriteButtonGL::mouseReleased( double inX, double inY ) {
    mPressed = false;
    
    if( isEnabled() && isInside( inX, inY ) ) {
        if( mPressedOnce ) {
            // second press!
            mPressedOnce = false;
            fireActionPerformed( this );
            }
        else {
            mPressedOnce = true;
            // wait for second press
            }

        // swap sprites
        Sprite *temp = mSprite;
        mSprite = mOtherSprite;
        mOtherSprite = temp;
        }
    else {
        // press outside
        if( mPressedOnce ) {
            // cancel
            mPressedOnce = false;
            
            // swap sprites
            Sprite *temp = mSprite;
            mSprite = mOtherSprite;
            mOtherSprite = temp;
            }
        }


    if( mTip != NULL ) {
        delete [] mTip;
        
        if( mPressedOnce && mConfirmTip != NULL ) {
            mTip = stringDuplicate( mConfirmTip );
            }
        else if( mFirstTip != NULL ) {
            mTip = stringDuplicate( mFirstTip );
            }
        if( isEnabled() && isInside( inX, inY ) ) {
            ToolTipManager::setTip( mTip );
            }
        if( !isEnabled() && isInside( inX, inY ) ) {
            ToolTipManager::setTip( NULL );
            }
        }
    }



DeleteButtonGL::DeleteButtonGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight ) 
        : DoublePressSpriteButtonGL( new Sprite( "delete.tga", true ),
                                     new Sprite( "confirm.tga", true ),
                                     inWidth / 8,
                                     inAnchorX, inAnchorY, 
                                     inWidth, inHeight ) {
    }



SendButtonGL::SendButtonGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight ) 
        : DoublePressSpriteButtonGL( new Sprite( "send.tga", true ),
                                     new Sprite( "sendConfirm.tga", true ),
                                     inWidth / 16,
                                     inAnchorX, inAnchorY, 
                                     inWidth, inHeight ) {
    }



ToggleSpriteButtonGL::ToggleSpriteButtonGL( 
    Sprite *inSprite, Sprite *inSecondSprite,
    double inSpriteDrawScale,
    double inAnchorX, double inAnchorY, 
    double inWidth, double inHeight )
        : SpriteButtonGL( inSprite, inSpriteDrawScale, inAnchorX, inAnchorY,
                          inWidth, inHeight ),
          mOtherSprite( inSecondSprite ),
          mState( false ),
          mFirstTip( NULL ),
          mSecondTip( NULL ) {

    }



ToggleSpriteButtonGL::~ToggleSpriteButtonGL() {
    delete mOtherSprite;
    if( mFirstTip != NULL ) {
        delete [] mFirstTip;
        }
    if( mSecondTip != NULL ) {
        delete [] mSecondTip;
        }
    }

        

void ToggleSpriteButtonGL::setToolTip( const char *inTipTranslationKey ) {
    ToolTipButtonGL::setToolTip( inTipTranslationKey );    

    if( mFirstTip != NULL ) {
        delete [] mFirstTip;
        mFirstTip = NULL;
        }
    
    if( mTip != NULL ) {
        mFirstTip = stringDuplicate( mTip );
        }
    }


void ToggleSpriteButtonGL::setSecondToolTip( 
    const char *inTipTranslationKey ) {
    
    if( mSecondTip != NULL ) {
        delete [] mSecondTip;
        mSecondTip = NULL;
        }
    
    if( inTipTranslationKey != NULL ) {
        char *string = 
            (char *)TranslationManager::translate( 
                (char*)inTipTranslationKey );
        
        mSecondTip = stringDuplicate( string );
        }
    }


        
void ToggleSpriteButtonGL::setSprite( Sprite *inSprite ) {
    AppLog::warning( "unsupported setSprite called on "
                     "ToggleSpriteButtonGL" );
    }


void ToggleSpriteButtonGL::setState( char inState ) {
    if( inState != mState ) {
        
        mState = !mState;
        
        // swap sprites
        Sprite *temp = mSprite;
        mSprite = mOtherSprite;
        mOtherSprite = temp;
        

        if( mTip != NULL ) {
            delete [] mTip;
            mTip = NULL;
            
            if( mState && mSecondTip != NULL ) {
                mTip = stringDuplicate( mSecondTip );
                }
            else if( mFirstTip != NULL ) {
                mTip = stringDuplicate( mFirstTip );
                }
            }
        }
    }


        
// overrides
void ToggleSpriteButtonGL::mouseReleased( double inX, double inY ) {
    if( isEnabled() && isInside( inX, inY ) && mPressStartedOnUs ) {
        setState( !mState );
        
        if( isEnabled() && isInside( inX, inY ) ) {
            ToolTipManager::setTip( mTip );
            }
        if( !isEnabled() && isInside( inX, inY ) ) {
            ToolTipManager::setTip( NULL );
            }
        }

    SpriteButtonGL::mouseReleased( inX, inY );
    }



StackSearchButtonGL::StackSearchButtonGL( double inAnchorX, double inAnchorY, 
                                          double inWidth, double inHeight ) 
        : ToggleSpriteButtonGL( new Sprite( "stack.tga", true ),
                                new Sprite( "search.tga", true ),
                                inWidth / 8,
                                inAnchorX, inAnchorY, inWidth, inHeight ) {
    setToolTip( "tip_stackMode" );
    setSecondToolTip( "tip_searchMode" );
    }





SpriteCellButtonGL::SpriteCellButtonGL( Sprite *inSprite,
                                        double inSpriteDrawScale,
                                        double inAnchorX, double inAnchorY, 
                                        double inWidth, double inHeight )
        : ToolTipButtonGL( inAnchorX, inAnchorY, inWidth, inHeight ),
          mSprite( inSprite ),
          mBlankSprite( NULL ),
          mSpriteDrawScale( inSpriteDrawScale ),
          mHighlightOn( false ),
          mColor( NULL ), mFireOnDrag( true ), mFireOnRelease( true ) {

    }


        
SpriteCellButtonGL::~SpriteCellButtonGL() {
    setSprite( NULL );

    if( mColor != NULL ) {
        delete mColor;
        }
    }


void SpriteCellButtonGL::setColor( Color *inColor ) {
    if( mColor != NULL ) {
        delete mColor;
        }
    mColor = inColor;
    }


void SpriteCellButtonGL::setHighlight( char inHighlightOn ) {
    mHighlightOn = inHighlightOn;
    }



void SpriteCellButtonGL::setSprite( Sprite *inSprite ) {
    if( mSprite != NULL ) {
        delete mSprite;
        }
    mSprite = inSprite;
    }



Sprite *SpriteCellButtonGL::getSprite() {
    return mSprite;
    }



void SpriteCellButtonGL::setBlankSprite( Sprite *inSprite ) {
    mBlankSprite = inSprite;
    }

        
void SpriteCellButtonGL::drawPressed() {
    // same as unpressed
    drawUnpressed();
    }

    

void SpriteCellButtonGL::drawUnpressed() {
    if( mEnabled ) {
        if( mSprite != NULL ) {
            
            Vector3D pos( mAnchorX + mWidth / 2, mAnchorY + mHeight/2, 0 );

            mSprite->draw( 0, 0, &pos, mSpriteDrawScale, 1, mColor );
            }
        else if( mBlankSprite != NULL ) {
            Vector3D pos( mAnchorX + mWidth / 2, mAnchorY + mHeight/2, 0 );

            mBlankSprite->draw( 0, 0, &pos, mSpriteDrawScale );
            }

        if( mHighlightOn ) {
            // draw highlight over top
            glColor4f( 1, 
                       0, 0, 0.36f ); 
            
            glBegin( GL_QUADS ); {		
                glVertex2d( mAnchorX, mAnchorY );
                glVertex2d( mAnchorX + mWidth, mAnchorY ); 
                glVertex2d( mAnchorX + mWidth, mAnchorY + mHeight );
                glVertex2d( mAnchorX, mAnchorY + mHeight );
                }
            glEnd();

            // draw x to make sure highlight visible
            glColor4f( 1, 1, 1, 0.5f );
            glBegin( GL_LINES ); {		
                glVertex2d( mAnchorX, mAnchorY );
                glVertex2d( mAnchorX + mWidth, mAnchorY + mHeight );
                
                glVertex2d( mAnchorX + mWidth, mAnchorY );
                glVertex2d( mAnchorX, mAnchorY + mHeight );
                }
            glEnd();

            }
        
        }
    }


void SpriteCellButtonGL::mousePressed( double inX, double inY ) {
    ButtonGL::mousePressed( inX, inY );
    
    if( isEnabled() ) {
        // event
        fireActionPerformed( this );
        }
    }



void SpriteCellButtonGL::mouseDragged( double inX, double inY ) {
    ButtonGL::mouseDragged( inX, inY );
    
    if( mFireOnDrag && isEnabled() && isInside( inX, inY ) ) {
        // override pressed behavior, even if press didn't start on us
        mPressed = true;
        
		// fire an event
		fireActionPerformed( this );
		}
    }



void SpriteCellButtonGL::mouseReleased( double inX, double inY ) {
	// always unpress on a release
	mPressed = false;
	
	if( mFireOnRelease && isEnabled() && isInside( inX, inY ) ) {
		// fire an event, even if press didn't start on us
		fireActionPerformed( this );
		}

    // reset for next time
    mPressStartedOnUs = false;
	}



#include "Song.h"
#include "musicPlayer.h"
// from musicPlayer.cpp
extern char noteToggles[PARTS][S][N][N];

extern int partLengths[PARTS];
extern int partPositions[PARTS];
extern int lastNoteColumnPlayed;


MusicCellButtonGL::MusicCellButtonGL( 
    Sprite *inSprite,
    double inSpriteDrawScale,
    double inAnchorX, double inAnchorY, 
    double inWidth, double inHeight )
        : SpriteCellButtonGL( inSprite, inSpriteDrawScale,
                              inAnchorX, inAnchorY, 
                              inWidth, inHeight ),
          mTimeMark( "timeMark.tga", true ),
          mPartNumber( 0 ), mPhraseNumber( 0 ) {

    // only fire on press and release
    // to avoid firing when press didn't start here
    mFireOnDrag = false;
    }
        
    

void MusicCellButtonGL::setMusicInfo( int inPartNumber, int inPhraseNumber ) {
    mPartNumber = inPartNumber;
    mPhraseNumber = inPhraseNumber;
    }



void MusicCellButtonGL::drawUnpressed() {
    
    
    if( mEnabled ) {
        if( mSprite != NULL ) {
            
            if( partPositions[ mPartNumber ] == mPhraseNumber ) {
                

                Vector3D timePos( mAnchorX + lastNoteColumnPlayed + 1, 
                                  mAnchorY + mHeight / 2, 0 );
                
                mTimeMark.draw( 0, 0, &timePos, mSpriteDrawScale );
                }
            }
        }
    // draw notes on top
    SpriteCellButtonGL::drawUnpressed();

    // thin white border, partly transparent
    glColor4f( 1, 
               1, 1, 0.25f ); 
    
    glBegin( GL_LINE_LOOP ); {		
        glVertex2d( mAnchorX, mAnchorY );
        glVertex2d( mAnchorX + mWidth, mAnchorY ); 
        glVertex2d( mAnchorX + mWidth, mAnchorY + mHeight );
        glVertex2d( mAnchorX, mAnchorY + mHeight );
        }
    glEnd();
    }



ScaleToggleButton::ScaleToggleButton( 
    Sprite *inSprite, Sprite *inSecondSprite,
    double inSpriteDrawScale,
    double inAnchorX, double inAnchorY, 
    double inWidth, double inHeight ) 
        : ToggleSpriteButtonGL( inSprite, inSecondSprite,
                                inSpriteDrawScale,
                                inAnchorX, inAnchorY,
                                inWidth, inHeight ), 
          mOnFade( 0 ) {
    }



void ScaleToggleButton::setMusicInfo( int inNoteNumber, 
                                      int inNotesPerOctave,
                                      int inPartToWatch ) {
    mNoteNumber = inNoteNumber;
    mNotesPerOctave = inNotesPerOctave;
    mPartToWatch = inPartToWatch;
    }


Color *ScaleToggleButton::getDrawColor() {
    char noteOn = false;

    if( mNotesPerOctave > 0 ) {
        
        for( int i=0; i<N && !noteOn; i++ ) {
            if( i % mNotesPerOctave == mNoteNumber ) {
                // our note!
                
                noteOn = noteOn || 
                    noteToggles[mPartToWatch][partPositions[mPartToWatch]]
                    [i][lastNoteColumnPlayed];
                }
            }
        }
    
    
    char columnChange = false;
    
    if( mLastColumnOn != lastNoteColumnPlayed ) {
        columnChange = true;
        mLastColumnOn = lastNoteColumnPlayed;
        }
    

    Color *c;
    
    if( noteOn && columnChange ) {
        // white
        c = new Color( 1, 1, 1 );
        mOnFade = true;
        }
    else if( mOnFade > 0 ) {
        // gradual fade out after note hit
        mOnFade -= 0.1;
        if( mOnFade < 0 ) {
            mOnFade = 0;
            }
        Color *cWhite = new Color( 1, 1, 1 );
        
        c = Color::linearSum( cWhite, mColor, mOnFade );
        delete cWhite;
        
        }
        
    /*
    else if( (noteOn && mOnFade < 1.0 ) || 
             (!noteOn && mOnFade > 0 ) ) {
        if( noteOn ) {
            // instant jump
            mOnFade = 1.0;
            }
        else {
            // gradual fade out
            mOnFade -= 0.2;
            if( mOnFade < 0 ) {
                mOnFade = 0;
                }
            }
        
        Color *cWhite = new Color( 1, 1, 1 );
        
        c = Color::linearSum( cWhite, mColor, mOnFade );
        }
    */
    else {
        // normal
        c = mColor->copy();
        }

    return c;
    }


        
void ScaleToggleButton::drawUnpressed() {
    Color *oldColor = mColor;

    mColor = getDrawColor();
        
    ToggleSpriteButtonGL::drawUnpressed();

    delete mColor;
    mColor = oldColor;
    }



void ScaleToggleButton::drawPressed() {
    Color *oldColor = mColor;

    mColor = getDrawColor();
        
    ToggleSpriteButtonGL::drawPressed();

    delete mColor;
    mColor = oldColor;
    }






HighlightColorButtonGL::HighlightColorButtonGL( 
    rgbaColor inColor,
    double inAnchorX, double inAnchorY, 
    double inWidth, double inHeight )
        : ColorButtonGL( inColor, inAnchorX, inAnchorY,
                         inWidth, inHeight ),
          mHighlightOn( false ),
          mSelectionOn( false ),
          mOverlayOn( false ),
          mOverlayTrans( false ) {

    }



void HighlightColorButtonGL::setHighlight( char inHighlightOn ) {
    mHighlightOn = inHighlightOn;
    }



void HighlightColorButtonGL::setSelection( char inSelectionOn ) {
    mSelectionOn = inSelectionOn;
    }


void HighlightColorButtonGL::setOverlay( char inOverlayOn ) {
    mOverlayOn = inOverlayOn;
    }

void HighlightColorButtonGL::setOverlayColor( rgbaColor inColor ) {
    mOverlayColor = inColor;
    }

void HighlightColorButtonGL::setOverlayTrans( char inTrans ) {
    mOverlayTrans = inTrans;
    }



void HighlightColorButtonGL::drawPressed() {
    // same as unpressed
    drawUnpressed();
    }

void HighlightColorButtonGL::drawCheckerboard( unsigned char inAlpha ) {
    double halfWidth = (int)( mWidth / 2 );
    double halfHeight = (int)( mHeight / 2 );
            
    glBegin( GL_QUADS ); {		
        /*
        // darker border
        glColor4f( 0.32f, 0.32f, 0.32f, inAlpha / 255.0f );
        glVertex2d( mAnchorX, mAnchorY );
        glVertex2d( mAnchorX + mWidth, mAnchorY ); 
        glVertex2d( mAnchorX + mWidth, mAnchorY + mHeight );
        glVertex2d( mAnchorX, mAnchorY + mHeight );
        */

        // black inner
        glColor4f( 0, 0, 0, inAlpha / 255.0f );
        glVertex2d( mAnchorX, mAnchorY );
        glVertex2d( mAnchorX + mWidth, mAnchorY ); 
        glVertex2d( mAnchorX + mWidth, mAnchorY + mHeight );
        glVertex2d( mAnchorX, mAnchorY + mHeight );

        // gray checks on top
        glColor4f( 0.1875f, 0.1875f, 0.1875f, 1 );
        glVertex2d( mAnchorX, mAnchorY );
        glVertex2d( mAnchorX + halfWidth, mAnchorY ); 
        glVertex2d( mAnchorX + halfWidth, mAnchorY + halfHeight );
        glVertex2d( mAnchorX, mAnchorY + halfHeight );

        glVertex2d( mAnchorX + halfWidth + 0.5, 
                    mAnchorY + halfHeight + 0.5 );
        glVertex2d( mAnchorX + mWidth, 
                    mAnchorY + halfHeight + 0.5 );
        glVertex2d( mAnchorX + mWidth, 
                    mAnchorY + mHeight ); 
        glVertex2d( mAnchorX + halfWidth + 0.5, 
                    mAnchorY + mHeight );
        }
    glEnd();
    }

    

void HighlightColorButtonGL::drawUnpressed() {
    ColorButtonGL::drawUnpressed();
    
    if( mEnabled ) {

        if( mHighlightOn ) {
            // the term "highlight" is outdated, since it used to be used
            // to show transparency mask pixels in the sprite editor
            // now a checkerboard is used instead

            // draw checkerboard
            drawCheckerboard( 255 );
            }


        if( mOverlayOn && ! mOverlayTrans ) {
            // draw color overlay over top
            glColor4f( mOverlayColor.comp.r / 255.0f, 
                        mOverlayColor.comp.g / 255.0f,
                        mOverlayColor.comp.b / 255.0f, 0.5f ); 
            
            glBegin( GL_QUADS ); {		
                glVertex2d( mAnchorX, mAnchorY );
                glVertex2d( mAnchorX + mWidth, mAnchorY ); 
                glVertex2d( mAnchorX + mWidth, mAnchorY + mHeight );
                glVertex2d( mAnchorX, mAnchorY + mHeight );
                }
            glEnd();
            
            // draw solid, internal box w/border to make sure overlay visible
            double gap = 3;

            glColor4f( mOverlayColor.comp.r / 255.0f, 
                       mOverlayColor.comp.g / 255.0f,
                       mOverlayColor.comp.b / 255.0f, 0.5f ); 
            glBegin( GL_QUADS ); {		
                glVertex2d( mAnchorX + gap, mAnchorY + gap );
                glVertex2d( mAnchorX + mWidth - gap, mAnchorY + gap ); 
                glVertex2d( mAnchorX + mWidth - gap, 
                            mAnchorY + mHeight - gap );
                glVertex2d( mAnchorX + gap, mAnchorY + mHeight - gap );
                }
            glEnd();
            }
        else if( mOverlayOn ) {
            // draw trans checkerboard overlay
            drawCheckerboard( 128 );
            }
        




        if( mSelectionOn ) {
            // draw selection over top
            glColor4f( 1, 
                       0, 0, 0.36f ); 
            
            glBegin( GL_QUADS ); {		
                glVertex2d( mAnchorX, mAnchorY );
                glVertex2d( mAnchorX + mWidth, mAnchorY ); 
                glVertex2d( mAnchorX + mWidth, mAnchorY + mHeight );
                glVertex2d( mAnchorX, mAnchorY + mHeight );
                }
            glEnd();

            // draw internal box to make sure selection visible
            double gap = 3;
            glColor4f( 0, 0, 0, 0.5f );
            glBegin( GL_LINE_LOOP ); {		
                glVertex2d( mAnchorX + gap, mAnchorY + gap );
                glVertex2d( mAnchorX + mWidth - gap, mAnchorY + gap ); 
                glVertex2d( mAnchorX + mWidth - gap, 
                            mAnchorY + mHeight - gap );
                glVertex2d( mAnchorX + gap, mAnchorY + mHeight - gap );
                }
            glEnd();

            }

        }
    }





TwoSpriteButtonGL::TwoSpriteButtonGL( Sprite *inSprite,
                                      Sprite *inFrontSprite,
                                      double inSpriteDrawScale,
                                      double inAnchorX, double inAnchorY, 
                                      double inWidth, double inHeight )
        : SpriteButtonGL( inSprite, inSpriteDrawScale, 
                          inAnchorX, inAnchorY, inWidth, inHeight ),
          mFrontSprite( inFrontSprite ) {
    }


        
TwoSpriteButtonGL::~TwoSpriteButtonGL() {
    setFrontSprite( NULL );
    }



void TwoSpriteButtonGL::setFrontSprite( Sprite *inSprite ) {
    if( mFrontSprite != NULL ) {
        delete mFrontSprite;
        }
    mFrontSprite = inSprite;
    }

        
void TwoSpriteButtonGL::drawPressed() {
    SpriteButtonGL::drawPressed();
    
    if( mEnabled ) {
        if( mFrontSprite != NULL ) {
            
            Vector3D pos( mAnchorX + mWidth / 2, mAnchorY + mHeight/2, 0 );
        
            // a bit darker when pressed
            Color c( .75, .75, .75, 1 );
            
            mFrontSprite->draw( 0, 0, &pos, mSpriteDrawScale, 1, &c );
            }
        }
    }

    

void TwoSpriteButtonGL::drawUnpressed() {
    SpriteButtonGL::drawUnpressed();

    if( mEnabled ) {
        if( mFrontSprite != NULL ) {
            
            Vector3D pos( mAnchorX + mWidth / 2, mAnchorY + mHeight/2, 0 );

            mFrontSprite->draw( 0, 0, &pos, mSpriteDrawScale );
            }
        }
    }



DraggableTwoSpriteButtonGL::DraggableTwoSpriteButtonGL( 
    Sprite *inSprite,
    Sprite *inFrontSprite,
    double inSpriteDrawScale,
    double inAnchorX, double inAnchorY, 
    double inWidth, double inHeight ) 
        : TwoSpriteButtonGL( inSprite, inFrontSprite, inSpriteDrawScale,
                             inAnchorX, inAnchorY, inWidth, inHeight ),
          mLastActionFromPress( false ) {

    }


        
void DraggableTwoSpriteButtonGL::mousePressed( double inX, double inY ) {
    TwoSpriteButtonGL::mousePressed( inX, inY );

    if( isEnabled() ) {
        mLastActionFromPress = true;
        fireActionPerformed( this );
        }
    }


void DraggableTwoSpriteButtonGL::mouseReleased( double inX, double inY ) {
    mLastActionFromPress = false;

    TwoSpriteButtonGL::mouseReleased( inX, inY );
    }
