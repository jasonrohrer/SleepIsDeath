#include "StateObjectPicker.h"

#include "TilePicker.h"

extern TilePicker *mainTilePicker;


// all share one stack
SimpleVector<uniqueID> globalObjectStack;



StateObjectPicker::StateObjectPicker(
    double inAnchorX, double inAnchorY )
    : ResourcePicker<StateObject>( inAnchorX, inAnchorY, &globalObjectStack ) {

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




StateObjectPicker::~StateObjectPicker() {

    }




Sprite *StateObjectPicker::getButtonGridBackground() {
    if( mTransButton->getSelected() ) {
        return mainTilePicker->getBackgroundTile().getSprite();
        }
    else {
        // no trans
        return NULL;
        }
    }


void StateObjectPicker::actionPerformed( GUIComponent *inTarget ) {
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
        ResourcePicker<StateObject>::actionPerformed( inTarget );
        }
    }



