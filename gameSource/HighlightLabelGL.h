#ifndef HIGHLIGHT_LABEL_GL_INCLUDED
#define HIGHLIGHT_LABEL_GL_INCLUDED 

#include "minorGems/graphics/openGL/gui/LabelGL.h"
#include "minorGems/graphics/Color.h"


// a label with a highlight that can be turned on and off
class HighlightLabelGL : public LabelGL, public ActionListenerList {


	public:


        // same params as LabelGL
		HighlightLabelGL( double inAnchorX, double inAnchorY, double inWidth,
                          double inHeight, const char *inString, 
                          TextGL *inText )
                : LabelGL( inAnchorX, inAnchorY, inWidth,
                           inHeight, inString, inText ),
                  mHighlightOn( false ), mHighlightColor( 1.0, 0.5, 0 ) {

            }



        void setHighlight( char inHighlightOn ) {
            mHighlightOn = inHighlightOn;
            }
        


        char isHighlightOn() {
            return mHighlightOn;
            }
                

		// override fireRedraw in LabelGL
		virtual void fireRedraw();

        
		virtual void mousePressed( double inX, double inY );

		
	protected:
        char mHighlightOn;
        
        Color mHighlightColor;
        
	};


		
inline void HighlightLabelGL::fireRedraw() {
    if( ! mEnabled ) {
        // invisible if disabled
        return;
        }
    

	Color *oldColor = NULL;
    
    if( mHighlightOn ) {
        
        oldColor = mText->getFontColor()->copy();
        
        mText->setFontColor( mHighlightColor.copy() );

        // lighten background to further highlight label
        glColor4f( 1, 1, 1, 0.25 ); 
        
        double drawWidth = mText->measureTextWidth( mString ) * mHeight;
        
        glColor4f( mHighlightColor.r, mHighlightColor.g, mHighlightColor.b, 
                   0.125 ); 

        glBegin( GL_QUADS ); {
            glVertex2d( mAnchorX - 1, mAnchorY - 1 ); 
            glVertex2d( mAnchorX + drawWidth + 1, mAnchorY - 1 ); 
            glVertex2d( mAnchorX + drawWidth + 1, mAnchorY + mHeight + 1 );
            glVertex2d( mAnchorX - 1, mAnchorY + mHeight + 1 );
            }
        glEnd();
        }
    
    mText->drawText( mString, mAnchorX, mAnchorY,
					 mWidth, mHeight );
	
    if( mHighlightOn ) {
        mText->setFontColor( oldColor );
        }
    
    }


inline void HighlightLabelGL::mousePressed( double inX, double inY ) {
	if( isEnabled() ) {
		// fire an event
		fireActionPerformed( this );
		}
	}


#endif



