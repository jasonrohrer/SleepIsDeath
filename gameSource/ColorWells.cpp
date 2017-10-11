#include "ColorWells.h"

#include "ColorEditor.h"


extern ColorEditor *mainColorEditor;



Color selectedBorder( 1, 1, 1, 1 );

Color unselectedBorder( 0.35, 0.35, 0.35, 1 );


static const char *wellFileName = "palette.txt";



ColorWellButtonGL::ColorWellButtonGL( double inAnchorX, double inAnchorY, 
                                  double inWidth, double inHeight )
        : ToolTipButtonGL( inAnchorX, inAnchorY, inWidth, inHeight ),
          mSelected( false ), mColor( 0, 0, 0, 1 ) {
    
    
    }


    


void ColorWellButtonGL::setColor( rgbaColor inColor ) {
    mColor.setValues( inColor.comp.r / 255.0f,
                      inColor.comp.g / 255.0f,
                      inColor.comp.b / 255.0f,
                      inColor.comp.a / 255.0f );


    }


void ColorWellButtonGL::setSelected( char inSelected ) {
    mSelected = inSelected;
    }



void ColorWellButtonGL::mouseDragged( double inX, double inY ) {
    ToolTipButtonGL::mouseDragged( inX, inY );
    
    if( isEnabled() && isInside( inX, inY ) ) {
		// fire an event
        fireActionPerformed( this );
		}
    }
                      
        
void ColorWellButtonGL::drawPressed() {
    
    Color *borderColor = &unselectedBorder;
    
    if( mSelected ) {
        borderColor = &selectedBorder;
        }
    
    // 1 pixel wide
    glColor4f( borderColor->r, 
               borderColor->g, 
               borderColor->b, 1 ); 

    glBegin( GL_QUADS ); {		
        glVertex2d( mAnchorX, mAnchorY );
        glVertex2d( mAnchorX + mWidth, mAnchorY ); 
        glVertex2d( mAnchorX + mWidth, mAnchorY + mHeight );
        glVertex2d( mAnchorX, mAnchorY + mHeight );
        }
    glEnd();

    // black fill
    glColor4f( 0, 0, 0, 1 );
    
    glBegin( GL_QUADS ); {		
        glVertex2d( mAnchorX + 1, mAnchorY + 1 );
        glVertex2d( mAnchorX + mWidth - 1, mAnchorY + 1 ); 
        glVertex2d( mAnchorX + mWidth - 1, mAnchorY + mHeight - 1 );
        glVertex2d( mAnchorX + 1 , mAnchorY + mHeight - 1 );
        }
    glEnd();


    // center color block
    glColor4f( mColor.r, 
               mColor.g, 
               mColor.b, 1); 

    glBegin( GL_QUADS ); {		
        glVertex2d( mAnchorX + 2, mAnchorY + 2 );
        glVertex2d( mAnchorX + mWidth - 2, mAnchorY + 2 ); 
        glVertex2d( mAnchorX + mWidth - 2, mAnchorY + mHeight - 2 );
        glVertex2d( mAnchorX + 2 , mAnchorY + mHeight - 2 );
        }
    glEnd();
    }


void ColorWellButtonGL::drawUnpressed() {
    drawPressed();
    }







ColorWells::ColorWells( double inAnchorX, double inAnchorY )
    : BorderPanel( inAnchorX, inAnchorY,50, 124,
                   new Color( 0, 0, 0, 1 ),
                   new Color( 0.35, 0.35, 0.35, 1 ),
                   1 ),
      mLastActionWellChange( false ) {

    int buttonW = 10;
    

    FILE *f = fopen( wellFileName, "r" );
    
    SimpleVector<rgbaColor> savedColors;

    if( f != NULL ) {
        
        rgbaColor c;
        c.comp.a = 255;
        
        int numRead = 1;

        while( numRead == 1 ) {
            int r, g, b;
            
            numRead = fscanf( f, "%d", &r );
            numRead = fscanf( f, "%d", &g );
            numRead = fscanf( f, "%d", &b );
            
            c.comp.r = (unsigned char)r;
            c.comp.g = (unsigned char)g;
            c.comp.b = (unsigned char)b;
            
            if( numRead == 1 ) {
                savedColors.push_back( c );
                }
            }
        fclose( f );
        }
    
    int numSavedColors = savedColors.size();
    int nextSavedColor = 0;

    for( int y=0; y<mColorGridH; y++ ) {
        for( int x=0; x<mColorGridW; x++ ) {
            mColorButtons[y][x] = 
                new ColorWellButtonGL(
                    inAnchorX + x * buttonW,
                    inAnchorY + y * buttonW,
                    buttonW, buttonW );
            
            add( mColorButtons[y][x] );
            
            mColorButtons[y][x]->addActionListener( this );
            
            if( nextSavedColor < numSavedColors ) {
                
                mColorWells[y][x] = 
                    *( savedColors.getElement( nextSavedColor ) );
            
                nextSavedColor++;
                }
            else {
                // random
                mColorWells[y][x].comp.r = rand() % 255;
                mColorWells[y][x].comp.g = rand() % 255;
                mColorWells[y][x].comp.b = rand() % 255;
                mColorWells[y][x].comp.a = 255;
                }
            
            mColorButtons[y][x]->setColor( mColorWells[y][x] );
            mColorButtons[y][x]->setToolTip( "tip_colorWell" );
            }
        }

    mSelected.y = mColorGridH - 1;
    mSelected.x = 0;

    mColorButtons[mSelected.y][mSelected.x]->setSelected( true );


    mMainColorButton = 
        new ColorWellButtonGL( inAnchorX, 
                               // right to top of panel, cover border
                               inAnchorY + mHeight - 2 * buttonW + 2,
                               mColorGridW * buttonW,
                               2 * buttonW );
    add( mMainColorButton );

    mMainColor = mColorWells[mSelected.y][mSelected.x];
    
    mMainColorButton->setColor( mMainColor );
    mMainColorButton->setSelected( false );
    mMainColorButton->setToolTip( "tip_colorCurrent" );

    // center below main color
    mAddButton = new AddButtonGL( inAnchorX + (mWidth - 16)/2 + 1,
                                  mMainColorButton->getAnchorY() - 20,
                                  16, 16 );
    add( mAddButton );
    mAddButton->addActionListener( this );        

    mAddButton->setToolTip( "tip_addColor" );
    }





ColorWells::~ColorWells() {

    }



        
rgbaColor ColorWells::getSelectedColor() {
    return mMainColor;
    }



#include "musicPlayer.h"
        

void ColorWells::pushColor( rgbaColor inColor, char inForceReplace ) {
    mMainColorButton->setColor( inColor );
    mMainColor = inColor;


    /*
    // set timbre from this color
    double coeffs[256];
    int numUsed = 0;


    rgbaColor c = getSelectedColor();

    float r = c.comp.r / 255.0f;
    float g = c.comp.g / 255.0f;
    float b = c.comp.b / 255.0f;
    

    // r for damping amount for higher harmonics
    // g for uniform damping of even harmonics
    // b for uniform damping of odd harmonics

    numUsed = 40;
    coeffs[0]=1;
    
    for( int i=1; i<numUsed; i++ ) {
        
        int harmNumber = i + 1;

        coeffs[i]= 1.0/ pow( harmNumber, (1-r) );
        
        if( harmNumber %2 == 0 ) {
            // even
            coeffs[i] *= g;
            }
        else {
            coeffs[i] *= b;
            }
        }
    
    setTimbre( 0, coeffs, numUsed );
    */
    /*
    Color c2( r, g, b );
    float h, s, v;
    c2.makeHSV( &h, &s, &v );
    
    setEnvelope( 0, 1-r, 1-s );
    */


    fireActionPerformed( this );
    if( true ) {
        return;
        }
    

    if( !inForceReplace ) {
        
        // first, check if color already exists, and select it

        for( int y=0; y<mColorGridH; y++ ) {
            for( int x=0; x<mColorGridW; x++ ) {
                
                if( equal( mColorWells[y][x], inColor ) ) {
                    
                    // clear border of last selected
                    mColorButtons[mSelected.y][mSelected.x]->setSelected( 
                        false );
                
                    mSelected.x = x;
                    mSelected.y = y;
                    
                    mColorButtons[mSelected.y][mSelected.x]->setSelected( 
                        true );
                    
                    fireActionPerformed( this );
                    return;
                    }
                }
            }
        }
    

    // else replace current well
        
    mColorWells[mSelected.y][mSelected.x] = inColor;

    mColorButtons[mSelected.y][mSelected.x]->setColor( inColor );
    
    fireActionPerformed( this );


    




    }



Palette ColorWells::getPalette() {
    Palette p;
    
    for( int y=0; y<mColorGridH; y++ ) {
        for( int x=0; x<mColorGridW; x++ ) {
            
            int i = y * mColorGridW + x;
            
            p.editPalette( i, mColorWells[y][x] );
            }
        }
    return p;
    }



void ColorWells::setPalette( Palette inPalette ) {
    for( int y=0; y<mColorGridH; y++ ) {
        for( int x=0; x<mColorGridW; x++ ) {
            
            int i = y * mColorGridW + x;
            
            mColorWells[y][x] = inPalette.getColor( i );
            mColorButtons[y][x]->setColor( mColorWells[y][x] );
            }
        }
    mMainColor = mColorWells[mSelected.y][mSelected.x];
                
    mMainColorButton->setColor( mMainColor );
                
    mLastActionWellChange = false;
    fireActionPerformed( this );
    }

                


void ColorWells::actionPerformed( GUIComponent *inTarget ) {
    
    if( inTarget == mAddButton ) {
        // replace well

        mColorWells[mSelected.y][mSelected.x] = mMainColor;

        mColorButtons[mSelected.y][mSelected.x]->setColor( mMainColor );
        

        // save wells to file
        FILE *f = fopen( wellFileName, "w" );
    
        
        if( f != NULL ) {
            
            for( int y=0; y<mColorGridH; y++ ) {
                for( int x=0; x<mColorGridW; x++ ) {
                    fprintf( f, "%d %d %d\n",
                             mColorWells[y][x].comp.r,
                             mColorWells[y][x].comp.g,
                             mColorWells[y][x].comp.b );
                    }
                }
            fclose( f );
            }
        // no change to main color
        
        // but fire action anyway, for those listening for well changes
        mLastActionWellChange = true;
        fireActionPerformed( this );
        return;
        }
        

    // well switch?
    
    // ignore if currently dragging in editor
    if( mainColorEditor->getDragging() ) {
        return;
        }
    

    for( int y=0; y<mColorGridH; y++ ) {
        for( int x=0; x<mColorGridW; x++ ) {
            
            if( inTarget == mColorButtons[y][x] ) {
                
                // clear border of last selected
                mColorButtons[mSelected.y][mSelected.x]->setSelected( false );
                
                mSelected.x = x;
                mSelected.y = y;

                mColorButtons[mSelected.y][mSelected.x]->setSelected( true );
                
                mMainColor = mColorWells[mSelected.y][mSelected.x];
                
                mMainColorButton->setColor( mMainColor );
                
                mLastActionWellChange = false;
                fireActionPerformed( this );
                return;
                }
            }
        }
    }


