#ifndef SONG_EDITOR_INCLUDED
#define SONG_EDITOR_INCLUDED


#include "Editor.h"
#include "buttons.h"
#include "Song.h"
#include "DrawToolSet.h"
#include "SizeLimitedVector.h"
#include "ToolTipSliderGL.h"


#include "minorGems/graphics/openGL/gui/TextFieldGL.h"


class SongEditor : public Editor {
        
    public:
        
        SongEditor( ScreenGL *inScreen );
        
        ~SongEditor();


        // may not be saved into picker yet
        Song getLiveSong();
        

        // tell editor to re-generate all timbres
        void scaleChanged();
        

        virtual void actionPerformed( GUIComponent *inTarget );
        

    protected:
        
        // implemented by all subclasses
        // called by parent class when editor is being closed
        virtual void editorClosing();

        // triggered by add button or close
        void addSong();
        
        void saveUndoPoint();
        


        AddButtonGL *mAddButton;
        char mAddAction;
        
        void setSongToEdit( Song inSong );
        
        void refreshMiniView();
        
        
        MusicCellButtonGL *mButtonGrid[SI][S];
        
        Sprite *mEmptyPhraseSprite;

        SpriteButtonGL *mTimbreButtons[SI];
        EditButtonGL *mEditTimbreButtons[SI];
        ToolTipSliderGL *mLoudnessSliders[SI];
        ToolTipSliderGL *mStereoSliders[SI];

        SpriteButtonGL *mMiniViewButton;
        
        Song mSongToEdit;
        

        EditButtonGL *mEditMusicButton;
        
        SpriteButtonGL *mEditScaleButton;
        

        SelectableButtonGL *mSpeedButtons[3];
        

        
        UndoButtonGL *mUndoButton;
        RedoButtonGL *mRedoButton;
        ClearButtonGL *mClearButton;
        

        DrawToolSet *mToolSet;

        SelectableButtonGL *mWallsButton;
        SelectableButtonGL *mEraseButton;

        // returns true if anything changed
        char recursiveFill( int inX, int inY, 
                            uniqueID inOldMusic, uniqueID inNewMusic,
                            drawTool inTool );


        // version for erasing
        char recursiveErase( int inX, int inY, 
                             uniqueID inOldMusic,
                             drawTool inTool );
        

        SizeLimitedVector<Song> mUndoStack;
        SimpleVector<Song> mRedoStack;
     

        TextFieldGL *mSetNameField;
        

        ToggleSpriteButtonGL *mAddToPackButton;
        SpriteButtonGL *mSavePackButton;


        char mPenDown;
        

        int mLastRowTouched;

        char mIgnoreSliders;
        
        char mFirstSongSet;
        
        // for usage tracking of phrases in current state
        uniqueID mCurrentWorkingPhrasesID;

        // for SEPARATE usage tracking of timbres in current state
        uniqueID mCurrentWorkingTimbresID;
    };

#endif
