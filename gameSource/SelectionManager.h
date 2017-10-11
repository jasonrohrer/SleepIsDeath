#include "common.h"
#include "color.h"


// maintains a global selection on a PxP grid
class SelectionManager {
    public:
        
        static void clearSelection();
        

        static char isInSelection( int inX, int inY );
        

        static rgbaColor getColor( int inX, int inY );

        static char getTrans( int inX, int inY );
        

        static void toggleSelection( int inX, int inY, char inSelected );
        

        static void setColor( int inX, int inY, rgbaColor inColor );
        static void setTrans( int inX, int inY, char inTrans );
        

        // if trans ignored, center computed based on colored pixels only
        static intPair getSelectionCenter( char inIgnoreTrans = false );
        


    protected:
        
        static char sSelection[P][P];
        
        static rgbaColor sColor[P][P];

        static char sTrans[P][P];
        
    };


        
