#include "RoomEditor.h"
#include "RoomPicker.h"
#include "TilePicker.h"
#include "BorderPanel.h"
#include "GridOverlay.h"
#include "labels.h"

#include "minorGems/util/log/AppLog.h"


#include <stdio.h>


extern TilePicker *mainTilePicker;
extern RoomPicker *mainRoomPicker;


extern int gameWidth, gameHeight;


extern TextGL *largeTextFixed;





template <>
void SizeLimitedVector<Room>::deleteElementOfType(
    Room inElement ) {
    // no delete necessary
    }




RoomEditor::RoomEditor( ScreenGL *inScreen )
        : Editor( inScreen ),
          mStateDisplay( NULL ),
          mUndoStack( MAX_UNDOS, false ),
          // gen a fake ID
          mCurrentWorkingStateID( makeUniqueID( (unsigned char*)"currRoom", 
                                                strlen( "currRoom" ) ) ) {
    
    AppLog::info( "Constructing room editor\n" );

    mCloseButton->setToolTip( "tip_closeEdit_room" );
    
    /*
    LabelGL *titleLabel = new LabelGL( 0.75, 0.5, 0.25,
                                       0.1, "Room Editor", largeText );
    

    
    mSidePanel->add( titleLabel );
    */

    mSidePanel->add( mainTilePicker );

    mainTilePicker->addActionListener( this );
    
    
    mSidePanel->add( mainRoomPicker );

    mainRoomPicker->addActionListener( this );
    

    mEditTileButton = 
        new EditButtonGL( 
            mainTilePicker->getAnchorX() - 1,
            mainTilePicker->getAnchorY() + mainTilePicker->getHeight() + 1,
            8,
            8 );
    
    mSidePanel->add( mEditTileButton );
    
    mEditTileButton->addActionListener( this );
    
    mEditTileButton->setToolTip( "tip_edit_tile" );
    
    /*
    // 1-pixel wide white border
    // inner panel to provide a 1-pixel wide black gap
    
    BorderPanel *workingColorBorderPanel = 
        new BorderPanel( 256, 206, 48, 14,
                         new Color( 0, 0, 0, 1 ),
                         new Color( 1, 1, 1, 1 ),
                         1 );
    

    // smaller to leave black gap
    GUIPanelGL *workingColorPanel = new GUIPanelGL( 258, 208, 44, 10,
                                                    mWorkingColor );
    
    mSidePanel->add( workingColorBorderPanel );
    workingColorBorderPanel->add( workingColorPanel );
    

    mAddButton = new AddButtonGL( 272, 184, 16, 16 );
    mSidePanel->add( mAddButton );
    mAddButton->addActionListener( this );

    */


    double offset = P;

    double buttonSize = P;
    
    
    for( int y=0; y<G; y++ ) {
        for( int x=0; x<G; x++ ) {

            mButtonGrid[y][x] = new SpriteCellButtonGL(
                NULL, 1.0,
                8 + x * buttonSize,
                gameWidth - ( 48 + offset + y * buttonSize ),
                buttonSize,
                buttonSize );
            

            mMainPanel->add( mButtonGrid[y][x] );
            
            mButtonGrid[y][x]->addActionListener( this );
            }
        }    

    
    

    // no stamp tool here
    mToolSet = new DrawToolSet( 2, 42, "tip_pickTile", false );
    mMainPanel->add( mToolSet );    


    
    
    

    EightPixelLabel *fieldLabel = new EightPixelLabel( 150, 54,
                                                       "roomName" );
    mMainPanel->add( fieldLabel );



    int fieldHeight = 8;
    int fieldWidth = 8 * 10;
    
    const char *defaultText = "default";

    mSetNameField = new TextFieldGL( 150,
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
    mMainPanel->add( mSetNameField );

    mSetNameField->setFocus( true );
    //mSetNameField->lockFocus( true );
    
    mSetNameField->setCursorPosition( strlen( defaultText ) );
    
    mSetNameField->addActionListener( this );

    




    // center add button
    double gridEdge = 8 + G * buttonSize;
    
    double extra = gameHeight - gridEdge;
    

    // center it vertically on room picker
    double addY = mainRoomPicker->getAnchorY() +
        mainRoomPicker->getHeight() - 15;
    
    double sideButtonsX = gridEdge + (extra - 16) / 2;

    mAddButton = new AddButtonGL( sideButtonsX, 
                                  addY, 
                                  16, 16 );
    mMainPanel->add( mAddButton );
    mAddButton->addActionListener( this );
    mAddButton->setToolTip( "tip_addRoom" );
    
    mAddAction = false;
    

    double miniButtonSize = P + 4;
    
    mMiniViewButton = new SpriteButtonGL( 
        NULL, 1,
        gridEdge + ( extra - miniButtonSize ) / 2,
        gameWidth / 2,
        miniButtonSize,
        miniButtonSize );
    
    mMainPanel->add( mMiniViewButton );
    


    mWallsButton = new SelectableButtonGL( 
        new Sprite( "walls.tga", true ),
        1,
        sideButtonsX - 2, mMiniViewButton->getAnchorY() + 30, 
        20, 20 );
    
    mMainPanel->add( mWallsButton );
    
    mWallsButton->setSelected( false );
    
    mWallsButton->addActionListener( this );
    mWallsButton->setToolTip( "tip_walls" );



    mHintsButton = new SelectableButtonGL( 
        new Sprite( "hints.tga", true ),
        1,
        sideButtonsX - 2, mMiniViewButton->getAnchorY() + 60, 
        20, 20 );
    
    mMainPanel->add( mHintsButton );
    
    mHintsButton->setSelected( false );
    
    mHintsButton->addActionListener( this );
    mHintsButton->setToolTip( "tip_hints" );


    
    double undoButtonY = gameWidth - ( 48 + offset + (G - 1) * buttonSize );

    mUndoButton = new UndoButtonGL( sideButtonsX, undoButtonY, 16, 16 );
    mMainPanel->add( mUndoButton );
    mUndoButton->addActionListener( this );
    mUndoButton->setEnabled( false );

    mRedoButton = new RedoButtonGL( sideButtonsX, undoButtonY + 19, 16, 16 );
    mMainPanel->add( mRedoButton );
    mRedoButton->addActionListener( this );
    mRedoButton->setEnabled( false );


    mClearButton = new ClearButtonGL( sideButtonsX, addY - 20, 16, 16 );
    mMainPanel->add( mClearButton );
    mClearButton->addActionListener( this );




    setRoomToEdit( mainRoomPicker->getSelectedResource() );
    

    mPenDown = false;
    

    GridOverlay *overlay = new GridOverlay( 
        8, 
        gameWidth - ( 48 + offset + (G - 1) * buttonSize ),
        G * buttonSize, G * buttonSize,
        G );
    
    mMainPanel->add( overlay );
    

    // do this later, after state display added
    //postConstruction();
    }



RoomEditor::~RoomEditor() {
    mSidePanel->remove( mainTilePicker );
    mSidePanel->remove( mainRoomPicker );
    
    if( mStateDisplay != NULL ) {
        mMainPanel->remove( mStateDisplay );
        }
    }



void RoomEditor::setGameStateDisplay( GameStateDisplay *inDisplay ) {
    mStateDisplay = inDisplay;
    
    mMainPanel->add( mStateDisplay );
    
    // now ready to add top-layer GUI components
    postConstruction();
    }



void RoomEditor::takeOverGameStateDisplay() {
    char showHints = mHintsButton->getSelected();
        
    if( mStateDisplay != NULL ) {
            
        mStateDisplay->setEnabled( showHints );
        }  
    }



void RoomEditor::setRoomToEdit( Room inRoom ) {

    // clear old working usages
    removeUsages( mCurrentWorkingStateID );

    mRoomToEdit = inRoom;
    
    for( int y=0; y<G; y++ ) {
        for( int x=0; x<G; x++ ) {
            uniqueID tileID = mRoomToEdit.getTile( x, y );
            Tile t( tileID );
            
            mButtonGrid[y][x]->setSprite( t.getSprite() );
            char *tileName = t.getName();
            mButtonGrid[y][x]->setToolTip( tileName, false );
            delete [] tileName;
            
            if( mWallsButton->getSelected() ) {
                mButtonGrid[y][x]->setHighlight( mRoomToEdit.getWall( x, y ) );
                }

            // add working usages
            addUsage( mCurrentWorkingStateID, tileID );
            }
        }
    mainTilePicker->recheckDeletable();
    
    refreshMiniView();

    char *name = mRoomToEdit.getRoomName();
    
    mSetNameField->setText( name );
    mSetNameField->setCursorPosition( strlen( name ) );
    
    delete [] name;
    }



void RoomEditor::refreshMiniView() {
    // don't use cached version
    mMiniViewButton->setSprite( mRoomToEdit.getSprite( false, false ) );
    }



char RoomEditor::recursiveFill( int inX, int inY, 
                                uniqueID inOldTile, uniqueID inNewTile,
                                drawTool inTool ) {

    if( equal( mRoomToEdit.getTile( inX, inY ), inOldTile )
        &&
        !equal( mRoomToEdit.getTile( inX, inY ), inNewTile ) ) {

        Tile t( inNewTile );
        

        mButtonGrid[inY][inX]->setSprite( t.getSprite() );

        char *tileName = t.getName();
        mButtonGrid[inY][inX]->setToolTip( tileName, false );
        delete [] tileName;
        
        mRoomToEdit.editRoomTile( inX, inY, inNewTile );
        
        // call on neighbors
        if( inTool == fill || inTool == horLine ) {    
            if( inX > 0 ) {
                recursiveFill( inX - 1, inY, inOldTile, inNewTile, inTool );
                }
            if( inX < G - 1 ) {
                recursiveFill( inX + 1, inY, inOldTile, inNewTile, inTool );
                }
            }

        if( inTool == fill || inTool == verLine ) {
            if( inY > 0 ) {
                recursiveFill( inX, inY - 1, inOldTile, inNewTile, inTool );
                }
            if( inY < G - 1 ) {
                recursiveFill( inX, inY + 1, inOldTile, inNewTile, inTool );
                }
            }

        return true;
        }

    return false;
    }




char RoomEditor::recursiveFill( int inX, int inY, 
                                char inOldWall, char inNewWall,
                                drawTool inTool ) {

    if( mRoomToEdit.getWall( inX, inY ) == inOldWall
        &&
        mRoomToEdit.getWall( inX, inY ) != inNewWall ) {

        mButtonGrid[inY][inX]->setHighlight( inNewWall );
        mRoomToEdit.editRoomWall( inX, inY, inNewWall );
        
        // call on neighbors
        if( inTool == fill || inTool == horLine ) {    
            if( inX > 0 ) {
                recursiveFill( inX - 1, inY, inOldWall, inNewWall, inTool );
                }
            if( inX < G - 1 ) {
                recursiveFill( inX + 1, inY, inOldWall, inNewWall, inTool );
                }
            }

        if( inTool == fill || inTool == verLine ) {
            if( inY > 0 ) {
                recursiveFill( inX, inY - 1, inOldWall, inNewWall, inTool );
                }
            if( inY < G - 1 ) {
                recursiveFill( inX, inY + 1, inOldWall, inNewWall, inTool );
                }
            }

        return true;
        }

    return false;
    }




void RoomEditor::actionPerformed( GUIComponent *inTarget ) {
    // superclass
    Editor::actionPerformed( inTarget );
    

    if( inTarget == mainTilePicker ) {
        // new tile picked
        }
    else if( inTarget == mainRoomPicker ) {
        // ignore if caused by our own Add action
        if( ! mAddAction &&
            ! mainRoomPicker->wasLastActionFromPress() ) {
            
            // will change room
                        
            mUndoStack.push_back( mRoomToEdit );
            mUndoButton->setEnabled( true );
            
            setRoomToEdit( mainRoomPicker->getSelectedResource() );
            
            mainTilePicker->recheckDeletable();


            // new branch... "redo" future now impossible
            mRedoStack.deleteAll();
            mRedoButton->setEnabled( false );
            }
        }
    else if( inTarget == mSetNameField ) {        
        mUndoStack.push_back( mRoomToEdit );
        mUndoButton->setEnabled( true );

        // new branch... "redo" future now impossible
        mRedoStack.deleteAll();
        mRedoButton->setEnabled( false );
        
        mRoomToEdit.editRoomName( mSetNameField->getText() );
        }
    else if( inTarget == mAddButton ) {
        addRoom();
        }
    else if( inTarget == mEditTileButton ) {
        showTileEditor();
        }
    else if( inTarget == mUndoButton ) {
        int lastIndex = mUndoStack.size() - 1;
        
        Room last = *( mUndoStack.getElement( lastIndex ) );
        mUndoStack.deleteElement( lastIndex );
        if( mUndoStack.size() == 0 ) {
            mUndoButton->setEnabled( false );
            }
        
        mRedoStack.push_back( mRoomToEdit );
        mRedoButton->setEnabled( true );
        
        setRoomToEdit( last );
        }
    else if( inTarget == mRedoButton ) {
        int nextIndex = mRedoStack.size() - 1;
        
        Room next = *( mRedoStack.getElement( nextIndex ) );
        mRedoStack.deleteElement( nextIndex );
        if( mRedoStack.size() == 0 ) {
            mRedoButton->setEnabled( false );
            }
        
        mUndoStack.push_back( mRoomToEdit );
        mUndoButton->setEnabled( true );
        
        setRoomToEdit( next );
        }
    else if( inTarget == mWallsButton ) {
        // toggle
        mWallsButton->setSelected( ! mWallsButton->getSelected() );
        
        char showWalls = mWallsButton->getSelected();

        for( int y=0; y<G; y++ ) {
            for( int x=0; x<G; x++ ) {
                mButtonGrid[y][x]->setHighlight(
                    mRoomToEdit.getWall( x, y ) && showWalls );
                }
            }
        
        
        }
    else if( inTarget == mHintsButton ) {
        // toggle
        mHintsButton->setSelected( ! mHintsButton->getSelected() );
        
        char showHints = mHintsButton->getSelected();
        
        if( mStateDisplay != NULL ) {
            
            mStateDisplay->setEnabled( showHints );
            }        
        }
    else if( inTarget == mClearButton ) {
        mUndoStack.push_back( mRoomToEdit );
        mUndoButton->setEnabled( true );

        // new branch... "redo" future now impossible
        mRedoStack.deleteAll();
        mRedoButton->setEnabled( false );


        // stick default everywhere
        Tile t = Tile::getDefaultResource();
        
        uniqueID id = t.getUniqueID();


        for( int y=0; y<G; y++ ) {
            for( int x=0; x<G; x++ ) {
                
                // all walls off
                mRoomToEdit.editRoomWall( x, y, false );
                // okay even if wall visibility turned off
                mButtonGrid[y][x]->setHighlight( false );

                mButtonGrid[y][x]->setSprite(  t.getSprite() );
                
                char *tileName = t.getName();
                mButtonGrid[y][x]->setToolTip( tileName, false );
                delete [] tileName;
            
                mRoomToEdit.editRoomTile( x, y, id );
                }
            }
        refreshMiniView();
        }
    else {
        // check grid 
        
        char found = false;
        
        for( int y=0; y<G && !found; y++ ) {
            for( int x=0; x<G && !found; x++ ) {
                if( inTarget == mButtonGrid[y][x] ) {
                    found = true;
                    
                    Room oldRoomState = mRoomToEdit;
                    char changed = false;
                    
                    char release = false;

                    switch( mToolSet->getSelected() ) {
                        case pen: {
                            if( mWallsButton->getSelected() ) {
                                // edit walls with pen
                                
                                if( !mPenDown ) {
                                    // set ink until released
                                    mWallInk = ! mRoomToEdit.getWall( x, y );
                                    
                                    mPenDown = true;
                                    changed = true;
                                    }
                                
                                // use ink already set
                                mRoomToEdit.editRoomWall( x, y, mWallInk );
                                
                                mButtonGrid[y][x]->setHighlight( 
                                    mWallInk );
                                }
                            else {
                                // place tiles
                                Tile t = 
                                    mainTilePicker->getSelectedResource();
                                
                                uniqueID id = t.getUniqueID();
                                

                                if( ! equal( id, 
                                             mRoomToEdit.getTile( x, y ) ) ) {
                                    mButtonGrid[y][x]->setSprite( 
                                        t.getSprite() );

                                    char *tileName = t.getName();
                                    mButtonGrid[y][x]->setToolTip( tileName, 
                                                                   false );
                                    delete [] tileName;
            

                                    mRoomToEdit.editRoomTile( x, y, id );
                                    refreshMiniView();
                                    
                                    // don't count single pen dots as undoable
                                    // until pen is released
                                    // save undo state only on on initial 
                                    // presses
                                    if( !mPenDown ) {
                                        mPenDown = true;
                                        changed = true;
                                        } 
                                    }
                                }
                            
                            if( !mButtonGrid[y][x]->isPressed() ) {
                                // a release
                                mPenDown = false;
                                release = true;
                                }
                            }
                            break;
                        case horLine:
                        case verLine:
                        case fill:
                            if( ! mWallsButton->getSelected() ) {
                                changed = recursiveFill( 
                                    x, y, 
                                    mRoomToEdit.getTile( x, y ),
                                    mainTilePicker->getSelectedResourceID(),
                                    mToolSet->getSelected() );
                                refreshMiniView();
                                }
                            else {
                                
                                if( !mPenDown ) {
                                    mWallInk = ! mRoomToEdit.getWall( x, y );
                                    }
                                
                                changed = recursiveFill( 
                                    x, y, 
                                    mRoomToEdit.getWall( x, y ),
                                    mWallInk,
                                    mToolSet->getSelected() );

                                refreshMiniView();

                                
                                }
                            if( !mPenDown ) {
                                mPenDown = true;
                                } 
                            if( !mButtonGrid[y][x]->isPressed() ) {
                                // a release
                                mPenDown = false;
                                release = true;
                                }
                            break;
                        case pickColor: {
                            Tile t( mRoomToEdit.getTile( x, y ) );
                            
                            mainTilePicker->setSelectedResource( t );
                            }
                            break;
                        case stamp:
                            AppLog::error( 
                                "Error: unsupported stamp tool used in"
                                "RoomEditor\n" );
                            break;
                        }
                    

                    
                    if( changed ) {
                        
                        // initial press, or step of fill
                        // we're certainly using the selected tile
                        addUsage( mCurrentWorkingStateID,
                                  mainTilePicker->getSelectedResourceID() );
                        mainTilePicker->recheckDeletable();
                        

                        mUndoStack.push_back( oldRoomState );
                        mUndoButton->setEnabled( true );

                        // new branch... "redo" future now impossible
                        mRedoStack.deleteAll();
                        mRedoButton->setEnabled( false );
                        }

                    if( release ) {
                        // will this be too slow?  How else to track all
                        // usages in a consistent way?

                        // only doing this on release, so maybe okay?
                        
                        removeUsages( mCurrentWorkingStateID );

                        for( int ty=0; ty<G; ty++ ) {
                            for( int tx=0; tx<G; tx++ ) {
                        
                                addUsage( mCurrentWorkingStateID,
                                          mRoomToEdit.getTile( tx, ty ) );
                                
                                }
                            }
                        mainTilePicker->recheckDeletable();
                        }
                    
                            
                    
                    
                    }
                }
            }
        }
    
    
    }




void RoomEditor::editorClosing() {
    mStateDisplay->setHintMode( false );
    mStateDisplay->setEnabled( true );

    addRoom();
    }



void RoomEditor::addRoom() {
    mAddAction = true;
    mRoomToEdit.finishEdit();
    mainRoomPicker->setSelectedResource( mRoomToEdit, true );
    
    mainTilePicker->recheckDeletable();
    
    mAddAction = false;
    }


