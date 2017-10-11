#include "SpritePicker.h"

#include "TilePicker.h"

extern TilePicker *mainTilePicker;


// all share one stack
SimpleVector<uniqueID> globalSpriteStack;


SpritePicker::SpritePicker(
    double inAnchorX, double inAnchorY )
    : ResourcePicker<SpriteResource>( inAnchorX, inAnchorY, 
                                      &globalSpriteStack ) {

    mainTilePicker->addActionListener( this );


    mTransButton = new SelectableButtonGL( 
        new Sprite( "transSmall.tga", true ),
        1,
        mAnchorX + 2 * (mWidth - 12) / 3 + 2,
        mAnchorY,
        12, 12 );
    
    add( mTransButton );
    
    mTransButton->setSelected( true );
    
    mTransButton->addActionListener( this );
    
    mTransButton->setToolTip( "tip_trans" );
    }




SpritePicker::~SpritePicker() {

    }




Sprite *SpritePicker::getButtonGridBackground() {
    if( mTransButton->getSelected() ) {
        return mainTilePicker->getBackgroundTile().getSprite();
        }
    else {
        // no trans
        return NULL;
        }
    }


void SpritePicker::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == mainTilePicker ) {
        // tile changed, new background sprite in result display
    
        // generate new result sprites
        getFreshResults();

        refreshSelectedResourceSprite();
        }
    else if( inTarget == mTransButton ) {
        // toggle
        mTransButton->setSelected( ! mTransButton->getSelected() );

        getFreshResults();

        refreshSelectedResourceSprite();
        }
    else {
        ResourcePicker<SpriteResource>::actionPerformed( inTarget );
        }
    }



void SpritePicker::cloneState( 
    ResourcePicker<SpriteResource> *inOtherPicker ) {
    
    SpritePicker *other = (SpritePicker *)inOtherPicker;
    
    other->mTransButton->setSelected( mTransButton->getSelected() );
    
    other->getFreshResults();
    other->refreshSelectedResourceSprite();
    

    ResourcePicker<SpriteResource>::cloneState( inOtherPicker );
    }



char SpritePicker::isTransOn() {
    return mTransButton->getSelected();
    }



SelectableButtonGL *SpritePicker::getTransButton() {
    return mTransButton;
    }






