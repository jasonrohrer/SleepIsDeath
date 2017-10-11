#include "TimbreEditor.h"
#include "TimbrePicker.h"
#include "BorderPanel.h"
#include "labels.h"
#include "SelectionManager.h"
#include "packSaver.h"
#include "GridOverlay.h"

#include "minorGems/util/log/AppLog.h"


extern TimbrePicker *mainTimbrePicker;



extern int gameWidth, gameHeight;


extern TextGL *largeTextFixed;



template <>
void SizeLimitedVector<TimbreResource>::deleteElementOfType(
    TimbreResource inElement ) {
    // no delete necessary
    }



static rgbaColor emptyC =  { { 0,0,0,255 } };



TimbreEditor::TimbreEditor( ScreenGL *inScreen )
        : Editor( inScreen ),
          mPlayerTimbreIndex( 0 ),
          mUndoStack( MAX_UNDOS, false ) {

    mCloseButton->setToolTip( "tip_closeEdit_timbre" );    
    
    mSidePanel->add( mainTimbrePicker );

    mainTimbrePicker->addActionListener( this );



    double offset = P;
 
    double buttonSize = (gameHeight - 2 * offset - 8) / P;
    
    
    rgbaColor c =  { { 0,0,0,255 } };
    
    
                
    for( int y=0; y<F; y++ ) {
        for( int x=0; x<FL; x++ ) {

            mButtonGrid[y][x] = new ColorButtonGL(
                c,
                8 + x * buttonSize,
                gameWidth - ( 48 + (y + 1) * buttonSize ),
                buttonSize,
                buttonSize );
            

            mMainPanel->add( mButtonGrid[y][x] );
            
            mButtonGrid[y][x]->addActionListener( this );
            }
       
        mZeroButtons[y] =new ColorButtonGL(
            c,
            8 - buttonSize,
            gameWidth - ( 48 + (y + 1) * buttonSize ),
            buttonSize,
            buttonSize );

        mMainPanel->add( mZeroButtons[y] );
        
        mZeroButtons[y]->addActionListener( this );
        
        // force dark border to make black button invisible
        mZeroButtons[y]->setColor( emptyC, true );
        }


    

    EightPixelLabel *fieldLabel = new EightPixelLabel( 150, 54,
                                                       "timbreName" );
    mMainPanel->add( fieldLabel );



    int fieldHeight = 8;
    int fieldWidth = 8 * 10;
    
    const char *defaultText = "default";

    mNameField = new TextFieldGL( 150,
                                     43,
                                     fieldWidth,
                                     fieldHeight,
                                     1,
                                     defaultText,
                                     largeTextFixed,
                                     new Color( 0.75, 0.75, 0.75 ),
                                     new Color( 0.75, 0.75, 0.75 ),
                                     new Color( 0.15, 0.15, 0.15 ),
                                     new Color( 0.75, 0.75, 0.75 ),
                                     10,
                                     false );
    mMainPanel->add( mNameField );

    mNameField->setFocus( true );
    //mNameField->lockFocus( true );
    
    mNameField->setCursorPosition( strlen( defaultText ) );
    
    mNameField->addActionListener( this );

    




    // center add button
    double gridEdge = 8 + buttonSize * FL;
    
    double extra = gameHeight - gridEdge;
    

    // center it vertically on timbre picker
    double addY = mainTimbrePicker->getAnchorY() +
        mainTimbrePicker->getHeight() - 15;
    
    double sideButtonsX = gridEdge + (extra - 16) / 2;

    mAddButton = new AddButtonGL( sideButtonsX, 
                                  addY, 
                                  16, 16 );
    mMainPanel->add( mAddButton );
    mAddButton->addActionListener( this );
    mAddButton->setToolTip( "tip_addTimbre" );
    
    mAddAction = false;
    

    double miniButtonSize = P + 4;
    
    mMiniViewButton = new SpriteButtonGL( 
        NULL, 1,
        gridEdge + ( extra - miniButtonSize ) / 2,
        addY - 24,
        miniButtonSize,
        miniButtonSize );
    
    mMainPanel->add( mMiniViewButton );    


    
    double undoButtonY = gameWidth - ( 48 + FL * buttonSize );

    mUndoButton = new UndoButtonGL( sideButtonsX, undoButtonY, 16, 16 );
    mMainPanel->add( mUndoButton );
    mUndoButton->addActionListener( this );
    mUndoButton->setEnabled( false );

    mRedoButton = new RedoButtonGL( sideButtonsX, undoButtonY + 19, 16, 16 );
    mMainPanel->add( mRedoButton );
    mRedoButton->addActionListener( this );
    mRedoButton->setEnabled( false );


    Color *thumbColor = new Color( .5, .5, .5, .5 );
    Color *borderColor = new Color( .35, .35, .35, .35 );
    
    
    mAttackSlider = new ToolTipSliderGL( 8, 48,
                                         40, 10,
                                         NULL, 0,
                                         new Color( 0, 1, 0, 1 ),
                                         new Color( 1, 1, 0, 1 ),
                                         thumbColor->copy(),
                                         borderColor->copy(),
                                         1, 4, 1 );
    
    mMainPanel->add( mAttackSlider );
    mAttackSlider->setThumbPosition( 1.0 );
    mAttackSlider->addActionListener( this );
    mAttackSlider->setToolTip( "tip_attack" );


    mHoldSlider = new ToolTipSliderGL( 50, 48,
                                        40, 10,
                                        NULL, 0,
                                        new Color( 1, 1, 0, 1 ),
                                        new Color( 1, 1, 0, 1 ),
                                        thumbColor->copy(),
                                        borderColor->copy(),
                                        1, 4, 1 );
    
    mMainPanel->add( mHoldSlider );
    mHoldSlider->setThumbPosition( 1.00 );
    mHoldSlider->addActionListener( this );
    mHoldSlider->setToolTip( "tip_hold" );



    mReleaseSlider = new ToolTipSliderGL( 92, 48,
                                          40, 10,
                                          NULL, 0,
                                          new Color( 1, 1, 0, 1 ),
                                          new Color( 1, 0, 0, 1 ),
                                          thumbColor->copy(),
                                          borderColor->copy(),
                                          1, 4, 1 );
    
    mMainPanel->add( mReleaseSlider );
    mReleaseSlider->setThumbPosition( 0 );
    mReleaseSlider->addActionListener( this );
    mReleaseSlider->setToolTip( "tip_release" );


    delete thumbColor;
    delete borderColor;
    


    for( int i=0; i<NUM_OCTAVES; i++ ) {
        
        double x = mMiniViewButton->getAnchorX() + 3;
        double y = mMiniViewButton->getAnchorY() - 10 - 11 * (i+1) - 1;
                
        mOctaveButtons[i] = new SelectableButtonGL( 
            new Sprite( "octave.tga", true ),
            1,
            x, y, 12, 12 );
        
        mOctaveButtons[i]->setToolTip( "tip_octave" );
        
        mMainPanel->add( mOctaveButtons[i] );
        
        mOctaveButtons[i]->addActionListener( this );
        mOctaveButtons[i]->setEnabled( true );
        }




    setTimbreToEdit( mainTimbrePicker->getSelectedResource() );
    





    mPenDown = false;
    mIgnoreSliders = false;
    

    GridOverlay *overlay = new GridOverlay( 
        8, 
        gameWidth - ( 48 + F * buttonSize ),
        FL * buttonSize, F * buttonSize,
        F );
    
    mMainPanel->add( overlay );


    postConstruction();
    }



TimbreEditor::~TimbreEditor() {
    mSidePanel->remove( mainTimbrePicker );    
    }


extern Color harmonicLowColor;
extern Color harmonicHighColor;

#include "musicPlayer.h"


void TimbreEditor::setTimbreToEdit( TimbreResource inTimbre,
                                    char inUpdatePlayerTimbre,
                                    char inUpdatePlayerEnvelope ) {
    mTimbreToEdit = inTimbre;
    /*
    rgbaColor fullC = { { (unsigned char)( harmonicLowColor.r * 255 ),
                          (unsigned char)( harmonicLowColor.g * 255 ),
                          (unsigned char)( harmonicLowColor.b * 255 ) } };
    rgbaColor fullHotC = { { (unsigned char)( harmonicHighColor.r * 255 ),
                             (unsigned char)( harmonicHighColor.g * 255 ),
                             (unsigned char)( harmonicHighColor.b * 255 ) } };
    */

    rgbaColor fullHotC = mTimbreToEdit.getTimbreColor();
    rgbaColor fullC = fullHotC;
    fullC.comp.r = (unsigned char)( fullC.comp.r * 0.50 );
    fullC.comp.g = (unsigned char)( fullC.comp.g * 0.50 );
    fullC.comp.b = (unsigned char)( fullC.comp.b * 0.50 );
    


    for( int y=0; y<F; y++ ) {
        for( int x=0; x<FL; x++ ) {
            if( mTimbreToEdit.getHarmonicLevel( y ) > x ) {
                
                mButtonGrid[F - y - 1][x]->setColor( 
                    blend( fullC, fullHotC, x / (double)FL ), true );
                }
            else {
                mButtonGrid[F - y - 1][x]->setColor( emptyC );
                }
            }
        }


    refreshMiniView();

    char *name = mTimbreToEdit.getTimbreName();
    
    mNameField->setText( name );
    mNameField->setCursorPosition( strlen( name ) );
    
    delete [] name;


    mIgnoreSliders = true;
    // round to nearest pixel
    // attack slider works in reverse
    mAttackSlider->setThumbPosition( 
        (int)( (255 - mTimbreToEdit.getAttack()) / 
               255.0 * 80 ) 
        / 80.0 );

    mHoldSlider->setThumbPosition( 
        (int)( mTimbreToEdit.getHold() / 
               255.0 * 80 ) 
        / 80.0 );

    mReleaseSlider->setThumbPosition( 
        (int)( mTimbreToEdit.getRelease() / 
               255.0 * 80 ) 
        / 80.0 );
    mIgnoreSliders = false;

    
    int octave = mTimbreToEdit.getOctavesDown();
    
    for( int i=0; i<NUM_OCTAVES; i++ ) {
        mOctaveButtons[i]->setSelected( i == octave );
        }
    


    mTimbreToEdit.setInPlayer( mPlayerTimbreIndex,
                               inUpdatePlayerTimbre, inUpdatePlayerEnvelope );
    }



void TimbreEditor::setPlayerTimbreToEdit( int inTimbre ) {
    mPlayerTimbreIndex = inTimbre;
    }



void TimbreEditor::refreshMiniView() {
    // don't use cached version
    mMiniViewButton->setSprite( mTimbreToEdit.getSprite( false, false ) );
    }


void TimbreEditor::saveUndoPoint() {
    mUndoStack.push_back( mTimbreToEdit );
    mUndoButton->setEnabled( true );
    
    // new branch... "redo" future now impossible
    mRedoStack.deleteAll();
    mRedoButton->setEnabled( false );
    }


void TimbreEditor::actionPerformed( GUIComponent *inTarget ) {
    // superclass
    Editor::actionPerformed( inTarget );
    

    for( int i=0; i<NUM_OCTAVES; i++ ) {
        if( inTarget == mOctaveButtons[i] ) {
            char oldSelected = mOctaveButtons[i]->getSelected();
            
            if( !oldSelected ) {
                saveUndoPoint();
                
                mTimbreToEdit.editOctavesDown( i );
            
                setTimbreToEdit( mTimbreToEdit, true, false );
                }
            return;
            }
        }
    

    if( inTarget == mainTimbrePicker ) {
        TimbreResource timbrePicked = mainTimbrePicker->getSelectedResource();
        
        if( ! mAddAction &&
            ! mainTimbrePicker->wasLastActionFromPress() ) {
            // will change timbre
                        
            mUndoStack.push_back( mTimbreToEdit );
            mUndoButton->setEnabled( true );
            
            setTimbreToEdit( timbrePicked );
            
            // new branch... "redo" future now impossible
            mRedoStack.deleteAll();
            mRedoButton->setEnabled( false );
            }        
        }
    else if( inTarget == mNameField ) {
        mUndoStack.push_back( mTimbreToEdit );
        mUndoButton->setEnabled( true );

        // new branch... "redo" future now impossible
        mRedoStack.deleteAll();
        mRedoButton->setEnabled( false );
        
        mTimbreToEdit.editTimbreName( mNameField->getText() );
        }
    else if( inTarget == mAddButton ) {
        addTimbre();
        }
    else if( inTarget == mUndoButton ) {
        int lastIndex = mUndoStack.size() - 1;
        
        TimbreResource last = *( mUndoStack.getElement( lastIndex ) );
        mUndoStack.deleteElement( lastIndex );
        if( mUndoStack.size() == 0 ) {
            mUndoButton->setEnabled( false );
            }
        
        mRedoStack.push_back( mTimbreToEdit );
        mRedoButton->setEnabled( true );
        
        setTimbreToEdit( last );
        }
    else if( inTarget == mRedoButton ) {
        int nextIndex = mRedoStack.size() - 1;
        
        TimbreResource next = *( mRedoStack.getElement( nextIndex ) );
        mRedoStack.deleteElement( nextIndex );
        if( mRedoStack.size() == 0 ) {
            mRedoButton->setEnabled( false );
            }
        
        mUndoStack.push_back( mTimbreToEdit );
        mUndoButton->setEnabled( true );
        
        setTimbreToEdit( next );
        }
    else if( inTarget == mAttackSlider && ! mIgnoreSliders ) {
        if( mAttackSlider->mJustPressed ) {
            // first move in this adjustment, save an undo point here
            saveUndoPoint();
            }
        // attack reversed
        int attack = 255 - 
            (int)( 255 * mAttackSlider->getThumbPosition() );
        int hold = 
            (int)( 255 * mHoldSlider->getThumbPosition() );
        int release = 
            (int)( 255 * mReleaseSlider->getThumbPosition() );
        
        if( release + attack + hold > 255 ) {
            if( hold > 0 ) {    
                // push hold slider
                hold = 255 - attack - release;    
                
                if( hold < 0 ) {
                    hold = 0;
                    }
                
                mIgnoreSliders = true;
                // round to nearest pixel
                mHoldSlider->setThumbPosition( 
                    (int)( hold / 255.0 * 80 ) 
                    / 80.0 );
                mIgnoreSliders = false;
                }
            
            if( release + attack + hold > 255 ) {
                // push release
                release = 255 - attack - hold;    
                mIgnoreSliders = true;
                // round to nearest pixel
                mReleaseSlider->setThumbPosition( 
                    (int)( release / 255.0 * 80 ) 
                    / 80.0 );
                mIgnoreSliders = false;
                }
            }
        

        mTimbreToEdit.editEnvelope( attack, hold, release );

        if( mAttackSlider->mJustReleased ) {
            // actually adjust 

            mTimbreToEdit.setInPlayer( mPlayerTimbreIndex,
                                       false, true );
            }
        }
    else if( inTarget == mHoldSlider && ! mIgnoreSliders ) {
        if( mHoldSlider->mJustPressed ) {
            // first move in this adjustment, save an undo point here
            saveUndoPoint();
            }
        
        // attack reversed
        int attack = 255 - 
            (int)( 255 * mAttackSlider->getThumbPosition() );
        int hold = 
            (int)( 255 * mHoldSlider->getThumbPosition() );
        int release = 
            (int)( 255 * mReleaseSlider->getThumbPosition() );
        
        if( release + attack + hold > 255 ) {

            if( attack > 0 ) {    
                // push attack slider
                attack = 255 - release - hold;    
                
                if( attack < 0 ) {
                    attack = 0;
                    }

                mIgnoreSliders = true;
                // round to nearest pixel
                mAttackSlider->setThumbPosition( 
                    (int)( (255 - attack) / 255.0 * 80 ) 
                    / 80.0 );
                mIgnoreSliders = false;
                }
            
            if( release + attack + hold > 255 ) {
                // push release
                release = 255 - hold - attack;    
                mIgnoreSliders = true;
                // round to nearest pixel
                mReleaseSlider->setThumbPosition( 
                    (int)( release / 255.0 * 80 ) 
                    / 80.0 );
                mIgnoreSliders = false;
                }
            }
        

        mTimbreToEdit.editEnvelope( attack, hold, release );
        
        if( mHoldSlider->mJustReleased ) {
            // actually adjust 

            mTimbreToEdit.setInPlayer( mPlayerTimbreIndex,
                                       false, true );
            }
        }
    else if( inTarget == mReleaseSlider && ! mIgnoreSliders ) {
        if( mReleaseSlider->mJustPressed ) {
            // first move in this adjustment, save an undo point here
            saveUndoPoint();
            }
        
        // release reversed
        int attack = 255 -
            (int)( 255 * mAttackSlider->getThumbPosition() );
        int hold = 
            (int)( 255 * mHoldSlider->getThumbPosition() );
        int release = 
            (int)( 255 * mReleaseSlider->getThumbPosition() );
        
        if( release + attack + hold > 255 ) {
            if( hold > 0 ) {
                if( hold < 0 ) {
                    hold = 0;
                    }

                // push hold
                hold = 255 - release - attack;    
                mIgnoreSliders = true;
                // round to nearest pixel
                mHoldSlider->setThumbPosition( 
                    (int)( hold / 255.0 * 80 ) 
                    / 80.0 );
                mIgnoreSliders = false;
                }
            

            if( release + attack + hold > 255 ) {
                // push attack slider
                attack = 255 - release - hold;    
                
                mIgnoreSliders = true;
                // round to nearest pixel
                mAttackSlider->setThumbPosition( 
                    (int)( (255 - attack) / 255.0 * 80 ) 
                    / 80.0 );
                mIgnoreSliders = false;
                }
            }
        

        mTimbreToEdit.editEnvelope( attack, hold, release );

        if( mReleaseSlider->mJustReleased ) {
            // actually adjust 
            
            mTimbreToEdit.setInPlayer( mPlayerTimbreIndex,
                                       false, true );
            }
        }
    else {
        // check grid
        char found = false;
        
        for( int y=0; y<F && !found; y++ ) {
            for( int x=0; x<FL && !found; x++ ) {
                if( ( inTarget == mButtonGrid[y][x] &&
                      ! mButtonGrid[y][x]->wasLastActionHover() )
                    || 
                    ( inTarget == mZeroButtons[y] &&
                      ! mZeroButtons[y]->wasLastActionHover() ) ) {
                    
                    if( inTarget == mZeroButtons[y] ) {
                        // okay, since we're leaving loop anyway
                        x = -1;
                        }
                    

                    ColorButtonGL *targetButton = (ColorButtonGL*)inTarget;
                    
                    found = true;
                    
                    if( !mPenDown ) {
                        mPenDown = true;
                        // save undo only from start of mouse action
                        saveUndoPoint();
                        }
                    else {
                        if( ! targetButton->isPressed() ) {
                            // release
                            mPenDown = false;
                            }
                        }
                    

                    int oldHarmonicLevel = 
                        mTimbreToEdit.getHarmonicLevel( F - y - 1 );
                    
                    if( x+1 != oldHarmonicLevel ) {


                        // improve performance by only updating when changed

                                            

                        mTimbreToEdit.editHarmonic( F - y - 1, x+1 );
                    
                        setTimbreToEdit( mTimbreToEdit, true, false );
                        }

                    }
                }
            }
        }
    
    
    }



void TimbreEditor::editorClosing() {
    addTimbre();
    }



void TimbreEditor::addTimbre() {
    mAddAction = true;
    mTimbreToEdit.finishEdit();
    mainTimbrePicker->setSelectedResource( mTimbreToEdit, true );
    mAddAction = false;
    }

