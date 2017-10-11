#include "PaletteEditor.h"
#include "PalettePicker.h"
#include "ColorWells.h"
#include "ColorEditor.h"
#include "BorderPanel.h"
#include "labels.h"
#include "SelectionManager.h"
#include "packSaver.h"



extern ColorWells *mainColorStack;
extern PalettePicker *mainPalettePicker;

extern ColorEditor *mainColorEditor;


extern int gameWidth, gameHeight;


extern TextGL *largeTextFixed;



template <>
void SizeLimitedVector<Palette>::deleteElementOfType(
    Palette inElement ) {
    // no delete necessary
    }



PaletteEditor::PaletteEditor( ScreenGL *inScreen )
        : Editor( inScreen ),
          mUndoStack( MAX_UNDOS, false ) {

    mCloseButton->setToolTip( "tip_closeEdit_palette" );

    mSidePanel->add( mainColorStack );

    mainColorStack->addActionListener( this );
    
    
    mSidePanel->add( mainPalettePicker );

    mainPalettePicker->addActionListener( this );
    

    mEditColorButton = 
        new EditButtonGL( 
            mainColorStack->getAnchorX() - 9,
            mainColorStack->getAnchorY() + mainColorStack->getHeight() - 7,
            8,
            8 );
    
    mSidePanel->add( mEditColorButton );
    
    mEditColorButton->addActionListener( this );
    mEditColorButton->setToolTip( "tip_edit_color" );


    double offset = P;

    double buttonSize = (gameHeight - 2 * offset - 8) / P;
    
    
    rgbaColor c =  { { 0,0,0,255 } };

    Color unselectedBorder( 0.35, 0.35, 0.35, 1 );
    
    double paletteButtonSize = 16;
    double paletteButtonSpace = 24;
            
    for( int y=0; y<8; y++ ) {
        for( int x=0; x<5; x++ ) {

            mButtonGrid[y][x] = new SpriteButtonGL(
                NULL,
                1,
                51 + x * paletteButtonSpace,
                gameWidth - ( 48 + (y + 1) * paletteButtonSpace ),
                paletteButtonSize,
                paletteButtonSize );
            
            mButtonGrid[y][x]->setBorderColor( c );

            GUIPanelGL *buttonBorderPanel = 
                new GUIPanelGL( mButtonGrid[y][x]->getAnchorX() - 1,
                                mButtonGrid[y][x]->getAnchorY() - 1,
                                mButtonGrid[y][x]->getWidth() + 2,
                                mButtonGrid[y][x]->getHeight() + 2,
                                unselectedBorder.copy() );
            buttonBorderPanel->add( mButtonGrid[y][x] );


            mMainPanel->add( buttonBorderPanel );
            
            mButtonGrid[y][x]->addActionListener( this );
            }
        } 





    EightPixelLabel *fieldLabel = new EightPixelLabel( 150, 54,
                                                       "paletteName" );
    mMainPanel->add( fieldLabel );



    int fieldHeight = 8;
    int fieldWidth = 8 * 10;
    
    const char *defaultText = "default";

    mNameField = new TextFieldGL( 150,
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
    mMainPanel->add( mNameField );

    mNameField->setFocus( true );
    //mNameField->lockFocus( true );
    
    mNameField->setCursorPosition( strlen( defaultText ) );
    
    mNameField->addActionListener( this );

    




    // center add button
    double gridEdge = 8 + P * buttonSize;
    
    double extra = gameHeight - gridEdge;
    

    // center it vertically on palette picker
    double addY = mainPalettePicker->getAnchorY() +
        mainPalettePicker->getHeight() - 15;
    
    double sideButtonsX = gridEdge + (extra - 16) / 2;

    mAddButton = new AddButtonGL( sideButtonsX, 
                                  addY, 
                                  16, 16 );
    mMainPanel->add( mAddButton );
    mAddButton->addActionListener( this );
    mAddButton->setToolTip( "tip_addPalette" );
    
    mAddAction = false;
    

    double miniButtonSize = P + 4;
    
    mMiniViewButton = new SpriteButtonGL( 
        NULL, 1,
        gridEdge + ( extra - miniButtonSize ) / 2,
        addY - 24,
        miniButtonSize,
        miniButtonSize );
    
    mMainPanel->add( mMiniViewButton );    


    
    double undoButtonY = gameWidth - ( 48 + P * buttonSize );

    mUndoButton = new UndoButtonGL( sideButtonsX, undoButtonY, 16, 16 );
    mMainPanel->add( mUndoButton );
    mUndoButton->addActionListener( this );
    mUndoButton->setEnabled( false );

    mRedoButton = new RedoButtonGL( sideButtonsX, undoButtonY + 19, 16, 16 );
    mMainPanel->add( mRedoButton );
    mRedoButton->addActionListener( this );
    mRedoButton->setEnabled( false );



    mAddToPackButton = new ToggleSpriteButtonGL( 
        new Sprite( "pack.tga", true ),
        new Sprite( "packAlreadyIn.tga", true ),
        1,
        mainPalettePicker->getAnchorX() + mainPalettePicker->getWidth() - 22,
        mainPalettePicker->getAnchorY() + 
        mainPalettePicker->getHeight() + 1,
        8,
        8 );
    
    mSidePanel->add( mAddToPackButton );
    
    mAddToPackButton->addActionListener( this );
    
    mAddToPackButton->setToolTip( "tip_addPaletteToPack" );
    mAddToPackButton->setSecondToolTip( "tip_paletteAlreadyInPack" );


    mSavePackButton = new SpriteButtonGL( 
        new Sprite( "packSave.tga", true ),
        1,
        mainPalettePicker->getAnchorX() + mainPalettePicker->getWidth() - 7,
        mainPalettePicker->getAnchorY() + 
        mainPalettePicker->getHeight() + 1,
        8,
        8 );
    
    mSidePanel->add( mSavePackButton );
    
    mSavePackButton->addActionListener( this );
    
    mSavePackButton->setToolTip( "tip_savePack" );





    setPaletteToEdit( mainColorStack->getPalette() );
    

    
    postConstruction();
    }



PaletteEditor::~PaletteEditor() {
    mSidePanel->remove( mainColorStack );
    mSidePanel->remove( mainPalettePicker );
    
    }



void PaletteEditor::grabPaletteFromWells() {

    Palette p = mainColorStack->getPalette();

    // preserve name as wells change
    p.editPaletteName( mNameField->getText() );

    setPaletteToEdit( p );
    }

    
    


void PaletteEditor::setPaletteToEdit( Palette inPalette ) {
    mPaletteToEdit = inPalette;
    
    for( int y=0; y<8; y++ ) {
        for( int x=0; x<5; x++ ) {

            mButtonGrid[y][x]->setFillColor( 
                mPaletteToEdit.getColor( (8 - y - 1) * 5 + x ) );
            }
        }
    refreshMiniView();

    char *name = mPaletteToEdit.getPaletteName();
    
    mNameField->setText( name );
    mNameField->setCursorPosition( strlen( name ) );
    
    delete [] name;

    mainColorStack->setPalette( mPaletteToEdit );
    }



void PaletteEditor::refreshMiniView() {
    // don't use cached version
    mMiniViewButton->setSprite( mPaletteToEdit.getSprite( false, false ) );
    }



void PaletteEditor::saveUndoPoint() {
    mUndoStack.push_back( mPaletteToEdit );
    mUndoButton->setEnabled( true );
    
    mRedoStack.deleteAll();
    mRedoButton->setEnabled( false );
    }


void PaletteEditor::actionPerformed( GUIComponent *inTarget ) {
    // superclass
    Editor::actionPerformed( inTarget );
    

    if( inTarget == mainColorStack ) {
        if( mainColorStack->mLastActionWellChange ) {
            // palette has changed
            saveUndoPoint();
            
            grabPaletteFromWells();
            }
        }
    else if( inTarget == mainPalettePicker ) {
        if( ! mAddAction &&
            ! mainPalettePicker->wasLastActionFromPress() ) {
            // will change palette
                        
            saveUndoPoint();
            
            Palette p = mainPalettePicker->getSelectedResource();
            setPaletteToEdit( p );

            mAddToPackButton->setState(
                alreadyInPack( mainPalettePicker->getSelectedResourceID() ) );
            }
        }
    else if( inTarget == mNameField ) {
        saveUndoPoint();
                
        mPaletteToEdit.editPaletteName( mNameField->getText() );
        }
    else if( inTarget == mAddButton ) {
        addPalette();
        }
    else if( inTarget == mEditColorButton ) {
        // already in palette editor!
        mainColorEditor->setEditPaletteButtonVisible( false );
        showColorEditor();
        }
    else if( inTarget == mUndoButton ) {
        int lastIndex = mUndoStack.size() - 1;
        
        Palette last = *( mUndoStack.getElement( lastIndex ) );
        mUndoStack.deleteElement( lastIndex );
        if( mUndoStack.size() == 0 ) {
            mUndoButton->setEnabled( false );
            }
        
        mRedoStack.push_back( mPaletteToEdit );
        mRedoButton->setEnabled( true );
        
        setPaletteToEdit( last );
        }
    else if( inTarget == mRedoButton ) {
        int nextIndex = mRedoStack.size() - 1;
        
        Palette next = *( mRedoStack.getElement( nextIndex ) );
        mRedoStack.deleteElement( nextIndex );
        if( mRedoStack.size() == 0 ) {
            mRedoButton->setEnabled( false );
            }
        
        mUndoStack.push_back( mPaletteToEdit );
        mUndoButton->setEnabled( true );
        
        setPaletteToEdit( next );
        }
    else if( inTarget == mAddToPackButton ) {
        AppLog::info( "Adding palette to the current resource pack" );
        mainPalettePicker->getSelectedResource().saveToPack();

        mAddToPackButton->setState( true );
        }
    else if( inTarget == mSavePackButton ) {
        AppLog::info( "Saving the current resource pack" );
        savePack();

        mAddToPackButton->setState( false );
        }

    }



void PaletteEditor::editorClosing() {
    addPalette();
    }



void PaletteEditor::addPalette() {
    mAddAction = true;
    mPaletteToEdit.finishEdit();
    mainPalettePicker->setSelectedResource( mPaletteToEdit, true );
    mAddAction = false;
    }

