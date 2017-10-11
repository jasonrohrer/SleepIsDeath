#ifndef GAME_STATE_EDITOR_INCLUDED
#define GAME_STATE_EDITOR_INCLUDED


#include "Editor.h"
#include "buttons.h"
#include "GameState.h"
#include "GameStateDisplay.h"
#include "StateToolSet.h"
#include "SizeLimitedVector.h"

#include "minorGems/graphics/openGL/gui/TextFieldGL.h"
#include "ToolTipSliderGL.h"



class GameStateEditor : public Editor {
        
    public:
        
        GameStateEditor( ScreenGL *inScreen );
        
        ~GameStateEditor();

        virtual void actionPerformed( GUIComponent *inTarget );

        GameState *mGameStateToEdit;

        void setGameStateToEdit( GameState *inGameState,
                                 char inUpdatePickers = true );        
        
        void enableSend( char inEnabled );
        void aboutToSend();


        // to receive redraw pings from display
        void displayRedrawed();
        

    protected:

        // implemented by all subclasses
        // called by parent class when editor is being closed
        virtual void editorClosing();
        

        GameStateDisplay *mStateDisplay;
        
        char mEditingSelectedObject;
        
        
        EditButtonGL *mEditStateObjectButton;
        
        
        UndoButtonGL *mUndoButton;
        RedoButtonGL *mRedoButton;
        ClearButtonGL *mClearButton;
        
        char mUndoOrRedoOrClearAction;
        char mIgnoreSliders;
        
        void clearRedoStack();
        
        void saveUndoPoint();
        

        StateToolSet *mToolSet;

        SizeLimitedVector<GameState*> mUndoStack;
        SimpleVector<GameState*> mRedoStack;


        AddButtonGL *mAddSceneButton;
        char mAddSceneAction;
        

        QuickDeleteButtonGL *mSpeechDeleteButton;


        ToggleSpriteButtonGL *mSpeechBubbleBoxButton;
        

        FlipHButtonGL *mFlipSpeechButton;
        
        // checks based on currently-selected object and how
        // close it is to screen (also whether speech bubble is even visible)
        // also checks cases for enabling speech delete button
        void checkFlipAndDeleteSpeechButtonEnabled();
        
        
        void setSelectedObject( int inIndex );
        

        void getTileUnder();
        

        SelectableButtonGL *mGridButton;

        SelectableButtonGL *mFreezeButton;

        SelectableButtonGL *mLockGlobalButton;
        SelectableButtonGL *mLockSelectedButton;

                
        SelectableButtonGL *mHoldObjectButton;


        SpriteButtonGL *mSetObjectButton;
        KeyEquivButtonGL *mAddObjectButton;
        KeyEquivButtonGL *mEditObjectButton;
        KeyEquivButtonGL *mRemoveObjectButton;


        SpriteButtonGL *mEditRoomButton;



        SendButtonGL *mSendButton;

        SpriteButtonGL *mPracticeButton;


        SpriteButtonGL *mEditMusicButton;
        
        
        ToolTipSliderGL *mRoomTransSlider;
        ToolTipSliderGL *mObjectTransSlider;


        TextFieldGL *mNameField;

        
        ToggleSpriteButtonGL *mAddToPackButton;
        SpriteButtonGL *mSavePackButton;
        
        
        Image *mNoDropImage;
        Image *mCanDropImage;

        


        char mIgnoreObjPickerEvents;
        

        // don't move speech anchor while this is happening
        char mObjectChanging;
        

        // for usage tracking of objects in current state
        uniqueID mCurrentWorkingStateID;
        

        
    };

#endif
