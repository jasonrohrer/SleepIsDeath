#include "GameStateDisplay.h"

#include "common.h"
#include "SpriteResource.h"
#include "GameStateEditor.h"

#include "minorGems/graphics/openGL/gui/TextGL.h"
#include "minorGems/util/stringUtils.h"


extern TextGL *largeText;



GameStateDisplay::GameStateDisplay( int inAnchorX, int inAnchorY,
                                    char inShowObjectToolTips )
        : GUIComponentGL( inAnchorX, inAnchorY,
                          G * P, G * P ),
          mHasAnySpeechBeenAutoFlipped( false ),
          mManualBubblePositioningLive( false ),
          mLastPixelClickX( 0 ), mLastPixelClickY( 0 ),
          mLastGridClickX( 0 ), mLastGridClickY( 0 ),
          mLastActionRelease( false ),
          mLastActionPress( false ),
          mLastActionKeyPress( false ),
          mLastMouseoverObjectIndex( -1 ),
          mMouseHover( false ),
          mLastHoverGridX( 0 ), mLastHoverGridY( 0 ),
          mEditingSpeech( false ),  
          mEditingAction( false ),
          mHighlightEditedBubble( false ),
          mShowObjectToolTips( inShowObjectToolTips ),
          mEditor( NULL ),
          mFocused( false ),
          mShowGrid( true ),
          mHintMode( false ),
          mState( NULL ),
          mLastSelected( -1 ),
          mBlinkCycle( 0 ) {


    for( int y=0; y<G; y++ ) {
        for( int x=0; x<G; x++ ) {
            mSprites[y][x] = NULL;
            }
        }

    mBubbleTopSprite = new Sprite( "bubbleTop.tga", true );
    mBubbleMiddleSprite = new Sprite( "bubbleMiddle.tga", true );
    mBubbleMiddleExtraSprite = new Sprite( "bubbleMiddleExtra.tga", true );
    mBubbleMiddleExtraThinSprite = new Sprite( "bubbleMiddleExtraThin.tga", 
                                               true );
    mBubbleBottomSprite = new Sprite( "bubbleBottom.tga", true );
    mBubbleBottomSpriteFlip = new Sprite( "bubbleBottomFlip.tga", true );
    mBubbleBottomNoTailSprite = new Sprite( "bubbleBottomNoTail.tga", true );
    mBubbleMiddleTailSprite = new Sprite( "bubbleMiddleTail.tga", true );
    mBubbleMiddleTailFlipSprite = 
        new Sprite( "bubbleMiddleTailFlip.tga", true );


    mSpeechBoxTopSprite = new Sprite( "speechBoxTop.tga", true );
    mSpeechBoxBottomSprite = new Sprite( "speechBoxBottom.tga", true );

    mActionBoxStartSprite = new Sprite( "actionBoxStart.tga", true );
    mActionBoxMiddleSprite = new Sprite( "actionBoxMiddle.tga", true );
    mActionBoxMiddleExtraSprite = 
        new Sprite( "actionBoxMiddleExtra.tga", true );
    mActionBoxEndSprite = new Sprite( "actionBoxEnd.tga", true );
    mActionBoxEndFlipSprite = new Sprite( "actionBoxEndFlip.tga", true );


    mActionBoxStartTallSprite = new Sprite( "actionBoxStartTall.tga", true );
    mActionBoxMiddleTallSprite = new Sprite( "actionBoxMiddleTall.tga", true );
    mActionBoxMiddleExtraTallSprite = 
        new Sprite( "actionBoxMiddleExtraTall.tga", true );
    mActionBoxEndTallSprite = new Sprite( "actionBoxEndTall.tga", true );
    mActionBoxEndFlipTallSprite = new Sprite( "actionBoxEndFlipTall.tga", true );
    }



GameStateDisplay::~GameStateDisplay() {
    for( int y=0; y<G; y++ ) {
        for( int x=0; x<G; x++ ) {
            if( mSprites[y][x] != NULL ) {
                delete mSprites[y][x];
                }
            }
        }

    int numSprites = mObjectSprites.size();
    for( int s=0; s<numSprites; s++ ) {
        delete *( mObjectSprites.getElement( s ) );
        }
    

    if( mState != NULL ) {
        delete mState;
        }

    delete mBubbleTopSprite;
    delete mBubbleMiddleSprite;
    delete mBubbleMiddleExtraSprite;
    delete mBubbleMiddleExtraThinSprite;
    delete mBubbleBottomSprite;
    delete mBubbleBottomSpriteFlip;
    delete mBubbleBottomNoTailSprite;
    delete mBubbleMiddleTailSprite;
    delete mBubbleMiddleTailFlipSprite;

    delete mSpeechBoxTopSprite;
    delete mSpeechBoxBottomSprite;

    delete mActionBoxStartSprite;
    delete mActionBoxMiddleSprite;
    delete mActionBoxMiddleExtraSprite;
    delete mActionBoxEndSprite;
    delete mActionBoxEndFlipSprite;

    delete mActionBoxStartTallSprite;
    delete mActionBoxMiddleTallSprite;
    delete mActionBoxMiddleExtraTallSprite;
    delete mActionBoxEndTallSprite;
    delete mActionBoxEndFlipTallSprite;
    }


        
void GameStateDisplay::setEditor( void *inEditor ) {
    mEditor = inEditor;
    }

    
    

void GameStateDisplay::setState( GameState *inState ) {
    
    //printf( "Before updating state, %d sprites\n", mObjectSprites.size() );
    
    
    /*
    char newRoom = false;
    
    if( !equal( inState.mRoom.getID(), mState.mRoom.getID() ) {
        newRoom = true;
        }
    */
    if( mState != NULL ) {
        delete mState;
        }

    mState = inState;

        
    for( int y=0; y<G; y++ ) {
        for( int x=0; x<G; x++ ) {
            if( mSprites[y][x] != NULL ) {
                delete mSprites[y][x];
                }

            Tile t( mState->mRoom.getTile( x, y ) );
            
            mSprites[y][x] = t.getSprite();
            }
        }


    // clear old
    int numSprites = mObjectSprites.size();
    int i;
    for( i=0; i<numSprites; i++ ) {
        delete *( mObjectSprites.getElement( i ) );
        }
    mObjectSprites.deleteAll();
    mObjectSpritePositions.deleteAll();
    mObjectSpriteOwners.deleteAll();
    mObjectSpriteTrans.deleteAll();
    mObjectSpriteGlows.deleteAll();
    mObjectSpriteDepth.deleteAll();
    mObjectSpriteLocked.deleteAll();
    
    mSpeechOffTop.deleteAll();
    

    // add new
    int numObjects = mState->mObjects.size();

    // flag for 0
    mSpeechOffTop.push_back( false );
    

    // add 0 (player object) last so it's always on top
    for( i=1; i<numObjects; i++ ) {
        mSpeechOffTop.push_back( false );
        addSpritesFromStateInstance( i );
        }

    // now do 0 
    addSpritesFromStateInstance( 0 );
    

    //printf( "After updating state, %d sprites\n", mObjectSprites.size() );

    recomputeDrawOrderMap();
    }




void GameStateDisplay::addSpritesFromStateInstance( int inIndex ) {
    

    int i = inIndex;
    
    StateObjectInstance *o = *( mState->mObjects.getElement( i ) );
    
        
    int numSprites = o->mObject.getNumLayers();
    
    intPair anchorPos = o->mAnchorPosition;
    
    int depth = G - (anchorPos.y / P) - 1;
    
    for( int s=0; s<numSprites; s++ ) {
            
        SpriteResource resource( o->mObject.getLayerSprite( s ) );
            
        mObjectSprites.push_back( resource.getSprite() );
            
            
        intPair p = o->mObject.getLayerOffset( s );
            
        p = ::add( p, anchorPos );
            
        mObjectSpritePositions.push_back( p );
        // no owner flags for non-anchors
        mObjectSpriteOwners.push_back( -1 );

        float trans =
            ( o->mObjectTrans / 255.0f ) *
            ( o->mObject.getLayerTrans( s ) / 255.0f );

        mObjectSpriteTrans.push_back( trans );
        
        mObjectSpriteGlows.push_back( o->mObject.getLayerGlow( s ) );

        mObjectSpriteDepth.push_back( depth );

        mObjectSpriteLocked.push_back( o->mLocked );
        }

    // last, add anchor sprite for this object, on top
    if( i==0 ) {
        // special anchor for player
        mObjectSprites.push_back( new Sprite( "playerAnchor.tga", true ) );
        }
    else {
        mObjectSprites.push_back( new Sprite( "anchor.tga", true ) );
        }
    
    mObjectSpritePositions.push_back( o->mAnchorPosition );
    // only "own" the anchors
    mObjectSpriteOwners.push_back( i );

    // anchors ignore trans and glow
    mObjectSpriteTrans.push_back( 1.0 );
    mObjectSpriteGlows.push_back( false );

    // anchors are all on top
    mObjectSpriteDepth.push_back( G );

    mObjectSpriteLocked.push_back( o->mLocked );
    }


        
int GameStateDisplay::updatePositionFromStateInstance( int inIndex,
                                                       int inSpriteIndex ) {
    int i = inIndex;
    

    StateObjectInstance *o = *( mState->mObjects.getElement( i ) );

        
    int numSprites = o->mObject.getNumLayers();

    intPair anchorPos = o->mAnchorPosition;
    
    int depth = G - (anchorPos.y / P) - 1;

    for( int s=0; s<numSprites; s++ ) {
        intPair p = o->mObject.getLayerOffset( s );
        
        p = ::add( p, anchorPos );
        
        *( mObjectSpritePositions.getElement( inSpriteIndex ) ) = p;
        
        float trans =
            ( o->mObjectTrans / 255.0f ) *
            ( o->mObject.getLayerTrans( s ) / 255.0f );

        *( mObjectSpriteTrans.getElement( inSpriteIndex ) ) = trans;

        *( mObjectSpriteGlows.getElement( inSpriteIndex ) ) = 
            o->mObject.getLayerGlow( s );
        
        *( mObjectSpriteDepth.getElement( inSpriteIndex ) ) = depth;
        
        *( mObjectSpriteLocked.getElement( inSpriteIndex ) ) = o->mLocked;

        inSpriteIndex++;
        }
    
    
    *( mObjectSpritePositions.getElement( inSpriteIndex ) ) = 
        o->mAnchorPosition;
    
    *( mObjectSpriteTrans.getElement( inSpriteIndex ) ) = 1.0;

    *( mObjectSpriteGlows.getElement( inSpriteIndex ) ) = false;
    
    *( mObjectSpriteLocked.getElement( inSpriteIndex ) ) = o->mLocked;
    
        
    inSpriteIndex++;


    return inSpriteIndex;
    }



void GameStateDisplay::updateSpritePositions( GameState *inState ) {
    
    if( mState != NULL ) {
        delete mState;
        }

    mState = inState;


    int numObjects = mState->mObjects.size();
    
    int spriteIndex = 0;
    
    // 0 last (player object on top)
    for( int i=1; i<numObjects; i++ ) {
        spriteIndex = updatePositionFromStateInstance( i, spriteIndex );
        }

    // now 0
    spriteIndex = updatePositionFromStateInstance( 0, spriteIndex );

    recomputeDrawOrderMap();
    }



void GameStateDisplay::recomputeDrawOrderMap() {
    mDrawOrderMap.deleteAll();
    
    // G+1 bins
    SimpleVector<int> depthBins[ G + 1 ];
    
    int numSprites = mObjectSprites.size();
    
    
    // put locked ones in first (behind) for each depth
    for( int i=0; i<numSprites; i++ ) {
        if( *( mObjectSpriteLocked.getElement( i ) ) ) {
            
            int depth = *( mObjectSpriteDepth.getElement( i ) );
            depthBins[depth].push_back( i );
            }
        }

    // now unlocked ones in front at each depth
    for( int i=0; i<numSprites; i++ ) {
        if( ! *( mObjectSpriteLocked.getElement( i ) ) ) {
            
            int depth = *( mObjectSpriteDepth.getElement( i ) );
            depthBins[depth].push_back( i );
            }
        }
    
    
    // dump bins in, in order
    for( int b=0; b<=G; b++ ) {
        
        int binSize = depthBins[b].size();
        
        for( int i=0; i<binSize; i++ ) {
            mDrawOrderMap.push_back( *( depthBins[b].getElement( i ) ) );
            }        
        }
    }



void GameStateDisplay::showGrid( char inShow ) {
    mShowGrid = inShow;
    }



void GameStateDisplay::setHintMode( char inHintsOnly ) {
    mHintMode = inHintsOnly;
    }



void GameStateDisplay::copySpeechCoordinates( GameState *inDestinationState ) {
    int numObjects = mState->mObjects.size();
    
    for( int i=0; i<numObjects; i++ ) {
        StateObjectInstance *o = *( mState->mObjects.getElement( i ) );
        
        StateObjectInstance *oDest = 
            *( inDestinationState->mObjects.getElement( i ) );


        oDest->mSpeechFlip = o->mSpeechFlip;
        oDest->mSpeechOffset = o->mSpeechOffset;
        }
    }



char GameStateDisplay::getSpeechOffTop( int inObject ) {
    return *( mSpeechOffTop.getElement( inObject ) );
    }

    


void GameStateDisplay::drawSpeech( int inIndex ) {
    int i = inIndex;

    int selected = mState->getSelectedObject();

    Color *bubbleColor = NULL;


    StateObjectInstance *o = *( mState->mObjects.getElement( i ) );
        
    // skip any blank messages
    // UNLESS we are editing text of selected
    if( strlen( o->mSpokenMessage ) >  0
        ||
        ( i == selected && mEditingSpeech ) ) {
        

        if( i == selected && isFocused() && mHighlightEditedBubble ) {
            bubbleColor = new Color( 1.0f, 1.0f, 0.5f, 1.0f );
            }
        

        intPair pos = ::add( o->mSpeechOffset, o->mAnchorPosition );
        
        double height = 8;
        double width = strlen( o->mSpokenMessage ) * height;
      

        double trueStringHeight = height * 
            largeText->measureTextHeight( o->mSpokenMessage );
        
        char bubbleHasHighLines = false;
        char bubbleHasReallyHighLines = false;
        
        if( trueStringHeight > 8 ) {
            // make room for high characters
            bubbleHasHighLines = true;
            }
        if( trueStringHeight > 9 ) {
            bubbleHasReallyHighLines = true;
            }
        
        

        int bubbleWidth = 64 - 8;
        
  


        SimpleVector<char*> lines;

        
        // new code:  split into words first, ingore player's spacing.
        // 1 space between words, two spaces after a period
        SimpleVector<char*> *words = tokenizeString( o->mSpokenMessage );
        
        int wordIndex = 0;
        int numWords = words->size();
        
        while( wordIndex < numWords ) {
            // a word left
            char *nextWord = *( words->getElement( wordIndex ) );
            double nextWordWidth =  
                largeText->measureTextWidth( nextWord ) * height;

            char *currentLine = NULL;
            double currentWidth = 0;
            int numLines = lines.size();
            
            if( numLines > 0 ) {
                
                currentLine = *( lines.getElement( numLines - 1 ) );
                currentWidth = 
                    largeText->measureTextWidth( currentLine ) * height;
                }
            
            char *spaceNeeded = (char *)" ";
            int lastCharIndex = -1;
            if( currentLine != NULL ) {
                lastCharIndex = strlen( currentLine ) - 1;
                }
            
            if( currentLine != NULL &&
                ( currentLine[ lastCharIndex ] == '.' ||
                  currentLine[ lastCharIndex ] == '?' ||
                  currentLine[ lastCharIndex ] == '!' ) ) {
                // line so far ends with period (or other sentence ender), 
                // extra space before next word in line
                spaceNeeded = (char *)"  ";
                }
            double spaceWidth = 
                largeText->measureTextWidth( spaceNeeded ) * height;

            if( currentLine != NULL &&
                currentWidth + spaceWidth + nextWordWidth 
                < bubbleWidth ) {
                
                // good!  we can add it
                char *newLine = autoSprintf( "%s%s%s", currentLine,
                                             spaceNeeded, nextWord );
                delete [] currentLine;
                *( lines.getElement( numLines - 1 ) ) = newLine;
                
                delete [] nextWord;
                }
            else {
                // not enough room on this line, or no line yet
                // start a new one with just this word
                
                if( nextWordWidth < bubbleWidth ) {
                    lines.push_back( nextWord );
                    }
                else {
                    // problem!  really long word
                
                    // split it into parts... make sure second part has at 
                    // least 3 chars
                    
                    char *partialWord = stringDuplicate( nextWord );

                    double hypenWidth = largeText->measureTextWidth( "-" );

                    while( ( hypenWidth + 
                             largeText->measureTextWidth( partialWord ) )
                           * height
                           >= bubbleWidth ) {
                    
                        // truncate
                        partialWord[ strlen( partialWord ) - 1 ] = '\0';
                        }

                    int partialLength = strlen( partialWord );
                    
                    int nextWordLength = strlen( nextWord );
                    
                    int extraLength = nextWordLength - partialLength;
                    
                    while( extraLength < 3 ) {
                        // trim more off partial
                        partialWord[ partialLength - 1 ] = '\0';
                        
                        partialLength -= 1;
                    
                        extraLength ++;
                        }
                    
                    // add hyphen
                    partialWord[ partialLength ] = '-';
                    partialWord[ partialLength + 1 ] = '\0';
                    
                    lines.push_back( partialWord );
                    
                    char *extraChars = stringDuplicate( 
                        &( nextWord[ partialLength ] ) );
                    
                    *( words->getElement( wordIndex ) ) = extraChars;
                    delete [] nextWord;
                    
                    // part of the word still there, back up and look 
                    // at it again
                    wordIndex --;
                    }
                }
            wordIndex ++;
            }
        delete words;        





        // start at bottom line and draw up
        double bubbleFade = 0.63;
        
        int flipSign = 1;
            
        if( o->mSpeechFlip ) {
            flipSign = -1;
            }
            

        // draw bottom w/ tip at anchor
        Vector3D bubblePos( mAnchorX + pos.x + flipSign * 32,
                            mAnchorY + pos.y + 7,
                            0 );
        
        
        
        if( o->mSpeechBox ) {
            bubblePos.mY -= 8;
            }
        

        int numLines = lines.size();
        
        int bubbleHeight = 7 + 9 * numLines + 7;
        
        if( bubbleHasHighLines ) {
            
            if( bubbleHasReallyHighLines ) {
                // use extended mode
                bubbleHeight = 7 + 12 * numLines + 7;
                }
            else {
                // thin extended mode
                bubbleHeight = 7 + 10 * numLines + 7;
                }
            }
        
        
        char offTop = false;
        
        if( bubblePos.mY + bubbleHeight > mHeight + mAnchorY ) {
            offTop = true;
            }

        char oldOffTop = *( mSpeechOffTop.getElement( i ) );
        
        
        *( mSpeechOffTop.getElement( i ) ) = offTop;
        
        if( offTop != oldOffTop ) {
            // a change in off-top status.  Tell editor about it
            mHasAnySpeechBeenAutoFlipped = true;
            }
        

        // auto-flip if off the side of screen

        char flipThisBubble = o->mSpeechFlip;
        
        int bubbleHalfWidth =  32;
        int bubbleFullWidth =  64;
        
        // off-top boxes are no wider
        if( ! o->mSpeechBox &&
            offTop && numLines > 0 ) {
            bubbleHalfWidth += 10;
            }


        // apply flipping heuristic positioning
        int autoFlipExtraOffset = mState->getAutoOffsetOnFlip( i );
        
        if( mManualBubblePositioningLive ) {
            // don't mess with the mouse, drop heuristic and go with 
            // mouse position
            autoFlipExtraOffset = 0;
            }
        


        if( o->mSpeechFlip && bubblePos.mX - bubbleHalfWidth < mAnchorX ) {
            // unflip
            flipThisBubble = false;
            bubblePos.mX += bubbleFullWidth;
            bubblePos.mX += autoFlipExtraOffset;

            // auto-flip this bubble for the future, too, so that mouse-based
            // positioning
            // works correctly, etc
            o->mSpeechFlip = false;
            o->mSpeechOffset.x += autoFlipExtraOffset;
            // thus, we WON'T have to auto-flip next time we draw this same
            // frame
            mHasAnySpeechBeenAutoFlipped = true;
            }
        else if( ! o->mSpeechFlip && bubblePos.mX + bubbleHalfWidth >
                 mWidth + mAnchorX ) {
            // force a flip
            flipThisBubble = true;
            bubblePos.mX -= bubbleFullWidth;
            bubblePos.mX += autoFlipExtraOffset;
            
            // again, permanently re-flip
            o->mSpeechFlip = true;
            o->mSpeechOffset.x += autoFlipExtraOffset;
            mHasAnySpeechBeenAutoFlipped = true;
            }

        
        if( o->mSpeechFlip ) {
            bubblePos.mX += 1;
            }
        

        Sprite *bottomSprite = mBubbleBottomSprite;
        Sprite *middleTailSprite = mBubbleMiddleTailSprite;
            

        if( flipThisBubble ) {
            bottomSprite = mBubbleBottomSpriteFlip;
            middleTailSprite = mBubbleMiddleTailFlipSprite;
            }
        if( o->mSpeechBox ) {
            bottomSprite = mSpeechBoxBottomSprite;
            // no middle tails
            middleTailSprite = mBubbleMiddleSprite;
            }
        
            
        int lineWithTail = 0;
        
        if( offTop ) {
            if( !o->mSpeechBox ) {
                
                if( numLines > 0 ) {
                    bottomSprite = mBubbleBottomNoTailSprite;
                    // scoot over to make room for horizontal tail
                    if( flipThisBubble ) {
                        bubblePos.mX -= 10;
                        }
                    else {
                        bubblePos.mX += 10;
                        }
                    
                
                

                
                    // scoot the whole thing down by some number of lines
                    int linesToScoot = numLines;
                    //lineWithTail = numLines - 1;
                
                    while( bubblePos.mY + bubbleHeight > mHeight + mAnchorY
                           &&
                           linesToScoot > 0 ) {
                        bubblePos.mY -= 9;
                        if( linesToScoot == numLines ) {
                            // anchor pos for bubble is above bottom tail, now
                            // up aligned with a line instead.
                            bubblePos.mY -= 10;
                            }
                    
                        linesToScoot --;
                        }
                    //bubblePos.mY -= 10;

                    // how many lines left?
                    lineWithTail = linesToScoot;
                    }
                }
            else {
                // simply flush boxes with top... no tail to align!
                bubblePos.mY -= 
                    (bubblePos.mY + bubbleHeight) 
                    - (mHeight + mAnchorY);
                }
            
            }
        


        bottomSprite->draw( 0, 0, &bubblePos, 1, 
                            bubbleFade,
                            bubbleColor );
            
        bubblePos.mY += 4;

        // draw lines of text last, to ensure they're on top of bubble parts
        double *lineY = new double[numLines];
        
        
        for( int i=numLines-1; i>=0; i-- ) {
            bubblePos.mY += 8;

            if( i < numLines - 1 ) {
                // extra space for all but bottom line
                bubblePos.mY += 1;
                }
            if( bubbleHasHighLines ) {
                // extended line height mode
                
                if( i < numLines - 1 ) {
                    // extra space for all but bottom line
                    bubblePos.mY += 1;

                    if( bubbleHasReallyHighLines ) {
                        bubblePos.mY += 2;
                        }
                    }
                // padding above all but top line
                if( i > 0 ) {
                    
                    if( bubbleHasReallyHighLines ) {
                        
                        mBubbleMiddleExtraSprite->draw( 0, 0, &bubblePos, 1, 
                                                        bubbleFade,
                                                        bubbleColor );
                        }
                    else {
                        mBubbleMiddleExtraThinSprite->draw( 
                            0, 0, &bubblePos, 1, 
                            bubbleFade,
                            bubbleColor );
                        }
                    }
                
                }
            
            
            if( offTop && lineWithTail == i ) {
                // normal line
                middleTailSprite->draw( 0, 0, &bubblePos, 1, 
                                        bubbleFade,
                                        bubbleColor );
                }
            else {
                // normal line
                mBubbleMiddleSprite->draw( 0, 0, &bubblePos, 1, 
                                           bubbleFade,
                                           bubbleColor );
                }
            
            lineY[i] = bubblePos.mY - 4;
            /*
            char *line = *( lines.getElement( i ) );
            
            width = strlen( line ) * height;
            
            glColor4f( 0, 0, 0, 1 );

            largeText->drawText( line, 
                                 bubblePos.mX - 32 + 4,
                                 bubblePos.mY - 4,
                                 width, height );
            delete [] line;
            */
            }        
        
        bubblePos.mY += 7;
        if( numLines > 0 ) {
            // extra space before top
            bubblePos.mY += 1;
            }
        
        if( o->mSpeechBox ) {
            // flip, because this one is upside down in file (so lower
            // corner can have transparency)
            mSpeechBoxTopSprite->draw( 0, 0, &bubblePos, -1, bubbleFade,
                                       bubbleColor );
            }
        else {
            mBubbleTopSprite->draw( 0, 0, &bubblePos, 1, bubbleFade,
                                    bubbleColor );
            }

        // lines last 
        for( int i=numLines-1; i>=0; i-- ) {
            
            char *line = *( lines.getElement( i ) );
            
            width = strlen( line ) * height;
            
            glColor4f( 0, 0, 0, 1 );

            largeText->drawText( line, 
                                 bubblePos.mX - 32 + 4,
                                 lineY[i],
                                 width, height );
            delete [] line;
            }
        delete []lineY;
        }
    
    

    if( bubbleColor != NULL ) {
        delete bubbleColor;
        }
    
    }



// defined in ScreenGL_SDL.cpp

// we draw object hint mode differently depending on this
extern char screenGLStencilBufferSupported;

      

void GameStateDisplay::fireRedraw() {
    
    // invisible if disabled
    if( !mEnabled ) {
        return;
        }
    

    if( ! mHintMode ) {
        
        double roomTrans = mState->mRoomTrans / 255.0;
    
        for( int y=0; y<G; y++ ) {
            for( int x=0; x<G; x++ ) {
                if( mSprites[y][x] != NULL ) {    
                    Vector3D pos( mAnchorX + x * P + P/2, 
                                  mAnchorY + mHeight - (y * P + P/2), 0 );
                    
                    mSprites[y][x]->draw( 0, 0, &pos, 1, roomTrans );
                    }
                
                }
            }
        }


    if( mHintMode && screenGLStencilBufferSupported ) {
        // use stenciling to draw *shadows* of sprites only

        // don't update color
        glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

        // skip fully-transparent areas
        glEnable( GL_ALPHA_TEST );
        glAlphaFunc( GL_GREATER, 0 );
        
        // Draw 1 into the stencil buffer wherever a sprite is
        glEnable( GL_STENCIL_TEST );
        glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
        glStencilFunc( GL_ALWAYS, 1, 0xffffffff );
        }
    

    // now draw all sprites
    int numSprites = mObjectSprites.size();
    int selected = mState->getSelectedObject();
    
    if( mLastSelected != selected  || ! mShowGrid ) {
        // reset
        mBlinkCycle = 0;
        mLastSelected = selected;
        }
    else {
        mBlinkCycle ++;
        }

    double blinkFadeFactor = 0.35 * sin( mBlinkCycle / 3.0 ) + 0.65;
    

    //printf( "Drawing %d sprites, %d selected\n", numSprites, selected );
    
    for( int i=0; i<numSprites; i++ ) {
        
        // map index to sort by depth
        int s = *( mDrawOrderMap.getElement( i ) );
        
        

        Sprite *sprite = *( mObjectSprites.getElement( s ) );
        intPair pixelPos = *( mObjectSpritePositions.getElement( s ) );
        int owner = *( mObjectSpriteOwners.getElement( s ) );
        
        Vector3D pos( mAnchorX + pixelPos.x + P/2, 
                      mAnchorY + pixelPos.y + P/2, 0 );
        
        char drawIt = true;
        
        Color c( 1, 1, 1, 1 );
        
        double fadeFactor = *( mObjectSpriteTrans.getElement( s ) );
        
        if( owner != -1 ) {
            // an anchor, override fade
            fadeFactor = 0.5;
            
            if( owner == selected ) {
                fadeFactor *= blinkFadeFactor;
                
                if( owner != 0 ) {
                    // green
                    c.r = 0;
                    c.b = 0;
                    }
                else if( owner == 0 ) {
                    // player's object selected in red
                    c.g = 0;
                    c.b = 0;
                    }
                }
            
            
                
            
            if( !mShowGrid || mHintMode ) {
                // hide anchors
                drawIt = false;
                }
            else if( mState->mLocksOn ) {
                // anchors on... but for this object?
                StateObjectInstance *o = 
                    *( mState->mObjects.getElement( owner ) );
                
                if( o->mLocked ) {
                    drawIt = false;
                    }
                }
            
            
            }
        
        if( drawIt ) {
            char glow = *( mObjectSpriteGlows.getElement( s ) );
            if( glow ) {
                // brighten only
                glBlendFunc( GL_SRC_ALPHA, GL_ONE );
                }

            if( mHintMode ) {
                // override fade
                fadeFactor = 0.25;
                
                // black color
                c.r = 0;
                c.g = 0;
                c.b = 0;
                }
            

            sprite->draw( 0, 0, &pos, 1, fadeFactor, &c );
            
            if( glow ) {
                // back to normal blend
                glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
                }
            }
        }

    if( mHintMode && screenGLStencilBufferSupported ) {
        // stencil buffer has 1 wherever sprite is

        // Re-enable update of color
        glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
        glDisable( GL_ALPHA_TEST );
        
        // Now, only render where stencil is set to 1.
        glStencilFunc( GL_EQUAL, 1, 0xffffffff );  // draw if == 1
        glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
    
        
        // draw a big dark rectangle, cut out by stenciled areas

        glColor4f( 0, 0, 0, 0.25f ); 
        glBegin( GL_QUADS ); {
            glVertex2d( mAnchorX, mAnchorY );
            glVertex2d( mAnchorX, mAnchorY + mHeight );
            glVertex2d( mAnchorX + mWidth, mAnchorY + mHeight );
            glVertex2d( mAnchorX + mWidth, mAnchorY );
            }
        glEnd();
        

        // draw diagonal yellow hash lines cut out by stenciled areas

        glColor4f( 1, 1, 1, 0.25f ); 
        glBegin( GL_LINES ); {
            
            for( int x=0; x<mWidth; x+=3 ) {
                glVertex2d( mAnchorX + x, mAnchorY );
                glVertex2d( mAnchorX, mAnchorY + x );
                glVertex2d( mAnchorX + x + 2, mAnchorY + mHeight );
                glVertex2d( mAnchorX + mWidth + 2, mAnchorY + x );
                }
            }
        glEnd();
        
        // back to unstenciled drawing
        glDisable( GL_STENCIL_TEST );
        glClear( GL_STENCIL_BUFFER_BIT );
        }
    


    Color *oldFontColor = largeText->getFontColor()->copy();
    largeText->setFontColor( new Color( 0, 0, 0, 1 ) );


    if( ! mHintMode ) {
        
        // draw speech for each object
        int numObjects = mState->mObjects.size();
        
        // put 0's speech on top
        for( int i=1; i<numObjects; i++ ) {
            drawSpeech( i );
            }
        drawSpeech( 0 );
        }
    




    // draw any action text for object 0
    
    StateObjectInstance *o = *( mState->mObjects.getElement( 0 ) );

    if( ! mHintMode &&
        ( strlen( o->mAction ) >  0
          ||
          ( 0 == selected && mEditingAction ) ) ) {
        
        intPair pos = ::add( o->mActionOffset, o->mAnchorPosition );
        
        double height = 8;
        double width = strlen( o->mAction ) * height;
        
        double trueLength = largeText->measureTextWidth( o->mAction ) * height;

        int totalArrowLength = (int)trueLength + 10;
        
        
        double trueStringHeight = height * 
            largeText->measureTextHeight( o->mAction );
        
        char actionTall = false;
        
        if( trueStringHeight > 9 ) {
            // make room for high characters
            actionTall = true;
            }



        // flip anchor if on left side of player
        int flip = 1;
        Sprite *endSprite = mActionBoxEndSprite;
        
        if( o->mActionOffset.x < P/2 ) {
            flip = -1;
            endSprite = mActionBoxEndFlipSprite;
            }
        
        if( flip == 1 && pos.x - totalArrowLength < 0 ) {
            // off left, force flip
            flip = -1;
            endSprite = mActionBoxEndFlipSprite;
            }
        else if( flip == -1 && pos.x + totalArrowLength > P * G - 1 ) {
            // off right, force flip
            flip = 1;
            endSprite = mActionBoxEndSprite;
            }
        
             
        if( actionTall ) {
            if( endSprite == mActionBoxEndSprite ) {
                endSprite = mActionBoxEndTallSprite;
                }
            else {
                endSprite = mActionBoxEndFlipTallSprite;
                }
            }
        


        Vector3D boxPos( mAnchorX + pos.x - flip * 4,
                         mAnchorY + pos.y + 1,
                         0 );
        
        if( flip == -1 ) {
            // nudge
            boxPos.mX += 1;
            }
        
        
        double boxFade = 0.63;
        

        Vector3D boxStartPos = boxPos;

        endSprite->draw( 0, 0, &boxPos, 1, 
                         boxFade );
        
        int trueLengthLeft = (int)trueLength;
        
        // start can contain... 5 pixels?
        //trueLengthLeft -= 7;

        // go a slight bit over to leave extra space at end if it's tight
        // end cap adds to it as well
        while( trueLengthLeft > 14 ) {
            boxPos.mX -= flip * 8;
            
            if( actionTall ) {
                mActionBoxMiddleTallSprite->draw( 0, 0, &boxPos, 1, 
                                                  boxFade );
                }
            else {    
                mActionBoxMiddleSprite->draw( 0, 0, &boxPos, 1, 
                                              boxFade );
                }
            
            trueLengthLeft -= 8;
            }
        
        // now fill in rest, which is less that 8 pixels, with
        // smaller lines
        // sprite is 2 columns wide, but one column is transparent
        // (drawing 1-column-wide sprites, using centered-based drawing,
        //  causes round-off errors)

        // adjust a bit, since tall versions are lopsided
        if( actionTall && flip < 0 ) {
            boxPos.mY += 2;
            }

        char firstExtraLine = true;
        while( trueLengthLeft > 6 ) {
            boxPos.mX -= flip * 1;
            if( firstExtraLine ) {
                // extra offset from center of last full middle segment
                boxPos.mX -= flip * 4;
                firstExtraLine = false;
                }
            
            if( actionTall ) {    
                mActionBoxMiddleExtraTallSprite->draw( 0, 0, &boxPos, flip, 
                                                       boxFade );
                }
            else {
                mActionBoxMiddleExtraSprite->draw( 0, 0, &boxPos, flip, 
                                                   boxFade );
                }
            

            trueLengthLeft -= 1;
            }


        // cap with end

        if( !firstExtraLine ) {
            // used at least one extra line
            boxPos.mX -= flip * 4;
            }
        else {
            // used no extra lines, offst from last full middle segment
            boxPos.mX -= flip * 8;
            }
        
        
        if( actionTall ) {            
            mActionBoxStartTallSprite->draw( 0, 0, &boxPos, flip, 
                                             boxFade );
            }
        else {
            mActionBoxStartSprite->draw( 0, 0, &boxPos, flip, 
                                         boxFade );
            }
        
        
        if( flip == 1 ) {
            boxStartPos = boxPos;
            }
        else {
            // use real start pos, but adjust
            boxStartPos.mX += 7;
            }
        
        // make lower to increase chances of it fitting in box
        char *lowerAction = stringToLowerCase( o->mAction  );
        
        largeText->drawText( lowerAction, 
                             boxStartPos.mX - 4 + 1,
                             boxStartPos.mY - 4,
                             width, height );
        
        delete [] lowerAction;
        }
        
    
    

    // restore
    largeText->setFontColor( oldFontColor );
    
    

    if( mShowGrid && ! mHintMode ) {
        //grid lines
        
        glColor4f( 1, 1, 1, 0.25f ); 
        glBegin( GL_LINES ); {
            
            for( int x=0; x<G+1; x++ ) {
                glVertex2d( mAnchorX + x * P, mAnchorY );
                glVertex2d( mAnchorX + x * P, mAnchorY + mHeight );
                }
            for( int y=0; y<G+1; y++ ) {
                glVertex2d( mAnchorX, mAnchorY + y * P );
                glVertex2d( mAnchorX + mWidth, mAnchorY + y * P );
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
    

    if( mEditor != NULL ) {
        GameStateEditor *editor = (GameStateEditor *)mEditor;
        
        editor->displayRedrawed();
        }
    
    }



void GameStateDisplay::setLastMouse( double inX, double inY ) {
    mLastPixelClickX = (int)( inX - mAnchorX );
    mLastPixelClickY = (int)( inY - mAnchorY );
    
    mLastGridClickX = (int)( mLastPixelClickX / P );
    mLastGridClickY = (int)( mLastPixelClickY / P );
    
    mLastHoverGridX = mLastGridClickX;
    mLastHoverGridY = mLastGridClickY;
    }



void GameStateDisplay::mouseDragged( double inX, double inY ) {
    if( mHintMode ) return;
    

    mLastActionRelease = false;
    mLastActionPress = false;
    mLastActionKeyPress = false;

    if( isEnabled() && isInside( inX, inY ) ) {
        setLastMouse( inX, inY );
        fireActionPerformed( this );
        }
    }



void GameStateDisplay::mousePressed( double inX, double inY ) {
    if( mHintMode ) return;
    
    mLastActionRelease = false;
    mLastActionPress = true;
    mLastActionKeyPress = false;

    if( isEnabled() && isInside( inX, inY ) ) {
        setLastMouse( inX, inY );
        fireActionPerformed( this );

        // selected object may have changed... update tool tip
        mLastMouseoverObjectIndex = -1;
        
        if( mShowObjectToolTips ) {
            redoToolTip( mLastGridClickX, mLastGridClickY );
            }
        }
    
    }



void GameStateDisplay::mouseReleased( double inX, double inY ) {
    if( mHintMode ) return;
    
    mLastActionRelease = true;
    mLastActionPress = false;
	mLastActionKeyPress = false;
    
    // done dragging with mouse
    mManualBubblePositioningLive = false;
    

    if( isEnabled() && isInside( inX, inY ) ) {
        setLastMouse( inX, inY );
        fireActionPerformed( this );
        }
    }




#include "ToolTipManager.h"
#include "minorGems/util/TranslationManager.h"


void GameStateDisplay::redoToolTip( int inGridX, int inGridY ) {
    int objIndex = mState->getHitObject( inGridX, inGridY );
        
    if( objIndex != -1 ) {
        if( objIndex != mLastMouseoverObjectIndex ) {
                

            StateObjectInstance *objInstance =
                *( mState->mObjects.getElement( objIndex ) );
                
            char *objName = 
                objInstance->mObject.getStateObjectName();
                

            char *tipString;
                
            int selected = mState->getSelectedObject();
                
            char *selectedString = (char *)"";
            if( selected == objIndex ) {
                selectedString = 
                    (char*)
                    TranslationManager::translate( "tip_anchor_selected" );
                }
                

            char *transKey;
            if( objIndex == 0 ) {
                transKey = (char*)"tip_player_anchor";
                }
            else {
                transKey = (char*)"tip_object_anchor";
                }

                
            tipString = autoSprintf( 
                "%s (%s) %s", 
                TranslationManager::translate( transKey ),
                objName,
                selectedString );
                
            delete [] objName;
                
                        
            ToolTipManager::setTip( tipString );
            delete [] tipString;
            }
        }
    else {            
        ToolTipManager::setTip( NULL );
        }

    mLastMouseoverObjectIndex = objIndex;
    }



void GameStateDisplay::mouseMoved( double inX, double inY ) {
    if( mHintMode ) return;
    
    if( isEnabled() && isInside( inX, inY ) ) {

        int pixelX = (int)( inX - mAnchorX );
        int pixelY = (int)( inY - mAnchorY );
    
        int gridX = (int)( pixelX / P );
        int gridY = (int)( pixelY / P );
        
        if( mShowObjectToolTips ) {
            redoToolTip( gridX, gridY );
            }
        
        mMouseHover = true;
        setLastMouse( inX, inY );
        }
    else {
        mMouseHover = false;
        }

    }



void GameStateDisplay::setFocus( char inFocus ) {
    mFocused = inFocus;
    }

char GameStateDisplay::isFocused() {
    return mFocused;
    }


void GameStateDisplay::keyPressed( unsigned char inKey, 
                                   double inX, double inY ) {
    if( mHintMode ) return;

    mLastActionKeyPress = true;
    mLastKeyPressed = inKey;
    
    if( isEnabled() ) {
        fireActionPerformed( this );
        }
    }
