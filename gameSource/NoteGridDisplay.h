#ifndef NOTE_GRID_DISPLAY_INCLUDED
#define NOTE_GRID_DISPLAY_INCLUDED

#include "minorGems/graphics/openGL/gui/GUIComponentGL.h"
#include "minorGems/ui/event/ActionListenerList.h"
#include "minorGems/ui/event/ActionListener.h"


#include "Sprite.h"
#include "musicPlayer.h"


// note width
#define W 12



class NoteGridDisplay : public GUIComponentGL, public ActionListenerList,
                        public ActionListener {

    public:
        
        // sets its own width and height
        NoteGridDisplay( int inAnchorX, int inAnchorY );
        
        virtual ~NoteGridDisplay();

        
        void resetColumnGlow();
        
        

        char mLastActionRelease;
        char mLastActionPress;

        // override
        virtual void fireRedraw();
        virtual void mouseDragged( double inX, double inY );
		virtual void mousePressed( double inX, double inY );
		virtual void mouseReleased( double inX, double inY );
        

        virtual void actionPerformed( GUIComponent *inTarget );


    protected:

        int mNotesInOctave;
        
        char mPressedInk;
        
        Sprite *mNoteSprite;
        
        Sprite *mNoteSpaceSprite;


        double mColumnGlow[N];
        
    };


#endif
