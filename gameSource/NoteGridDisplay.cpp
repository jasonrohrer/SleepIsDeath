#include "NoteGridDisplay.h"
#include "Song.h"
#include "musicPlayer.h"
#include "ScalePicker.h"


// from musicPlayer.cpp
extern char noteToggles[PARTS][S][N][N];

extern int lastNoteColumnPlayed;


extern ScalePicker *mainScalePicker;



NoteGridDisplay::NoteGridDisplay( int inAnchorX, int inAnchorY )
        : GUIComponentGL( inAnchorX, inAnchorY, N * W, N * W ),
          mLastActionRelease( false ), mLastActionPress( false ) {
    
    Scale s = mainScalePicker->getSelectedResource();
    mNotesInOctave = s.getNumOn();

    mNoteSprite = new Sprite( "note.tga", true );
    mNoteSpaceSprite = new Sprite( "noteSpace.tga", true );
    
    for( int i=0; i<N; i++ ) {
        mColumnGlow[i] = 0;
        }

    mainScalePicker->addActionListener( this );    
    }

        
NoteGridDisplay::~NoteGridDisplay() {
    delete mNoteSprite;
    delete mNoteSpaceSprite;
    }



void NoteGridDisplay::resetColumnGlow() {
    for( int x=0; x<N; x++ ) {
        mColumnGlow[x] = 0;
        }
    }



extern Color lowColor;
extern Color medColor;
extern Color hiColor;

static Color *drawColors[3] = { &lowColor, &medColor, &hiColor };

        

void NoteGridDisplay::fireRedraw() {


    
    
    
    Color *drawColor = &lowColor;
    for( int y=0; y<N; y++ ) {

        // new color band per octave, wrap around
        drawColor = drawColors[ ( y / mNotesInOctave ) % 3 ];
        

        for( int x=0; x<N; x++ ) {
            Vector3D pos( mAnchorX + W * x + W/2,
                          mAnchorY + W * y + W/2,
                          0 );
            
            double fadeFactor = 0.5;
            
            if( x == lastNoteColumnPlayed  ) {
                if( mColumnGlow[x] == 0 ) {
                    // fresh glow here
                    mColumnGlow[x] = 1;
                    }
                }

            if( mColumnGlow[x] > 0 ) {
                
                mColumnGlow[x] -= 0.0015;
                if( mColumnGlow[x] < 0 ) {
                    mColumnGlow[x] = 0;
                    }
                }
            
            fadeFactor = 0.4 + 0.6 * mColumnGlow[x];
            

            mNoteSpaceSprite->draw( 0, 0, &pos, 1, fadeFactor * 0.5,
                                    drawColor );
            
            // last instrument reserved for phrase display
            if( noteToggles[SI][0][y][x] ) {
                mNoteSprite->draw( 0, 0, &pos, 1, fadeFactor * 1.0,
                                   drawColor );
                }
            }
        }
    
    }



void NoteGridDisplay::mouseDragged( double inX, double inY ) {
    mLastActionRelease = false;
    mLastActionPress = false;

    if( isEnabled() && isInside( inX, inY ) ) {
        
        int x = (int)( (inX - mAnchorX) / W );
        int y = (int)( (inY - mAnchorY) / W );
        
        noteToggles[SI][0][y][x] = mPressedInk;

        fireActionPerformed( this );
        }
    }




void NoteGridDisplay::mousePressed( double inX, double inY ) {
    mLastActionRelease = false;
    mLastActionPress = true;
    
    if( isEnabled() && isInside( inX, inY ) ) {
        
        int x = (int)( (inX - mAnchorX) / W );
        int y = (int)( (inY - mAnchorY) / W );
        

        noteToggles[SI][0][y][x] = ! noteToggles[SI][0][y][x];
        
        mPressedInk = noteToggles[SI][0][y][x];

        fireActionPerformed( this );
        }
    }



void NoteGridDisplay::mouseReleased( double inX, double inY ) {
    mLastActionRelease = true;
    mLastActionPress = false;
    
    if( isEnabled() && isInside( inX, inY ) ) {
        
        int x = (int)( (inX - mAnchorX) / W );
        int y = (int)( (inY - mAnchorY) / W );
        
        noteToggles[SI][0][y][x] = mPressedInk;

        fireActionPerformed( this );
        }
    }



void NoteGridDisplay::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == mainScalePicker ) {
        Scale s = mainScalePicker->getSelectedResource();
        mNotesInOctave = s.getNumOn();
        }
    }

