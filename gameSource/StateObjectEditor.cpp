#include "StateObjectEditor.h"
#include "StateObjectPicker.h"
#include "SpritePicker.h"
#include "SpriteEditor.h"
#include "TilePicker.h"
#include "BorderPanel.h"
#include "DragAndDropManager.h"
#include "labels.h"
#include "ToolTipManager.h"


#include "minorGems/util/TranslationManager.h"



#include <stdio.h>


extern SpritePicker *mainSpritePickerLower;
extern SpritePicker *mainSpritePicker;
extern StateObjectPicker *mainStateObjectPicker;
extern TilePicker *mainTilePicker;

extern SpriteEditor *mainSpriteEditor;

extern DragAndDropManager *mainDragAndDrop;


extern int gameWidth, gameHeight;



template <>
void SizeLimitedVector<StateObject>::deleteElementOfType(
    StateObject inElement ) {
    // no delete necessary
    }




StateObjectEditor::StateObjectEditor( ScreenGL *inScreen )
        : Editor( inScreen ),
          mSelectedLayer( -1 ),
          mUndoStack( MAX_UNDOS, false ),
          mEditingSelectedLayer( false ),
          mDragFromObjectPicker( false ),
          mClickStartedOnLayer( false ),
          // gen a fake ID
          mCurrentWorkingStateID( makeUniqueID( (unsigned char*)"currObject", 
                                                strlen( "currObject" ) ) ) {
    
    AppLog::info( "Constructing state object editor\n" );
    

    mCloseButton->setToolTip( "tip_closeEdit_object" );


    mSidePanel->add( mainStateObjectPicker );

    mainStateObjectPicker->addActionListener( this );


    mSidePanel->add( mainSpritePickerLower );

    mainSpritePickerLower->addActionListener( this );
    
    mainSpritePicker->addActionListener( this );



    mStateDisplay = new StateObjectDisplay( 8, gameWidth - 48 - G * P );
    mMainPanel->add( mStateDisplay );
    mStateDisplay->addActionListener( this );
    
    

    mEditSpriteButton = 
        new EditButtonGL( 
            mainSpritePickerLower->getAnchorX() - 1,
            mainSpritePickerLower->getAnchorY() + 
            mainSpritePickerLower->getHeight() + 1,
            8,
            8 );
    
    mSidePanel->add( mEditSpriteButton );
    
    mEditSpriteButton->addActionListener( this );
    mEditSpriteButton->setToolTip( "tip_edit_sprite" );


    double offset = P;

    double buttonSize = P;

    


    mGridButton = new SelectableButtonGL( 
        new Sprite( "grid.tga", true ),
        1,
        72, 42, 20, 20 );
    
    mMainPanel->add( mGridButton );
    
    mGridButton->setSelected( true );
    
    mGridButton->addActionListener( this );
    
    mGridButton->setToolTip( "tip_obj_grid" );
    


    mZoomButton = new SpriteButtonGL( new Sprite( "zoom.tga", true ),
                                      1,
                                      8, 44, 16, 16 );
    
    mUnZoomButton = new SpriteButtonGL( new Sprite( "unzoom.tga", true ),
                                        1,
                                        26, 44, 16, 16 );
    mMainPanel->add( mZoomButton );
    mMainPanel->add( mUnZoomButton );
    mZoomButton->addActionListener( this );
    mUnZoomButton->addActionListener( this );
    

        
    


    mZoomButton->setToolTip( "tip_zoom" );
    mUnZoomButton->setToolTip( "tip_unzoom" );
    

    mTransButton = new SelectableButtonGL( 
        new Sprite( "trans.tga", true ),
        1,
        50, 42, 20, 20 );
    
    mMainPanel->add( mTransButton );
    
    mTransButton->setSelected( true );
    
    mTransButton->addActionListener( this );

    mTransButton->setToolTip( "tip_obj_trans" );
    
        


    EightPixelLabel *fieldLabel = new EightPixelLabel( 150, 54,
                                                       "objectName" );
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
    double gridEdge = 8 + G * buttonSize;
    
    double extra = gameHeight - gridEdge;
    

    
    double sideButtonsX = gridEdge + (extra - 16) / 2;


    // center it vertically on room picker
    double addY = mainStateObjectPicker->getAnchorY() +
        mainStateObjectPicker->getHeight() - 15;


    mAddButton = new AddButtonGL( sideButtonsX, 
                                  addY, 
                                  16, 16 );
    mMainPanel->add( mAddButton );
    mAddButton->addActionListener( this );
    mAddButton->setToolTip( "tip_addObject" );
    
    mAddAction = false;


    mClearButton = new ClearButtonGL( sideButtonsX, 
                                    addY - 20, 
                                    16, 16 );
    mMainPanel->add( mClearButton );
    mClearButton->addActionListener( this );

    
    mCWButton = new RotateCWButtonGL( sideButtonsX, 
                                      addY - 40, 
                                      16, 16 );
    mMainPanel->add( mCWButton );
    mCWButton->addActionListener( this );
    mCWButton->setToolTip( "tip_rotateLayer" );


    mFlipHButton = new FlipHButtonGL( sideButtonsX, 
                                   addY - 60, 
                                   16, 16 );
    mMainPanel->add( mFlipHButton );
    mFlipHButton->addActionListener( this );
    mFlipHButton->setToolTip( "tip_flipLayer" );
    


    Color *thumbColor = new Color( .5, .5, .5, .5 );
    Color *borderColor = new Color( .35, .35, .35, .35 );
    
    
    mLayerTransSlider = new ToolTipSliderGL( sideButtonsX - 1 - 3, addY - 80 + 9,
                                             16, 10,
                                             NULL, 0,
                                             new Color( 0, 0, 0, 1 ),
                                             new Color( 1, 1, 1, 1 ),
                                             thumbColor->copy(),
                                             borderColor->copy(),
                                             1, 4, 1 );
    
    mMainPanel->add( mLayerTransSlider );
    mLayerTransSlider->setThumbPosition( 1.0 );
    mLayerTransSlider->addActionListener( this );
    mLayerTransSlider->setToolTip( "tip_fadeLayer" );


    delete thumbColor;
    delete borderColor;
    



    double layerButtonSize = P + 4;
    
    mObjectLayerViewButton = new TwoSpriteButtonGL( 
        NULL, NULL, 1,
        108, 42,
        layerButtonSize,
        layerButtonSize );
    
    mMainPanel->add( mObjectLayerViewButton );



    mObjectPrevLayerButton = new LeftButtonGL( 
        mObjectLayerViewButton->getAnchorX() - 8,
        mObjectLayerViewButton->getAnchorY() + layerButtonSize - 8,
        8,
        8 );
    mMainPanel->add( mObjectPrevLayerButton );
    
    mObjectPrevLayerButton->addActionListener( this );

    mObjectPrevLayerButton->setToolTip( "tip_prev_layer" );


    mObjectNextLayerButton = new RightButtonGL( 
        mObjectLayerViewButton->getAnchorX() + layerButtonSize,
        mObjectLayerViewButton->getAnchorY() + layerButtonSize - 8,
        8,
        8 );
    mMainPanel->add( mObjectNextLayerButton );
    
    mObjectNextLayerButton->addActionListener( this );

    mObjectNextLayerButton->setToolTip( "tip_next_layer" );
    


    static rgbaColor black = {{0,0,0,255}};

    mObjectBottomLayerButton = 
        new SpriteButtonGL( new Sprite( "first.tga", true ),
                            1,
                            mObjectPrevLayerButton->getAnchorX(), 
                            mObjectPrevLayerButton->getAnchorY() - 12,
                            8, 8 );
    mObjectBottomLayerButton->setBorderColor( black );

    mMainPanel->add( mObjectBottomLayerButton );
    
    mObjectBottomLayerButton->addActionListener( this );

    mObjectBottomLayerButton->setToolTip( "tip_bottom_layer" );



    mObjectTopLayerButton = 
        new SpriteButtonGL( new Sprite( "last.tga", true ),
                            1,
                            mObjectNextLayerButton->getAnchorX(), 
                            mObjectNextLayerButton->getAnchorY() - 12,
                            8, 8 );
    mObjectTopLayerButton->setBorderColor( black );

    mMainPanel->add( mObjectTopLayerButton );
    
    mObjectTopLayerButton->addActionListener( this );

    mObjectTopLayerButton->setToolTip( "tip_top_layer" );



    mLayerUpButton = 
        new SpriteButtonGL( new Sprite( "layerUp.tga", true ),
                            1,
                            mObjectNextLayerButton->getAnchorX() + 10, 
                            mObjectNextLayerButton->getAnchorY() - 1,
                            8, 8 );
    mLayerUpButton->setBorderColor( black );

    mMainPanel->add( mLayerUpButton );
    
    mLayerUpButton->addActionListener( this );

    mLayerUpButton->setToolTip( "tip_layer_up" );

    mLayerDownButton = 
        new SpriteButtonGL( new Sprite( "layerDown.tga", true ),
                            1,
                            mObjectNextLayerButton->getAnchorX() + 10, 
                            mObjectNextLayerButton->getAnchorY() - 11,
                            8, 8 );
    mLayerDownButton->setBorderColor( black );

    mMainPanel->add( mLayerDownButton );
    
    mLayerDownButton->addActionListener( this );

    mLayerDownButton->setToolTip( "tip_layer_down" );


    


    mLayerGlowButton = new ToggleSpriteButtonGL( 
        new Sprite( "glow.tga", true ), 
        new Sprite( "noGlow.tga", true ), 
        1,
        mLayerTransSlider->getAnchorX() + mLayerTransSlider->getWidth(),
        mLayerTransSlider->getAnchorY() + 1,
        8, 8 );
    

    mLayerGlowButton->setToolTip( "tip_glowLayer" );
    mLayerGlowButton->setSecondToolTip( "tip_noGlowLayer" );

    mMainPanel->add( mLayerGlowButton );
    
    mLayerGlowButton->addActionListener( this );



    double replaceY = mainSpritePickerLower->getAnchorY() +
        mainSpritePickerLower->getHeight() - 15;
    
    mReplaceSpriteButton = new SpriteButtonGL( 
        new Sprite( "replaceSprite.tga", true ),
        1,
        sideButtonsX, replaceY, 16, 16 );
    
    mMainPanel->add( mReplaceSpriteButton );
    
    mReplaceSpriteButton->addActionListener( this );
    
    mReplaceSpriteButton->setToolTip( "tip_replace_layer" );


    // reuse object add sprite
    mAddSpriteButton = new KeyEquivButtonGL( 
        new Sprite( "addObject.tga", true ),
        1,
        sideButtonsX, replaceY - 20, 16, 16, 'a', 'A' );
    
    mMainPanel->add( mAddSpriteButton );
    
    mAddSpriteButton->addActionListener( this );
    
    mAddSpriteButton->setToolTip( "tip_new_layer" );



    mEditSelectedSpriteButton = new KeyEquivButtonGL( 
        new Sprite( "editSprite.tga", true ),
        1,
        sideButtonsX, replaceY + 40, 16, 16, 'e', 'E' );
    
    mMainPanel->add( mEditSelectedSpriteButton );
    
    mEditSelectedSpriteButton->addActionListener( this );
    
    mEditSelectedSpriteButton->setToolTip( "tip_edit_layer" );

    
    mRemoveSpriteButton = new KeyEquivButtonGL( 
        new Sprite( "removeSprite.tga", true ),
        1,
        sideButtonsX, replaceY + 20, 16, 16,
        'x', 'X' );
    
    mMainPanel->add( mRemoveSpriteButton );
    
    mRemoveSpriteButton->addActionListener( this );
    
    mRemoveSpriteButton->setToolTip( "tip_delete_layer" );


    // obj 0 starts out selected, can't be deleted or edited
    mRemoveSpriteButton->setEnabled( false );
    mEditSelectedSpriteButton->setEnabled( false );




    
    double undoButtonY = gameWidth - ( 48 + offset + (G - 1) * buttonSize );

    mUndoButton = new UndoButtonGL( sideButtonsX, undoButtonY, 16, 16 );
    mMainPanel->add( mUndoButton );
    mUndoButton->addActionListener( this );
    mUndoButton->setEnabled( false );

    mRedoButton = new RedoButtonGL( sideButtonsX, undoButtonY + 19, 16, 16 );
    mMainPanel->add( mRedoButton );
    mRedoButton->addActionListener( this );
    mRedoButton->setEnabled( false );


    mIgnoreSliders = false;
    mUndoOrRedoAction = false;

    
    setStateObjectToEdit( mStateObjectToEdit );

    
    // start one step zoomed in (can zoom in and out)
    mStateDisplay->setZoom( 2 );
    mZoomButton->setEnabled( true );
    mUnZoomButton->setEnabled( true );



    mNoDropImage = readTGA( "noDrop.tga" );
    mCanDropImage = readTGA( "canDrop.tga" );



    mainTilePicker->addActionListener( this );



    // dnd on top of side panel
    postConstructionSide();
    
    mMainPanel->add( mainDragAndDrop );
    
    // but *below* special main panel widgets
    postConstructionMain();
    }



StateObjectEditor::~StateObjectEditor() {
    mSidePanel->remove( mainSpritePickerLower );
    mSidePanel->remove( mainStateObjectPicker );
    
    mMainPanel->remove( mainDragAndDrop );

    delete mNoDropImage;
    delete mCanDropImage;
    }



void StateObjectEditor::setStateObjectToEdit( StateObject inStateObject ) {

    // clear old working usages
    removeUsages( mCurrentWorkingStateID );

    
    mStateObjectToEdit = inStateObject;
    
    //mainRoomPicker->setSelectedResource( mStateObjectToEdit->mRoom );
    
    mStateDisplay->setStateObject( mStateObjectToEdit, 
                                   mTransButton->getSelected() );


    //int selectedObj = mStateObjectToEdit->getSelectedObject();
    //int selectedLayer = mStateObjectToEdit->getSelectedLayer();
    
    int numLayers = mStateObjectToEdit.getNumLayers();
    
    if( mSelectedLayer >= numLayers ) {
        mSelectedLayer = numLayers - 1;
        }
    
    mStateObjectToEdit.mSelectedLayer = mSelectedLayer;
    
        

    
    mRemoveSpriteButton->setEnabled( true );
    mReplaceSpriteButton->setEnabled( true );
    mEditSelectedSpriteButton->setEnabled( true );
    if( mSelectedLayer == -1 ) {
        // can't manipulate anchor
        mRemoveSpriteButton->setEnabled( false );
        mReplaceSpriteButton->setEnabled( false );
        mEditSelectedSpriteButton->setEnabled( false );
        }
    

    char useTrans = mTransButton->getSelected();
    
    if( useTrans ) {
        // backdrop behind sprite when transparency shown
        mObjectLayerViewButton->setSprite( 
            mainTilePicker->getBackgroundTile().getSprite() );
        }
    else {
        // no backdrop
        mObjectLayerViewButton->setSprite( NULL );
        }


    if( mSelectedLayer == -1 ) {
        // display anchor
        mObjectLayerViewButton->setFrontSprite( 
            new Sprite( "anchor.tga", true ) );
        
        mObjectPrevLayerButton->setEnabled( false );
        mCWButton->setToolTip( "tip_rotateWholeObject" );
        mFlipHButton->setToolTip( "tip_flipWholeObject" );
        mLayerTransSlider->setToolTip( "tip_fadeWholeObject" );
        
        mLayerGlowButton->setToolTip( "tip_glowWholeObject" );
        mLayerGlowButton->setSecondToolTip( "tip_noGlowWholeObject" );
        

        mObjectNextLayerButton->setEnabled(
            mStateObjectToEdit.getNumLayers() > 0 );


        mObjectBottomLayerButton->setEnabled( false );

        // don't show if redundant
        mObjectTopLayerButton->setEnabled( 
            mStateObjectToEdit.getNumLayers() > 1 );

        mLayerUpButton->setEnabled( false );
        mLayerDownButton->setEnabled( false );
        }
    else {
        // display selected sprite
        SpriteResource resource( 
            mStateObjectToEdit.getLayerSprite( mSelectedLayer ) ); 
            
        mObjectLayerViewButton->setFrontSprite( 
            resource.getSprite( useTrans ) );

        mObjectPrevLayerButton->setEnabled( true );
        mCWButton->setToolTip( "tip_rotateLayer" );
        mFlipHButton->setToolTip( "tip_flipLayer" );
        mLayerTransSlider->setToolTip( "tip_fadeLayer" );

        mLayerGlowButton->setToolTip( "tip_glowLayer" );
        mLayerGlowButton->setSecondToolTip( "tip_noGlowLayer" );

        
        mObjectNextLayerButton->setEnabled(
            mStateObjectToEdit.getNumLayers() - 1 > mSelectedLayer );

        
                                    
        // don't show if redundant
        mObjectBottomLayerButton->setEnabled( mSelectedLayer > 0 );

        mObjectTopLayerButton->setEnabled( 
            mStateObjectToEdit.getNumLayers() - 2 > mSelectedLayer );


        mLayerUpButton->setEnabled( mObjectNextLayerButton->isEnabled() );
        mLayerDownButton->setEnabled( mObjectBottomLayerButton->isEnabled() );
        }


    char *name = mStateObjectToEdit.getStateObjectName();
    
    mNameField->setText( name );
    mNameField->setCursorPosition( strlen( name ) );
    
    delete [] name;


    if( mSelectedLayer >= 0 ) {
        // auto-change selected sprite in picker
        SpriteResource resource( 
            mStateObjectToEdit.getLayerSprite( mSelectedLayer) );
        
        mainSpritePickerLower->setSelectedResource( resource );
        }

    mIgnoreSliders = true;

    if( mSelectedLayer >= 0 ) {
        mLayerTransSlider->setEnabled( true );
        
        // only 32 pixels in slider... avoid round-off errors
        mLayerTransSlider->setThumbPosition( 
            (int)( mStateObjectToEdit.getLayerTrans( mSelectedLayer ) 
                   / 255.0 * 32 ) / 32 );

        mLayerGlowButton->setState( 
            mStateObjectToEdit.getLayerGlow( mSelectedLayer ) ); 
        }
    else {
        
        mLayerTransSlider->setThumbPosition( 1 );
        
        // show glow button default to on or off for whole object?

        int countGlowing = 0;
        
        int numLayers = mStateObjectToEdit.getNumLayers();
        
        for( int i=0; i<numLayers; i++ ) {
            if( mStateObjectToEdit.getLayerGlow( i ) ) {
                countGlowing ++;
                }
            }

        // more than half glowing?
        mLayerGlowButton->setState( countGlowing > numLayers / 2 );
        }
    
    
    mIgnoreSliders = false;

    
    
    mAddSpriteButton->setEnabled( mStateObjectToEdit.canAdd( 1 ) );


    // no longer valid
    mWholeDragOffsets.deleteAll();


    // add usages for current working object
    for( int i=0; i<numLayers; i++ ) {
        addUsage( mCurrentWorkingStateID, 
                  mStateObjectToEdit.getLayerSprite( i ) );
        }
    mainSpritePicker->recheckDeletable();
    mainSpritePickerLower->recheckDeletable();
    }


void StateObjectEditor::clearRedoStack() {
    mRedoStack.deleteAll();
    mRedoButton->setEnabled( false );
    }


void StateObjectEditor::saveUndoPoint() {
    mUndoStack.push_back( mStateObjectToEdit );
    mUndoButton->setEnabled( true );
    
    // new branch, old redo future impossible
    clearRedoStack();
    }



void StateObjectEditor::generateDraggingOffsets() {
    mWholeDragOffsets.deleteAll();
                    
    int numLayers = mStateObjectToEdit.getNumLayers();
    
    for( int i=0; i<numLayers; i++ ) {
        mWholeDragOffsets.push_back(
            mStateObjectToEdit.getLayerOffset( i ) );
        }
    }



void StateObjectEditor::actionPerformed( GUIComponent *inTarget ) {
    // superclass
    Editor::actionPerformed( inTarget );
    

    if( inTarget == mainSpritePickerLower ) {
        if( mainSpritePickerLower->wasLastActionFromPress() &&
            this->isVisible() ) {

            SpriteResource resource = 
                mainSpritePickerLower->getDraggedResource();
                
            intPair zeroOffset = { 0, 0 };
                
                
            intPair *offsets = new intPair[ 2 ];
            Sprite **sprites = new Sprite*[ 2 ];
            float *trans = new float[ 2 ];
            char *glows = new char[ 2 ];

            offsets[0] = zeroOffset;
            sprites[0] = resource.getSprite( false, true );
            trans[0] = 1.0f;
            glows[0] = false;
                
            offsets[1] = zeroOffset;
            trans[1] = 0.5f;
            glows[1] = false;
                
            if( !mStateObjectToEdit.canAdd( 1 ) ) {
                sprites[1] = new Sprite( mNoDropImage, true );
                
                ToolTipManager::setTip( 
                    (char*)TranslationManager::translate( "tip_objectFull" ) );
                }
            else {
                sprites[1] = new Sprite( mCanDropImage, true );

                ToolTipManager::setTip( 
                    (char*)TranslationManager::translate( 
                        "tip_draggingSprite" ) );
                }
            ToolTipManager::freeze( true );

            mainDragAndDrop->setSprites( 2, sprites, offsets,
                                         trans, glows,
                                         mStateDisplay->getZoom() );
            mDragFromObjectPicker = false;
            }
        }
    else if( inTarget == mainSpritePicker ) {
                
        if( mEditingSelectedLayer &&
            mainSpriteEditor->isVisible() ) {
                    
            // auto-replace
            saveUndoPoint();
        
            SpriteResource resource = 
                mainSpritePicker->getSelectedResource();
        

            mStateObjectToEdit.editLayerSprite( mSelectedLayer, 
                                                resource.getUniqueID() );
        
            setStateObjectToEdit( mStateObjectToEdit );
            }
        }
    else if( inTarget == mainStateObjectPicker ) {

        if( mainStateObjectPicker->wasLastActionFromPress() &&
            this->isVisible() ) {
            StateObject resource = 
                mainStateObjectPicker->getDraggedResource();
            
            int numSprites = resource.getNumLayers();
            

            int numTotalSprites = numSprites + 1;
            
            intPair *offsets = new intPair[ numTotalSprites ];
            Sprite **sprites = new Sprite*[ numTotalSprites ];
            float *trans = new float[ numTotalSprites ];
            char *glows = new char[ numTotalSprites ];
            
            for( int i=0; i<numSprites; i++ ) {
                offsets[i] = resource.getLayerOffset( i );
                trans[i] = resource.getLayerTrans( i ) / 255.0f;
                glows[i] = resource.getLayerGlow( i );
                SpriteResource r( resource.getLayerSprite( i ) );
                sprites[i] = r.getSprite( false, true );
                }
            
            Sprite *topSprite;
            if( !mStateObjectToEdit.canAdd( numSprites ) ) {
                // buster on top
                topSprite = new Sprite( mNoDropImage, true );
                ToolTipManager::setTip( 
                    (char*)TranslationManager::translate( "tip_objectFull" ) );
                }
            else {
                // nothing on top
                topSprite = new Sprite( mCanDropImage, true );

                ToolTipManager::setTip( 
                    (char*)TranslationManager::translate( 
                        "tip_draggingObjectObject" ) );
                }
            ToolTipManager::freeze( true );

            offsets[numSprites].x = 0;
            offsets[numSprites].y = 0;
            trans[numSprites] = 0.5f;
            glows[numSprites] = false;
            sprites[numSprites] = topSprite;
                
                
            mainDragAndDrop->setSprites( numTotalSprites, sprites, offsets,
                                         trans, glows,
                                         mStateDisplay->getZoom() );
            mDragFromObjectPicker = true;
            }
        // ignore if caused by our own Add action
        else if( ! mAddAction &&
            ! mainStateObjectPicker->wasLastActionFromPress() ) {
            
            // will change object
                        
            mUndoStack.push_back( mStateObjectToEdit );
            mUndoButton->setEnabled( true );
            
            StateObject resource = 
                mainStateObjectPicker->getSelectedResource();
            
            int numLayers = resource.getNumLayers();
    
            // always jump to top layer
            mSelectedLayer = numLayers - 1;
            resource.mSelectedLayer = mSelectedLayer;

            setStateObjectToEdit( resource );            
            
            mainSpritePickerLower->recheckDeletable();
            mainSpritePicker->recheckDeletable();

            // new branch... "redo" future now impossible
            clearRedoStack();
            }
        }
    else if( inTarget == mainTilePicker ) {
        // tile change... tile used behind sprite transparency
        
        // don't refresh entire object, no need to!
        char useTrans = mTransButton->getSelected();
    
        if( useTrans ) {
            // backdrop behind sprite when transparency shown
            mObjectLayerViewButton->setSprite( 
                mainTilePicker->getBackgroundTile().getSprite() );
            }
        }
    else if( inTarget == mNameField ) {
                
        mUndoStack.push_back( mStateObjectToEdit );
        mUndoButton->setEnabled( true );

        // new branch... "redo" future now impossible
        clearRedoStack();
        
        mStateObjectToEdit.editStateObjectName( mNameField->getText() );
        }
    else if( inTarget == mEditSpriteButton ) {
        mEditingSelectedLayer = false;
        showSpriteEditor();
        }
    else if( inTarget == mEditSelectedSpriteButton ) {
        mEditingSelectedLayer = true;

        // make sure selected one is chosen in picker
        setStateObjectToEdit( mStateObjectToEdit );

        showSpriteEditor();
        }
    else if( inTarget == mUndoButton ) {
        mUndoOrRedoAction = true;
        
        int lastIndex = mUndoStack.size() - 1;
        
        StateObject last = *( mUndoStack.getElement( lastIndex ) );
        mUndoStack.deleteElement( lastIndex );
        if( mUndoStack.size() == 0 ) {
            mUndoButton->setEnabled( false );
            }
        
        mRedoStack.push_back( mStateObjectToEdit );
        mRedoButton->setEnabled( true );
        
        // restore selected layer
        mSelectedLayer = last.mSelectedLayer;

        setStateObjectToEdit( last );
        
        mUndoOrRedoAction = false;
        }
    else if( inTarget == mRedoButton ) {
        mUndoOrRedoAction = true;

        int nextIndex = mRedoStack.size() - 1;
        
        StateObject next = *( mRedoStack.getElement( nextIndex ) );
        mRedoStack.deleteElement( nextIndex );
        if( mRedoStack.size() == 0 ) {
            mRedoButton->setEnabled( false );
            }
        
        mUndoStack.push_back( mStateObjectToEdit );
        mUndoButton->setEnabled( true );
        
        // restore selected layer
        mSelectedLayer = next.mSelectedLayer;
        
        setStateObjectToEdit( next );
        
        mUndoOrRedoAction = false;
        }
    else if( inTarget == mAddButton ) {
        addStateObject();
        }
    else if( inTarget == mAddSpriteButton ) {
        saveUndoPoint();
        
        SpriteResource resource = mainSpritePickerLower->getSelectedResource();
        
        char added = mStateObjectToEdit.addLayer( 
            resource.getUniqueID(), mSelectedLayer );
        
        if( added ) {    
            // select newly-added layer
            mSelectedLayer ++;
            mStateObjectToEdit.mSelectedLayer = mSelectedLayer;

            if( mStateDisplay->mMouseHover ) {
                // "button press" while over display (not over button)
                // must be Ctrl-a keyboard shortcut
                // Add sprite at mouse location on display
                intPair offset = 
                    { mStateDisplay->mLastHoverX ,
                      mStateDisplay->mLastHoverY };
                
                mStateObjectToEdit.editLayerOffset( 
                    mSelectedLayer, 
                    offset );
                }
            // else leave with 0 offsets, centered
            }
        
        setStateObjectToEdit( mStateObjectToEdit );
        }
    else if( inTarget == mRemoveSpriteButton ) {
        saveUndoPoint();
        
        if( mSelectedLayer >= 0 ) {
            mStateObjectToEdit.deleteLayer( mSelectedLayer );
            
            int numLayersLeft = mStateObjectToEdit.getNumLayers();
            
            if( mSelectedLayer >= numLayersLeft ) {
                mSelectedLayer --;
                }
            }
        
        mStateObjectToEdit.mSelectedLayer = mSelectedLayer;
        setStateObjectToEdit( mStateObjectToEdit );
        }
    else if( inTarget == mReplaceSpriteButton ) {
        saveUndoPoint();
        
        SpriteResource resource = mainSpritePickerLower->getSelectedResource();
        

        mStateObjectToEdit.editLayerSprite( mSelectedLayer, 
                                            resource.getUniqueID() );
        
        setStateObjectToEdit( mStateObjectToEdit );
        }
    else if( inTarget == mObjectNextLayerButton ) {
        mSelectedLayer ++;
        mStateObjectToEdit.mSelectedLayer = mSelectedLayer;

        setStateObjectToEdit( mStateObjectToEdit );
        }
    else if( inTarget == mObjectPrevLayerButton ) {
        mSelectedLayer --;
        mStateObjectToEdit.mSelectedLayer = mSelectedLayer;
        
        setStateObjectToEdit( mStateObjectToEdit );
        }
    else if( inTarget == mObjectBottomLayerButton ) {
        mSelectedLayer = -1;
        mStateObjectToEdit.mSelectedLayer = mSelectedLayer;

        setStateObjectToEdit( mStateObjectToEdit );
        }
    else if( inTarget == mObjectTopLayerButton ) {
        mSelectedLayer = mStateObjectToEdit.getNumLayers() - 1;
        mStateObjectToEdit.mSelectedLayer = mSelectedLayer;

        setStateObjectToEdit( mStateObjectToEdit );
        }
    else if( inTarget == mLayerUpButton ) {
        saveUndoPoint();
        
        mStateObjectToEdit.moveLayerUp( mSelectedLayer );
        mSelectedLayer ++;
        mStateObjectToEdit.mSelectedLayer = mSelectedLayer;

        setStateObjectToEdit( mStateObjectToEdit );
        }
    else if( inTarget == mLayerDownButton ) {
        saveUndoPoint();
        
        mStateObjectToEdit.moveLayerDown( mSelectedLayer );
        mSelectedLayer --;
        mStateObjectToEdit.mSelectedLayer = mSelectedLayer;

        setStateObjectToEdit( mStateObjectToEdit );
        }
    else if( inTarget == mTransButton ) {
        // toggle
        mTransButton->setSelected( ! mTransButton->getSelected() );

        setStateObjectToEdit( mStateObjectToEdit );
        }
    else if( inTarget == mGridButton ) {
        // toggle
        mGridButton->setSelected( ! mGridButton->getSelected() );
        
        mStateDisplay->showGrid( mGridButton->getSelected() );
        }
    else if( inTarget == mZoomButton ) {
        mStateDisplay->setZoom( mStateDisplay->getZoom() + 1 );
        mUnZoomButton->setEnabled( true );
        if( mStateDisplay->getZoom() > 2 ) {
            mZoomButton->setEnabled( false );
            }
        }
    else if( inTarget == mUnZoomButton ) {
        mStateDisplay->setZoom( mStateDisplay->getZoom() - 1 );
        mZoomButton->setEnabled( true );
        if( mStateDisplay->getZoom() == 1 ) {
            mUnZoomButton->setEnabled( false );
            }
        }
    else if( inTarget == mClearButton ) {
        saveUndoPoint();

        int numLayers = mStateObjectToEdit.getNumLayers();
        
        for( int i=numLayers-1; i>=0; i-- ) {
            mStateObjectToEdit.deleteLayer( i );
            }
        mSelectedLayer = -1;
        mStateObjectToEdit.mSelectedLayer = mSelectedLayer;

        
        setStateObjectToEdit( mStateObjectToEdit );
        }
    else if( inTarget == mCWButton ) {
        saveUndoPoint();

        if( mSelectedLayer >= 0 ) {
            // rotate just one layer
        
            uniqueID id = mStateObjectToEdit.getLayerSprite( mSelectedLayer );

            SpriteResource oldSpriteState( id );
            
            SpriteResource newSpriteState( id );
            
            for( int y=0; y<P; y++ ) {
                for( int x=0; x<P; x++ ) {
                    rgbaColor flipColor = 
                        oldSpriteState.getColor( y, P - x - 1 );
                    char flipTrans =
                        oldSpriteState.getTrans( y, P - x - 1 );
                    
                    newSpriteState.editSprite( x, y, flipColor );
                    newSpriteState.editTrans( x, y, flipTrans );
                    }
                }
            newSpriteState.finishEdit();
            
            mStateObjectToEdit.editLayerSprite( 
                mSelectedLayer, newSpriteState.getUniqueID() );
            }
        else {
            // rotate whole object
            int numLayers = mStateObjectToEdit.getNumLayers();
        
            for( int i=0; i<numLayers; i++ ) {
                uniqueID id = 
                    mStateObjectToEdit.getLayerSprite( i );

                SpriteResource oldSpriteState( id );
            
                SpriteResource newSpriteState( id );
            
                for( int y=0; y<P; y++ ) {
                    for( int x=0; x<P; x++ ) {
                        rgbaColor flipColor = 
                            oldSpriteState.getColor( y, P - x - 1 );
                        char flipTrans =
                            oldSpriteState.getTrans( y, P - x - 1 );
                        
                        newSpriteState.editSprite( x, y, flipColor );
                        newSpriteState.editTrans( x, y, flipTrans );
                        }
                    }
                newSpriteState.finishEdit();
            
                mStateObjectToEdit.editLayerSprite( 
                    i, newSpriteState.getUniqueID() );

                // rotate layer position too
                intPair oldOffset = mStateObjectToEdit.getLayerOffset( i );
                
                intPair newOffset = { oldOffset.y, - oldOffset.x };
                
                mStateObjectToEdit.editLayerOffset( i, newOffset );
                }
            }

        setStateObjectToEdit( mStateObjectToEdit );
        }
    else if( inTarget == mFlipHButton ) {
        saveUndoPoint();

        if( mSelectedLayer >= 0 ) {
            // flip just one layer
        
            uniqueID id = mStateObjectToEdit.getLayerSprite( mSelectedLayer );

            SpriteResource oldSpriteState( id );
            
            SpriteResource newSpriteState( id );
            
            for( int y=0; y<P; y++ ) {
                for( int x=0; x<P; x++ ) {
                    rgbaColor flipColor = 
                        oldSpriteState.getColor( P - x - 1, y );
                    char flipTrans =
                        oldSpriteState.getTrans( P - x - 1, y );
                        
                    newSpriteState.editSprite( x, y, flipColor );
                    newSpriteState.editTrans( x, y, flipTrans );
                    }
                }
            newSpriteState.finishEdit();
            
            mStateObjectToEdit.editLayerSprite( 
                mSelectedLayer, newSpriteState.getUniqueID() );
            }
        else {
            // flip whole object
            int numLayers = mStateObjectToEdit.getNumLayers();
        
            for( int i=0; i<numLayers; i++ ) {
                uniqueID id = 
                    mStateObjectToEdit.getLayerSprite( i );

                SpriteResource oldSpriteState( id );
            
                SpriteResource newSpriteState( id );
            
                for( int y=0; y<P; y++ ) {
                    for( int x=0; x<P; x++ ) {
                        rgbaColor flipColor = 
                            oldSpriteState.getColor( P - x - 1, y );
                        char flipTrans =
                            oldSpriteState.getTrans( P - x - 1, y );
                                                
                        newSpriteState.editSprite( x, y, flipColor );
                        newSpriteState.editTrans( x, y, flipTrans );
                        }
                    }
                newSpriteState.finishEdit();
            
                mStateObjectToEdit.editLayerSprite( 
                    i, newSpriteState.getUniqueID() );

                // flip layer position too
                intPair oldOffset = mStateObjectToEdit.getLayerOffset( i );
                
                intPair newOffset = { - oldOffset.x, oldOffset.y };
                
                mStateObjectToEdit.editLayerOffset( i, newOffset );
                }
            }

        setStateObjectToEdit( mStateObjectToEdit );
        }
    else if( inTarget == mLayerTransSlider && ! mIgnoreSliders ) {
        if( mLayerTransSlider->mJustPressed ) {
            // first move in this adjustment, save an undo point here
            saveUndoPoint();
            }
        
        if( mSelectedLayer >= 0 ) {
            // just current layer
            mStateObjectToEdit.editLayerTrans( 
                mSelectedLayer,
                (unsigned char)( 
                    255 * mLayerTransSlider->getThumbPosition() ) );
            }
        else {
            // all layers together
            int numLayers = mStateObjectToEdit.getNumLayers();
        
            for( int i=0; i<numLayers; i++ ) {
                mStateObjectToEdit.editLayerTrans( 
                    i,
                    (unsigned char)( 
                        255 * mLayerTransSlider->getThumbPosition() ) );
                }
            }
        
            
        // don't redo sprites
        mStateDisplay->updateSpritePositions( mStateObjectToEdit );
        }
    else if( inTarget == mLayerGlowButton ) {
        if( mLayerTransSlider->mJustPressed ) {
            // first move in this adjustment, save an undo point here
            saveUndoPoint();
            }
        
        char glow = mLayerGlowButton->getState();

        if( mSelectedLayer >= 0 ) {
            // just current layer
            mStateObjectToEdit.editLayerGlow( mSelectedLayer, glow );
            }
        else {
            // all layers together
            int numLayers = mStateObjectToEdit.getNumLayers();
        
            for( int i=0; i<numLayers; i++ ) {
                mStateObjectToEdit.editLayerGlow( i, glow );
                }
            }
            
        // don't redo sprites
        mStateDisplay->updateSpritePositions( mStateObjectToEdit );
        }
    else if( inTarget == mStateDisplay &&
             ! mLayerTransSlider->mDragging ) {
        // ignore events caused by off-slider draggin
        
        
        // selected layer change?
        if( mStateDisplay->mLastActionPress ) {
            int clickedLayer = mStateDisplay->getClickedLayer( 
                mStateDisplay->mLastPixelClickX,
                mStateDisplay->mLastPixelClickY,
                &( mLayerClickOffset.x ),
                &( mLayerClickOffset.y ) );
            
            if( clickedLayer != -1 ) {
                if( clickedLayer != mSelectedLayer ) {
                    // new layer hit!

                    mSelectedLayer = clickedLayer;
                    mStateObjectToEdit.mSelectedLayer = mSelectedLayer;

                    setStateObjectToEdit( mStateObjectToEdit );
                    }
                mClickStartedOnLayer = true;
                }
            else if( clickedLayer == -1 ) {
                // same layer again, but clicking away from it
                // (causes it to jump)
                
                // center selected layer on mouse
                mLayerClickOffset.x = 0;
                mLayerClickOffset.y = 0;

                mClickStartedOnLayer = false;
                }
            }

        if( mStateDisplay->mLastActionRelease &&
            mainDragAndDrop->isDragging() ) {
            
            // add new layer (or layers, of obj dragged)

            saveUndoPoint();
        
            if( mDragFromObjectPicker ) {
                StateObject resource = 
                    mainStateObjectPicker->getDraggedResource();

                int numNewLayers = resource.getNumLayers();
                
                if( mStateObjectToEdit.canAdd( numNewLayers ) ) {
                    

                    for( int i=0; i<numNewLayers; i++ ) {
                        char added = 
                            mStateObjectToEdit.addLayer( 
                                resource.getLayerSprite( i ), 
                                mSelectedLayer );
                
                        if( added ) {
                        
                            // select newly-added layer
                            mSelectedLayer ++;
                            mStateObjectToEdit.mSelectedLayer = mSelectedLayer;

                            // position it properly based on location of drop
                            intPair layerOffset = resource.getLayerOffset( i );
                        
                            intPair offset = 
                                { mStateDisplay->mLastPixelClickX + 
                                  layerOffset.x,
                                  mStateDisplay->mLastPixelClickY + 
                                  layerOffset.y + 1 };
                
                            mStateObjectToEdit.editLayerOffset( 
                                mSelectedLayer, 
                                offset );
                            
                            mStateObjectToEdit.editLayerTrans( 
                                mSelectedLayer,
                                resource.getLayerTrans( i ) );
                            }
                        }
                    }
                }
            else {
                SpriteResource resource = 
                    mainSpritePickerLower->getDraggedResource();
                
                if( mStateObjectToEdit.canAdd( 1 ) ) {
                    
                    char added = mStateObjectToEdit.addLayer( 
                        resource.getUniqueID(), 
                        mSelectedLayer );
                
                    if( added ) {    
                        // select newly-added layer
                        mSelectedLayer ++;
                        mStateObjectToEdit.mSelectedLayer = mSelectedLayer;

                        // position it properly based on location of drop
                        intPair offset = 
                            { mStateDisplay->mLastPixelClickX,
                              mStateDisplay->mLastPixelClickY + 1 };
                
                        mStateObjectToEdit.editLayerOffset( mSelectedLayer, 
                                                            offset );
                        }
                    }
                }

            setStateObjectToEdit( mStateObjectToEdit );
            }
        else if( ! mainDragAndDrop->isDragging() ) {
            // move layer (or all layers)
            
            if( ! mStateDisplay->mLastActionRelease && 
                ! mStateDisplay->mLastActionPress ) {
                // hide selected border during drag for precise positioning
                mStateDisplay->mShowSelected = false;
                }
            

            // only save an undo point when mouse initially pressed
            // (ignore micro-state changes until release)
            if( mStateDisplay->mLastActionPress ) {
                

                if( mSelectedLayer == -1 ) {
                    
                    // save all relative offsets
                    generateDraggingOffsets();
                    
                    // move relative to first click
                    mLayerClickOffset.x = mStateDisplay->mLastPixelClickX;
                    mLayerClickOffset.y = mStateDisplay->mLastPixelClickY;

                    // whole-object moving:  doesn't matter if click started
                    // on a layer
                    saveUndoPoint();
                    }
                else {
                    // some layer selected
                    // only save undo point if click started on a layer,
                    // else nothing is going to move
                    if( mClickStartedOnLayer ) {
                        saveUndoPoint();
                        }
                    }
                }

            if( mSelectedLayer == -1 ) {
                // whole object, move all
                
                intPair offset = 
                    { mStateDisplay->mLastPixelClickX - mLayerClickOffset.x,
                      mStateDisplay->mLastPixelClickY - 
                      mLayerClickOffset.y };

                int numLayers = mStateObjectToEdit.getNumLayers();
                
                if( mWholeDragOffsets.size() != numLayers ) {
                    // our offsets aren't set up properly
                    // maybe this drag started with an off-display click?
                    generateDraggingOffsets();
                    }
                

                for( int i=0; i<numLayers; i++ ) {
                    
                    intPair thisOffset = 
                        add( offset,
                             *( mWholeDragOffsets.getElement( i ) ) );
                    
                    mStateObjectToEdit.editLayerOffset( i, 
                                                        thisOffset );
                    }
                }
            else if( mClickStartedOnLayer ) {
                // selected layer only, and mouse started over it
                
                intPair offset = 
                    { mStateDisplay->mLastPixelClickX - mLayerClickOffset.x,
                      mStateDisplay->mLastPixelClickY + 
                      mLayerClickOffset.y + 1 };
                
                mStateObjectToEdit.editLayerOffset( mSelectedLayer, 
                                                    offset );

                // no longer valid
                mWholeDragOffsets.deleteAll();
                }
            
            // no need to generate all new sprites
            mStateDisplay->updateSpritePositions( mStateObjectToEdit );
            }
        }
    }



void StateObjectEditor::editorClosing() {
    addStateObject();
    }



void StateObjectEditor::addStateObject() {
    mAddAction = true;
    mStateObjectToEdit.finishEdit();
    mainStateObjectPicker->setSelectedResource( mStateObjectToEdit, true );
    
    mainSpritePickerLower->recheckDeletable();
    mainSpritePicker->recheckDeletable();
    

    mAddAction = false;
    }

        

