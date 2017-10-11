#include "Editor.h"
#include "BorderPanel.h"
#include "buttons.h"
#include "ColorEditor.h"
#include "TileEditor.h"
#include "RoomEditor.h"
#include "SpriteEditor.h"
#include "StateObjectEditor.h"
#include "SongEditor.h"
#include "MusicEditor.h"
#include "TimbreEditor.h"
#include "ScaleEditor.h"
#include "PaletteEditor.h"
#include "PlayerMoveEditor.h"

#include "SpritePicker.h"

#include "ToolTipDisplay.h"
#include "TimerDisplay.h"
#include "WarningDisplay.h"


extern int gameWidth, gameHeight;

extern ColorEditor *mainColorEditor;
extern TileEditor *mainTileEditor;
extern RoomEditor *mainRoomEditor;
extern SpriteEditor *mainSpriteEditor;
extern StateObjectEditor *mainStateObjectEditor;
extern MusicEditor *mainMusicEditor;
extern TimbreEditor *mainTimbreEditor;
extern ScaleEditor *mainScaleEditor;
extern SongEditor *mainSongEditor;
extern PaletteEditor *mainPaletteEditor;
extern PlayerMoveEditor *mainPracticePlayerEditor;


extern SpritePicker *mainSpritePicker;
extern SpritePicker *mainSpritePickerLower;


extern ToolTipDisplay *mainTipDisplay;
extern TimerDisplay *mainTimerDisplay;
extern WarningDisplay *mainWarningDisplay;



Editor::Editor( ScreenGL *inScreen, char inHasMainArea, char inHasSidePanel )
        : mScreen( inScreen ),
          mVisible( false ), 
          mHasSidePanel( inHasSidePanel ),
          mHasMainArea( inHasMainArea ) {


    double mainPanelXOffset = 0;
    
    if( !mHasMainArea ) {
        mainPanelXOffset = gameHeight;
        }
    

    // panel covering whole screen
    mMainPanel = new PressActionGUIPanelGL( mainPanelXOffset, 0, 
                                            gameWidth - mainPanelXOffset, 
                                            gameWidth,
                                            new Color( 0, 0, 0, 1.0 ) );

    

    mMainPanelGuiTranslator = new GUITranslatorGL( mMainPanel, mScreen,
                                                   gameWidth );

    mSidePanel = NULL;
    
    if( mHasSidePanel ) {
        
        // panel covering right edge of screen, 
        // leaving a perfect square to left
        mSidePanel = new BorderPanel( gameHeight, 40, 
                                      gameWidth - gameHeight, gameHeight,
                                      new Color( 0, 0, 0, 1.0 ),
                                      new Color( .4, .4, .4, 1.0 ), 2 );



        
        }
    
    mCloseButton = NULL;
    
    if( ! mHasMainArea && mHasSidePanel ) {
        
        // don't attach key equivalent for button that won't close whole
        // window
        mCloseButton = new NoKeyCloseButtonGL( 242, 270, 8, 8 );
        mSidePanel->add( mCloseButton );
        }
    else {
        // whole window close... key equivalent
        mCloseButton = new CloseButtonGL( 0, 272, 8, 8 );
        // add to Main on top of all else
        }
    
    if( mCloseButton != NULL ) {
        mCloseButton->addActionListener( this );
        }

    
    }



void Editor::postConstruction() {
    // put side panel and main widgets on top of everything else
    
    postConstructionSide();
    postConstructionMain();
    }



void Editor::postConstructionSide() {
    
    
    if( mHasSidePanel ) {
        // on top of everything else
        mSidePanel->add( mainWarningDisplay );


        mMainPanel->add( mSidePanel );
        }
    }



void Editor::postConstructionMain() {
    if( ! mHasMainArea && mHasSidePanel ) {
        // side-panel only, nothing left to add
        }
    else {
        mMainPanel->add( mCloseButton );

        mMainPanel->add( mainTipDisplay );
        mMainPanel->add( mainTimerDisplay );
        }
    }

        
Editor::~Editor() {

    setVisible( false );
        
    if( mSidePanel != NULL ) {
        mSidePanel->remove( mainWarningDisplay );
        
        if( ! mMainPanel->contains( mSidePanel ) ) {
            delete mSidePanel;
            }
        }
    

    if( mMainPanel->contains( mainTipDisplay ) ) {
        // we don't destroy this
        mMainPanel->remove( mainTipDisplay );
        mMainPanel->remove( mainTimerDisplay );
        }
        
    

    // recursively deletes all sub-components
    delete mMainPanelGuiTranslator;
    }




void Editor::setVisible( char inIsVisible ) {
    
    if( inIsVisible && !mVisible) {
        
        mScreen->addSceneHandler( mMainPanelGuiTranslator );
        mScreen->addKeyboardHandler( mMainPanelGuiTranslator );
        mScreen->addMouseHandler( mMainPanelGuiTranslator );
        }
    if( !inIsVisible && mVisible) {
        mScreen->removeSceneHandler( mMainPanelGuiTranslator );
        mScreen->removeKeyboardHandler( mMainPanelGuiTranslator );
        mScreen->removeMouseHandler( mMainPanelGuiTranslator );
        }

    mVisible = inIsVisible;
    }



char Editor::isVisible() {
    return mVisible;
    }



void Editor::showColorEditor() {
    mainColorEditor->addActionListener( this );
    
    if( mSidePanel != NULL ) {
        mMainPanel->remove( mSidePanel );
        }
    
    mainColorEditor->setVisible( true );
    }




void Editor::showTileEditor() {
    setVisible( false );
    
    mainTileEditor->addActionListener( this );
    
    mainTileEditor->setVisible( true );
    }



void Editor::showRoomEditor() {
    setVisible( false );
    
    mainRoomEditor->addActionListener( this );
    
    mainRoomEditor->setVisible( true );
    }



void Editor::showSpriteEditor() {
    setVisible( false );
    
    // make sure pickers match (editor shows mainSpritePicker)
    mainSpritePickerLower->cloneState( mainSpritePicker );
    

    mainSpriteEditor->addActionListener( this );
    
    mainSpriteEditor->setVisible( true );
    }



void Editor::showStateObjectEditor() {
    setVisible( false );
    
    mainStateObjectEditor->addActionListener( this );
    
    mainStateObjectEditor->setVisible( true );
    }



void Editor::showMusicEditor() {
    setVisible( false );
    
    mainMusicEditor->addActionListener( this );
    
    mainMusicEditor->setVisible( true );
    }

void Editor::showTimbreEditor() {
    setVisible( false );
    
    mainTimbreEditor->addActionListener( this );
    
    mainTimbreEditor->setVisible( true );
    }

void Editor::showScaleEditor() {
    setVisible( false );
    
    mainScaleEditor->addActionListener( this );
    
    mainScaleEditor->setVisible( true );
    }


void Editor::showSongEditor() {
    setVisible( false );
    
    mainSongEditor->addActionListener( this );
    
    mainSongEditor->setVisible( true );
    }



void Editor::showPaletteEditor() {
    setVisible( false );
    
    mainPaletteEditor->addActionListener( this );
    
    mainPaletteEditor->setVisible( true );
    }



void Editor::showPracticePlayerEditor() {
    setVisible( false );

    // any sub-editor open?

    if( mainPaletteEditor->isVisible() ) {
        if( mainColorEditor->isVisible() ) {
            mainColorEditor->setVisible( false );
            mainColorEditor->removeActionListener( mainPaletteEditor );
            
            if( mainPaletteEditor->mSidePanel != NULL ) {
                mainPaletteEditor->mMainPanel->add( 
                    mainPaletteEditor->mSidePanel );
                }
            }
        mainPaletteEditor->setVisible( false );

        if( mainPaletteEditor->isListening( mainTileEditor ) ) {
            
            mainPaletteEditor->removeActionListener( mainTileEditor );
            mainTileEditor->setVisible( true );
            }
        else if( mainPaletteEditor->isListening( mainRoomEditor ) ) {
            
            mainPaletteEditor->removeActionListener( mainRoomEditor );
            mainRoomEditor->setVisible( true );
            }
        }
        

    if( mainTileEditor->isVisible() ) {
        
        if( mainColorEditor->isVisible() ) {
            mainColorEditor->setVisible( false );
            mainColorEditor->removeActionListener( mainTileEditor );
            
            if( mainTileEditor->mSidePanel != NULL ) {
                mainTileEditor->mMainPanel->add( mainTileEditor->mSidePanel );
                }
            }
        mainTileEditor->setVisible( false );
        mainTileEditor->removeActionListener( mainRoomEditor );
        mainRoomEditor->setVisible( true );
        }


    if( mainSpriteEditor->isVisible() ) {
        
        if( mainColorEditor->isVisible() ) {
            mainColorEditor->setVisible( false );
            mainColorEditor->removeActionListener( mainSpriteEditor );
            
            if( mainSpriteEditor->mSidePanel != NULL ) {
                mainSpriteEditor->mMainPanel->add( 
                    mainSpriteEditor->mSidePanel );
                }
            }
        mainSpriteEditor->setVisible( false );
        mainSpriteEditor->removeActionListener( mainStateObjectEditor );
        mainStateObjectEditor->setVisible( true );
        }
    
    if( mainTimbreEditor->isVisible() ) {
        
        mainTimbreEditor->setVisible( false );
        mainTimbreEditor->removeActionListener( mainSongEditor );
        mainSongEditor->setVisible( true );
        }

    if( mainScaleEditor->isVisible() ) {
        
        mainScaleEditor->setVisible( false );
        mainScaleEditor->removeActionListener( mainSongEditor );
        mainSongEditor->setVisible( true );
        }

    if( mainMusicEditor->isVisible() ) {
        mainMusicEditor->setVisible( false );
        mainMusicEditor->removeActionListener( mainSongEditor );
        mainSongEditor->setVisible( true );
        }



    if( mainRoomEditor->isVisible() ) {
        // special case for RoomEditor, so it can reset shared
        // GameStateDisplay
        mainRoomEditor->editorClosing();

        mainRoomEditor->setVisible( false );
        mainRoomEditor->removeActionListener( this );
        }


    if( mainStateObjectEditor->isVisible() ) {
        mainStateObjectEditor->setVisible( false );
        mainRoomEditor->removeActionListener( this );
        }

    if( mainSongEditor->isVisible() ) {
        mainSongEditor->setVisible( false );
        mainSongEditor->removeActionListener( this );
        }
        

    mainPracticePlayerEditor->setVisible( true );
    }



void Editor::hidePracticePlayerEditor() {
    mainPracticePlayerEditor->setVisible( false );
    
    setVisible( true );
    }



void Editor::redrawMainPanel() {
    mMainPanel->fireRedraw();
    }



void Editor::actionPerformed( GUIComponent *inTarget ) {
    if( mCloseButton != NULL && inTarget == mCloseButton ) {
        
        // tell subclass about it
        editorClosing();
        

        if( this != mainColorEditor &&
            mainColorEditor->isVisible() ) {
            // color editor on top of our side panel

            // force it to close and redisplay our side panel
            mainColorEditor->setVisible( false );
            mainColorEditor->removeActionListener( this );
            
            if( mSidePanel != NULL ) {
                mMainPanel->add( mSidePanel );
                }
            }

        // fire action telling others that we got a close-button press
        fireActionPerformed( this );
        }
    else if( inTarget == mainColorEditor ) {
        
        // color editor closed
        mainColorEditor->setVisible( false );
        mainColorEditor->removeActionListener( this );
        
        if( mSidePanel != NULL ) {
            mMainPanel->add( mSidePanel );
            }
        colorEditorClosed();
        }
    else if( inTarget == mainTileEditor ) {
        // tile editor closed
        
        mainTileEditor->setVisible( false );
        mainTileEditor->removeActionListener( this );
        
        setVisible( true );
        }
    else if( inTarget == mainRoomEditor ) {
        // room editor closed
        
        mainRoomEditor->setVisible( false );
        mainRoomEditor->removeActionListener( this );
        
        setVisible( true );
        }
    else if( inTarget == mainSpriteEditor ) {
        // sprite editor closed
        
        mainSpriteEditor->setVisible( false );
        mainSpriteEditor->removeActionListener( this );
        

        // make sure pickers match (editor showed mainSpritePicker, so copy
        // changes from there back into Lower picker)
        mainSpritePicker->cloneState( mainSpritePickerLower );


        setVisible( true );
        }
    else if( inTarget == mainStateObjectEditor ) {
        // object editor closed
        
        mainStateObjectEditor->setVisible( false );
        mainStateObjectEditor->removeActionListener( this );
        
        setVisible( true );
        }
    else if( inTarget == mainMusicEditor ) {
        // music editor closed
        
        mainMusicEditor->setVisible( false );
        mainMusicEditor->removeActionListener( this );
        
        setVisible( true );
        }
    else if( inTarget == mainTimbreEditor ) {
        // music editor closed
        
        mainTimbreEditor->setVisible( false );
        mainTimbreEditor->removeActionListener( this );
        
        setVisible( true );
        }
    else if( inTarget == mainScaleEditor ) {
        // music editor closed
        
        mainScaleEditor->setVisible( false );
        mainScaleEditor->removeActionListener( this );
        
        setVisible( true );
        }
    else if( inTarget == mainSongEditor ) {
        // song editor closed
        
        mainSongEditor->setVisible( false );
        mainSongEditor->removeActionListener( this );
        
        setVisible( true );
        }
    else if( inTarget == mainPaletteEditor ) {
        // music editor closed
        
        mainPaletteEditor->setVisible( false );
        mainPaletteEditor->removeActionListener( this );
        
        setVisible( true );
        }
    else if( inTarget == mainPracticePlayerEditor ) {
        // practice player editor closed
        
        mainPracticePlayerEditor->setVisible( false );
        mainPracticePlayerEditor->removeActionListener( this );
        
        setVisible( true );
        }
    
    }
