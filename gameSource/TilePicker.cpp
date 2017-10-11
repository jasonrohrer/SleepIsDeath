#include "TilePicker.h"

// all share one stack
SimpleVector<uniqueID> globalTileStack;


TilePicker::TilePicker(
    double inAnchorX, double inAnchorY )
    : ResourcePicker<Tile>( inAnchorX, inAnchorY, &globalTileStack ) {
    }




TilePicker::~TilePicker() {

    }


void TilePicker::setBackgroundTile( Tile inTile ) {

    if( !equal( inTile.getUniqueID(), 
                mChosenBackgroundTile.getUniqueID () ) ) {
        
        mChosenBackgroundTile = inTile;
        
        fireActionPerformed( this );
        }
        
    }

        
Tile TilePicker::getBackgroundTile() {
    return mChosenBackgroundTile;
    }

