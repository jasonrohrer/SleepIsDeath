#ifndef PLAYER_MOVE_EDITOR_INCLUDED
#define PLAYER_MOVE_EDITOR_INCLUDED


#include "Editor.h"
#include "buttons.h"
#include "GameState.h"
#include "GameStateDisplay.h"
#include "MoveToolSet.h"
#include "FixedTipDisplay.h"


class PlayerMoveEditor : public Editor {
        
    public:
        
        PlayerMoveEditor( ScreenGL *inScreen );
        
        ~PlayerMoveEditor();

        virtual void actionPerformed( GUIComponent *inTarget );


        void setGameStateToEdit( GameState *inGameState );        
        
        GameState *mGameStateToEdit;
        

        void setMovesDisabled( char inDisabled );
        
        
        // clears all but player speech
        void clearNonPlayerSpeech();
        

        void enableSend( char inEnabled );


        // has no effect if flip book turned off
        void saveFlipBookImage();
        

    protected:

        // implemented by all subclasses
        // called by parent class when editor is being closed
        virtual void editorClosing();


        GameStateDisplay *mStateDisplay;

        MoveToolSet *mToolSet;

        DeleteButtonGL *mSpeechDeleteButton;
        DeleteButtonGL *mActionDeleteButton;
        

        SendButtonGL *mSendButton;
        SpriteButtonGL *mPracticeStopButton;
        
        
        FixedTipDisplay *mTipDisplay;


        SelectableButtonGL *mFlipBookButton;


        char mMovesDisabled;


        // updated once per play session
        int mFlipBookFolderNumber;
        
        // starts at 1 for each play session
        int mFlipBookImageNumber;
        
    };

#endif
