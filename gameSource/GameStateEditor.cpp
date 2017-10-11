#include "GameStateEditor.h"
#include "RoomPicker.h"
#include "RoomEditor.h"
#include "ScenePicker.h"
#include "TilePicker.h"
#include "StateObjectPicker.h"
#include "StateObjectEditor.h"
#include "DragAndDropManager.h"
#include "BorderPanel.h"
#include "labels.h"
#include "SpriteResource.h"
#include "packSaver.h"
#include "ToolTipManager.h"


#include "minorGems/util/TranslationManager.h"

#include "minorGems/util/log/AppLog.h"


#include <stdio.h>


extern StateObjectPicker *mainStateObjectPicker;
extern RoomPicker *mainRoomPicker;
extern RoomEditor *mainRoomEditor;
extern ScenePicker *mainScenePicker;
extern TilePicker *mainTilePicker;

extern StateObjectEditor *mainStateObjectEditor;

extern DragAndDropManager *mainDragAndDrop;


extern int gameWidth, gameHeight;


extern TextGL *largeTextFixed;


template <>
void SizeLimitedVector<GameState*>::deleteElementOfType(
    GameState *inElement ) {
    // special delete needed, because vector stores pointers
    delete inElement;
    }



GameStateEditor::GameStateEditor( ScreenGL *inScreen )
        : Editor( inScreen ),
          mEditingSelectedObject( false ),
          mUndoStack( MAX_UNDOS, true ), // stack contains pointers
          mIgnoreObjPickerEvents( false ),
          mObjectChanging( false ),
          // gen a fake ID
          mCurrentWorkingStateID( makeUniqueID( (unsigned char*)"currState", 
                                                strlen( "currState" ) ) )  {
    
    AppLog::info( "Constructing game state editor\n" );
    
    /*
    LabelGL *titleLabel = new LabelGL( 0.75, 0.5, 0.25,
                                       0.1, "Room Editor", largeText );
    

    
    mSidePanel->add( titleLabel );
    */

    // hide the close button for this editor, since it's the base editor
    mCloseButton->setEnabled( false );

    mSidePanel->add( mainStateObjectPicker );

    mainStateObjectPicker->addActionListener( this );
    
    
    mSidePanel->add( mainScenePicker );

    mainScenePicker->addActionListener( this );


    // listen for room changes too
    mainRoomPicker->addActionListener( this );
    


    mStateDisplay = new GameStateDisplay( 8, gameWidth - 48 - G * P,
                                          true );
    mMainPanel->add( mStateDisplay );
    mStateDisplay->addActionListener( this );
    
    // turn on active bubble coloring
    mStateDisplay->mHighlightEditedBubble = true;

    mStateDisplay->setEditor( this );
    

    mainRoomEditor->setGameStateDisplay( mStateDisplay );
    


    mEditStateObjectButton = 
        new EditButtonGL( 
            mainStateObjectPicker->getAnchorX() - 1,
            mainStateObjectPicker->getAnchorY() + 
            mainStateObjectPicker->getHeight() + 1,
            8,
            8 );
    
    mSidePanel->add( mEditStateObjectButton );
    
    mEditStateObjectButton->addActionListener( this );
    mEditStateObjectButton->setToolTip( "tip_edit_object" );



    double offset = P;

    double buttonSize = P;
    
    


    mToolSet = new StateToolSet( 26, 42 );
    mMainPanel->add( mToolSet );    
    mToolSet->addActionListener( this );
    


    mSpeechDeleteButton = new QuickDeleteButtonGL( 
        mToolSet->getAnchorX() + mToolSet->getWidth(),
        mToolSet->getAnchorY() + mToolSet->getHeight() - 8,
        8,
        8 );
    mMainPanel->add( mSpeechDeleteButton );
    mSpeechDeleteButton->addActionListener( this );

    mSpeechDeleteButton->setEnabled( false );
    
    mSpeechDeleteButton->setToolTip( "tip_delete_speech" );



    mSpeechBubbleBoxButton = new ToggleSpriteButtonGL( 
        new Sprite( "speechBox.tga", true ),
        new Sprite( "speechBubble.tga", true ),
        1,
        mToolSet->getAnchorX() + mToolSet->getWidth(),
        mToolSet->getAnchorY(),
        8,
        8 );
    mMainPanel->add( mSpeechBubbleBoxButton );
    mSpeechBubbleBoxButton->addActionListener( this );

    mSpeechBubbleBoxButton->setEnabled( false );
    
    mSpeechBubbleBoxButton->setToolTip( "tip_toggle_box" );
    mSpeechBubbleBoxButton->setSecondToolTip( "tip_toggle_bubble" );




    mFlipSpeechButton = 
        new FlipHButtonGL( mSpeechDeleteButton->getAnchorX()
                           + mSpeechDeleteButton->getWidth() + 1, 
                           44,
                           16, 16 );
    mMainPanel->add( mFlipSpeechButton );
    mFlipSpeechButton->addActionListener( this );
    
    mFlipSpeechButton->setToolTip( "tip_flip_speech" );
    mFlipSpeechButton->setEnabled( false );



    



    


    





    EightPixelLabel *fieldLabel = new EightPixelLabel( 150, 54,
                                                       "sceneName" );
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

    mNameField->setFocus( false );
    //mNameField->lockFocus( true );
    
    mNameField->setCursorPosition( strlen( defaultText ) );
    
    // not undoable, ignore actions (so that we can hold
    // focus on GameStateDisplay whenever we can)
    //mNameField->addActionListener( this );




    

    // center side buttons
    double gridEdge = 8 + G * buttonSize;
    
    double extra = gameHeight - gridEdge;
    

    
    double sideButtonsX = gridEdge + (extra - 16) / 2;
    



    
    double undoButtonY = gameWidth - ( 48 + offset + (G - 1) * buttonSize );

    mUndoButton = new UndoButtonGL( sideButtonsX, undoButtonY, 16, 16 );
    mMainPanel->add( mUndoButton );
    mUndoButton->addActionListener( this );
    mUndoButton->setEnabled( false );

    mRedoButton = new RedoButtonGL( sideButtonsX, undoButtonY + 19, 16, 16 );
    mMainPanel->add( mRedoButton );
    mRedoButton->addActionListener( this );
    mRedoButton->setEnabled( false );


    mUndoOrRedoOrClearAction = false;
    mIgnoreSliders = false;


    
    






    double setY = mainStateObjectPicker->getAnchorY() +
        mainStateObjectPicker->getHeight() - 15;
    
    mSetObjectButton = new SpriteButtonGL( 
        new Sprite( "setObject.tga", true ),
        1,
        sideButtonsX, setY, 16, 16 );
    
    mMainPanel->add( mSetObjectButton );
    
    mSetObjectButton->addActionListener( this );
    
    mSetObjectButton->setToolTip( "tip_objSet" );



    mAddObjectButton = new KeyEquivButtonGL( 
        new Sprite( "addObject.tga", true ),
        1,
        sideButtonsX, setY - 20, 16, 16, 'a', 'A' );
    
    mMainPanel->add( mAddObjectButton );
    
    mAddObjectButton->addActionListener( this );
    
    mAddObjectButton->setToolTip( "tip_objAdd" );



    mEditObjectButton = new KeyEquivButtonGL( 
        new Sprite( "editObject.tga", true ),
        1,
        sideButtonsX, setY - 40, 16, 16, 'e', 'E' );
    
    mMainPanel->add( mEditObjectButton );
    
    mEditObjectButton->addActionListener( this );
    
    mEditObjectButton->setToolTip( "tip_objEdit" );

    
    mRemoveObjectButton = new KeyEquivButtonGL( 
        new Sprite( "removeObject.tga", true ),
        1,
        sideButtonsX, setY - 60, 16, 16, 'x', 'X' );
    
    mMainPanel->add( mRemoveObjectButton );
    
    mRemoveObjectButton->addActionListener( this );
    
    mRemoveObjectButton->setToolTip( "tip_objRemove" );

    // obj 0 starts out selected, can't be deleted
    mRemoveObjectButton->setEnabled( false );
    
    
    
    // freeze only works on obj 0, which can't be removed
    // so re-use the space
    // actually, reusing space causes cross-events on click--- UGH
    // Also probably BAD UI design...
    mFreezeButton = new SelectableButtonGL( 
        new Sprite( "freeze.tga", true ),
        1,
        104, 42, 20, 20 );
    
    mMainPanel->add( mFreezeButton );
    
    mFreezeButton->setSelected( false );
    
    mFreezeButton->addActionListener( this );
    
    mFreezeButton->setToolTip( "tip_freezePlayer" );


    
    mLockSelectedButton = new SelectableButtonGL( 
        new Sprite( "lockSelected.tga", true ),
        1,
        104, 42, 20, 20 );
    
    mMainPanel->add( mLockSelectedButton );
    
    mLockSelectedButton->setSelected( false );
    
    mLockSelectedButton->addActionListener( this );

    mLockSelectedButton->setEnabled( false );

    mLockSelectedButton->setToolTip( "tip_lockSelected" );



    mLockGlobalButton = new SelectableButtonGL( 
        new Sprite( "lock.tga", true ),
        1,
        126, 42, 20, 20 );
    
    mMainPanel->add( mLockGlobalButton );
    
    mLockGlobalButton->setSelected( true );
    
    mLockGlobalButton->addActionListener( this );

    mLockGlobalButton->setEnabled( true );

    mLockGlobalButton->setToolTip( "tip_lockGlobal" );
    


    mHoldObjectButton = new SelectableButtonGL( 
        new Sprite( "hold.tga", true ), 
        1,
        mLockSelectedButton->getAnchorX() - 13,
        mLockSelectedButton->getAnchorY() + 2,
        12, 12 );
    

    mHoldObjectButton->setSelected( false );

    mHoldObjectButton->setToolTip( "tip_holdObject" );

    mMainPanel->add( mHoldObjectButton );
    
    mHoldObjectButton->addActionListener( this );



    Color *thumbColor = new Color( .5, .5, .5, .5 );
    Color *borderColor = new Color( .35, .35, .35, .35 );
    
    
    mObjectTransSlider = new ToolTipSliderGL( sideButtonsX - 1, setY - 80 + 9,
                                              18, 10,
                                              NULL, 0,
                                              new Color( 0, 0, 0, 1 ),
                                              new Color( 1, 1, 1, 1 ),
                                              thumbColor->copy(),
                                              borderColor->copy(),
                                              1, 4, 1 );
    
    mMainPanel->add( mObjectTransSlider );
    mObjectTransSlider->setThumbPosition( 1.0 );
    mObjectTransSlider->addActionListener( this );
    mObjectTransSlider->setToolTip( "tip_object_fade" );


    

    mGridButton = new SelectableButtonGL( 
        new Sprite( "grid.tga", true ),
        1,
        sideButtonsX - 2, 158, 20, 20 );
    
    mMainPanel->add( mGridButton );
    
    mGridButton->setSelected( true );
    
    mGridButton->addActionListener( this );
    
    mGridButton->setToolTip( "tip_grid" );



    mRoomTransSlider = new ToolTipSliderGL( sideButtonsX - 1, setY - 120 + 13,
                                            18, 10,
                                            NULL, 0,
                                            new Color( 0, 0, 0, 1 ),
                                            new Color( 1, 1, 1, 1 ),
                                            thumbColor->copy(),
                                            borderColor->copy(),
                                            1, 4, 1 );
    
    mMainPanel->add( mRoomTransSlider );
    mRoomTransSlider->setThumbPosition( 1.0 );
    mRoomTransSlider->addActionListener( this );
    mRoomTransSlider->setToolTip( "tip_room_fade" );










    // center it vertically on scene picker
    double addSceneY = mainScenePicker->getAnchorY() +
        mainScenePicker->getHeight() - 15;
    

    mAddSceneButton = new AddButtonGL( sideButtonsX, 
                                       addSceneY, 
                                       16, 16 );
    mMainPanel->add( mAddSceneButton );
    mAddSceneButton->addActionListener( this );
    mAddSceneButton->setToolTip( "tip_addScene" );
    
    mAddSceneAction = false;

    double clearY = addSceneY - 20;
    
    mClearButton = new ClearButtonGL( sideButtonsX, clearY, 16, 16 );
    mMainPanel->add( mClearButton );
    mClearButton->addActionListener( this );



    mSendButton = new SendButtonGL(
        4,
        // below undo button
        44,
        16, 16 );
    
    mMainPanel->add( mSendButton );
    
    mSendButton->addActionListener( this );
    
    mSendButton->setToolTip( "tip_send" );



    mPracticeButton = new SpriteButtonGL(
        new Sprite( "practice.tga", true ),
        1,
        4,
        // below undo button
        44,
        16, 16 );
    
    mMainPanel->add( mPracticeButton );
    
    mPracticeButton->addActionListener( this );
    
    mPracticeButton->setToolTip( "tip_practice" );
    mPracticeButton->setEnabled( false );
    

    // music button in side panel
    double musicButtonY =
        mainScenePicker->getAnchorY() + mainScenePicker->getHeight() +
        (int)( mainStateObjectPicker->getAnchorY() -
          ( mainScenePicker->getAnchorY() + mainScenePicker->getHeight() )
          - 16 ) / 2;
    
    
    mEditMusicButton = new SpriteButtonGL( 
        new Sprite( "music.tga", true ),
        1,
        mSidePanel->getAnchorX() + ( mSidePanel->getWidth() - 16 ) / 2, 
        musicButtonY, 16, 16 );
    
    mSidePanel->add( mEditMusicButton );
    
    mEditMusicButton->addActionListener( this );
    
    mEditMusicButton->setToolTip( "tip_edit_music" );

    


    mEditRoomButton = new SpriteButtonGL( 
        new Sprite( "editRoom.tga", true ),
        1,
        mainScenePicker->getAnchorX() - 1, musicButtonY, 16, 16 );
    
    mSidePanel->add( mEditRoomButton );
    
    mEditRoomButton->addActionListener( this );
    
    mEditRoomButton->setToolTip( "tip_edit_room" );





    
    delete thumbColor;
    delete borderColor;



    
    mEditStateObjectButton = 
        new EditButtonGL( 
            mainStateObjectPicker->getAnchorX() - 1,
            mainStateObjectPicker->getAnchorY() + 
            mainStateObjectPicker->getHeight() + 1,
            8,
            8 );
    
    mSidePanel->add( mEditStateObjectButton );
    
    mEditStateObjectButton->addActionListener( this );
    mEditStateObjectButton->setToolTip( "tip_edit_object" );

    

    mAddToPackButton = new ToggleSpriteButtonGL( 
        new Sprite( "pack.tga", true ),
        new Sprite( "packAlreadyIn.tga", true ),
        1,
        mainScenePicker->getAnchorX() + mainScenePicker->getWidth() - 22,
        mainScenePicker->getAnchorY() + 
        mainScenePicker->getHeight() + 1,
        8,
        8 );
    
    mSidePanel->add( mAddToPackButton );
    
    mAddToPackButton->addActionListener( this );
    
    mAddToPackButton->setToolTip( "tip_addSceneToPack" );
    mAddToPackButton->setSecondToolTip( "tip_sceneAlreadyInPack" );


    mSavePackButton = new SpriteButtonGL( 
        new Sprite( "packSave.tga", true ),
        1,
        mainScenePicker->getAnchorX() + mainScenePicker->getWidth() - 7,
        mainScenePicker->getAnchorY() + 
        mainScenePicker->getHeight() + 1,
        8,
        8 );
    
    mSidePanel->add( mSavePackButton );
    
    mSavePackButton->addActionListener( this );
    
    mSavePackButton->setToolTip( "tip_savePack" );





    mGameStateToEdit = NULL;
    
    GameState *state = new GameState();
    
    setGameStateToEdit( state );



    mNoDropImage = readTGA( "noDrop.tga" );
    mCanDropImage = readTGA( "canDrop.tga" );



    // listen for clicks on main panel so we can re-focus for speech input
    mMainPanel->addActionListener( this );
    

    // dnd on top of side panel
    postConstructionSide();
    
    mMainPanel->add( mainDragAndDrop );
    
    // but *below* special main panel widgets
    postConstructionMain();
    }



GameStateEditor::~GameStateEditor() {
    mSidePanel->remove( mainStateObjectPicker );
    mSidePanel->remove( mainScenePicker );

    mMainPanel->remove( mainDragAndDrop );
    
    if( mGameStateToEdit != NULL ) {
        delete mGameStateToEdit;
        }
    
    int numInStack = mUndoStack.size();
    int i;
    for( i=0; i<numInStack; i++ ) {
        delete *( mUndoStack.getElement( i ) );
        }

    clearRedoStack();

    delete mNoDropImage;
    delete mCanDropImage;
    }



void GameStateEditor::setGameStateToEdit( GameState *inGameState,
                                          char inUpdatePickers ) {

    AppLog::trace( 
        "Setting new game state, generating new display sprites\n" );
    
    if( mGameStateToEdit != NULL ) {
        delete mGameStateToEdit;
        }

    // clear old working usages
    removeUsages( mCurrentWorkingStateID );
    


    mGameStateToEdit = inGameState;


    if( inUpdatePickers ) {
        
        int selectedObj = mGameStateToEdit->getSelectedObject();

        
        StateObjectInstance *inst =
            *( mGameStateToEdit-> 
               mObjects.getElement( selectedObj ) );
                        
        
        mainStateObjectPicker->setSelectedResource(
            inst->mObject );


    
        mainRoomPicker->setSelectedResource( mGameStateToEdit->mRoom );
        }
    
    

    mStateDisplay->setState( mGameStateToEdit->copy() );


    
    // make sure speech toggles match current state (in case of undo/redo)
    mStateDisplay->mEditingSpeech =
        (  mToolSet->getSelected() == speak );
    
    // always keep focus on display... 
    //mStateDisplay->setFocus( mStateDisplay->mEditingSpeech );
    
    // show delete button whenever non-empty speech object selected,
    // even if not currently in Speech mode
    if( mGameStateToEdit->getSelectedSpeechLength() > 0 ) {
        mSpeechDeleteButton->setEnabled( true );
        }
    else {
        mSpeechDeleteButton->setEnabled( false );
        }


    if( mGameStateToEdit->getSelectedObject() > 0 ) {
        mRemoveObjectButton->setEnabled( true );
        }
    else {
        // never delete object 0 (player object)
        mRemoveObjectButton->setEnabled( false );
        }


    mFreezeButton->setSelected( mGameStateToEdit->isObjectZeroFrozen() );

    mFreezeButton->setEnabled( mGameStateToEdit->getSelectedObject() == 0 );

    int selectedObjectIndex = mGameStateToEdit->getSelectedObject();

    mLockSelectedButton->setEnabled( selectedObjectIndex != 0 );

    mLockSelectedButton->setSelected( 
        mGameStateToEdit->getSelectedLocked() );

    mLockGlobalButton->setSelected(
        mGameStateToEdit->mLocksOn );

    mHoldObjectButton->setSelected( 
        mGameStateToEdit->getObjectHeld( selectedObjectIndex ) );
    

    checkFlipAndDeleteSpeechButtonEnabled();

    if( mGameStateToEdit->getSelectedSpeechLength() > 0 ) {
        mSpeechDeleteButton->setEnabled( true );
        }
    else {
        mSpeechDeleteButton->setEnabled( false );
        }


    mIgnoreSliders = true;
    
    // only 36 pixels in slider... avoid round-off errors
    mRoomTransSlider->setThumbPosition( 
        (int)( mGameStateToEdit->mRoomTrans / 255.0 * 36 ) / 36.0 );
    
    mObjectTransSlider->setThumbPosition( 
        (int)( mGameStateToEdit->getSelectedTrans() / 255.0 * 36 ) / 36.0 );
    
    mIgnoreSliders = false;

    mAddObjectButton->setEnabled( mGameStateToEdit->canAdd() );

    getTileUnder();


    // add usages for current working state
    int numObjects = mGameStateToEdit->mObjects.size();
    for( int i=0; i<numObjects; i++ ) {
        StateObjectInstance *obj = 
            *( mGameStateToEdit->mObjects.getElement( i ) );
        addUsage( mCurrentWorkingStateID, obj->mObject.getUniqueID() );
        }
    mainStateObjectPicker->recheckDeletable();
    }



#include "Connection.h"


extern Connection *connection;

extern char practiceMode;


void GameStateEditor::enableSend( char inEnabled ) {
    mSendButton->setEnabled( inEnabled );

    mPracticeButton->setEnabled( false );

    if( !inEnabled ) {
        // show practice button?

        if( !practiceMode &&
            ( connection == NULL || ! connection->isConnected() ) ) {
            
            mPracticeButton->setEnabled( true );
            }
        }   
    
    }




void GameStateEditor::aboutToSend() {
    mSendButton->setEnabled( false );    
    //mSendButton->resetPressState();

    // clear player's actions/speech
    StateObjectInstance *objZero = 
        *( mGameStateToEdit->mObjects.getElement( 0 ) );
    
    delete [] objZero->mSpokenMessage;
    objZero->mSpokenMessage = stringDuplicate( "" );
    
    objZero->mAction[0] = '\0';
    
    // re-center action (now empty) on player
    mGameStateToEdit->resetActionAnchor( 0 );
    

    // no need to redo all sprites
    mStateDisplay->updateSpritePositions( 
        mGameStateToEdit->copy() );
    }




void GameStateEditor::clearRedoStack() {    
    int numInStack = mRedoStack.size();
    for( int i=0; i<numInStack; i++ ) {
        delete *( mRedoStack.getElement( i ) );
        }

    mRedoStack.deleteAll();
    mRedoButton->setEnabled( false );
    }


void GameStateEditor::saveUndoPoint() {
    mUndoStack.push_back( mGameStateToEdit->copy() );
    mUndoButton->setEnabled( true );
    
    // new branch, old redo future impossible
    clearRedoStack();
    }



void GameStateEditor::checkFlipAndDeleteSpeechButtonEnabled() {
    int speechLength = mGameStateToEdit->getSelectedSpeechLength();
    
    // show delete button whenever there is speech, even if not 
    // in delete mode.
    if( speechLength > 0 ) {
        mSpeechDeleteButton->setEnabled( true );
        }
    else {
        mSpeechDeleteButton->setEnabled( false );
        }

    int selObject = mGameStateToEdit->getSelectedObject();
    
    StateObjectInstance *o = 
        *( mGameStateToEdit->mObjects.getElement( selObject ) );

    mSpeechBubbleBoxButton->setState( o->mSpeechBox );

    
    if( mStateDisplay->mEditingSpeech || speechLength > 0 ) {
        
        // speech visible, always allow to switch to a box
        mSpeechBubbleBoxButton->setEnabled( true );
        

        
        int bubbleWidth = 64;
        
        intPair bubblePos = ::add( o->mSpeechOffset, o->mAnchorPosition );

        char offTop = mStateDisplay->getSpeechOffTop( selObject );
        
        if( ! o->mSpeechBox && offTop ) {
            bubbleWidth += 10;
            }
        
        int autoOffsetIfFlipped = 
            mGameStateToEdit->getAutoOffsetOnFlip( selObject );
        
        if( !o->mSpeechFlip && 
            bubblePos.x + autoOffsetIfFlipped - bubbleWidth < 0 ) {
            // would be off left edge if flipped
            mFlipSpeechButton->setEnabled( false );
            }
        else if( o->mSpeechFlip && 
                 bubblePos.x + autoOffsetIfFlipped + bubbleWidth >
                 P * G ) {
            // would be off right edge if flipped
            mFlipSpeechButton->setEnabled( false );
            }
        else {
            mFlipSpeechButton->setEnabled( true );
            }
        }
    else {
        // else speech bubble not even visible
        mFlipSpeechButton->setEnabled( false );
        mSpeechBubbleBoxButton->setEnabled( false );
        }
    }

    


void GameStateEditor::displayRedrawed() {
    
    // don't wait for action to detect an auto-flip
    if( mStateDisplay->mHasAnySpeechBeenAutoFlipped ) {
        // make sure we get speech pos/flip tweaks added for display purposes
        mStateDisplay->copySpeechCoordinates( mGameStateToEdit );
    
        // got it!
        mStateDisplay->mHasAnySpeechBeenAutoFlipped = false;
                
        checkFlipAndDeleteSpeechButtonEnabled();
        }
    }



void GameStateEditor::setSelectedObject( int inIndex ) {

    int objIndex = inIndex;

    mGameStateToEdit->setSelectedObject( objIndex );
                    
    mIgnoreSliders = true;
                    
    mObjectTransSlider->setThumbPosition( 
        (int)( mGameStateToEdit->getSelectedTrans() / 
               255.0 * 36 ) 
        / 36.0 );
                    
    mIgnoreSliders = false;
                    

    // ignore change events that we cause
    mIgnoreObjPickerEvents = true;
            
    // no need to generate all new sprites---it's slow!
    //setGameStateToEdit( mGameStateToEdit->copy() );
                        
    mFreezeButton->setEnabled( objIndex == 0 ); 
    mLockSelectedButton->setEnabled( objIndex != 0 );

    mLockSelectedButton->setSelected( 
        mGameStateToEdit->getSelectedLocked() );

    mHoldObjectButton->setSelected( 
        mGameStateToEdit->getObjectHeld( objIndex ) );


    // switch obj picker
    StateObjectInstance *inst =
        *( mGameStateToEdit-> 
           mObjects.getElement( objIndex ) );

    mainStateObjectPicker->setSelectedResource( 
        inst->mObject );

    mStateDisplay->updateSpritePositions( 
        mGameStateToEdit->copy() );

    checkFlipAndDeleteSpeechButtonEnabled();

    mIgnoreObjPickerEvents = false;
                    
    if( objIndex > 0 ) {
        // deleteable
        mRemoveObjectButton->setEnabled( true );
        }
    else {
        mRemoveObjectButton->setEnabled( false );
        }

    getTileUnder();
    }



void GameStateEditor::getTileUnder() {
    StateObjectInstance *o = 
        *( mGameStateToEdit->mObjects.getElement( 
               mGameStateToEdit->getSelectedObject() ) );
    
    uniqueID hitTileID = mGameStateToEdit->mRoom.getTile(
        o->mAnchorPosition.x / P, 
        G - o->mAnchorPosition.y / P - 1 );
    
    Tile hitTile( hitTileID );
    
    mainTilePicker->setBackgroundTile( hitTile );
    }




void GameStateEditor::actionPerformed( GUIComponent *inTarget ) {
    // superclass
    Editor::actionPerformed( inTarget );


    if( inTarget == mainStateObjectPicker ) {
        if( mainStateObjectPicker->wasLastActionFromPress() &&
            this->isVisible()  ) {
            
            StateObject resource = 
                mainStateObjectPicker->getDraggedResource();
            
            int numSprites = resource.getNumLayers();

                        
            int numTotalSprites = numSprites + 1;
            
            intPair *offsets = new intPair[ numTotalSprites ];
            Sprite **sprites = new Sprite*[ numTotalSprites ];
            float *trans = new float[ numTotalSprites ];
            char *glows = new char[ numTotalSprites ];
            
            for( int i=0; i<numSprites; i++ ) {
                offsets[i] = resource.getLayerOffset( i );
                trans[i] = resource.getLayerTrans( i ) / 255.0f;
                glows[i] = resource.getLayerGlow( i );
                SpriteResource r( resource.getLayerSprite( i ) );
                sprites[i] = r.getSprite( false, true );
                }
            
            Sprite *topSprite;
            if( !mGameStateToEdit->canAdd() ) {
                // buster on top
                topSprite = new Sprite( mNoDropImage, true );
                ToolTipManager::setTip( 
                    (char*)TranslationManager::translate( "tip_sceneFull" ) );
                }
            else {
                // nothing on top
                topSprite = new Sprite( mCanDropImage, true );

                ToolTipManager::setTip( 
                    (char*)TranslationManager::translate( 
                        "tip_draggingObjectScene" ) );
                }
            ToolTipManager::freeze( true );

            offsets[numSprites].x = 0;
            offsets[numSprites].y = 0;
            trans[numSprites] = 0.5f;
            glows[numSprites] = false;
            sprites[numSprites] = topSprite;
                
                
            mainDragAndDrop->setSprites( numTotalSprites, sprites, offsets,
                                         trans, glows, 1 );




            /*
            int numSprites = resource.getNumLayers();
            intPair *offsets = new intPair[ numSprites ];
            Sprite **sprites = new Sprite*[ numSprites ];
            float *trans = new float[ numSprites ];
            
            for( int i=0; i<numSprites; i++ ) {
                offsets[i] = resource.getLayerOffset( i );
                trans[i] = resource.getLayerTrans( i ) / 255.0f;
                SpriteResource r( resource.getLayerSprite( i ) );
                sprites[i] = r.getSprite( false, true );
                }
            
            mainDragAndDrop->setSprites( numSprites, sprites, offsets,
                                         trans, 1 );
            */
            }
        else if( ! mIgnoreObjPickerEvents && ! mUndoOrRedoOrClearAction &&
                 ! mainStateObjectPicker->wasLastActionFromPress() ) {
            
            // new obj picked through user picker action, 
            // replace selected object
        
            // changed:  don't do anything here
            // wait for SetObjectButton

            // except if selected object is default... then replace it

            StateObjectInstance *objInstance = 
                *( mGameStateToEdit->mObjects.getElement( 
                    mGameStateToEdit->getSelectedObject() ) );
            
            if( ( mEditingSelectedObject && 
                  mainStateObjectEditor->isVisible() ) 
                ||
                equal( objInstance->mObject.getUniqueID(),
                       StateObject::sBlankObject->getUniqueID() ) ) {
                
                // we're editing the selected one
                // OR
                // selected object is blank (so we obviously should change it)

                                

                // replace automatically with newly-picked object

                saveUndoPoint();
        
                mGameStateToEdit->changeSelectedObject( 
                    mainStateObjectPicker->getSelectedResource() );
        
                // generate new sprites
                setGameStateToEdit( 
                    mGameStateToEdit->copy() );
                }
            }
        }
    else if( inTarget == mainRoomPicker ) {
        // ignore if this room change was triggered by our Undo/Redo
        if( !mUndoOrRedoOrClearAction &&
            ! mainRoomPicker->wasLastActionFromPress() ) {
            
            // will change room
            
            saveUndoPoint();

            mGameStateToEdit->mRoom = mainRoomPicker->getSelectedResource();
            
            mStateDisplay->setState( mGameStateToEdit->copy() );
            
            getTileUnder();
            // don't pass through this, because it changes picker again
            // setGameStateToEdit( mGameStateToEdit->copy() );
            }
        }
    else if( inTarget == mainScenePicker ) {
        
        Scene scenePicked = mainScenePicker->getSelectedResource();

        // ignore if this scene change was triggered by an add
        if( !mAddSceneAction &&
            ! mainScenePicker->wasLastActionFromPress() ) {

            // will change everything
            saveUndoPoint();
            
            // block picker changes from firing
            mUndoOrRedoOrClearAction = true;
            
        
            
            GameState *loaded = new GameState();
            
            char *sceneName = scenePicked.getSceneName();
            
            mNameField->setText( sceneName );
            
            delete [] sceneName;

            // keep global lock button state intact
            loaded->mLocksOn = mGameStateToEdit->mLocksOn;

            Room loadedRoom( scenePicked.mRoom );
            
            loaded->mRoom = loadedRoom;
            loaded->mRoomTrans = scenePicked.mRoomTrans;
            
            loaded->mObjectZeroFrozen = scenePicked.mObjectZeroFrozen;
            

            int numObj = scenePicked.mObjects.size();
        
            for( int i=0; i<numObj; i++ ) {
                StateObject obj( *( scenePicked.mObjects.getElement( i ) ) );

                char held = false;
                if( i == 0 &&
                    mGameStateToEdit->getObjectHeld( 0 ) ) {
                
                    // copy object 0 over, ignoring obj 0 in scene

                    obj = ( *( mGameStateToEdit->mObjects.getElement( 0 ) ) )->
                        mObject;
                    held = true;
                    }
                
                
                intPair pos = 
                    *( scenePicked.mObjectOffsets.getElement( i ) );

                if( i == 0 ) {
                    // 0 already there in empty state... replace it
                    loaded->changeSelectedObject( obj );
                    loaded->moveSelectedObject( pos.x / P, pos.y / P );
                    }
                else {
                    // add new, it will be selected
                    loaded->newObject( pos.x / P, pos.y / P, obj );
                    }
                
                intPair speechOffset = 
                    *( scenePicked.mSpeechOffsets.getElement( i ) );
                intPair speechPos = add( speechOffset, pos );
                
                loaded->moveSelectedSpeechAnchor( speechPos.x, speechPos.y );
                                 
                if( *( scenePicked.mSpeechFlipFlags.getElement( i ) ) ) {
                    loaded->flipSelectedSpeech();
                    }

                loaded->setSelectedSpeechBox(
                    *( scenePicked.mSpeechBoxFlags.getElement( i ) ) );

                loaded->setSelectedLocked(
                    *( scenePicked.mLockedFlags.getElement( i ) ) );

                loaded->adjustSelectedTrans( 
                    *( scenePicked.mObjectTrans.getElement( i ) ) );

                if( held ) {
                    // leave held status on for held objects across 
                    // scene changes
                    loaded->setObjectHeld( i, true );
                    }
                }

            // finally, any non-player objects held are added ON TOP of the
            // scene (in addition to all scene objects

            int numOldObjects = mGameStateToEdit->mObjects.size();
            for( int i=1; i<numOldObjects; i++ ) {
                StateObjectInstance *objInst =
                    *( mGameStateToEdit->mObjects.getElement( i ) );
                
                if( objInst->mHeld ) {
                    loaded->mObjects.push_back( objInst->copy() ); 
                    }
                }
            

            // always switch to player object after loading a scene
            loaded->setSelectedObject( 0 );
            
            setGameStateToEdit( loaded );

            mainStateObjectPicker->recheckDeletable();

            mUndoOrRedoOrClearAction = false;
            }

        mAddToPackButton->setState(
            alreadyInPack( scenePicked.getUniqueID() ) );
        }
    else if( inTarget == mAddSceneButton ) {
        mAddSceneAction = true;
        
        Scene sceneToSave;

        sceneToSave.editSceneName( mNameField->getText() );

        // pack game state into this scene
        sceneToSave.mRoom = mGameStateToEdit->mRoom.getUniqueID();
        
        sceneToSave.mRoomTrans = mGameStateToEdit->mRoomTrans;
        
        sceneToSave.mObjectZeroFrozen = 
            mGameStateToEdit->mObjectZeroFrozen;

        int numObj = mGameStateToEdit->mObjects.size();
        
        for( int i=0; i<numObj; i++ ) {
            StateObjectInstance *objInstance = 
                *( mGameStateToEdit->mObjects.getElement( i ) );
            
            sceneToSave.mObjects.push_back( 
                objInstance->mObject.getUniqueID() );
            
            sceneToSave.mObjectOffsets.push_back( 
                objInstance->mAnchorPosition );
            
            sceneToSave.mSpeechOffsets.push_back( 
                objInstance->mSpeechOffset );
            
            sceneToSave.mSpeechFlipFlags.push_back( 
                objInstance->mSpeechFlip );

            sceneToSave.mSpeechBoxFlags.push_back( 
                objInstance->mSpeechBox );

            sceneToSave.mLockedFlags.push_back( 
                objInstance->mLocked );
            
            sceneToSave.mObjectTrans.push_back( 
                objInstance->mObjectTrans );
            }
        

        sceneToSave.finishEdit();
        mainScenePicker->setSelectedResource( sceneToSave, true );
        mainStateObjectPicker->recheckDeletable();
        
        mAddSceneAction = false;
        }
    else if( inTarget == mSetObjectButton ) {
        saveUndoPoint();
        
        mGameStateToEdit->changeSelectedObject( 
            mainStateObjectPicker->getSelectedResource() );
        
        // generate new sprites
        setGameStateToEdit( 
            mGameStateToEdit->copy() );
        }
    else if( inTarget == mAddObjectButton ) {
        saveUndoPoint();
        
        StateObject resource = 
            mainStateObjectPicker->getDraggedResource();

        if( mStateDisplay->mMouseHover ) {
            // mouse over grid
            // add new object at mouse
            mGameStateToEdit->newObject( 
                mStateDisplay->mLastHoverGridX,
                mStateDisplay->mLastHoverGridY,
                resource );
            }
        else {
            
            // center
            mGameStateToEdit->newObject( 
                G / 2,
                G / 2,
                resource );
            }
        
        // generate new sprites
        setGameStateToEdit( mGameStateToEdit->copy() );
        }
    else if( inTarget == mEditObjectButton ) {
        // editing selected... auto-replace
        mEditingSelectedObject = true;

        // make sure selected one is chosen in picker
        mIgnoreObjPickerEvents = true;
        
        StateObjectInstance *inst =
                            *( mGameStateToEdit-> 
                               mObjects.getElement( 
                                   mGameStateToEdit->getSelectedObject() ) );

        mainStateObjectPicker->setSelectedResource( 
            inst->mObject );
        mIgnoreObjPickerEvents = false;

        showStateObjectEditor();
        }
    else if( inTarget == mRemoveObjectButton ) {
        saveUndoPoint();
        mGameStateToEdit->deleteSelectedObject();
        
        if( mGameStateToEdit->getSelectedObject() > 0 ) {
            mRemoveObjectButton->setEnabled( true );
            }
        else {
            // never delete object 0 (player object)
            mRemoveObjectButton->setEnabled( false );
            }

        setGameStateToEdit( mGameStateToEdit->copy() );
        }
    else if( inTarget == mSendButton ) {
        mSendButton->setEnabled( false );

        fireActionPerformed( this );
        }
    else if( inTarget == mPracticeButton ) {
        mPracticeButton->setEnabled( false );
        practiceMode = true;
        enableSend( true );
        }
    else if( inTarget == mEditStateObjectButton ) {
        // edit whatever is in picker
        mEditingSelectedObject = false;
        showStateObjectEditor();
        }
    else if( inTarget == mEditRoomButton ) {
        mStateDisplay->setHintMode( true );
        mainRoomEditor->takeOverGameStateDisplay();
        showRoomEditor();
        }
    else if( inTarget == mEditMusicButton ) {
        showSongEditor();
        }
    else if( inTarget == mAddToPackButton ) {
        Scene sceneToLoad = mainScenePicker->getSelectedResource();
        
        AppLog::info( "Adding a scene to the current resource pack" );
        sceneToLoad.saveToPack();

        mAddToPackButton->setState( true );
        }
    else if( inTarget == mSavePackButton ) {
        AppLog::info( "Saving the current resource pack" );
        savePack();

        mAddToPackButton->setState( false );
        }
    else if( inTarget == mUndoButton ) {
        mUndoOrRedoOrClearAction = true;
        
        int lastIndex = mUndoStack.size() - 1;
        
        GameState *last = *( mUndoStack.getElement( lastIndex ) );
        mUndoStack.deleteElement( lastIndex );
        if( mUndoStack.size() == 0 ) {
            mUndoButton->setEnabled( false );
            }
        
        mRedoStack.push_back( mGameStateToEdit->copy() );
        mRedoButton->setEnabled( true );
        
        setGameStateToEdit( last );
        
        mUndoOrRedoOrClearAction = false;
        }
    else if( inTarget == mRedoButton ) {
        mUndoOrRedoOrClearAction = true;

        int nextIndex = mRedoStack.size() - 1;
        
        GameState *next = *( mRedoStack.getElement( nextIndex ) );
        mRedoStack.deleteElement( nextIndex );
        if( mRedoStack.size() == 0 ) {
            mRedoButton->setEnabled( false );
            }
        
        mUndoStack.push_back( mGameStateToEdit->copy() );
        mUndoButton->setEnabled( true );
        
        setGameStateToEdit( next );
        
        mUndoOrRedoOrClearAction = false;
        }
    else if( inTarget == mToolSet ) {
        // tool change?
        
        // we only care about one:
        // if speech tool, switch keyboard to main display
        
        mStateDisplay->mEditingSpeech =
            (  mToolSet->getSelected() == speak );

        checkFlipAndDeleteSpeechButtonEnabled();
        }
    else if( inTarget == mSpeechDeleteButton ) {
        saveUndoPoint();
        mGameStateToEdit->deleteAllCharsFromSelectedSpeech();

        if( mGameStateToEdit->getSelectedObject() == 0 ) {
            
            // clear player's action too
            StateObjectInstance *objZero = 
                *( mGameStateToEdit->mObjects.getElement( 0 ) );
    
            
            objZero->mAction[0] = '\0';
            
            // re-center action (now empty) on player
            mGameStateToEdit->resetActionAnchor( 0 );
            }
        

        // no need to redo all sprites
        mStateDisplay->updateSpritePositions( 
            mGameStateToEdit->copy() );
        
        checkFlipAndDeleteSpeechButtonEnabled();
        }
    else if( inTarget == mFlipSpeechButton ) {
        saveUndoPoint();

        int selObject = mGameStateToEdit->getSelectedObject();
        
        StateObjectInstance *o = 
            *( mGameStateToEdit->mObjects.getElement( selObject ) );
        
        o->mSpeechOffset.x += 
            mGameStateToEdit->getAutoOffsetOnFlip( selObject );
        
        mGameStateToEdit->flipSelectedSpeech();

        // don't redo sprites
        mStateDisplay->updateSpritePositions( 
            mGameStateToEdit->copy() );
        }
    else if( inTarget == mSpeechBubbleBoxButton ) {
        saveUndoPoint();

        mGameStateToEdit->setSelectedSpeechBox( 
            mSpeechBubbleBoxButton->getState() );
        
        checkFlipAndDeleteSpeechButtonEnabled();

        // don't redo sprites
        mStateDisplay->updateSpritePositions( 
            mGameStateToEdit->copy() );
        }
    else if( inTarget == mGridButton ) {
        // toggle
        mGridButton->setSelected( ! mGridButton->getSelected() );
        
        mStateDisplay->showGrid( mGridButton->getSelected() );
        }
    else if( inTarget == mFreezeButton ) {
        // toggle
        mFreezeButton->setSelected( ! mFreezeButton->getSelected() );
        
        saveUndoPoint();
        
        mGameStateToEdit->freezeObjectZero( mFreezeButton->getSelected() );

        // don't need to update display at all
        }
    else if( inTarget == mLockSelectedButton ) {
        // toggle
        mLockSelectedButton->setSelected( 
            ! mLockSelectedButton->getSelected() );
        
        saveUndoPoint();
        
        
        mGameStateToEdit->setSelectedLocked( 
            mLockSelectedButton->getSelected() );

        if( mGameStateToEdit->mLocksOn ) {
            // look for an unlocked object to make selected
            char objChange = false;
            
            while( mGameStateToEdit->getSelectedLocked() ) {
                mGameStateToEdit->setSelectedObject( 
                    mGameStateToEdit->getSelectedObject() - 1 );
                objChange = true;
                }
            if( objChange ) {
                setSelectedObject( mGameStateToEdit->getSelectedObject() );
                }
            }


        // don't redo sprites
        mStateDisplay->updateSpritePositions( 
            mGameStateToEdit->copy() );
        }
    else if( inTarget == mLockGlobalButton ) {
        mLockGlobalButton->setSelected( ! mLockGlobalButton->getSelected() );
        
        saveUndoPoint();
        
        mGameStateToEdit->mLocksOn = mLockGlobalButton->getSelected();
        
        if( mGameStateToEdit->mLocksOn ) {
            // look for an unlocked object to make selected
            char objChange = false;
            
            while( mGameStateToEdit->getSelectedLocked() ) {
                mGameStateToEdit->setSelectedObject( 
                    mGameStateToEdit->getSelectedObject() - 1 );
                objChange = true;
                }
            if( objChange ) {
                setSelectedObject( mGameStateToEdit->getSelectedObject() );
                }
            }

        // don't redo sprites
        mStateDisplay->updateSpritePositions( 
            mGameStateToEdit->copy() );
        }
    else if( inTarget == mHoldObjectButton ) {
        // toggle
        mHoldObjectButton->setSelected( 
            ! mHoldObjectButton->getSelected() );
        
        saveUndoPoint();
        
        
        mGameStateToEdit->setObjectHeld( 
            mGameStateToEdit->getSelectedObject(),
            mHoldObjectButton->getSelected() );

        // don't update anything display-wise
        }
    else if( inTarget == mRoomTransSlider && ! mIgnoreSliders ) {
        if( mRoomTransSlider->mJustPressed ) {
            // first move in this adjustment, save an undo point here
            saveUndoPoint();
            }
        
        mGameStateToEdit->mRoomTrans = 
            (unsigned char)( 255 * mRoomTransSlider->getThumbPosition() );
        
        // don't redo sprites
        mStateDisplay->updateSpritePositions( 
            mGameStateToEdit->copy() );
        }
    else if( inTarget == mObjectTransSlider && ! mIgnoreSliders ) {
        if( mObjectTransSlider->mJustPressed ) {
            // first move in this adjustment, save an undo point here
            saveUndoPoint();
            }
        
        mGameStateToEdit->adjustSelectedTrans( 
            (unsigned char)( 255 * mObjectTransSlider->getThumbPosition() ) );
        
        // don't redo sprites
        mStateDisplay->updateSpritePositions( 
            mGameStateToEdit->copy() );
        }
    else if( inTarget == mClearButton ) {
        saveUndoPoint();

        mGameStateToEdit->mRoom = Room::getDefaultResource();
        
        mGameStateToEdit->mRoomTrans = 255;
        

        mGameStateToEdit->deleteSelectedObject();

        int numObjects = mGameStateToEdit->mObjects.size();
        
        // don't delete obj 0
        for( int i=numObjects-1; i>0; i-- ) {
            mGameStateToEdit->setSelectedObject( i );
            mGameStateToEdit->deleteSelectedObject();
            }
        // obj 0 left and selected
        mGameStateToEdit->changeSelectedObject( 
            StateObject::getDefaultResource() );
        
        mGameStateToEdit->deleteAllCharsFromSelectedSpeech();
        
        // revert
        mGameStateToEdit->adjustSelectedTrans( 255 );
                

        // leave action in place (controller cannot edit it)


        // mStateDisplay->setState( mGameStateToEdit->copy() );
        mUndoOrRedoOrClearAction = true;
        
        setGameStateToEdit( mGameStateToEdit->copy() );
        
        mUndoOrRedoOrClearAction = false;
        }
    else if( inTarget == mStateDisplay
             &&
             !mRoomTransSlider->mDragging
             &&
             !mObjectTransSlider->mDragging ) {
        // ignore display events if dragging off edge of slider




        /*
        if( mStateDisplay->mLastActionRelease ) { 
            // grab hit room tile and set it as bg tile for other pickers
            uniqueID hitTileID = mGameStateToEdit->mRoom.getTile( 
                mStateDisplay->mLastGridClickX,
                G - mStateDisplay->mLastGridClickY - 1 );
            
            Tile hitTile( hitTileID );
            
            mainTilePicker->setBackgroundTile( hitTile );
            }
        */



        if( mStateDisplay->mLastActionRelease &&
            mainDragAndDrop->isDragging() ) {
            
            // add new obj

            saveUndoPoint();
        
            if( mGameStateToEdit->canAdd() ) {
                
                StateObject resource = 
                    mainStateObjectPicker->getDraggedResource();
        

                mGameStateToEdit->newObject( 
                    mStateDisplay->mLastGridClickX,
                    mStateDisplay->mLastGridClickY,
                    resource );
                }
            
            setGameStateToEdit( mGameStateToEdit->copy() );
            }
        else if( !mainDragAndDrop->isDragging() ) {
            
        

            if( mToolSet->getSelected() == move ) {
                // pick object anchor whenever we press over it with move tool
                // tool it is
                if( mStateDisplay->mLastActionPress ) { 

                    int objIndex = mGameStateToEdit->getHitObject(
                        mStateDisplay->mLastGridClickX,
                        mStateDisplay->mLastGridClickY );
                    
                    if( objIndex != -1 ) {
                        int oldSelected =
                            mGameStateToEdit->getSelectedObject();
                
                        if( oldSelected != objIndex ) {
                            mObjectChanging = true;
                    
                            saveUndoPoint();
                    
                            setSelectedObject( objIndex );
                            }
                        }
                    }
                }
            

            // allow typing whenever, even if not in speech mode
            // but never let Controller type into Player's bubble
            if( mStateDisplay->mLastActionKeyPress &&
                mGameStateToEdit->getSelectedObject() != 0 ) {
                unsigned char key = mStateDisplay->mLastKeyPressed;
                
                if( key == 127 || key == 8 ) {
                    // backspace and delete
                    saveUndoPoint();
                        
                    mGameStateToEdit->deleteCharFromSelectedSpeech();
                    }
                else if( (key >= 32 && key <= 126)
                         || key >= 160 ) {
                    // allowed range, ascii and extended ascii
                        
                    // only save undo points before a space is pressed
                    // or before first chars are typed
                    if( key == 32 ||
                        mGameStateToEdit->getSelectedSpeechLength() == 
                        0 ) {

                        saveUndoPoint();
                        }
                        
                    mGameStateToEdit->addCharToSelectedSpeech(
                        (char)key );
                    }

                checkFlipAndDeleteSpeechButtonEnabled();
                

                // no need to redo all sprites
                mStateDisplay->updateSpritePositions( 
                    mGameStateToEdit->copy() );
                }
            


        
            switch( mToolSet->getSelected() ) {
                case move: {
                    if( !mStateDisplay->mLastActionKeyPress ) {
                        
                        // only save an undo point when mouse initially pressed
                        // (ignore micro-state changes until release)
                        // undo point already saved if object changing
                        if( !mObjectChanging && 
                            mStateDisplay->mLastActionPress ) {
                            saveUndoPoint();
                            }
                
                        // move even in response to mouse dragging
                        
                        // whole object only
                        mGameStateToEdit->moveSelectedObject(
                            mStateDisplay->mLastGridClickX,
                            mStateDisplay->mLastGridClickY );
                        
                        // no need to generate all new sprites
                        mStateDisplay->updateSpritePositions( 
                            mGameStateToEdit->copy() );

                        // flip might become forbidden because of move
                        checkFlipAndDeleteSpeechButtonEnabled();

                        getTileUnder();
                        }
                    }
                    break;
                case speak: {
                    if( !mStateDisplay->mLastActionKeyPress ) {
                        // mouse movement with speech tool
                        // adjust speech anchor

                        // ignore this if this mouse action is picking 
                        // a new object
                        if( !mObjectChanging ) {
                        
                            // only save an undo point when mouse 
                            // initially pressed
                            // (ignore micro-state changes until release)
                            if( mStateDisplay->mLastActionPress ) {
                                saveUndoPoint();
                                }
                            
                            
                            mGameStateToEdit->moveSelectedSpeechAnchor(
                                mStateDisplay->mLastPixelClickX,
                                mStateDisplay->mLastPixelClickY );

                    
                            // no need to generate all new sprites
                            mStateDisplay->updateSpritePositions( 
                                mGameStateToEdit->copy() );
                            mStateDisplay->mManualBubblePositioningLive = true;
                            
                            // flip might become forbidden because of move
                            checkFlipAndDeleteSpeechButtonEnabled();
                            }
                        }
                
                    }
                    break;
                }
            
        
            // if mouse released, object change is over
            if( mStateDisplay->mLastActionRelease ) {
                mObjectChanging = false;
                }
            }    
        
        }
    
    
    if( ! mNameField->isFocused() &&
        ! mainStateObjectPicker->isSearchFieldFocused() &&
        ! mainScenePicker->isSearchFieldFocused() ) {
        
        
        // always refocus whenever an action happens
        // essentially, keep focus on display always
        // (except when user edits Scene name or types in a search box)
        mStateDisplay->setFocus( true );
        }
    
    }



void GameStateEditor::editorClosing() {
    }
