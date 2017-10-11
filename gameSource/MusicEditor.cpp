#include "MusicEditor.h"
#include "MusicPicker.h"
#include "Song.h"
#include "BorderPanel.h"
#include "labels.h"
#include "SelectionManager.h"
#include "musicPlayer.h"

#include "minorGems/util/log/AppLog.h"


// from musicPlayer.cpp
extern char noteToggles[PARTS][S][N][N];
extern int partLengths[PARTS];


extern MusicPicker *mainMusicPicker;



extern int gameWidth, gameHeight;


extern TextGL *largeTextFixed;



template <>
void SizeLimitedVector<Music>::deleteElementOfType(
    Music inElement ) {
    // no delete necessary
    }



MusicEditor::MusicEditor( ScreenGL *inScreen )
        : Editor( inScreen ),
          mUndoStack( MAX_UNDOS, false ) {

    mCloseButton->setToolTip( "tip_closeEdit_music" );    
    
    mSidePanel->add( mainMusicPicker );

    mainMusicPicker->addActionListener( this );


    mNoteDisplay = new NoteGridDisplay( 8, gameWidth - 48 - N * W );
    mMainPanel->add( mNoteDisplay );

    mNoteDisplay->addActionListener( this );
    

    EightPixelLabel *fieldLabel = new EightPixelLabel( 150, 54,
                                                       "musicName" );
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
    double gridEdge = 8 + N * W;
    
    double extra = gameHeight - gridEdge;
    

    // center it vertically on music picker
    double addY = mainMusicPicker->getAnchorY() +
        mainMusicPicker->getHeight() - 15;
    
    double sideButtonsX = gridEdge + (extra - 16) / 2;

    mAddButton = new AddButtonGL( sideButtonsX, 
                                  addY, 
                                  16, 16 );
    mMainPanel->add( mAddButton );
    mAddButton->addActionListener( this );
    mAddButton->setToolTip( "tip_addMusic" );
    
    mAddAction = false;
    

    double miniButtonSize = P + 4;
    
    mMiniViewButton = new SpriteButtonGL( 
        NULL, 1,
        gridEdge + ( extra - miniButtonSize ) / 2,
        addY - 24,
        miniButtonSize,
        miniButtonSize );
    
    mMainPanel->add( mMiniViewButton );
    



    mTransformToolSet = new TransformToolSet( sideButtonsX, 
                                              mMiniViewButton->getAnchorY()
                                              - 4 - 100 ) ;
    
    mMainPanel->add( mTransformToolSet );    
    
    mTransformToolSet->addActionListener( this );
    


    
    double undoButtonY = gameWidth - ( 48 + N * W );

    mUndoButton = new UndoButtonGL( sideButtonsX, undoButtonY, 16, 16 );
    mMainPanel->add( mUndoButton );
    mUndoButton->addActionListener( this );
    mUndoButton->setEnabled( false );

    mRedoButton = new RedoButtonGL( sideButtonsX, undoButtonY + 19, 16, 16 );
    mMainPanel->add( mRedoButton );
    mRedoButton->addActionListener( this );
    mRedoButton->setEnabled( false );



    setMusicToEdit( mainMusicPicker->getSelectedResource() );
    
    postConstruction();
    }



MusicEditor::~MusicEditor() {
    mSidePanel->remove( mainMusicPicker );    
    }



void MusicEditor::setMusicToEdit( Music inMusic ) {
    mMusicToEdit = inMusic;
    
    for( int y=0; y<N; y++ ) {
        for( int x=0; x<N; x++ ) {
            
            // last instrument for part editing
            noteToggles[SI][0][y][x] = mMusicToEdit.getNoteOn( x, y );
            }
        }
    refreshMiniView();

    char *name = mMusicToEdit.getMusicName();
    
    mNameField->setText( name );
    mNameField->setCursorPosition( strlen( name ) );
    
    delete [] name;
    }



void MusicEditor::refreshMiniView() {
    // don't use cached version
    mMiniViewButton->setSprite( mMusicToEdit.getSprite( false, false ) );
    }



void MusicEditor::actionPerformed( GUIComponent *inTarget ) {
    // superclass
    Editor::actionPerformed( inTarget );
    

    if( inTarget == mainMusicPicker ) {
        if( ! mAddAction &&
            ! mainMusicPicker->wasLastActionFromPress() ) {
            // will change music
                        
            mUndoStack.push_back( mMusicToEdit );
            mUndoButton->setEnabled( true );
            
            setMusicToEdit( mainMusicPicker->getSelectedResource() );
            
            // new branch... "redo" future now impossible
            mRedoStack.deleteAll();
            mRedoButton->setEnabled( false );
            }        
        }
    else if( inTarget == mNameField ) {
        mUndoStack.push_back( mMusicToEdit );
        mUndoButton->setEnabled( true );

        // new branch... "redo" future now impossible
        mRedoStack.deleteAll();
        mRedoButton->setEnabled( false );
        
        mMusicToEdit.editMusicName( mNameField->getText() );
        }
    else if( inTarget == mAddButton ) {
        addMusic();
        }
    else if( inTarget == mUndoButton ) {
        int lastIndex = mUndoStack.size() - 1;
        
        Music last = *( mUndoStack.getElement( lastIndex ) );
        mUndoStack.deleteElement( lastIndex );
        if( mUndoStack.size() == 0 ) {
            mUndoButton->setEnabled( false );
            }
        
        mRedoStack.push_back( mMusicToEdit );
        mRedoButton->setEnabled( true );
        
        setMusicToEdit( last );
        }
    else if( inTarget == mRedoButton ) {
        int nextIndex = mRedoStack.size() - 1;
        
        Music next = *( mRedoStack.getElement( nextIndex ) );
        mRedoStack.deleteElement( nextIndex );
        if( mRedoStack.size() == 0 ) {
            mRedoButton->setEnabled( false );
            }
        
        mUndoStack.push_back( mMusicToEdit );
        mUndoButton->setEnabled( true );
        
        setMusicToEdit( next );
        }
    else if( inTarget == mTransformToolSet ) {
        mUndoStack.push_back( mMusicToEdit );
        mUndoButton->setEnabled( true );

        // new branch... "redo" future now impossible
        mRedoStack.deleteAll();
        mRedoButton->setEnabled( false );


        char oldToggles[N][N];
                
        for( int y=0; y<N; y++ ) {
            for( int x=0; x<N; x++ ) {
                oldToggles[y][x] = noteToggles[SI][0][y][x];
                }
            }


        switch( mTransformToolSet->getLastPressed() ) {
            case flipH: {
                for( int y=0; y<N; y++ ) {
                    for( int x=0; x<N; x++ ) {
                        noteToggles[SI][0][y][x] = oldToggles[y][N - x - 1];
                        }
                    }
                }
                break;
            case flipV: {
                for( int y=0; y<N; y++ ) {
                    for( int x=0; x<N; x++ ) {
                        noteToggles[SI][0][y][x] = oldToggles[N - y - 1][x];
                        }
                    }

                }
                break;
            case rotateCCW: {
                for( int y=0; y<N; y++ ) {
                    for( int x=0; x<N; x++ ) {
                        noteToggles[SI][0][y][x] = oldToggles[N - x - 1][y];
                        }
                    }

                }
                break;
            case rotateCW: {
                for( int y=0; y<N; y++ ) {
                    for( int x=0; x<N; x++ ) {
                        noteToggles[SI][0][y][x] = oldToggles[x][N - y - 1];
                        }
                    }

                }
                break;
            case clear: {
                for( int y=0; y<N; y++ ) {
                    for( int x=0; x<N; x++ ) {
                        noteToggles[SI][0][y][x] = false;
                        }
                    }
                }
                break;
            case colorize: {
                // does not apply to music
                }
                break;                
            }
        
        for( int y=0; y<N; y++ ) {
            for( int x=0; x<N; x++ ) {
                mMusicToEdit.editMusic( x, y, noteToggles[SI][0][y][x] );
                }
            }

        refreshMiniView();
        }
    else if( inTarget == mNoteDisplay ) {

        
        if( mNoteDisplay->mLastActionPress ) {
            // note grid has changed, 
            // but our music object hasn't been touched yet, so we can 
            // still save it as an undo point

            mUndoStack.push_back( mMusicToEdit );
            mUndoButton->setEnabled( true );
            
            // new branch... "redo" future now impossible
            mRedoStack.deleteAll();
            mRedoButton->setEnabled( false );
            }
        

        // copy changes  into our object
        for( int y=0; y<N; y++ ) {
            for( int x=0; x<N; x++ ) {
                mMusicToEdit.editMusic( x, y, noteToggles[SI][0][y][x] );
                }
            }

        refreshMiniView();
        }
    
    
    }



void MusicEditor::editorClosing() {
    addMusic();

    // redraws will be off while we are closed, glow gets out of sync
    mNoteDisplay->resetColumnGlow();
    
    // silence our extra part (only plays when editor open
    partLengths[SI] = 0;
    }



void MusicEditor::addMusic() {
    mAddAction = true;
    mMusicToEdit.finishEdit();
    mainMusicPicker->setSelectedResource( mMusicToEdit, true );
    mAddAction = false;
    }

