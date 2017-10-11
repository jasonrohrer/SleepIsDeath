#ifndef ROOM_EDITOR_INCLUDED
#define ROOM_EDITOR_INCLUDED


#include "Editor.h"
#include "buttons.h"
#include "Room.h"
#include "GameStateDisplay.h"
#include "DrawToolSet.h"
#include "SizeLimitedVector.h"

#include "minorGems/graphics/openGL/gui/TextFieldGL.h"


class RoomEditor : public Editor {
        
    public:
        
        RoomEditor( ScreenGL *inScreen );
        
        ~RoomEditor();


        // a bit messy, but this allows GameStateEditor and RoomEditor
        // to share the same state display (so room editor can show 
        // object hints)
        //
        // Room Editor construction not complete until this has been called.
        //
        // We don't destroy this
        void setGameStateDisplay( GameStateDisplay *inDisplay );
        
        // called by GameStateEditor before it opens the RoomEditor
        // tells us that the shared GameStateDisplay is now ours
        void takeOverGameStateDisplay();
        

        virtual void actionPerformed( GUIComponent *inTarget );


        // implemented by all subclasses
        // called by parent class when editor is being closed
        virtual void editorClosing();
        

    protected:
        
        GameStateDisplay *mStateDisplay;
        


        // triggered by add button or close
        void addRoom();
        

        AddButtonGL *mAddButton;
        char mAddAction;
        
        void setRoomToEdit( Room inRoom );
        
        void refreshMiniView();
        
        
        SpriteCellButtonGL *mButtonGrid[G][G];
        

        SpriteButtonGL *mMiniViewButton;
        
        Room mRoomToEdit;
        

        EditButtonGL *mEditTileButton;
        
        
        UndoButtonGL *mUndoButton;
        RedoButtonGL *mRedoButton;
        ClearButtonGL *mClearButton;
        

        DrawToolSet *mToolSet;

        SelectableButtonGL *mWallsButton;
        SelectableButtonGL *mHintsButton;
        

        // returns true if anything changed
        char recursiveFill( int inX, int inY, 
                            uniqueID inOldTile, uniqueID inNewTile,
                            drawTool inTool );


        // version for wall toggles
        char recursiveFill( int inX, int inY, 
                            char inOldWall, char inNewWall,
                            drawTool inTool );
        
        SizeLimitedVector<Room> mUndoStack;
        SimpleVector<Room> mRedoStack;
     

        TextFieldGL *mSetNameField;
        

        char mPenDown;
        char mWallInk;
        

        // for usage tracking of objects in current state
        uniqueID mCurrentWorkingStateID;


    };

#endif
