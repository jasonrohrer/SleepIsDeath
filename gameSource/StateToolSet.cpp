#include "StateToolSet.h"



StateToolSet::StateToolSet( double inAnchorX, double inAnchorY )
        : GUIPanelGL( inAnchorX, inAnchorY, 40, 20,
                      new Color( 0, 0, 0, 0 ) ) {
    
    mMoveButton = new SelectableButtonGL( 
        new Sprite( "move.tga", true ),
        1,
        inAnchorX, inAnchorY, 20, 20 );
    
    mSpeakButton = new SelectableButtonGL( 
        new Sprite( "speak.tga", true ),
        1,
        inAnchorX + 20, inAnchorY, 20, 20 );

    mMoveButton->setToolTip( "tip_objMove" );
    mSpeakButton->setToolTip( "tip_objSpeak" );
    
    
    add( mMoveButton );
    add( mSpeakButton );
    
    mMoveButton->addActionListener( this );
    mSpeakButton->addActionListener( this );
    
    mMoveButton->setSelected( true );
    }



stateTool StateToolSet::getSelected() {
    if( mMoveButton->getSelected() ) {
        return move;
        }
    if( mSpeakButton->getSelected() ) {
        return speak;
        }
    
    return move;
    }


        
void StateToolSet::actionPerformed( GUIComponent *inTarget ) {    
    // select hit, turn others off
    mMoveButton->setSelected( inTarget == mMoveButton );
    mSpeakButton->setSelected( inTarget == mSpeakButton );
    
    fireActionPerformed( this );
    }
