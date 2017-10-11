#include "GameHalf.h"
#include "Editor.h"
#include "ColorEditor.h"
#include "TileEditor.h"
#include "RoomEditor.h"
#include "SpriteEditor.h"
#include "GameStateEditor.h"
#include "PlayerMoveEditor.h"

#include "minorGems/ui/event/ActionListenerList.h"


class ControllerGame : public GameHalf, public ActionListener {
    public:

        ControllerGame( ScreenGL *inScreen );
        
        ~ControllerGame();

        virtual void resetTimer();
        
        virtual void step();
        virtual void drawScene();

        // pay attention to redraws
        virtual void postRedraw();


        
        // implements ActionListener
        virtual void actionPerformed( GUIComponent *inTarget );

    protected:
        Editor *mCurrentEditor;
        TileEditor *mTileEditor;
        RoomEditor *mRoomEditor;
        SpriteEditor *mSpriteEditor;
        GameStateEditor *mGameStateEditor;
        
    };
