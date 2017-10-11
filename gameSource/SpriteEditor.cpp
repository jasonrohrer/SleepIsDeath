#include "SpriteEditor.h"
#include "SpritePicker.h"
#include "TilePicker.h"
#include "ColorWells.h"
#include "ColorEditor.h"
#include "BorderPanel.h"
#include "labels.h"
#include "SelectionManager.h"
#include "GridOverlay.h"



extern ColorWells *mainColorStack;
extern SpritePicker *mainSpritePicker;
extern TilePicker *mainTilePicker;

extern ColorEditor *mainColorEditor;


extern int gameWidth, gameHeight;


extern TextGL *largeTextFixed;



template <>
void SizeLimitedVector<SpriteResource>::deleteElementOfType(
    SpriteResource inElement ) {
    // no delete necessary
    }



SpriteEditor::SpriteEditor( ScreenGL *inScreen )
        : Editor( inScreen ),
          mUndoStack( MAX_UNDOS, false ) {

    mCloseButton->setToolTip( "tip_closeEdit_sprite" );

    /*
    LabelGL *titleLabel = new LabelGL( 0.75, 0.5, 0.25,
                                       0.1, "Tile Editor", largeText );
    

    
    mSidePanel->add( titleLabel );
    */

    mSidePanel->add( mainColorStack );

    mainColorStack->addActionListener( this );
    
    
    mSidePanel->add( mainSpritePicker );

    mainSpritePicker->addActionListener( this );
    

    mPickerTransButton = mainSpritePicker->getTransButton();
    
    mPickerTransButton->addActionListener( this );
    


    mEditColorButton = 
        new EditButtonGL( 
            mainColorStack->getAnchorX() - 9,
            mainColorStack->getAnchorY() + mainColorStack->getHeight() - 7,
            8,
            8 );
    
    mSidePanel->add( mEditColorButton );
    
    mEditColorButton->addActionListener( this );
    mEditColorButton->setToolTip( "tip_edit_color" );
    

    mEditPaletteButton = 
        new EditButtonGL( 
            mainColorStack->getAnchorX() - 9,
            mainColorStack->getAnchorY() + mainColorStack->getHeight() - 51,
            8,
            8 );
    
    mSidePanel->add( mEditPaletteButton );
    
    mEditPaletteButton->addActionListener( this );
    mEditPaletteButton->setToolTip( "tip_edit_palette" );

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


    double offset = P;

    double buttonSize = (gameHeight - 2 * offset - 8) / P;
    
    
    rgbaColor c =  { { 0,0,0,255 } };
    
    
                
    for( int y=0; y<P; y++ ) {
        for( int x=0; x<P; x++ ) {

            mButtonGrid[y][x] = new HighlightColorButtonGL(
                c,
                8 + x * buttonSize,
                gameWidth - ( 48 + (y + 1) * buttonSize ),
                buttonSize,
                buttonSize );
            

            mMainPanel->add( mButtonGrid[y][x] );
            
            mButtonGrid[y][x]->addActionListener( this );
            }
        }    

    
    


    mToolSet = new DrawToolSet( 2, 42 );
    mMainPanel->add( mToolSet );
    mToolSet->addActionListener( this );
    


    mSelectionButton = new SelectableButtonGL( 
        new Sprite( "selection.tga", true ),
        1,
        mToolSet->getAnchorX() + mToolSet->getWidth() + 5, 42, 
        20, 20 );
    
    mMainPanel->add( mSelectionButton );
    
    mSelectionButton->setSelected( false );
    
    mSelectionButton->addActionListener( this );

    mSelectionButton->setToolTip( "tip_selection" );





    

    EightPixelLabel *fieldLabel = new EightPixelLabel( 150, 54,
                                                       "spriteName" );
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
    

    // center it vertically on sprite picker
    double addY = mainSpritePicker->getAnchorY() +
        mainSpritePicker->getHeight() - 15;
    
    double sideButtonsX = gridEdge + (extra - 16) / 2;

    mAddButton = new AddButtonGL( sideButtonsX, 
                                  addY, 
                                  16, 16 );
    mMainPanel->add( mAddButton );
    mAddButton->addActionListener( this );
    mAddButton->setToolTip( "tip_addSprite" );
    
    mAddAction = false;
    

    double miniButtonSize = P + 4;
    
    mMiniViewButton = new TwoSpriteButtonGL( 
        NULL, NULL,
        1,
        gridEdge + ( extra - miniButtonSize ) / 2,
        addY - 24,
        miniButtonSize,
        miniButtonSize );
    
    mMainPanel->add( mMiniViewButton );
    



    mTransformToolSet = new TransformToolSet( sideButtonsX, 
                                              mMiniViewButton->getAnchorY()
                                              - 8 - 100,
                                              true ) ;
    
    mMainPanel->add( mTransformToolSet );    
    
    mTransformToolSet->addActionListener( this );


    // old:
    // align with grid bottom
    double undoButtonY = gameWidth - ( 48 + P * buttonSize );
    
    // new
    // down more to make room for Trans button
    undoButtonY -= 10;


    mUndoButton = new UndoButtonGL( sideButtonsX, undoButtonY, 16, 16 );
    mMainPanel->add( mUndoButton );
    mUndoButton->addActionListener( this );
    mUndoButton->setEnabled( false );

    mRedoButton = new RedoButtonGL( sideButtonsX, undoButtonY + 19, 16, 16 );
    mMainPanel->add( mRedoButton );
    mRedoButton->addActionListener( this );
    mRedoButton->setEnabled( false );

    

    mEraseButton = new SelectableButtonGL( 
        new Sprite( "erase.tga", true ),
        1,
        sideButtonsX - 2, undoButtonY + 20 + 17, 20, 20 );
    
    mMainPanel->add( mEraseButton );
    
    mEraseButton->setSelected( false );
    
    mEraseButton->addActionListener( this );

    mEraseButton->setToolTip( "tip_erase" );



    setSpriteToEdit( mainSpritePicker->getSelectedResource() );
    

    mPenDown = false;


    // listen for tile changes
    mainTilePicker->addActionListener( this );



    // fully-opaque gray to avoid color distortions
    Color gridColor( 0.25, 0.25, 0.25, 1.0 );
    
    GridOverlay *overlay = new GridOverlay( 
        8, 
        gameWidth - ( 48 + P * buttonSize ),
        P * buttonSize, P * buttonSize,
        P,
        gridColor );
    
    mMainPanel->add( overlay );

    
    postConstruction();
    }



SpriteEditor::~SpriteEditor() {
    mSidePanel->remove( mainColorStack );
    mSidePanel->remove( mainSpritePicker );
    }



void SpriteEditor::setSpriteToEdit( SpriteResource inSprite ) {
    mSpriteToEdit = inSprite;
    
    for( int y=0; y<P; y++ ) {
        for( int x=0; x<P; x++ ) {

            mButtonGrid[y][x]->setColor( mSpriteToEdit.getColor( x, y ) );
            mButtonGrid[y][x]->setHighlight( 
                mSpriteToEdit.getTrans( x, y ) );
            }
        }
    refreshMiniView();

    char *name = mSpriteToEdit.getSpriteName();
    
    mNameField->setText( name );
    mNameField->setCursorPosition( strlen( name ) );
    
    delete [] name;
    }



void SpriteEditor::refreshMiniView() {
    char useTrans = mainSpritePicker->isTransOn();

    // don't use cached version
    mMiniViewButton->setFrontSprite( mSpriteToEdit.getSprite( useTrans, 
                                                              false ) );
    
    if( useTrans ) {
        // backdrop behind sprite when transparency shown
        mMiniViewButton->setSprite( 
            mainTilePicker->getBackgroundTile().getSprite() );
        }
    else {
        // no backdrop
        mMiniViewButton->setSprite( NULL );
        }
    }



char SpriteEditor::recursiveFill( int inX, int inY, 
                                  rgbaColor inOldColor,
                                  char inOldTrans,
                                  rgbaColor inNewColor,
                                  drawTool inTool ) {
    
    char fillable =
        // filling non-trans area, and colors match
        ( !inOldTrans && 
          ! mSpriteToEdit.getTrans( inX, inY ) &&
          equal( mSpriteToEdit.getColor( inX, inY ), inOldColor ) )
        
        // or filling a trans area, and this pixel is trans
        ||
        ( inOldTrans && mSpriteToEdit.getTrans( inX, inY ) );
    
    
    char alreadyFilled =
        ! mSpriteToEdit.getTrans( inX, inY ) 
        &&
        equal( mSpriteToEdit.getColor( inX, inY ), inNewColor );
    
        

    if( fillable && ! alreadyFilled ) {
        
        mButtonGrid[inY][inX]->setColor( inNewColor );
        mSpriteToEdit.editSprite( inX, inY, inNewColor );

        // always turn trans off wherever user draws with colors
        mSpriteToEdit.editTrans( inX, inY, false );
        mButtonGrid[inY][inX]->setHighlight( false );

        // call on neighbors
        if( inTool == fill || inTool == horLine ) {    
            if( inX > 0 ) {
                recursiveFill( inX - 1, inY, inOldColor, inOldTrans,
                               inNewColor, inTool );
                }
            if( inX < P - 1 ) {
                recursiveFill( inX + 1, inY, inOldColor, inOldTrans,
                               inNewColor, inTool );
                }
            }

        if( inTool == fill || inTool == verLine ) {
            if( inY > 0 ) {
                recursiveFill( inX, inY - 1, inOldColor, inOldTrans,
                               inNewColor, inTool );
                }
            if( inY < P - 1 ) {
                recursiveFill( inX, inY + 1, inOldColor, inOldTrans,
                               inNewColor, inTool );
                }
            }

        return true;
        }

    return false;
    }




char SpriteEditor::recursiveFill( int inX, int inY, 
                                  rgbaColor inOldColor,
                                  char inOldTrans, char inNewTrans,
                                  drawTool inTool ) {

    if( equal( mSpriteToEdit.getColor( inX, inY ), inOldColor )
        &&
        mSpriteToEdit.getTrans( inX, inY ) == inOldTrans
        &&
        mSpriteToEdit.getTrans( inX, inY ) != inNewTrans ) {

        mButtonGrid[inY][inX]->setHighlight( inNewTrans );
        mSpriteToEdit.editTrans( inX, inY, inNewTrans );
        
        // call on neighbors
        if( inTool == fill || inTool == horLine ) {    
            if( inX > 0 ) {
                recursiveFill( inX - 1, inY, inOldColor, 
                               inOldTrans, inNewTrans, inTool );
                }
            if( inX < P - 1 ) {
                recursiveFill( inX + 1, inY, inOldColor, 
                               inOldTrans, inNewTrans, inTool );
                }
            }

        if( inTool == fill || inTool == verLine ) {
            if( inY > 0 ) {
                recursiveFill( inX, inY - 1, inOldColor, 
                               inOldTrans, inNewTrans, inTool );
                }
            if( inY < P - 1 ) {
                recursiveFill( inX, inY + 1, inOldColor, 
                               inOldTrans, inNewTrans, inTool );
                }
            }

        return true;
        }

    return false;
    }




char SpriteEditor::recursiveSelectionFill( int inX, int inY, 
                                           rgbaColor inOldColor,
                                           char inOldTrans,
                                           char inOldSelection, 
                                           char inNewSelection,
                                           drawTool inTool ) {

    if( ( equal( mSpriteToEdit.getColor( inX, inY ), inOldColor )
          ||
          ( inOldTrans && mSpriteToEdit.getTrans( inX, inY ) ) )
        &&
        SelectionManager::isInSelection( inX, inY ) == inOldSelection
        &&
        SelectionManager::isInSelection( inX, inY ) != inNewSelection ) {

        SelectionManager::toggleSelection( inX, inY, inNewSelection );
                
        mButtonGrid[inY][inX]->setSelection( inNewSelection );
        
        if( inNewSelection ) {
            SelectionManager::setColor( 
                inX, inY,
                mSpriteToEdit.getColor( inX, inY ) );
            SelectionManager::setTrans( 
                inX, inY,
                mSpriteToEdit.getTrans( inX, inY ) );
            }

        // call on neighbors
        if( inTool == fill || inTool == horLine ) {    
            if( inX > 0 ) {
                recursiveSelectionFill( inX - 1, inY, inOldColor,
                                        inOldTrans,
                                        inOldSelection, inNewSelection, 
                                        inTool );
                }
            if( inX < P - 1 ) {
                recursiveSelectionFill( inX + 1, inY, inOldColor,
                                        inOldTrans, 
                                        inOldSelection, inNewSelection, 
                                        inTool );
                }
            }

        if( inTool == fill || inTool == verLine ) {
            if( inY > 0 ) {
                recursiveSelectionFill( inX, inY - 1, inOldColor,
                                        inOldTrans, 
                                        inOldSelection, inNewSelection, 
                                        inTool );
                }
            if( inY < P - 1 ) {
                recursiveSelectionFill( inX, inY + 1, inOldColor,
                                        inOldTrans, 
                                        inOldSelection, inNewSelection, 
                                        inTool );
                }
            }

        return true;
        }

    return false;
    }



void SpriteEditor::toggleErase() {
    mEraseButton->setSelected( ! mEraseButton->getSelected() );
        
    char showErase = mEraseButton->getSelected();
    
    /*
      // FIXME
    for( int y=0; y<P; y++ ) {
        for( int x=0; x<P; x++ ) {
            mButtonGrid[y][x]->setHighlight(
                mSpriteToEdit.getTrans( x, y ) && showTrans );
            }
        }
    */


    if( showErase && mSelectionButton->getSelected() ) {
        // only one on at a time
        toggleSelection();
        }
    
    

    // update display of trans in mini view
    refreshMiniView();
    }






void SpriteEditor::toggleSelection() {
    mSelectionButton->setSelected( ! mSelectionButton->getSelected() );
        
    char showSel = mSelectionButton->getSelected();
        
    for( int y=0; y<P; y++ ) {
        for( int x=0; x<P; x++ ) {
            mButtonGrid[y][x]->setSelection(
                SelectionManager::isInSelection( x, y ) && showSel );
            }
        }
    
    // commit selection only when it is turned on
    // (right before it is turned off, the last edits of selection have updated
    //  the selected colors)
    if( showSel ) {    
        // copy colors
        for( int y=0; y<P; y++ ) {
            for( int x=0; x<P; x++ ) {
                if( SelectionManager::isInSelection( x, y ) ) {
                    SelectionManager::setColor( 
                        x, y,
                        mSpriteToEdit.getColor( x, y ) );
                    SelectionManager::setTrans( 
                        x, y,
                        mSpriteToEdit.getTrans( x, y ) );
                    }
                }
            }
        }
    
    
    if( showSel && mEraseButton->getSelected() ) {
        // only one on at a time
        toggleErase();
        }
    
    clearStampOverlay();
    }



void SpriteEditor::clearStampOverlay() {
    // turn all overlays off
    for( int by=0; by<P; by++ ) {
        for( int bx=0; bx<P; bx++ ) {
            mButtonGrid[by][bx]->
                setOverlay( false );
            }
        }
    }



void SpriteEditor::colorEditorClosed() {
    // it might have been closed by an EditPalette button press
    if( mainColorEditor->mEditPalettePressed ) {
        showPaletteEditor();
        }
    }


void SpriteEditor::actionPerformed( GUIComponent *inTarget ) {
    // superclass
    Editor::actionPerformed( inTarget );
    

    if( inTarget == mainColorStack ) {
        // new color picked on stack
        
        // non-erase mode
        if( mEraseButton->getSelected() ) {
            toggleErase();
            }
        }
    else if( inTarget == mainSpritePicker ) {
        if( ! mAddAction &&
            ! mainSpritePicker->wasLastActionFromPress() ) {
            
            // will change sprite
                        
            mUndoStack.push_back( mSpriteToEdit );
            mUndoButton->setEnabled( true );
            
            setSpriteToEdit( mainSpritePicker->getSelectedResource() );
            
            // new branch... "redo" future now impossible
            mRedoStack.deleteAll();
            mRedoButton->setEnabled( false );
            }
        }
    else if( inTarget == mPickerTransButton ) {
        refreshMiniView();
        }
    else if( inTarget == mainTilePicker ) {
        // tile change... tile used behind sprite transparency
        refreshMiniView();
        }
    else if( inTarget == mNameField ) {
                
        mUndoStack.push_back( mSpriteToEdit );
        mUndoButton->setEnabled( true );

        // new branch... "redo" future now impossible
        mRedoStack.deleteAll();
        mRedoButton->setEnabled( false );
        
        mSpriteToEdit.editSpriteName( mNameField->getText() );
        }
    else if( inTarget == mAddButton ) {
        addSprite();
        }
    else if( inTarget == mEditColorButton ) {
        mainColorEditor->setEditPaletteButtonVisible( true );
        showColorEditor();
        }
    else if( inTarget == mEditPaletteButton ) {
        showPaletteEditor();
        }
    else if( inTarget == mUndoButton ) {
        int lastIndex = mUndoStack.size() - 1;
        
        SpriteResource last = *( mUndoStack.getElement( lastIndex ) );
        mUndoStack.deleteElement( lastIndex );
        if( mUndoStack.size() == 0 ) {
            mUndoButton->setEnabled( false );
            }
        
        mRedoStack.push_back( mSpriteToEdit );
        mRedoButton->setEnabled( true );
        
        setSpriteToEdit( last );
        }
    else if( inTarget == mRedoButton ) {
        int nextIndex = mRedoStack.size() - 1;
        
        SpriteResource next = *( mRedoStack.getElement( nextIndex ) );
        mRedoStack.deleteElement( nextIndex );
        if( mRedoStack.size() == 0 ) {
            mRedoButton->setEnabled( false );
            }
        
        mUndoStack.push_back( mSpriteToEdit );
        mUndoButton->setEnabled( true );
        
        setSpriteToEdit( next );
        }
    else if( inTarget == mEraseButton ) {
        toggleErase();
        }
    else if( inTarget == mSelectionButton ) {
        toggleSelection();
        }
    else if( inTarget == mTransformToolSet ) {
        mUndoStack.push_back( mSpriteToEdit );
        mUndoButton->setEnabled( true );

        // new branch... "redo" future now impossible
        mRedoStack.deleteAll();
        mRedoButton->setEnabled( false );
        
        SpriteResource oldSpriteState = mSpriteToEdit;

        switch( mTransformToolSet->getLastPressed() ) {
            case flipH: {
                for( int y=0; y<P; y++ ) {
                    for( int x=0; x<P; x++ ) {
                        rgbaColor flipColor = 
                            oldSpriteState.getColor( P - x - 1, y );
                        char flipTrans =
                            oldSpriteState.getTrans( P - x - 1, y );

                        mButtonGrid[y][x]->setColor( flipColor );
                        
                        mSpriteToEdit.editSprite( x, y, flipColor );
                        mSpriteToEdit.editTrans( x, y, flipTrans );
                        }
                    }
                }
                break;
            case flipV: {
                for( int y=0; y<P; y++ ) {
                    for( int x=0; x<P; x++ ) {
                        rgbaColor flipColor = 
                            oldSpriteState.getColor( x, P - y - 1 );
                        char flipTrans =
                            oldSpriteState.getTrans( x, P - y - 1 );

                        mButtonGrid[y][x]->setColor( flipColor );
                        
                        mSpriteToEdit.editSprite( x, y, flipColor );
                        mSpriteToEdit.editTrans( x, y, flipTrans );
                        }
                    }

                }
                break;
            case rotateCCW: {
                for( int y=0; y<P; y++ ) {
                    for( int x=0; x<P; x++ ) {
                        rgbaColor flipColor = 
                            oldSpriteState.getColor( P - y - 1, x );
                        char flipTrans =
                            oldSpriteState.getTrans( P - y - 1, x );

                        mButtonGrid[y][x]->setColor( flipColor );
                        
                        mSpriteToEdit.editSprite( x, y, flipColor );
                        mSpriteToEdit.editTrans( x, y, flipTrans );
                        }
                    }

                }
                break;
            case rotateCW: {
                for( int y=0; y<P; y++ ) {
                    for( int x=0; x<P; x++ ) {
                        rgbaColor flipColor = 
                            oldSpriteState.getColor( y, P - x - 1 );
                        char flipTrans =
                            oldSpriteState.getTrans( y, P - x - 1 );

                        mButtonGrid[y][x]->setColor( flipColor );
                        
                        mSpriteToEdit.editSprite( x, y, flipColor );
                        mSpriteToEdit.editTrans( x, y, flipTrans );
                        }
                    }

                }
                break;
            case clear: {
                rgbaColor black;
                black.comp.r = 0;
                black.comp.g = 0;
                black.comp.b = 0;
                black.comp.a = 255;
                
                for( int y=0; y<P; y++ ) {
                    for( int x=0; x<P; x++ ) {
                        mButtonGrid[y][x]->setColor( black );
                        mSpriteToEdit.editSprite( x, y, black );
                        mSpriteToEdit.editTrans( x, y, true );
                        }
                    }
                }
                break;
            case colorize: {
                rgbaColor c = 
                    mainColorStack->getSelectedColor();
                
                for( int y=0; y<P; y++ ) {
                    for( int x=0; x<P; x++ ) {
                        rgbaColor oldColor = mSpriteToEdit.getColor( x, y );
                        
                        // multiply all components to apply color
                        for( int b=0; b<3; b++ ) {
                            oldColor.bytes[b] = (unsigned char)(
                                ( oldColor.bytes[b] * c.bytes[b] )
                                / 255 );
                            }
                    
                        mButtonGrid[y][x]->setColor( oldColor );
                        mSpriteToEdit.editSprite( x, y, oldColor );
                        }
                    }
                }
                break;


            }


        char showTrans = true;

        for( int y=0; y<P; y++ ) {
            for( int x=0; x<P; x++ ) {
                mButtonGrid[y][x]->setHighlight(
                    mSpriteToEdit.getTrans( x, y ) && showTrans );
                }
            }

        refreshMiniView();
        }
    else if( inTarget == mToolSet ) {
        clearStampOverlay();
        }
    else if( !mainColorEditor->getDragging() ){
        // check grid 
        // but only if not in the middle of drag-picking a color in the editor


        char found = false;
        
        for( int y=0; y<P && !found; y++ ) {
            for( int x=0; x<P && !found; x++ ) {
                if( inTarget == mButtonGrid[y][x] &&
                    ! mButtonGrid[y][x]->wasLastActionHover() ) {
                    found = true;
                    
                    SpriteResource oldSpriteState = mSpriteToEdit;
                    char changed = false;

                    switch( mToolSet->getSelected() ) {
                        case pen: {
                            if( mEraseButton->getSelected() ) {
                                // add transparency with pen
                                
                                if( !mPenDown ) {
                                    // set ink until released
                                    mTransInk = true;
                                    
                                    mPenDown = true;
                                    changed = true;
                                    }
                                
                                // use ink already set
                                mSpriteToEdit.editTrans( x, y, mTransInk );
                                
                                mButtonGrid[y][x]->setHighlight( mTransInk );
                                refreshMiniView();
                                }
                            else if( mSelectionButton->getSelected() ) {
                                // edit selection with pen
                                
                                if( !mPenDown ) {
                                    // set ink until released
                                    mSelectionInk = 
                                        ! SelectionManager::
                                             isInSelection( x, y );
                                    
                                    mPenDown = true;
                                    changed = true;
                                    }
                                
                                // use ink already set
                                SelectionManager::toggleSelection( 
                                    x, y, mSelectionInk );
                                
                                mButtonGrid[y][x]->setSelection( 
                                    mSelectionInk );

                                if( mSelectionInk ) {
                                    SelectionManager::setColor( 
                                        x, y,
                                        mSpriteToEdit.getColor( x, y ) );
                                    
                                    SelectionManager::setTrans( 
                                        x, y,
                                        mSpriteToEdit.getTrans( x, y ) );
                                    }
                                }
                            else {
                                if( mainColorEditor->isVisible() ) {
                                    // push edited color onto stack first
                                    mainColorEditor->addColor();
                                    }
                                

                                // edit pixel colors
                                rgbaColor c = 
                                    mainColorStack->getSelectedColor();
                        
                                if( ! equal( 
                                        c, 
                                        mSpriteToEdit.getColor( x, y ) )
                                    ||
                                    mSpriteToEdit.getTrans( x, y ) ) {

                                    mButtonGrid[y][x]->setColor( c );
                                    mSpriteToEdit.editSprite( x, y, c );

                                    // always turn trans off wherever user
                                    // draws with colors
                                    mSpriteToEdit.editTrans( x, y, false );
                                    mButtonGrid[y][x]->setHighlight( false );
                                    
                                    refreshMiniView();
                                    
                                    // don't count single pen dots as undoable
                                    // until pen is released
                                    // save undo state only on on initial 
                                    // presses
                                    if( !mPenDown ) {
                                        mPenDown = true;
                                        changed = true;
                                        }
                                    }    
                                }
                            if( !mButtonGrid[y][x]->isPressed() ) {
                                // a release
                                mPenDown = false;
                                }
                            }
                            break;
                        case horLine:
                        case verLine:
                        case fill:
                            if( mEraseButton->getSelected() ) {
                                // add to trans
                                if( !mPenDown ) {
                                    mTransInk = true;
                                    }
                                
                                changed = recursiveFill( 
                                    x, y,
                                    mSpriteToEdit.getColor( x, y ),
                                    mSpriteToEdit.getTrans( x, y ),
                                    mTransInk,
                                    mToolSet->getSelected() );

                                refreshMiniView();

                                if( !mPenDown ) {
                                    mPenDown = true;
                                    } 
                                if( !mButtonGrid[y][x]->isPressed() ) {
                                    // a release
                                    mPenDown = false;
                                    }
                                }
                            else if( mSelectionButton->getSelected() ) {
                                if( !mPenDown ) {
                                    mSelectionInk = ! SelectionManager::
                                        isInSelection( x, y );
                                    }
                                
                                recursiveSelectionFill( 
                                    x, y,
                                    mSpriteToEdit.getColor( x, y ),
                                    mSpriteToEdit.getTrans( x, y ),
                                    SelectionManager::
                                      isInSelection( x, y ),
                                    mSelectionInk,
                                    mToolSet->getSelected() );

                                if( !mPenDown ) {
                                    mPenDown = true;
                                    } 
                                if( !mButtonGrid[y][x]->isPressed() ) {
                                    // a release
                                    mPenDown = false;
                                    }
                                }
                            else {
                                if( mainColorEditor->isVisible() ) {
                                    // push edited color onto stack first
                                    mainColorEditor->addColor();
                                    }

                                changed = recursiveFill( 
                                    x, y, 
                                    mSpriteToEdit.getColor( x, y ),
                                    mSpriteToEdit.getTrans( x, y ),
                                    mainColorStack->getSelectedColor(),
                                    mToolSet->getSelected() );
                                refreshMiniView();
                                }
                            break;
                        case pickColor:
                            if( mSpriteToEdit.getTrans( x, y ) ) {
                                // trans ink here, switch to erase mode
                                // thus "picking up" trans ink
                                
                                if( ! mEraseButton->getSelected() ) {
                                    toggleErase();
                                    }
                                }
                            else {
                                // pick up color
                                mainColorStack->pushColor(
                                    mSpriteToEdit.getColor( x, y ) );

                                // non-erase mode
                                if( mEraseButton->getSelected() ) {
                                    toggleErase();
                                    }
                                }
                            break;
                        case stamp: {
                            if( !mPenDown ) {
                                changed = true;
                                mPenDown = true;
                                }
                            // insert colors from selection
                            
                            intPair center = 
                                SelectionManager::getSelectionCenter();
                            
                            for( int sy=0; sy<P; sy++ ) {
                                for( int sx=0; sx<P; sx++ ) {
                                    if( SelectionManager::isInSelection( 
                                            sx, sy ) ) {
                                        
                                        int iy = y + sy - center.y;
                                        int ix = x + sx - center.x;
                                        
                                        if( iy < P && ix < P 
                                            && 
                                            iy >= 0 && ix >= 0 ) {
                                            rgbaColor c = SelectionManager::
                                                getColor( sx, sy );
                                            
                                            char t = SelectionManager::
                                                getTrans( sx, sy );
                                            
                                            // always turn trans off wherever 
                                            // user draws with colors,
                                            // unless that spot in selection is
                                            // trans

                                            mButtonGrid[iy][ix]->setColor( c );
                                            mButtonGrid[iy][ix]->setHighlight( 
                                                t );

                                            mSpriteToEdit.editSprite( 
                                                ix, iy, c );
                                            mSpriteToEdit.editTrans( 
                                                ix, iy, t );
                                            }
                                        }
                                    }
                                }
                            
                            refreshMiniView();  

                            if( !mButtonGrid[y][x]->isPressed() ) {
                                // a release
                                mPenDown = false;
                                }
                            }
                            break;
                        }
                    

                    
                    if( changed ) {
                        mUndoStack.push_back( oldSpriteState );
                        mUndoButton->setEnabled( true );

                        // new branch... "redo" future now impossible
                        mRedoStack.deleteAll();
                        mRedoButton->setEnabled( false );
                        }

                    
                    
                    }
                else if( inTarget == mButtonGrid[y][x] &&
                         mButtonGrid[y][x]->wasLastActionHover() ) {
                    found = true;
                    
                    if( mToolSet->getSelected() == stamp ) {
                        
                        // preview of selection that will be inserted 
                        // upon click

                        // first, turn all overlays off
                        for( int by=0; by<P; by++ ) {
                            for( int bx=0; bx<P; bx++ ) {
                                mButtonGrid[by][bx]->
                                    setOverlay( false );
                                }
                            }
                        
                        intPair center = 
                            SelectionManager::getSelectionCenter();
                            
                        for( int sy=0; sy<P; sy++ ) {
                            for( int sx=0; sx<P; sx++ ) {
                                if( SelectionManager::isInSelection( 
                                        sx, sy ) ) {
                                    
                                    int iy = y + sy - center.y;
                                    int ix = x + sx - center.x;
                                    
                                    if( iy < P && ix < P 
                                        && 
                                        iy >= 0 && ix >= 0 ) {
                                        rgbaColor c = SelectionManager::
                                            getColor( sx, sy );
                                        char t = SelectionManager::
                                            getTrans( sx, sy );
                                        
                                        mButtonGrid[iy][ix]->
                                            setOverlayColor( c );
                                        mButtonGrid[iy][ix]->
                                            setOverlayTrans( t );

                                        mButtonGrid[iy][ix]->
                                            setOverlay( true );
                                        }
                                    }
                                }
                            }
                        
                        }

                    }
                
                }
            }
        }
    
    
    
    }



void SpriteEditor::editorClosing() {
    addSprite();
    }





void SpriteEditor::addSprite() {
    mAddAction = true;
    mSpriteToEdit.finishEdit();
    mainSpritePicker->setSelectedResource( mSpriteToEdit, true );
    mAddAction = false;
    }
