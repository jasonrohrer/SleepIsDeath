#ifndef TIMBRE_EDITOR_INCLUDED
#define TIMBRE_EDITOR_INCLUDED


#include "Editor.h"
#include "buttons.h"
#include "TimbreResource.h"
#include "SizeLimitedVector.h"
#include "ToolTipSliderGL.h"



#include "minorGems/graphics/openGL/gui/TextFieldGL.h"


#define NUM_OCTAVES 5

class TimbreEditor : public Editor {
        
    public:
        
        TimbreEditor( ScreenGL *inScreen );
        
        ~TimbreEditor();


        // sets index of timbre to realtime-edit in musicPlayer
        void setPlayerTimbreToEdit( int inTimbre );
        

        virtual void actionPerformed( GUIComponent *inTarget );
        

    protected:

        int mPlayerTimbreIndex;


        // implemented by all subclasses
        // called by parent class when editor is being closed
        virtual void editorClosing();

        // triggered by add button or close
        void addTimbre();

        
        AddButtonGL *mAddButton;
        char mAddAction;
        
        void setTimbreToEdit( TimbreResource inTimbre, 
                              char inUpdatePlayerTimbre = true,
                              char inUpdatePlayerEnvelope = true );
        
        void refreshMiniView();
        
        void saveUndoPoint();
        
        
        ColorButtonGL *mButtonGrid[F][FL];
        // invisible buttons off left edge, to field zero clicks
        ColorButtonGL *mZeroButtons[F];
        

        SpriteButtonGL *mMiniViewButton;
        
        TimbreResource mTimbreToEdit;
        

        UndoButtonGL *mUndoButton;
        RedoButtonGL *mRedoButton;
        
        
        SizeLimitedVector<TimbreResource> mUndoStack;
        SimpleVector<TimbreResource> mRedoStack;
     

        TextFieldGL *mNameField;

        ToolTipSliderGL *mAttackSlider;
        ToolTipSliderGL *mHoldSlider;
        ToolTipSliderGL *mReleaseSlider;
        

        SelectableButtonGL  *mOctaveButtons[ NUM_OCTAVES ];


        char mPenDown;
        char mIgnoreSliders;
        
    };

#endif
