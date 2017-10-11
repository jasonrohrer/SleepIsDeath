#ifndef SPRITE_EDITOR_INCLUDED
#define SPRITE_EDITOR_INCLUDED


#include "Editor.h"
#include "buttons.h"
#include "SpriteResource.h"
#include "DrawToolSet.h"
#include "TransformToolSet.h"
#include "SizeLimitedVector.h"

#include "minorGems/graphics/openGL/gui/TextFieldGL.h"


class SpriteEditor : public Editor {
        
    public:
        
        SpriteEditor( ScreenGL *inScreen );
        
        ~SpriteEditor();

        virtual void actionPerformed( GUIComponent *inTarget );
        

    protected:


        // implemented by all subclasses
        // called by parent class when editor is being closed
        virtual void editorClosing();

        // triggered by add button or close
        void addSprite();
        

        // override from Editor
        virtual void colorEditorClosed();



        AddButtonGL *mAddButton;
        char mAddAction;
        
        void setSpriteToEdit( SpriteResource inSprite );
        
        void refreshMiniView();
        
        
        HighlightColorButtonGL *mButtonGrid[P][P];
        

        TwoSpriteButtonGL *mMiniViewButton;
        
        SpriteResource mSpriteToEdit;
        

        EditButtonGL *mEditColorButton;
        EditButtonGL *mEditPaletteButton;
        
        
        UndoButtonGL *mUndoButton;
        RedoButtonGL *mRedoButton;
        

        TransformToolSet *mTransformToolSet;


        DrawToolSet *mToolSet;
        
        SelectableButtonGL *mEraseButton;

        SelectableButtonGL *mSelectionButton;

        
        SelectableButtonGL *mPickerTransButton;
        

        // returns true if anything changed
        char recursiveFill( int inX, int inY, rgbaColor inOldColor, 
                            char inOldTrans,
                            rgbaColor inNewColor,
                            drawTool inTool );

        // version for transparency
        char recursiveFill( int inX, int inY, 
                            rgbaColor inOldColor,
                            char inOldTrans, char inNewTrans,
                            drawTool inTool );
        
        // version for selection
        char recursiveSelectionFill( int inX, int inY, 
                                     rgbaColor inOldColor,
                                     char inOldTrans,
                                     char inOldSelection, 
                                     char inNewSelection,
                                     drawTool inTool );
        


        void toggleErase();
        void toggleSelection();
        
        void clearStampOverlay();
        
        
        
        SizeLimitedVector<SpriteResource> mUndoStack;
        SimpleVector<SpriteResource> mRedoStack;
     

        TextFieldGL *mNameField;
        

        char mPenDown;
        char mTransInk;
        char mSelectionInk;
        
    };

#endif
