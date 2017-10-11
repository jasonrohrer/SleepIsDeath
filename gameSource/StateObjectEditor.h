#ifndef STATE_OBJECT_EDITOR_INCLUDED
#define STATE_OBJECT_EDITOR_INCLUDED


#include "Editor.h"
#include "buttons.h"
#include "StateObject.h"
#include "StateObjectDisplay.h"
#include "SizeLimitedVector.h"
#include "ToolTipSliderGL.h"

#include "minorGems/graphics/openGL/gui/TextFieldGL.h"


class StateObjectEditor : public Editor {
        
    public:
        
        StateObjectEditor( ScreenGL *inScreen );
        
        ~StateObjectEditor();

        virtual void actionPerformed( GUIComponent *inTarget );
        

    protected:
        
        // implemented by all subclasses
        // called by parent class when editor is being closed
        virtual void editorClosing();

        // triggered by add button or close
        void addStateObject();
        

        void generateDraggingOffsets();
        


        AddButtonGL *mAddButton;
        char mAddAction;


        int mSelectedLayer;
        
        // where user clicked on layer, move layer center relative to
        // mouse movement using this offset
        intPair mLayerClickOffset;
        
        
        // when dragging the whole object around, track offset of each layer
        SimpleVector<intPair> mWholeDragOffsets;
        


        void setStateObjectToEdit( StateObject inStateObject );        
        
        StateObject mStateObjectToEdit;
        

        StateObjectDisplay *mStateDisplay;
        

        //EditButtonGL *mEditRoomButton;
        EditButtonGL *mEditSpriteButton;
        
        
        UndoButtonGL *mUndoButton;
        RedoButtonGL *mRedoButton;
        ClearButtonGL *mClearButton;

        RotateCWButtonGL *mCWButton;
        FlipHButtonGL *mFlipHButton;
        
        ToolTipSliderGL *mLayerTransSlider;
        
        char mIgnoreSliders;
        

        char mUndoOrRedoAction;
        
        void clearRedoStack();
        
        void saveUndoPoint();
        


        SizeLimitedVector<StateObject> mUndoStack;
        SimpleVector<StateObject> mRedoStack;
        



        

        //QuickDeleteButtonGL *mObjectLayerDeleteButton;
        
        //SmallAddButtonGL *mObjectLayerAddButton;
        //SpriteButtonGL *mLayerReplaceButton;        


        SpriteButtonGL *mReplaceSpriteButton;
        KeyEquivButtonGL *mAddSpriteButton;
        KeyEquivButtonGL *mEditSelectedSpriteButton;
        KeyEquivButtonGL *mRemoveSpriteButton;


        char mEditingSelectedLayer;


        TwoSpriteButtonGL *mObjectLayerViewButton;

        LeftButtonGL *mObjectPrevLayerButton;
        RightButtonGL *mObjectNextLayerButton;

        SpriteButtonGL *mObjectBottomLayerButton;
        SpriteButtonGL *mObjectTopLayerButton;

        SpriteButtonGL *mLayerUpButton;
        SpriteButtonGL *mLayerDownButton;
        
        ToggleSpriteButtonGL *mLayerGlowButton;
        

        SelectableButtonGL *mTransButton;

        
        SelectableButtonGL *mGridButton;
        
        

        SpriteButtonGL *mZoomButton;
        SpriteButtonGL *mUnZoomButton;


        TextFieldGL *mNameField;
        

        
        char mDragFromObjectPicker;
        
        char mClickStartedOnLayer;
        
        

        Image *mNoDropImage;
        Image *mCanDropImage;
        
        
        // for usage tracking of objects in current state
        uniqueID mCurrentWorkingStateID;
        
    };

#endif
