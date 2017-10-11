#include "GameHalf.h"
#include "PlayerMoveEditor.h"

#include "minorGems/ui/event/ActionListener.h"


class PlayerGame : public GameHalf, public ActionListener {
    public:

        PlayerGame( ScreenGL *inScreen );
        
        ~PlayerGame();

        
        virtual void resetTimer();
        
        
        virtual void step();
        virtual void drawScene();
        

        // implements ActionListener
        virtual void actionPerformed( GUIComponent *inTarget );
        
    protected:
        PlayerMoveEditor *mEditor;
        
        char mConnected;

        // last live song received from Controller
        Song mLiveSong;
    };

