#ifndef LABELS_INCLUDED
#define LABELS_INCLUDED


#include "minorGems/graphics/openGL/gui/LabelGL.h"


// label that sets its own size based on the default font
// automatically translates its string
class EightPixelLabel : public LabelGL {
        
    public:
        
        EightPixelLabel( double inAnchorX, double inAnchorY, 
                         const char *inTranslationKey,
                         double inScaleFactor = 1.0 );
                
    };


#endif
