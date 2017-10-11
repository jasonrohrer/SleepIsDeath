#ifndef TILE_EDITOR_INCLUDED
#define TILE_EDITOR_INCLUDED


#include "Editor.h"
#include "buttons.h"
#include "Tile.h"
#include "DrawToolSet.h"
#include "TransformToolSet.h"
#include "SizeLimitedVector.h"

#include "minorGems/graphics/openGL/gui/TextFieldGL.h"


class TileEditor : public Editor {
        
    public:
        
        TileEditor( ScreenGL *inScreen );
        
        ~TileEditor();

        virtual void actionPerformed( GUIComponent *inTarget );
        

    protected:

        // implemented by all subclasses
        // called by parent class when editor is being closed
        virtual void editorClosing();

        // triggered by add button or close
        void addTile();
        
        
        // override from Editor
        virtual void colorEditorClosed();
        

        
        AddButtonGL *mAddButton;
        char mAddAction;
        
        void setTileToEdit( Tile inTile );
        
        void refreshMiniView();
        
        
        HighlightColorButtonGL *mButtonGrid[P][P];
        

        SpriteButtonGL *mMiniViewButton;
        
        Tile mTileToEdit;
        

        EditButtonGL *mEditColorButton;
        EditButtonGL *mEditPaletteButton;
        
        
        UndoButtonGL *mUndoButton;
        RedoButtonGL *mRedoButton;
        
        
        TransformToolSet *mTransformToolSet;
        

        SelectableButtonGL *mSelectionButton;



        DrawToolSet *mToolSet;
        
        // returns true if anything changed
        char recursiveFill( int inX, int inY, rgbaColor inOldColor,
                            rgbaColor inNewColor,
                            drawTool inTool );
        
        // version for selection
        char recursiveSelectionFill( int inX, int inY, 
                                     rgbaColor inOldColor,
                                     char inOldSelection, 
                                     char inNewSelection,
                                     drawTool inTool );


        void toggleSelection();

        void clearStampOverlay();



        SizeLimitedVector<Tile> mUndoStack;
        SimpleVector<Tile> mRedoStack;
     

        TextFieldGL *mSetNameField;
        

        char mPenDown;
        char mSelectionInk;

    };

#endif
