#ifndef PALETTE_EDITOR_INCLUDED
#define PALETTE_EDITOR_INCLUDED


#include "Editor.h"
#include "buttons.h"
#include "Palette.h"
#include "SizeLimitedVector.h"

#include "minorGems/graphics/openGL/gui/TextFieldGL.h"


class PaletteEditor : public Editor {
        
    public:
        
        PaletteEditor( ScreenGL *inScreen );
        
        ~PaletteEditor();

        
        void grabPaletteFromWells();
        

        virtual void actionPerformed( GUIComponent *inTarget );
        

    protected:

        // implemented by all subclasses
        // called by parent class when editor is being closed
        virtual void editorClosing();

        // triggered by add button or close
        void addPalette();
        
        void saveUndoPoint();

        
        AddButtonGL *mAddButton;
        char mAddAction;
        
        void setPaletteToEdit( Palette inPalette );
        
        void refreshMiniView();
        
        
        SpriteButtonGL *mButtonGrid[8][5];
        

        SpriteButtonGL *mMiniViewButton;
        
        Palette mPaletteToEdit;
        

        EditButtonGL *mEditColorButton;
        
        
        UndoButtonGL *mUndoButton;
        RedoButtonGL *mRedoButton;


        SizeLimitedVector<Palette> mUndoStack;
        SimpleVector<Palette> mRedoStack;
     

        TextFieldGL *mNameField;
        
        ToggleSpriteButtonGL *mAddToPackButton;
        SpriteButtonGL *mSavePackButton;


    };

#endif
