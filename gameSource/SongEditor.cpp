#include "SongEditor.h"
#include "SongPicker.h"
#include "MusicPicker.h"
#include "TimbrePicker.h"
#include "TimbreEditor.h"
#include "ScalePicker.h"
#include "ScaleEditor.h"
#include "BorderPanel.h"
#include "labels.h"
#include "musicPlayer.h"
#include "packSaver.h"


#include "minorGems/util/log/AppLog.h"
#include "minorGems/util/TranslationManager.h"


#include <stdio.h>


// from musicPlayer.cpp
extern char noteToggles[PARTS][S][N][N];

extern int partLengths[PARTS];
extern double partLoudness[PARTS];
extern double partStereo[PARTS];


extern MusicPicker *mainMusicPicker;
extern SongPicker *mainSongPicker;
extern TimbrePicker *mainTimbrePicker;
extern TimbreEditor *mainTimbreEditor;

extern ScalePicker *mainScalePicker;
extern ScaleEditor *mainScaleEditor;


extern int gameWidth, gameHeight;


extern TextGL *largeTextFixed;





template <>
void SizeLimitedVector<Song>::deleteElementOfType(
    Song inElement ) {
    // no delete necessary
    }




SongEditor::SongEditor( ScreenGL *inScreen )
        : Editor( inScreen ),
          mUndoStack( MAX_UNDOS, false ),
          // gen \fake IDs
          mCurrentWorkingPhrasesID( makeUniqueID( 
                                        (unsigned char*)"currSongPhrases", 
                                        strlen( "currSongPhrases" ) ) ),
          mCurrentWorkingTimbresID( makeUniqueID( 
                                        (unsigned char*)"currSongTimbres", 
                                        strlen( "currSongTimbres" ) ) ) {
    
    AppLog::info( "Constructing song editor\n" );

    mCloseButton->setToolTip( "tip_closeEdit_song" );
    
    /*
    LabelGL *titleLabel = new LabelGL( 0.75, 0.5, 0.25,
                                       0.1, "Song Editor", largeText );
    

    
    mSidePanel->add( titleLabel );
    */

    mSidePanel->add( mainMusicPicker );

    mainMusicPicker->addActionListener( this );
    
    
    mSidePanel->add( mainSongPicker );

    mainSongPicker->addActionListener( this );
    

    mainTimbrePicker->addActionListener( this );
    
    mainScalePicker->addActionListener( this );
    

    mEditMusicButton = 
        new EditButtonGL( 
            mainMusicPicker->getAnchorX() - 1,
            mainMusicPicker->getAnchorY() + mainMusicPicker->getHeight() + 1,
            8,
            8 );
    
    mSidePanel->add( mEditMusicButton );
    
    mEditMusicButton->addActionListener( this );
    
    mEditMusicButton->setToolTip( "tip_edit_phrase" );
    
    /*
    // 1-pixel wide white border
    // inner panel to provide a 1-pixel wide black gap
    
    BorderPanel *workingColorBorderPanel = 
        new BorderPanel( 256, 206, 48, 14,
                         new Color( 0, 0, 0, 1 ),
                         new Color( 1, 1, 1, 1 ),
                         1 );
    

    // smaller to leave black gap
    GUIPanelGL *workingColorPanel = new GUIPanelGL( 258, 208, 44, 10,
                                                    mWorkingColor );
    
    mSidePanel->add( workingColorBorderPanel );
    workingColorBorderPanel->add( workingColorPanel );
    

    mAddButton = new AddButtonGL( 272, 184, 16, 16 );
    mSidePanel->add( mAddButton );
    mAddButton->addActionListener( this );

    */


    mEmptyPhraseSprite = new Sprite( "emptyPhrase.tga", true );


    double offset = P;

    double buttonSize = P;

    double timbreButtonSize = P + 4;

    // for volume sliders
    Color *thumbColor = new Color( .5, .5, .5, .5 );
    Color *borderColor = new Color( .35, .35, .35, .35 );

    for( int y=0; y<SI; y++ ) {
        for( int x=0; x<S; x++ ) {

            mButtonGrid[y][x] = new MusicCellButtonGL(
                NULL, 1.0,
                8 + 48 + x * buttonSize,
                gameWidth - ( 48 + offset + 8 + y * (buttonSize+8) ),
                buttonSize,
                buttonSize );
            
            mButtonGrid[y][x]->setBlankSprite( mEmptyPhraseSprite );

            mButtonGrid[y][x]->setMusicInfo( y, x );

            mMainPanel->add( mButtonGrid[y][x] );
            
            mButtonGrid[y][x]->addActionListener( this );
            }
        mTimbreButtons[y] = new SpriteButtonGL( 
            NULL, 1,
            mButtonGrid[y][0]->getAnchorX() - timbreButtonSize - 2,
            mButtonGrid[y][0]->getAnchorY() - 2,
            timbreButtonSize,
            timbreButtonSize );
    
        mMainPanel->add( mTimbreButtons[y] );

        
        mEditTimbreButtons[y] = 
            new EditButtonGL( 
                mTimbreButtons[y]->getAnchorX() - 8,
                mTimbreButtons[y]->getAnchorY() + 
                mTimbreButtons[y]->getHeight() - 8,
                8,
                8 );
    
        mMainPanel->add( mEditTimbreButtons[y] );
    
        mEditTimbreButtons[y]->addActionListener( this );
        
        mEditTimbreButtons[y]->setToolTip( "tip_edit_timbre" );

        MusicCellButtonGL *rowLastButton =
            mButtonGrid[y][S-1];
        
        
        mLoudnessSliders[y] =
            new ToolTipSliderGL( rowLastButton->getAnchorX() + 18, 
                                 rowLastButton->getAnchorY() + 8,
                                 14, 10,
                                 NULL, 0,
                                 new Color( 0, 0, 0, 1 ),
                                 new Color( 1, 1, 1, 1 ),
                                 thumbColor->copy(),
                                 borderColor->copy(),
                                 1, 4, 1 );
    
        mMainPanel->add( mLoudnessSliders[y] );
        mLoudnessSliders[y]->setThumbPosition( 1.0 );
        mLoudnessSliders[y]->addActionListener( this );
        mLoudnessSliders[y]->setToolTip( "tip_trackLoudness" );


        mStereoSliders[y] =
            new ToolTipSliderGL( rowLastButton->getAnchorX() + 18, 
                                 rowLastButton->getAnchorY() - 2,
                                 14, 10,
                                 NULL, 0,
                                 new Color( 0, 1, 0, 1 ),
                                 new Color( 1, 0, 0, 1 ),
                                 thumbColor->copy(),
                                 borderColor->copy(),
                                 1, 4, 1 );
    
        mMainPanel->add( mStereoSliders[y] );
        mStereoSliders[y]->setThumbPosition( 1.0 );
        mStereoSliders[y]->addActionListener( this );
        mStereoSliders[y]->setToolTip( "tip_trackStereo" );
        }    

    delete thumbColor;
    delete borderColor;
    
    
    

    // no stamp tool here
    mToolSet = new DrawToolSet( 2, 42, "tip_pickPhrase", false );
    mMainPanel->add( mToolSet );    


    
    
    

    EightPixelLabel *fieldLabel = new EightPixelLabel( 150, 54,
                                                       "songName" );
    mMainPanel->add( fieldLabel );



    int fieldHeight = 8;
    int fieldWidth = 8 * 10;
    
    const char *defaultText = "default";

    mSetNameField = new TextFieldGL( 150,
                                     43,
                                     fieldWidth,
                                     fieldHeight,
                                     1,
                                     defaultText,
                                     largeTextFixed,
                                     new Color( 0.75, 0.75, 0.75 ),
                                     new Color( 0.75, 0.75, 0.75 ),
                                     new Color( 0.15, 0.15, 0.15 ),
                                     new Color( 0.75, 0.75, 0.75 ),
                                     10,
                                     false );
    mMainPanel->add( mSetNameField );

    mSetNameField->setFocus( true );
    //mSetNameField->lockFocus( true );
    
    mSetNameField->setCursorPosition( strlen( defaultText ) );
    
    mSetNameField->addActionListener( this );

    




    // center add button
    double gridEdge = 8 + G * buttonSize;
    
    double extra = gameHeight - gridEdge;
    

    // center it vertically on song picker
    double addY = mainSongPicker->getAnchorY() +
        mainSongPicker->getHeight() - 15;
    
    double sideButtonsX = gridEdge + (extra - 16) / 2;

    mAddButton = new AddButtonGL( sideButtonsX, 
                                  addY, 
                                  16, 16 );
    mMainPanel->add( mAddButton );
    mAddButton->addActionListener( this );
    mAddButton->setToolTip( "tip_addSong" );
    
    mAddAction = false;
    

    double miniButtonSize = P + 4;
    
    mMiniViewButton = new SpriteButtonGL( 
        NULL, 1,
        gridEdge + ( extra - miniButtonSize ) / 2,
        gameWidth / 2 + 2,
        miniButtonSize,
        miniButtonSize );
    
    mMainPanel->add( mMiniViewButton );
    

    /*
    mWallsButton = new SelectableButtonGL( 
        new Sprite( "walls.tga", true ),
        1,
        sideButtonsX - 2, mMiniViewButton->getAnchorY() + 30, 
        20, 20 );
    
    mMainPanel->add( mWallsButton );
    
    mWallsButton->setSelected( false );
    
    mWallsButton->addActionListener( this );
    mWallsButton->setToolTip( "tip_walls" );
    */


    mEraseButton = new SelectableButtonGL( 
        new Sprite( "erase.tga", true ),
        1,
        sideButtonsX - 2, mMiniViewButton->getAnchorY() + 30, 
        //sideButtonsX - 2, undoButtonY + 20 + 19, 
        20, 20 );
    
    mMainPanel->add( mEraseButton );
    
    mEraseButton->setSelected( false );
    
    mEraseButton->addActionListener( this );

    mEraseButton->setToolTip( "tip_erase" );



    
    double undoButtonY = gameWidth - ( 48 + offset + (G - 1) * buttonSize );

    mUndoButton = new UndoButtonGL( sideButtonsX, undoButtonY, 16, 16 );
    mMainPanel->add( mUndoButton );
    mUndoButton->addActionListener( this );
    mUndoButton->setEnabled( false );

    mRedoButton = new RedoButtonGL( sideButtonsX, undoButtonY + 19, 16, 16 );
    mMainPanel->add( mRedoButton );
    mRedoButton->addActionListener( this );
    mRedoButton->setEnabled( false );


    mClearButton = new ClearButtonGL( sideButtonsX, addY - 20, 16, 16 );
    mMainPanel->add( mClearButton );
    mClearButton->addActionListener( this );


    
    // scale button in side panel
    double scaleButtonY =
        mainSongPicker->getAnchorY() + mainSongPicker->getHeight() +
        (int)( mainMusicPicker->getAnchorY() -
          ( mainSongPicker->getAnchorY() + mainSongPicker->getHeight() )
          - 16 ) / 2;

    mEditScaleButton = new SpriteButtonGL( 
        new Sprite( "editScale.tga", true ),
        1,
        mainSongPicker->getAnchorX() - 1, scaleButtonY, 16, 16 );
    
    mSidePanel->add( mEditScaleButton );
    
    mEditScaleButton->addActionListener( this );
    
    mEditScaleButton->setToolTip( "tip_edit_scale" );




    mSpeedButtons[0] = new SelectableButtonGL( 
        new Sprite( "slow.tga", true ),
        1,
        3, gameWidth / 2 - 18, 20, 20 );
    mSpeedButtons[0]->setToolTip( "tip_slowSpeed" );


    mSpeedButtons[1] = new SelectableButtonGL( 
        new Sprite( "normalSpeed.tga", true ),
        1,
        3, gameWidth / 2 - 18 + 20, 20, 20 );
    mSpeedButtons[1]->setToolTip( "tip_normalSpeed" );


    mSpeedButtons[2] = new SelectableButtonGL( 
        new Sprite( "fast.tga", true ),
        1,
        3, gameWidth / 2 - 18 + 40, 20, 20 );
    mSpeedButtons[2]->setToolTip( "tip_fastSpeed" );
    
    

    for( int i=0; i<3; i++ ) {
        mMainPanel->add( mSpeedButtons[i] );
        mSpeedButtons[i]->addActionListener( this );
        }
    



    mAddToPackButton = new ToggleSpriteButtonGL( 
        new Sprite( "pack.tga", true ),
        new Sprite( "packAlreadyIn.tga", true ),
        1,
        mainSongPicker->getAnchorX() + mainSongPicker->getWidth() - 22,
        mainSongPicker->getAnchorY() + 
        mainSongPicker->getHeight() + 1,
        8,
        8 );
    
    mSidePanel->add( mAddToPackButton );
    
    mAddToPackButton->addActionListener( this );
    
    mAddToPackButton->setToolTip( "tip_addSongToPack" );
    mAddToPackButton->setSecondToolTip( "tip_songAlreadyInPack" );


    mSavePackButton = new SpriteButtonGL( 
        new Sprite( "packSave.tga", true ),
        1,
        mainSongPicker->getAnchorX() + mainSongPicker->getWidth() - 7,
        mainSongPicker->getAnchorY() + 
        mainSongPicker->getHeight() + 1,
        8,
        8 );
    
    mSidePanel->add( mSavePackButton );
    
    mSavePackButton->addActionListener( this );
    
    mSavePackButton->setToolTip( "tip_savePack" );






    // make sure all timbres get set to default
    mFirstSongSet = false;
    setSongToEdit( mainSongPicker->getSelectedResource() );
    

    mPenDown = false;
    mLastRowTouched = 0;
    mIgnoreSliders = false;
    
    postConstruction();
    }



SongEditor::~SongEditor() {
    mSidePanel->remove( mainMusicPicker );
    mSidePanel->remove( mainSongPicker );

    delete mEmptyPhraseSprite;
    }


Song SongEditor::getLiveSong() {
    return mSongToEdit;
    }



void SongEditor::setSongToEdit( Song inSong ) {
    
    // clear old working usages
    removeUsages( mCurrentWorkingPhrasesID );
    removeUsages( mCurrentWorkingTimbresID );


    // force update of everything only the first time
    char forceUpdate = false;
    if( !mFirstSongSet ) {
        forceUpdate = true;
        mFirstSongSet = true;
        }

    Song::setInPlayer( mSongToEdit, inSong, forceUpdate );
    


    // timbre updates are slow... only do the ones we need
    uniqueID oldTimbres[SI];
    for( int y=0; y<SI; y++ ) {
        oldTimbres[y] = mSongToEdit.getTimbre( y );
        }
    
    // update ALL on a scale change or speed change
    uniqueID oldScale = mSongToEdit.getScale();
    uniqueID newScale = inSong.getScale();
    
    char scaleChange = ( !equal( oldScale, newScale ) );
    
    if( scaleChange ) {
        Scale s( newScale );
    
        // no event fired
        mainScalePicker->setSelectedResource( s,
                                              true, false );
        
        char *name = s.getName();
        char *tip = autoSprintf( 
            "%s (%s)",
            TranslationManager::translate( "tip_edit_scale" ),
            name );
        mEditScaleButton->setToolTip( tip );
        delete [] tip;
        delete [] name;
        }


    mSongToEdit = inSong;
    
    for( int y=0; y<SI; y++ ) {
        
        uniqueID timbreID = mSongToEdit.getTimbre( y );
        TimbreResource t( timbreID );
        Color buttonColor = toColor( t.getTimbreColor() );
        
        // usage
        addUsage( mCurrentWorkingTimbresID, timbreID );

        
        for( int x=0; x<S; x++ ) {
            uniqueID phraseID = mSongToEdit.getPhrase( x, y );
            Music m( phraseID );
            
            if( mSongToEdit.getRowLength( y ) > x ) {
                mButtonGrid[y][x]->setSprite( m.getSprite() );
                mButtonGrid[y][x]->setColor( buttonColor.copy() );
                char *name = m.getName();
                char *tip = autoSprintf( 
                    "%s (%s)",
                    TranslationManager::translate( "tip_trackPhrase" ),
                    name );
        
                mButtonGrid[y][x]->setToolTip( tip );
                
                delete [] tip;
                delete [] name;

                // add working usage
                addUsage( mCurrentWorkingPhrasesID, phraseID );
                }
            else {
                mButtonGrid[y][x]->setSprite( NULL );
                mButtonGrid[y][x]->setToolTip( "tip_trackPhraseEmpty" );
                }
            }
        
        mTimbreButtons[y]->setSprite( t.getSprite() );
        
        char *name = t.getName();
        char *tip = autoSprintf( 
            "%s (%s)",
            TranslationManager::translate( "tip_trackTimbre" ),
            name );
        
        mTimbreButtons[y]->setToolTip( tip );

        delete [] tip;
        delete [] name;

        
        double loudness = mSongToEdit.getRowLoudness( y ) / 255.0;
        double stereo = mSongToEdit.getRowStereo( y ) / 255.0;
        
        mIgnoreSliders = true;
        // only 28 pixels in slider... avoid round-off errors
        mLoudnessSliders[y]->setThumbPosition( 
            (int)( loudness * 28 ) / 28.0 );
        mStereoSliders[y]->setThumbPosition( 
            (int)( stereo * 28 ) / 28.0 );
        mIgnoreSliders = false;
        }

    // usages changed (due to current working usages)
    mainMusicPicker->recheckDeletable();
    mainTimbrePicker->recheckDeletable();

    
    for( int i=0; i<3; i++ ) {
        mSpeedButtons[i]->setSelected( mSongToEdit.getSpeed() == i );
        }

    refreshMiniView();




    char *name = mSongToEdit.getSongName();
    
    mSetNameField->setText( name );
    mSetNameField->setCursorPosition( strlen( name ) );
    
    delete [] name;
    }



void SongEditor::refreshMiniView() {
    // don't use cached version
    mMiniViewButton->setSprite( mSongToEdit.getSprite( false, false ) );
    }



void SongEditor::scaleChanged() {
    for( int y=0; y<SI; y++ ) {
        TimbreResource t( mSongToEdit.getTimbre( y ) );
        
        // force a timbre regen, keep envelopes
        t.setInPlayer( y, true, false );
        }
    }




char SongEditor::recursiveFill( int inX, int inY, 
                                uniqueID inOldMusic, uniqueID inNewMusic,
                                drawTool inTool ) {

    if( equal( mSongToEdit.getPhrase( inX, inY ), inOldMusic )
        &&
        !equal( mSongToEdit.getPhrase( inX, inY ), inNewMusic ) ) {

        Music m( inNewMusic );
        

        mButtonGrid[inY][inX]->setSprite( m.getSprite() );
        mSongToEdit.editSongPhrase( inX, inY, inNewMusic );
        
        // call on neighbors
        if( inTool == fill || inTool == horLine ) {    
            if( inX > 0 ) {
                recursiveFill( inX - 1, inY, inOldMusic, inNewMusic, inTool );
                }
            if( inX < S - 1 ) {
                recursiveFill( inX + 1, inY, inOldMusic, inNewMusic, inTool );
                }
            }

        if( inTool == fill || inTool == verLine ) {
            if( inY > 0 ) {
                recursiveFill( inX, inY - 1, inOldMusic, inNewMusic, inTool );
                }
            if( inY < SI - 1 ) {
                recursiveFill( inX, inY + 1, inOldMusic, inNewMusic, inTool );
                }
            }

        return true;
        }

    return false;
    }





char SongEditor::recursiveErase( int inX, int inY,
                                 uniqueID inOldID,
                                 drawTool inTool ) {
    
    if( mButtonGrid[inY][inX]->getSprite() != NULL &&
        equal( inOldID, mSongToEdit.getPhrase( inX, inY ) ) ) {

        mButtonGrid[inY][inX]->setSprite( NULL );
        
        // call on neighbors
        if( inTool == fill || inTool == horLine ) {    
            if( inX > 0 ) {
                recursiveErase( inX - 1, inY, inOldID, inTool );
                }
            if( inX < S - 1 ) {
                recursiveErase( inX + 1, inY, inOldID, inTool );
                }
            }

        if( inTool == fill || inTool == verLine ) {
            if( inY > 0 ) {
                recursiveErase( inX, inY - 1, inOldID, inTool );
                }
            if( inY < SI - 1 ) {
                recursiveErase( inX, inY + 1, inOldID, inTool );
                }
            }

        return true;
        }
    
    return false;
    }


void SongEditor::saveUndoPoint() {
    mUndoStack.push_back( mSongToEdit );
    mUndoButton->setEnabled( true );
    
    mRedoStack.deleteAll();
    mRedoButton->setEnabled( false );
    }



void SongEditor::actionPerformed( GUIComponent *inTarget ) {
    // superclass
    Editor::actionPerformed( inTarget );
    

    for( int y=0; y<SI; y++ ) {
        if( inTarget == mEditTimbreButtons[y] ) {
            mLastRowTouched = y;
            mainScaleEditor->setPartToWatch( y );
            
            mainTimbreEditor->setPlayerTimbreToEdit( y );

            mainTimbrePicker->setSelectedResource( 
                mSongToEdit.getTimbre( y ) );
            
            // working usages for all EXCEPT last row touched
            // this allows us to delete a timbre, which if we're editing
            // it, is ALWAYS present in the working song (at least in
            // the last row touched).  We should be allowed to delete it
            // in that case, but not if it is also used in other working
            // rows.
            removeUsages( mCurrentWorkingTimbresID );
            
            for( int y=0; y<SI; y++ ) {
                if( y != mLastRowTouched ) {
                    
                    addUsage( mCurrentWorkingTimbresID, 
                              mSongToEdit.getTimbre( y ) );
                    }
                }
            mainTimbrePicker->recheckDeletable();


            showTimbreEditor();
            return;
            }
        
        if( !mIgnoreSliders && inTarget == mLoudnessSliders[y] ) {
            if( mLoudnessSliders[y]->mJustPressed ) {
                // first move in this adjustment, save an undo point here
                saveUndoPoint();
                }

            mLastRowTouched = y;
            mainScaleEditor->setPartToWatch( y );

            double loudness = mLoudnessSliders[y]->getThumbPosition();
            partLoudness[y] = loudness;
            mSongToEdit.editRowLoudness( y, (int)( loudness * 255 ) );
            return;
            }

        if( !mIgnoreSliders && inTarget == mStereoSliders[y] ) {
            if( mStereoSliders[y]->mJustPressed ) {
                // first move in this adjustment, save an undo point here
                saveUndoPoint();
                }

            mLastRowTouched = y;
            mainScaleEditor->setPartToWatch( y );

            double stereo = mStereoSliders[y]->getThumbPosition();
            partStereo[y] = stereo;
            mSongToEdit.editRowStereo( y, (int)( stereo * 255 ) );
            return;
            }
        }
    
    for( int i=0; i<3; i++ ) {
        if( inTarget == mSpeedButtons[i] ) {
            
            if( ! mSpeedButtons[i]->getSelected() ) {
                
                mSpeedButtons[i]->setSelected( true );
            
                mSpeedButtons[ (i+1) % 3 ]->setSelected( false );
                mSpeedButtons[ (i+2) % 3 ]->setSelected( false );
            

                saveUndoPoint();
                            
                mSongToEdit.editSpeed( i );
            
                // set in player
                setSpeed( i );
            
                for( int y=0; y<SI; y++ ) {
                    TimbreResource t( mSongToEdit.getTimbre( y ) );
                
                    // force a timbre and env regen
                    t.setInPlayer( y, false, true );
                    }
                }
            
            return;
            }
        }
    
            


    if( inTarget == mainMusicPicker ) {
        if( ! mainMusicPicker->wasLastActionFromPress() ) {
            // new music phrase picked
            // erase mode off
            mEraseButton->setSelected( false );
            }
        
        }
    else if( inTarget == mainTimbrePicker ) {
        if( ! mainTimbrePicker->wasLastActionFromPress() ) {
            saveUndoPoint();
                        
            mSongToEdit.editTimbre( 
                mLastRowTouched, 
                mainTimbrePicker->getSelectedResourceID() );
        
            // working usages for all EXCEPT last row touched
            // this allows us to delete a timbre, which if we're editing
            // it, is ALWAYS present in the working song (at least in
            // the last row touched).  We should be allowed to delete it
            // in that case, but not if it is also used in other working
            // rows.
            removeUsages( mCurrentWorkingTimbresID );
            
            for( int y=0; y<SI; y++ ) {
                if( y != mLastRowTouched ) {
                    
                    addUsage( mCurrentWorkingTimbresID, 
                              mSongToEdit.getTimbre( y ) );
                    }
                }
            mainTimbrePicker->recheckDeletable();
            

            // just update that one timbre view (since music player already
            // updated in realtime by timbre editor)
            TimbreResource t( mSongToEdit.getTimbre( mLastRowTouched ) );
            mTimbreButtons[mLastRowTouched]->setSprite( t.getSprite() );
            
            char *name = t.getName();
            char *tip = autoSprintf( 
                "%s (%s)",
                TranslationManager::translate( "tip_trackTimbre" ),
                name );
            mTimbreButtons[mLastRowTouched]->setToolTip( tip );
            delete [] tip;
            delete [] name;
            

            Color buttonColor = toColor( t.getTimbreColor() );
            

            int numActive = mSongToEdit.getRowLength( mLastRowTouched );
            
            for( int i=0; i<numActive; i++ ) {
                mButtonGrid[mLastRowTouched][i]->setColor( 
                    buttonColor.copy() );
                }
            
            //setSongToEdit( mSongToEdit );
            }
        }
    else if( inTarget == mainScalePicker ) {
        if( ! mainTimbrePicker->wasLastActionFromPress() ) {
            //printf( "Scale picker action\n" );
            
            saveUndoPoint();
            
            uniqueID oldScale = mSongToEdit.getScale();
            uniqueID newScale = mainScalePicker->getSelectedResourceID();
            
            if( !equal( oldScale, newScale ) ) {
                mSongToEdit.editScale( newScale );

                Scale s( newScale );

                char *name = s.getName();
                char *tip = autoSprintf( 
                    "%s (%s)",
                    TranslationManager::translate( "tip_edit_scale" ),
                    name );
                mEditScaleButton->setToolTip( tip );
                delete [] tip;
                delete [] name;

                scaleChanged();
                }
            }
        }
    else if( inTarget == mainSongPicker ) {
        // ignore if caused by our own Add action
        if( ! mAddAction &&
            ! mainSongPicker->wasLastActionFromPress() ) {
            
            // will change song
            
            saveUndoPoint();
            
            setSongToEdit( mainSongPicker->getSelectedResource() );
            
            mainMusicPicker->recheckDeletable();
            mainScalePicker->recheckDeletable();
            mainTimbrePicker->recheckDeletable();
            }

        mAddToPackButton->setState(
            alreadyInPack( mainSongPicker->getSelectedResourceID() ) );
        }
    else if( inTarget == mSetNameField ) {        
        saveUndoPoint();
        
        mSongToEdit.editSongName( mSetNameField->getText() );
        }
    else if( inTarget == mAddButton ) {
        addSong();
        }
    else if( inTarget == mEditMusicButton ) {
        // enable music editor extra part
        partLengths[SI] = 1;

        // turn on last timbre for music editor
        TimbreResource t( mSongToEdit.getTimbre( mLastRowTouched ) );
        t.setInPlayer( SI, true, true );
        
        showMusicEditor();
        }
    else if( inTarget == mEditScaleButton ) {        
        showScaleEditor();
        }
    else if( inTarget == mUndoButton ) {
        int lastIndex = mUndoStack.size() - 1;
        
        Song last = *( mUndoStack.getElement( lastIndex ) );
        mUndoStack.deleteElement( lastIndex );
        if( mUndoStack.size() == 0 ) {
            mUndoButton->setEnabled( false );
            }
        
        mRedoStack.push_back( mSongToEdit );
        mRedoButton->setEnabled( true );
        
        setSongToEdit( last );
        }
    else if( inTarget == mRedoButton ) {
        int nextIndex = mRedoStack.size() - 1;
        
        Song next = *( mRedoStack.getElement( nextIndex ) );
        mRedoStack.deleteElement( nextIndex );
        if( mRedoStack.size() == 0 ) {
            mRedoButton->setEnabled( false );
            }
        
        mUndoStack.push_back( mSongToEdit );
        mUndoButton->setEnabled( true );
        
        setSongToEdit( next );
        }
    /*
    else if( inTarget == mWallsButton ) {
        // toggle
        mWallsButton->setSelected( ! mWallsButton->getSelected() );
        
        char showWalls = mWallsButton->getSelected();

        for( int y=0; y<SI; y++ ) {
            for( int x=0; x<S; x++ ) {
                
                mButtonGrid[y][x]->setHighlight(
                    mSongToEdit.getWall( x, y ) && showWalls );
                
                }
            }
        
        
        }
    */
    else if( inTarget == mEraseButton ) {
        // toggle
        mEraseButton->setSelected( ! mEraseButton->getSelected() );
        }
    else if( inTarget == mClearButton ) {
        saveUndoPoint();
        
        setSongToEdit( Song::getDefaultResource() );
        }
    else if( inTarget == mAddToPackButton ) {
        AppLog::info( "Adding song to the current resource pack" );
        mainSongPicker->getSelectedResource().saveToPack();

        mAddToPackButton->setState( true );
        }
    else if( inTarget == mSavePackButton ) {
        AppLog::info( "Saving the current resource pack" );
        savePack();

        mAddToPackButton->setState( false );
        }
    else {
        // check grid 
        
        char found = false;
        
        for( int y=0; y<SI && !found; y++ ) {
            for( int x=0; x<S && !found; x++ ) {
                // dragging to draw doesn't work well with the
                // stack-to-left trick that we're using
                // only pay attention to initial presses

                // only heed first press (we've turned off dragging events)
                if( inTarget == mButtonGrid[y][x] && 
                    mButtonGrid[y][x]->isPressed() ) {


                    found = true;
                    mLastRowTouched = y;
                    mainScaleEditor->setPartToWatch( y );
                    
                    Song oldSongState = mSongToEdit;
                    char changed = false;

                    switch( mToolSet->getSelected() ) {
                        case pen: {
                            if( mEraseButton->getSelected() ) {
                                // erase with pen
                                
                                if( mButtonGrid[y][x]->getSprite() != NULL ) {
                                    
                                    mButtonGrid[y][x]->setSprite( NULL );
                                    changed = true;
                                    }
                                }
                            else {
                                // place phrase
                                Music m = 
                                    mainMusicPicker->getSelectedResource();
                                
                                uniqueID id = m.getUniqueID();
                                

                                if( mButtonGrid[y][x]->getSprite() == NULL
                                    ||
                                    ! equal( 
                                        id, 
                                        mSongToEdit.getPhrase( x, y ) ) ) {
                                    mButtonGrid[y][x]->setSprite( 
                                        m.getSprite() );
                                    mSongToEdit.editSongPhrase( x, y, id );
                                    refreshMiniView();
                                    
                                    
                                    changed = true;
                                    }
                                }
                            }
                            break;
                        case horLine:
                        case verLine:
                        case fill:
                            if( ! mEraseButton->getSelected() ) {
                                changed = recursiveFill( 
                                    x, y, 
                                    mSongToEdit.getPhrase( x, y ),
                                    mainMusicPicker->getSelectedResourceID(),
                                    mToolSet->getSelected() );
                                refreshMiniView();
                                }
                            else {
                                
                                changed = recursiveErase( 
                                    x, y, 
                                    mSongToEdit.getPhrase( x, y ),
                                    mToolSet->getSelected() );

                                refreshMiniView();
                                }
                            break;
                        case pickColor: {
                            Music m( mSongToEdit.getPhrase( x, y ) );
                            
                            mainMusicPicker->setSelectedResource( m );
                            if( mSongToEdit.getRowLength( y ) <= x ) {
                                // picked an empty spot
                                // erase mode on.
                                mEraseButton->setSelected( true );
                                }
                            else {
                                // picked a full spot
                                // erase mode off
                                mEraseButton->setSelected( false );
                                }
                            }
                            break;
                        case stamp:
                            AppLog::error( 
                                "Error: unsupported stamp tool used in"
                                "SongEditor\n" );
                            break;
                        }
                    

                    
                    if( changed ) {
                        // we saved old song state from before, in
                        // case of change.
                        mUndoStack.push_back( oldSongState );
                        mUndoButton->setEnabled( true );
                        
                        mRedoStack.deleteAll();
                        mRedoButton->setEnabled( false );
                        }



                    // now, after whatever operation we performed above
                    // "smash" all active cells over to the left
                    
                    // update usages in process
                    removeUsages( mCurrentWorkingPhrasesID );

                    uniqueID defaultPhraseID = 
                        Music::sBlankMusic->getUniqueID();
                    
                    for( int cy=0; cy<SI; cy++ ) {
                        
                        SimpleVector<uniqueID> activeCells;

                        for( int cx=0; cx<S; cx++ ) {
                            
                            if( mButtonGrid[cy][cx]->getSprite() != NULL ) {
                                activeCells.push_back( 
                                    mSongToEdit.getPhrase( cx, cy ) );
                                }

                            // clear here, set again below
                            mButtonGrid[cy][cx]->setSprite( NULL );
                            mSongToEdit.editSongPhrase( cx, cy, 
                                                        defaultPhraseID );
                            }

                        int numActive = activeCells.size();
                        mSongToEdit.editRowLength( cy, numActive );

                        TimbreResource timbre = 
                            TimbreResource( mSongToEdit.getTimbre( cy ) );
                        Color buttonColor = 
                            toColor( timbre.getTimbreColor() );
                        
                                           

                        // propagate to music player
                        partLengths[cy] = numActive;
                        
                        for( int i=0; i<numActive; i++ ) {
                            uniqueID id = *( activeCells.getElement( i ) );
                            mSongToEdit.editSongPhrase( 
                                i, cy, id );
                            
                            // working usage
                            addUsage( mCurrentWorkingPhrasesID, id );

                            Music m( id );
                            mButtonGrid[cy][i]->setSprite( m.getSprite() );
                            mButtonGrid[cy][i]->setColor( 
                                buttonColor.copy() );
                            
                            char *name = m.getName();
                            char *tip = autoSprintf( 
                                "%s (%s)",
                                TranslationManager::translate( 
                                    "tip_trackPhrase" ),
                                name );
                            
                            mButtonGrid[cy][i]->setToolTip( tip );
                            
                            delete [] tip;
                            delete [] name;


                            for( int py=0; py<N; py++ ) {
                                for( int px=0; px<N; px++ ) {
                                
                                    noteToggles[cy][i][py][px] = 
                                        m.getNoteOn( px, py );
                                    }
                                }
                            }
                        for( int i=numActive; i<S; i++ ) {
                            // rest, inactive
                            mButtonGrid[cy][i]->setToolTip( 
                                "tip_trackPhraseEmpty" );

                            for( int py=0; py<N; py++ ) {
                                for( int px=0; px<N; px++ ) {
                                    noteToggles[cy][i][py][px] = false;
                                    }
                                }
                            }
                        
                        }
                    
                    // only changes usage for phrase picker
                    mainMusicPicker->recheckDeletable();
                            
                    
                    }
                }
            }
        }
    
    
    }




void SongEditor::editorClosing() {
    addSong();
    }



void SongEditor::addSong() {
    mAddAction = true;
    mSongToEdit.finishEdit();
    mainSongPicker->setSelectedResource( mSongToEdit, true );

    mainMusicPicker->recheckDeletable();
    mainScalePicker->recheckDeletable();
    mainTimbrePicker->recheckDeletable();

    mAddAction = false;
    }


