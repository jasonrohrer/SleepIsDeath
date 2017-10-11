#ifndef GAME_HALF_INCLUDED
#define GAME_HALF_INCLUDED


#include "minorGems/graphics/openGL/MouseHandlerGL.h"
#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"
#include "minorGems/graphics/openGL/SceneHandlerGL.h"
#include "minorGems/graphics/openGL/RedrawListenerGL.h"
#include "minorGems/graphics/openGL/ScreenGL.h"

// superclass for each half of the game (player or controller)
class GameHalf : public MouseHandlerGL,
                 public KeyboardHandlerGL,
                 public SceneHandlerGL,
                 public RedrawListenerGL {

    public:
        
        GameHalf();
        
        virtual ~GameHalf();
        

        virtual void setScreen( ScreenGL *inScreen ) {
            mScreen = inScreen;
            }


        // true if game is trying to quit
        virtual char isQuitting();
        
        

        virtual void resetTimer() = 0;
        
        

        virtual void step() = 0;
        
        // implements the SceneHandlerGL interface
		virtual void drawScene() = 0;

        // redraw listener, defaults to doing nothing
        virtual void fireRedraw() {}
        virtual void postRedraw() {}
        


        // empty implementations of each input function, 
        // in case subclasses don't need them

        // implements the MouseHandlerGL interface
		virtual void mouseMoved( int inX, int inY ) {}

		virtual void mouseDragged( int inX, int inY );
		virtual void mousePressed( int inX, int inY );
		virtual void mouseReleased( int inX, int inY );
		
		// implements the KeyboardHandlerGL interface
		virtual char isFocused() { 
            return true;            
            }
        // handles quit and hardQuit mode
		virtual void keyPressed( unsigned char inKey, int inX, int inY );
        
		virtual void specialKeyPressed( int inKey, int inX, int inY ) {}
		virtual void keyReleased( unsigned char inKey, int inX, int inY ) {}
		virtual void specialKeyReleased( int inKey, int inX, int inY ) {}
		

    protected:
        
        ScreenGL *mScreen;

        int mSecondsPerMove;
        int mStepsPerSecond;
        int mStepsLeft;
        
        double mLastStepTime;
        

        void decrementStepCount();
        


        char mQuitting;
        
    };



#endif
