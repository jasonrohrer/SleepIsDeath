#ifndef MUSIC_EDITOR_INCLUDED
#define MUSIC_EDITOR_INCLUDED


#include "Editor.h"
#include "buttons.h"
#include "Music.h"
#include "TransformToolSet.h"
#include "SizeLimitedVector.h"

#include "NoteGridDisplay.h"


#include "minorGems/graphics/openGL/gui/TextFieldGL.h"


class MusicEditor : public Editor {
        
    public:
        
        MusicEditor( ScreenGL *inScreen );
        
        ~MusicEditor();

        virtual void actionPerformed( GUIComponent *inTarget );
        

    protected:

        // implemented by all subclasses
        // called by parent class when editor is being closed
        virtual void editorClosing();

        // triggered by add button or close
        void addMusic();
        

        NoteGridDisplay *mNoteDisplay;
        

        
        AddButtonGL *mAddButton;
        char mAddAction;
        
        void setMusicToEdit( Music inMusic );
        
        void refreshMiniView();
        

        SpriteButtonGL *mMiniViewButton;
        
        Music mMusicToEdit;
        
        
        UndoButtonGL *mUndoButton;
        RedoButtonGL *mRedoButton;
        
        
        TransformToolSet *mTransformToolSet;


        SizeLimitedVector<Music> mUndoStack;
        SimpleVector<Music> mRedoStack;
     

        TextFieldGL *mNameField;

    };

#endif
