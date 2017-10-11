#include "labels.h"

extern TextGL *largeText;


#include "minorGems/util/TranslationManager.h"


EightPixelLabel::EightPixelLabel( double inAnchorX, double inAnchorY, 
                                  const char *inTranslationKey, 
                                  double inScaleFactor )
        : LabelGL( inAnchorX, inAnchorY, 0, 0, "", largeText ) {
    

    char *string = (char *)TranslationManager::translate( inTranslationKey );
    

    double height = 8 * inScaleFactor;
    
    double width = height * strlen( string );

    setText( string );
    setPosition( inAnchorX, inAnchorY, width, height );
    }

        
