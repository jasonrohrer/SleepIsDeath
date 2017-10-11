#ifndef SCALE_EDITOR_INCLUDED
#define SCALE_EDITOR_INCLUDED


#include "Editor.h"
#include "buttons.h"
#include "Scale.h"
#include "SizeLimitedVector.h"



#include "minorGems/graphics/openGL/gui/TextFieldGL.h"



class ScaleEditor : public Editor {
        
    public:
        
        ScaleEditor( ScreenGL *inScreen );
        
        ~ScaleEditor();

        // for realtime highlights
        void setPartToWatch( int inPart );
        
        

        virtual void actionPerformed( GUIComponent *inTarget );
        

    protected:

        // implemented by all subclasses
        // called by parent class when editor is being closed
        virtual void editorClosing();

        // triggered by add button or close
        void addScale();

        
        AddButtonGL *mAddButton;
        char mAddAction;
        
        void setScaleToEdit( Scale inScale  );
        
        void refreshMiniView();
        
        void saveUndoPoint();
        
        
        ScaleToggleButton *mScaleButtons[HT];
                

        SpriteButtonGL *mMiniViewButton;
        
        Scale mScaleToEdit;
        

        UndoButtonGL *mUndoButton;
        RedoButtonGL *mRedoButton;
        
        
        SizeLimitedVector<Scale> mUndoStack;
        SimpleVector<Scale> mRedoStack;
     

        TextFieldGL *mNameField; 


        int mPartToWatch;
        
    };

#endif
