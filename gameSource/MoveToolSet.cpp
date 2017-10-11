#include "MoveToolSet.h"



MoveToolSet::MoveToolSet( double inAnchorX, double inAnchorY )
        : GUIPanelGL( inAnchorX, inAnchorY, 60, 20,
                      new Color( 0, 0, 0, 0 ) ) {
    
    mActButton = new SelectableButtonGL( 
        new Sprite( "act.tga", true ),
        1,
        inAnchorX, inAnchorY, 20, 20 );

    mMoveButton = new SelectableButtonGL( 
        new Sprite( "move.tga", true ),
        1,
        inAnchorX + 20, inAnchorY, 20, 20 );

    mSpeakButton = new SelectableButtonGL( 
        new Sprite( "speak.tga", true ),
        1,
        inAnchorX + 40, inAnchorY, 20, 20 );
    
    mActButton->setToolTip( "tip_playerAct" );
    mMoveButton->setToolTip( "tip_playerMove" );
    mSpeakButton->setToolTip( "tip_playerSpeak" );
    
    add( mActButton );
    add( mMoveButton );
    add( mSpeakButton );
    
    mActButton->addActionListener( this );
    mMoveButton->addActionListener( this );
    mSpeakButton->addActionListener( this );
    
    mMoveButton->setSelected( true );
    }


void MoveToolSet::setMoveAllowed( char inAllowed ) {
    if( !inAllowed && getSelected() == playerMove ) {
        setSelected( playerSpeak );
        //setSelected( playerAct );
        // change
        fireActionPerformed( this );
        }
    
    mMoveButton->setEnabled( inAllowed );
    }



moveTool MoveToolSet::getSelected() {
    if( mActButton->getSelected() ) {
        return playerAct;
        }
    if( mMoveButton->getSelected() ) {
        return playerMove;
        }
    if( mSpeakButton->getSelected() ) {
        return playerSpeak;
        }
    
    return playerMove;
    }



void MoveToolSet::setSelected( moveTool inTool ) {
    mActButton->setSelected( inTool == playerAct );
    mMoveButton->setSelected( inTool == playerMove );
    mSpeakButton->setSelected( inTool == playerSpeak );
    }


        
void MoveToolSet::actionPerformed( GUIComponent *inTarget ) {    
    // select hit, turn others off
    
    mActButton->setSelected( inTarget == mActButton );
    mMoveButton->setSelected( inTarget == mMoveButton );
    mSpeakButton->setSelected( inTarget == mSpeakButton );
    
    fireActionPerformed( this );
    }


void MoveToolSet::setEnabled( char inEnabled ) {
    GUIPanelGL::setEnabled( inEnabled );
    
    mActButton->setEnabled( inEnabled );
    mMoveButton->setEnabled( inEnabled );
    mSpeakButton->setEnabled( inEnabled );
    }

