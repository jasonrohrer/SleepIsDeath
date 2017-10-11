#include "TileEditor.h"
#include "TilePicker.h"
#include "ColorWells.h"
#include "ColorEditor.h"
#include "PaletteEditor.h"
#include "BorderPanel.h"
#include "labels.h"
#include "SelectionManager.h"
#include "GridOverlay.h"



extern ColorWells *mainColorStack;
extern TilePicker *mainTilePicker;

extern ColorEditor *mainColorEditor;

extern PaletteEditor *mainPaletteEditor;


extern int gameWidth, gameHeight;


extern TextGL *largeTextFixed;



template <>
void SizeLimitedVector<Tile>::deleteElementOfType(
    Tile inElement ) {
    // no delete necessary
    }



TileEditor::TileEditor( ScreenGL *inScreen )
        : Editor( inScreen ),
          mUndoStack( MAX_UNDOS, false ) {

    mCloseButton->setToolTip( "tip_closeEdit_tile" );
    /*
    LabelGL *titleLabel = new LabelGL( 0.75, 0.5, 0.25,
                                       0.1, "Tile Editor", largeText );
    

    
    mSidePanel->add( titleLabel );
    */

    mSidePanel->add( mainColorStack );

    mainColorStack->addActionListener( this );
    
    
    mSidePanel->add( mainTilePicker );

    mainTilePicker->addActionListener( this );
    

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
                                                       "tileSetName" );
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
    double gridEdge = 8 + P * buttonSize;
    
    double extra = gameHeight - gridEdge;
    

    // center it vertically on tile picker
    double addY = mainTilePicker->getAnchorY() +
        mainTilePicker->getHeight() - 15;
    
    double sideButtonsX = gridEdge + (extra - 16) / 2;

    mAddButton = new AddButtonGL( sideButtonsX, 
                                  addY, 
                                  16, 16 );
    mMainPanel->add( mAddButton );
    mAddButton->addActionListener( this );
    mAddButton->setToolTip( "tip_addTile" );
    
    mAddAction = false;
    

    double miniButtonSize = P + 4;
    
    mMiniViewButton = new SpriteButtonGL( 
        NULL, 1,
        gridEdge + ( extra - miniButtonSize ) / 2,
        addY - 24,
        miniButtonSize,
        miniButtonSize );
    
    mMainPanel->add( mMiniViewButton );
    



    mTransformToolSet = new TransformToolSet( sideButtonsX, 
                                              mMiniViewButton->getAnchorY()
                                              - 10 - 100,
                                              true ) ;
    
    mMainPanel->add( mTransformToolSet );    
    
    mTransformToolSet->addActionListener( this );
    


    
    double undoButtonY = gameWidth - ( 48 + P * buttonSize );

    mUndoButton = new UndoButtonGL( sideButtonsX, undoButtonY, 16, 16 );
    mMainPanel->add( mUndoButton );
    mUndoButton->addActionListener( this );
    mUndoButton->setEnabled( false );

    mRedoButton = new RedoButtonGL( sideButtonsX, undoButtonY + 19, 16, 16 );
    mMainPanel->add( mRedoButton );
    mRedoButton->addActionListener( this );
    mRedoButton->setEnabled( false );




    setTileToEdit( mainTilePicker->getSelectedResource() );
    

    mPenDown = false;


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



TileEditor::~TileEditor() {
    mSidePanel->remove( mainColorStack );
    mSidePanel->remove( mainTilePicker );
    
    }



void TileEditor::setTileToEdit( Tile inTile ) {
    mTileToEdit = inTile;
    
    for( int y=0; y<P; y++ ) {
        for( int x=0; x<P; x++ ) {

            mButtonGrid[y][x]->setColor( mTileToEdit.getColor( x, y ) );
            }
        }
    refreshMiniView();

    char *name = mTileToEdit.getTileSetName();
    
    mSetNameField->setText( name );
    mSetNameField->setCursorPosition( strlen( name ) );
    
    delete [] name;
    }



void TileEditor::refreshMiniView() {
    // don't use cached version
    mMiniViewButton->setSprite( mTileToEdit.getSprite( false, false ) );
    }



char TileEditor::recursiveFill( int inX, int inY, rgbaColor inOldColor,
                                rgbaColor inNewColor,
                                drawTool inTool ) {

    if( equal( mTileToEdit.getColor( inX, inY ), inOldColor )
        &&
        !equal( mTileToEdit.getColor( inX, inY ), inNewColor ) ) {

        mButtonGrid[inY][inX]->setColor( inNewColor );
        mTileToEdit.editTile( inX, inY, inNewColor );
        
        // call on neighbors
        if( inTool == fill || inTool == horLine ) {    
            if( inX > 0 ) {
                recursiveFill( inX - 1, inY, inOldColor, inNewColor, inTool );
                }
            if( inX < P - 1 ) {
                recursiveFill( inX + 1, inY, inOldColor, inNewColor, inTool );
                }
            }

        if( inTool == fill || inTool == verLine ) {
            if( inY > 0 ) {
                recursiveFill( inX, inY - 1, inOldColor, inNewColor, inTool );
                }
            if( inY < P - 1 ) {
                recursiveFill( inX, inY + 1, inOldColor, inNewColor, inTool );
                }
            }

        return true;
        }

    return false;
    }



char TileEditor::recursiveSelectionFill( int inX, int inY, 
                                           rgbaColor inOldColor,
                                           char inOldSelection, 
                                           char inNewSelection,
                                           drawTool inTool ) {

    if( equal( mTileToEdit.getColor( inX, inY ), inOldColor )
        &&
        SelectionManager::isInSelection( inX, inY ) == inOldSelection
        &&
        SelectionManager::isInSelection( inX, inY ) != inNewSelection ) {

        SelectionManager::toggleSelection( inX, inY, inNewSelection );
                
        mButtonGrid[inY][inX]->setSelection( inNewSelection );
        
        if( inNewSelection ) {
            SelectionManager::setColor( 
                inX, inY,
                mTileToEdit.getColor( inX, inY ) );
            SelectionManager::setTrans( 
                inX, inY,
                false );
            }

        // call on neighbors
        if( inTool == fill || inTool == horLine ) {    
            if( inX > 0 ) {
                recursiveSelectionFill( inX - 1, inY, inOldColor, 
                                        inOldSelection, inNewSelection, 
                                        inTool );
                }
            if( inX < P - 1 ) {
                recursiveSelectionFill( inX + 1, inY, inOldColor, 
                                        inOldSelection, inNewSelection, 
                                        inTool );
                }
            }

        if( inTool == fill || inTool == verLine ) {
            if( inY > 0 ) {
                recursiveSelectionFill( inX, inY - 1, inOldColor, 
                                        inOldSelection, inNewSelection, 
                                        inTool );
                }
            if( inY < P - 1 ) {
                recursiveSelectionFill( inX, inY + 1, inOldColor, 
                                        inOldSelection, inNewSelection, 
                                        inTool );
                }
            }

        return true;
        }

    return false;
    }



void TileEditor::toggleSelection() {
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
                        mTileToEdit.getColor( x, y ) );
                    SelectionManager::setTrans( 
                        x, y,
                        false );
                    }
                }
            }
        }
    
    clearStampOverlay();
    }



void TileEditor::clearStampOverlay() {
    // turn all overlays off
    for( int by=0; by<P; by++ ) {
        for( int bx=0; bx<P; bx++ ) {
            mButtonGrid[by][bx]->
                setOverlay( false );
            }
        }
    }



void TileEditor::colorEditorClosed() {
    // it might have been closed by an EditPalette button press
    if( mainColorEditor->mEditPalettePressed ) {
        showPaletteEditor();
        }
    }




void TileEditor::actionPerformed( GUIComponent *inTarget ) {
    // superclass
    Editor::actionPerformed( inTarget );
    

    if( inTarget == mainColorStack ) {
        // new color picked on stack
        }
    else if( inTarget == mainTilePicker ) {
        if( ! mAddAction &&
            ! mainTilePicker->wasLastActionFromPress() ) {
            // will change tile
                        
            mUndoStack.push_back( mTileToEdit );
            mUndoButton->setEnabled( true );
            
            setTileToEdit( mainTilePicker->getSelectedResource() );
            
            // new branch... "redo" future now impossible
            mRedoStack.deleteAll();
            mRedoButton->setEnabled( false );
            }
        }
    else if( inTarget == mSetNameField ) {
        
        mUndoStack.push_back( mTileToEdit );
        mUndoButton->setEnabled( true );

        // new branch... "redo" future now impossible
        mRedoStack.deleteAll();
        mRedoButton->setEnabled( false );
        
        mTileToEdit.editTileSetName( mSetNameField->getText() );
        }
    else if( inTarget == mAddButton ) {
        addTile();
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
        
        Tile last = *( mUndoStack.getElement( lastIndex ) );
        mUndoStack.deleteElement( lastIndex );
        if( mUndoStack.size() == 0 ) {
            mUndoButton->setEnabled( false );
            }
        
        mRedoStack.push_back( mTileToEdit );
        mRedoButton->setEnabled( true );
        
        setTileToEdit( last );
        }
    else if( inTarget == mRedoButton ) {
        int nextIndex = mRedoStack.size() - 1;
        
        Tile next = *( mRedoStack.getElement( nextIndex ) );
        mRedoStack.deleteElement( nextIndex );
        if( mRedoStack.size() == 0 ) {
            mRedoButton->setEnabled( false );
            }
        
        mUndoStack.push_back( mTileToEdit );
        mUndoButton->setEnabled( true );
        
        setTileToEdit( next );
        }
    else if( inTarget == mSelectionButton ) {
        toggleSelection();
        }
    else if( inTarget == mTransformToolSet ) {
        mUndoStack.push_back( mTileToEdit );
        mUndoButton->setEnabled( true );

        // new branch... "redo" future now impossible
        mRedoStack.deleteAll();
        mRedoButton->setEnabled( false );
        
        Tile oldTileState = mTileToEdit;

        switch( mTransformToolSet->getLastPressed() ) {
            case flipH: {
                for( int y=0; y<P; y++ ) {
                    for( int x=0; x<P; x++ ) {
                        rgbaColor flipColor = 
                            oldTileState.getColor( P - x - 1, y );
                        
                        mButtonGrid[y][x]->setColor( flipColor );
                        mTileToEdit.editTile( x, y, flipColor );
                        }
                    }
                }
                break;
            case flipV: {
                for( int y=0; y<P; y++ ) {
                    for( int x=0; x<P; x++ ) {
                        rgbaColor flipColor = 
                            oldTileState.getColor( x, P - y - 1 );
                        
                        mButtonGrid[y][x]->setColor( flipColor );
                        mTileToEdit.editTile( x, y, flipColor );
                        }
                    }

                }
                break;
            case rotateCCW: {
                for( int y=0; y<P; y++ ) {
                    for( int x=0; x<P; x++ ) {
                        rgbaColor flipColor = 
                            oldTileState.getColor( P - y - 1, x );
                        
                        mButtonGrid[y][x]->setColor( flipColor );
                        mTileToEdit.editTile( x, y, flipColor );
                        }
                    }

                }
                break;
            case rotateCW: {
                for( int y=0; y<P; y++ ) {
                    for( int x=0; x<P; x++ ) {
                        rgbaColor flipColor = 
                            oldTileState.getColor( y, P - x - 1);
                        
                        mButtonGrid[y][x]->setColor( flipColor );
                        mTileToEdit.editTile( x, y, flipColor );
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
                        mTileToEdit.editTile( x, y, black );
                        }
                    }
                }
                break;
            case colorize: {
                rgbaColor c = 
                    mainColorStack->getSelectedColor();
                
                for( int y=0; y<P; y++ ) {
                    for( int x=0; x<P; x++ ) {
                        rgbaColor oldColor = mTileToEdit.getColor( x, y );
                        
                        // multiply all components to apply color
                        for( int b=0; b<3; b++ ) {
                            oldColor.bytes[b] = (unsigned char)(
                                ( oldColor.bytes[b] * c.bytes[b] )
                                / 255 );
                            }
                    
                        mButtonGrid[y][x]->setColor( oldColor );
                        mTileToEdit.editTile( x, y, oldColor );
                        }
                    }
                }
                break;
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
                    
                    Tile oldTileState = mTileToEdit;
                    char changed = false;

                    switch( mToolSet->getSelected() ) {
                        case pen: {
                            if( mSelectionButton->getSelected() ) {
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
                                        mTileToEdit.getColor( x, y ) );
                                    SelectionManager::setTrans( 
                                        x, y, false );
                                    }
                                }
                            else {
                                if( mainColorEditor->isVisible() ) {
                                    // push edited color onto stack first
                                    mainColorEditor->addColor();
                                    }

                                rgbaColor c = 
                                    mainColorStack->getSelectedColor();
                        
                                if( ! equal( c, 
                                             mTileToEdit.getColor( x, y ) ) ) {
                                    
                                    mButtonGrid[y][x]->setColor( c );
                                    mTileToEdit.editTile( x, y, c );
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
                                if( !mButtonGrid[y][x]->isPressed() ) {
                                    // a release
                                    mPenDown = false;
                                    }
                                }
                            }
                            break;
                        case horLine:
                        case verLine:
                        case fill:
                            if( mSelectionButton->getSelected() ) {
                                if( !mPenDown ) {
                                    mSelectionInk = ! SelectionManager::
                                        isInSelection( x, y );
                                    }
                                
                                recursiveSelectionFill( 
                                    x, y,
                                    mTileToEdit.getColor( x, y ),
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
                                    mTileToEdit.getColor( x, y ),
                                    mainColorStack->getSelectedColor(),
                                    mToolSet->getSelected() );
                                refreshMiniView();
                                }
                            break;
                        case pickColor:
                            mainColorStack->pushColor(
                                mTileToEdit.getColor( x, y ) );
                            break;
                        case stamp: {
                            if( !mPenDown ) {
                                changed = true;
                                mPenDown = true;
                                }
                            // insert colors from selection
                            
                            intPair center = 
                                SelectionManager::getSelectionCenter( true );
                            
                            for( int sy=0; sy<P; sy++ ) {
                                for( int sx=0; sx<P; sx++ ) {
                                    if( SelectionManager::isInSelection( 
                                            sx, sy ) 
                                        &&
                                        // ignore trans areas for tiles
                                        ! SelectionManager::getTrans( sx, sy )
                                        ) {
                                        
                                        int iy = y + sy - center.y;
                                        int ix = x + sx - center.x;
                                        
                                        if( iy < P && ix < P 
                                            && 
                                            iy >= 0 && ix >= 0 ) {
                                            rgbaColor c = SelectionManager::
                                                getColor( sx, sy );
                                            
                                            mButtonGrid[iy][ix]->setColor( c );
                                            mTileToEdit.editTile( 
                                                ix, iy, c );
                                            
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
                        mUndoStack.push_back( oldTileState );
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
                            SelectionManager::getSelectionCenter( true );
                            
                        for( int sy=0; sy<P; sy++ ) {
                            for( int sx=0; sx<P; sx++ ) {
                                if( SelectionManager::isInSelection( 
                                        sx, sy ) 
                                    &&
                                    // ignore trans areas for tiles
                                    ! SelectionManager::getTrans( sx, sy )
                                    ) {
                                    
                                    int iy = y + sy - center.y;
                                    int ix = x + sx - center.x;
                                    
                                    if( iy < P && ix < P 
                                        && 
                                        iy >= 0 && ix >= 0 ) {
                                        rgbaColor c = SelectionManager::
                                            getColor( sx, sy );
                                            
                                        mButtonGrid[iy][ix]->
                                            setOverlayColor( c );
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



void TileEditor::editorClosing() {
    addTile();
    }



void TileEditor::addTile() {
    mAddAction = true;
    mTileToEdit.finishEdit();
    mainTilePicker->setSelectedResource( mTileToEdit, true );
    mAddAction = false;
    }

