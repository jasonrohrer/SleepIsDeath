#ifndef DRAG_AND_DROP_MANAGER_INCLUDED
#define DRAG_AND_DROP_MANAGER_INCLUDED


#include "minorGems/graphics/openGL/gui/GUIComponentGL.h"

#include "Sprite.h"
#include "common.h"



// transparent and covers whole screen.
// should be added last in root panel so that it is on top
class DragAndDropManager : public GUIComponentGL {
        
    public:
        DragAndDropManager( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight );
        
        virtual ~DragAndDropManager();

        char isDragging();
        

        // sets one sprite with 0 offset
        // destroyed internally.
        // set to NULL to clear
        virtual void setSprite( Sprite *inSprite, double inZoom );
        
        // set of sprites with specific offsets
        // arrays and sprites destroyed internally
        // cleared by passing null to above function
        virtual void setSprites( int inNumSprites,
                                 Sprite **inSprites, intPair *inOffsets,
                                 float *inTrans,
                                 char *inGlow,
                                 double inZoom );
        

        // override
        virtual void mousePressed( double inX, double inY );
        virtual void mouseDragged( double inX, double inY );
        virtual void mouseReleased( double inX, double inY );
        virtual void fireRedraw();

    protected:
        void clear();
        

        int mNumSprites;
        Sprite **mSprites;
        intPair *mOffsets;
        float *mTrans;
        char *mGlows;
        
        double mZoom;
        
        double mX;
        double mY;

        char mDragging;
        
        
    };



#endif
