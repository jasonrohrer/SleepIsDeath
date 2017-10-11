
#include "ToolTipDisplay.h"


class FixedTipDisplay : public ToolTipDisplay {
        
    public:

        FixedTipDisplay( double inAnchorX, double inAnchorY );
        
        virtual ~FixedTipDisplay();
        

        // can be NULL, copied internally, so destroyed by caller
        void setTip( char *inTip );

    protected:
        
        // override to take tip from Fixed manager
        virtual char *getTip();
        

        char *mTip;
    };

        
