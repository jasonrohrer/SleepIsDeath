#include "ScaleEditor.h"
#include "ScalePicker.h"
#include "SongEditor.h"
#include "BorderPanel.h"
#include "labels.h"
#include "SelectionManager.h"
#include "packSaver.h"

#include "minorGems/util/log/AppLog.h"


extern ScalePicker *mainScalePicker;

extern SongEditor *mainSongEditor;



extern int gameWidth, gameHeight;


extern TextGL *largeTextFixed;



extern Color toneLowColor;
extern Color toneHighColor;



template <>
void SizeLimitedVector<Scale>::deleteElementOfType(
    Scale inElement ) {
    // no delete necessary
    }



ScaleEditor::ScaleEditor( ScreenGL *inScreen )
        : Editor( inScreen ),
          mUndoStack( MAX_UNDOS, false ) {

    mCloseButton->setToolTip( "tip_closeEdit_scale" );    
    
    mSidePanel->add( mainScalePicker );

    mainScalePicker->addActionListener( this );



    double offset = P;
 
    double buttonSize = (gameHeight - 2 * offset - 8) / P;
    
                
    for( int y=0; y<HT; y++ ) {
        mScaleButtons[y] = new ScaleToggleButton( 
            new Sprite( "scaleSpace.tga", true ), 
            new Sprite( "note.tga", true ),
            1,
            8 + y * 16,
            ( 48 + (y + 1) * 16 ),
            16,
            16 );
        
        Color *c = Color::linearSum( &toneLowColor, &toneHighColor, 
                                     (HT - y - 1) / (double)(HT - 1 ) );
        mScaleButtons[y]->setColor( c );
        

        mMainPanel->add( mScaleButtons[y] );
        
        mScaleButtons[y]->addActionListener( this );
        }


    

    EightPixelLabel *fieldLabel = new EightPixelLabel( 150, 54,
                                                       "scaleName" );
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
    double gridEdge = 8 + buttonSize * P;
    
    double extra = gameHeight - gridEdge;
    

    // center it vertically on scale picker
    double addY = mainScalePicker->getAnchorY() +
        mainScalePicker->getHeight() - 15;
    
    double sideButtonsX = gridEdge + (extra - 16) / 2;

    mAddButton = new AddButtonGL( sideButtonsX, 
                                  addY, 
                                  16, 16 );
    mMainPanel->add( mAddButton );
    mAddButton->addActionListener( this );
    mAddButton->setToolTip( "tip_addScale" );
    
    mAddAction = false;
    

    double miniButtonSize = P + 4;
    
    mMiniViewButton = new SpriteButtonGL( 
        NULL, 1,
        gridEdge + ( extra - miniButtonSize ) / 2,
        addY - 24,
        miniButtonSize,
        miniButtonSize );
    
    mMainPanel->add( mMiniViewButton );    


    
    double undoButtonY = gameWidth - ( 48 + P * buttonSize );

    mUndoButton = new UndoButtonGL( sideButtonsX, undoButtonY, 16, 16 );
    mMainPanel->add( mUndoButton );
    mUndoButton->addActionListener( this );
    mUndoButton->setEnabled( false );

    mRedoButton = new RedoButtonGL( sideButtonsX, undoButtonY + 19, 16, 16 );
    mMainPanel->add( mRedoButton );
    mRedoButton->addActionListener( this );
    mRedoButton->setEnabled( false );


    mPartToWatch = 0;

    setScaleToEdit( mainScalePicker->getSelectedResource() );

    postConstruction();
    }



ScaleEditor::~ScaleEditor() {
    mSidePanel->remove( mainScalePicker );    
    }


extern Color harmonicLowColor;
extern Color harmonicHighColor;

#include "musicPlayer.h"



void ScaleEditor::setPartToWatch( int inPartNumber ) {
    mPartToWatch = inPartNumber;
    
    int toneNumber = 0;
    int numTones = mScaleToEdit.getNumOn();
    
    for( int y=0; y<HT; y++ ) {
        char toneOn = mScaleToEdit.getToneOn( y );
        
        mScaleButtons[y]->setState( toneOn );
        if( toneOn ) {
            mScaleButtons[y]->setMusicInfo( toneNumber, numTones, 
                                            mPartToWatch );
        
            toneNumber++;
            }
        else {
            mScaleButtons[y]->setMusicInfo( -1, numTones, mPartToWatch );
            }
        }
    }




void ScaleEditor::setScaleToEdit( Scale inScale ) {
    mScaleToEdit = inScale;

    // redo button note monitors
    setPartToWatch( mPartToWatch );
    
    

    refreshMiniView();

    char *name = mScaleToEdit.getScaleName();
    
    mNameField->setText( name );
    mNameField->setCursorPosition( strlen( name ) );
    
    delete [] name;    


    mScaleToEdit.setInPlayer();

    if( mainSongEditor != NULL ) {
        mainSongEditor->scaleChanged();
        }
    }



void ScaleEditor::refreshMiniView() {
    // don't use cached version
    mMiniViewButton->setSprite( mScaleToEdit.getSprite( false, false ) );
    }


void ScaleEditor::saveUndoPoint() {
    mUndoStack.push_back( mScaleToEdit );
    mUndoButton->setEnabled( true );
    
    // new branch... "redo" future now impossible
    mRedoStack.deleteAll();
    mRedoButton->setEnabled( false );
    }


void ScaleEditor::actionPerformed( GUIComponent *inTarget ) {
    // superclass
    Editor::actionPerformed( inTarget );
    

    if( inTarget == mainScalePicker ) {
        Scale scalePicked = mainScalePicker->getSelectedResource();
        
        if( ! mAddAction &&
            ! mainScalePicker->wasLastActionFromPress() ) {
            // will change scale
                        
            mUndoStack.push_back( mScaleToEdit );
            mUndoButton->setEnabled( true );
            
            setScaleToEdit( scalePicked );
            
            // new branch... "redo" future now impossible
            mRedoStack.deleteAll();
            mRedoButton->setEnabled( false );
            }        
        }
    else if( inTarget == mNameField ) {
        mUndoStack.push_back( mScaleToEdit );
        mUndoButton->setEnabled( true );

        // new branch... "redo" future now impossible
        mRedoStack.deleteAll();
        mRedoButton->setEnabled( false );
        
        mScaleToEdit.editScaleName( mNameField->getText() );
        }
    else if( inTarget == mAddButton ) {
        addScale();
        }
    else if( inTarget == mUndoButton ) {
        int lastIndex = mUndoStack.size() - 1;
        
        Scale last = *( mUndoStack.getElement( lastIndex ) );
        mUndoStack.deleteElement( lastIndex );
        if( mUndoStack.size() == 0 ) {
            mUndoButton->setEnabled( false );
            }
        
        mRedoStack.push_back( mScaleToEdit );
        mRedoButton->setEnabled( true );
        
        setScaleToEdit( last );
        }
    else if( inTarget == mRedoButton ) {
        int nextIndex = mRedoStack.size() - 1;
        
        Scale next = *( mRedoStack.getElement( nextIndex ) );
        mRedoStack.deleteElement( nextIndex );
        if( mRedoStack.size() == 0 ) {
            mRedoButton->setEnabled( false );
            }
        
        mUndoStack.push_back( mScaleToEdit );
        mUndoButton->setEnabled( true );
        
        setScaleToEdit( next );
        }
    else {
        // check scale buttons
        char found = false;
        
        for( int y=0; y<HT && !found; y++ ) {
            if( inTarget == mScaleButtons[y]  ) {
                    

                mUndoStack.push_back( mScaleToEdit );
                mUndoButton->setEnabled( true );
                
                // new branch... "redo" future now impossible
                mRedoStack.deleteAll();
                mRedoButton->setEnabled( false );

                found = true;
                    
                int oldToneOn = 
                    mScaleToEdit.getToneOn( y );
                    
                mScaleToEdit.editScale( y, !oldToneOn );
                                    
                setScaleToEdit( mScaleToEdit );
                }
            }
        }
    
    
    }



void ScaleEditor::editorClosing() {
    addScale();
    }



void ScaleEditor::addScale() {
    mAddAction = true;
    mScaleToEdit.finishEdit();
    mainScalePicker->setSelectedResource( mScaleToEdit, true );
    mAddAction = false;
    }

