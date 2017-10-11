#ifndef EDITOR_INCLUDED
#define EDITOR_INCLUDED


#include "buttons.h"


#include "minorGems/graphics/openGL/ScreenGL.h"
#include "minorGems/graphics/openGL/gui/GUIPanelGL.h"
#include "minorGems/graphics/openGL/gui/GUITranslatorGL.h"

#include "minorGems/ui/event/ActionListener.h"
#include "minorGems/ui/event/ActionListenerList.h"


#include "PressActionGUIPanelGL.h"
#include "BorderPanel.h"


class Editor : public GUIComponent, 
               public ActionListener, public ActionListenerList {
        
    public:
        
        
        virtual ~Editor();


        // defaults to invisible
        virtual void setVisible( char inIsVisible );
        
        virtual char isVisible();
        

        // implements ActionListener
        virtual void actionPerformed( GUIComponent *inTarget );

        // hides our side panel and shows color editor until close
        virtual void showColorEditor();
        
        
        // hides our panels and shows tile editor until it is closed
        virtual void showTileEditor();

        // hides our panels and shows room editor until it is closed
        virtual void showRoomEditor();

        // hides our panels and shows sprite editor until it is closed
        virtual void showSpriteEditor();

        // hides our panels and shows Object editor until it is closed
        virtual void showStateObjectEditor();
        
        // hides our panels and shows music editor until it is closed
        virtual void showMusicEditor();

        // hides our panels and shows timbre editor until it is closed
        virtual void showTimbreEditor();

        // hides our panels and shows scale editor until it is closed
        virtual void showScaleEditor();

        // hides our panels and shows song editor until it is closed
        virtual void showSongEditor();

        // hides our panels and shows palette editor until it is closed
        virtual void showPaletteEditor();

        virtual void showPracticePlayerEditor();
        virtual void hidePracticePlayerEditor();
        


        // hack to draw main panel NOW... useful for making flip books
        virtual void redrawMainPanel();
        
        
        // implemented by all subclasses
        // called by parent class when editor is being closed
        virtual void editorClosing() = 0;


    protected:
        
        Editor( ScreenGL *inScreen, char inHasMainArea = true,
                char inHasSidePanel = true );

        // must be called by sub-classes at end of their constructors
        // (adds GUI components that must go on top of everything else)
        void postConstruction();
        
        // gives more control over overlay order
        // those that don't need it call above
        void postConstructionSide();
        void postConstructionMain();
        

        // can be overridden by subclasses to receive notification that
        // an overlying color editor closed
        virtual void colorEditorClosed() {};
        


        ScreenGL *mScreen;
        
        
        char mVisible;
        
        BorderPanel *mSidePanel;
        PressActionGUIPanelGL *mMainPanel;
        

        char mHasSidePanel, mHasMainArea;
        

        
        GUITranslatorGL *mMainPanelGuiTranslator;
        

        SpriteButtonGL *mCloseButton;
        
    };



#endif
