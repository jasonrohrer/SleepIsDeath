#include "ColorEditor.h"
#include "ColorWells.h"
#include "BorderPanel.h"


#include "minorGems/graphics/openGL/gui/LabelGL.h"


extern TextGL *largeText;

extern ColorWells *mainColorStack;



ColorEditor::ColorEditor( ScreenGL *inScreen )
        : Editor( inScreen, false ),
          mEditPalettePressed( false ),
          mIgnoreEvent( false ) {
    
    mCloseButton->setToolTip( "tip_closeEdit_color" );
    

    mSidePanel->add( mainColorStack );

    mainColorStack->addActionListener( this );
    

    Color *thumbColor = new Color( .5, .5, .5, .5 );
    Color *borderColor = new Color( .35, .35, .35, .35 );


    mValueSlider = new ToolTipSliderGL( 248, 188,
                                        64, 12,
                                        NULL, 0,
                                        new Color( 0, 0, 0, 1 ),
                                        new Color( 0, 1, 0, 1 ),
                                        thumbColor->copy(),
                                        borderColor->copy(),
                                        1, 4, 1 );


    mSaturationSlider = new ToolTipSliderGL( 248, 175,
                                             64, 12,
                                             NULL, 0,
                                             new Color( 0, 0, 0, 1 ),
                                             new Color( 0, 1, 0, 1 ),
                                             thumbColor->copy(),
                                             borderColor->copy(),
                                             1, 4, 1 );


    delete thumbColor;
    delete borderColor;
    
    mSidePanel->add( mValueSlider );
    mSidePanel->add( mSaturationSlider );

    mValueSlider->setToolTip( "tip_valueSlider" );
    mSaturationSlider->setToolTip( "tip_saturationSlider" );
    
    

    mHVPicker = new HueValuePicker( 248, 203,
                                    64, 64 );
    
    mSidePanel->add( mHVPicker );


    rgbaColor c = mainColorStack->getSelectedColor();
    
    mWorkingColor = new Color( c.comp.r / 255.0,
                               c.comp.g / 255.0,
                               c.comp.b / 255.0,
                               1.0 );
    
    float h, s, v;
    
    mWorkingColor->makeHSV( &h, &s, &v );

    // round to increments of 128 pixels to avoid round-off artifacts
    mValueSlider->setThumbPosition( (int)(v * 128) / 128.0 );
    mSaturationSlider->setThumbPosition( (int)(s * 128) / 128.0 );
    
    mHVPicker->setValues( h, v );

    adjustBarColors();
    
    mValueSlider->addActionListener( this );
    mSaturationSlider->addActionListener( this );
    mHVPicker->addActionListener( this );


    mEditPaletteButton = 
        new EditButtonGL( 
            mainColorStack->getAnchorX() - 9,
            mainColorStack->getAnchorY() + mainColorStack->getHeight() - 51,
            8,
            8 );
    
    mSidePanel->add( mEditPaletteButton );
    
    mEditPaletteButton->addActionListener( this );
    mEditPaletteButton->setToolTip( "tip_edit_palette" );


    postConstruction();
    }



ColorEditor::~ColorEditor() {
    mSidePanel->remove( mainColorStack );
    
    delete mWorkingColor;
    }



void ColorEditor::setEditPaletteButtonVisible( char inIsVisible ) {
    mEditPaletteButton->setEnabled( inIsVisible );
    }



void ColorEditor::adjustBarColors() {
    Color *valStart = Color::makeColorFromHSV( 
        mHVPicker->getSelectedHue(),
        mSaturationSlider->getThumbPosition(),
        0 );
    
    Color *valEnd = Color::makeColorFromHSV( 
        mHVPicker->getSelectedHue(),
        mSaturationSlider->getThumbPosition(),
        1 );
    
    mValueSlider->setBarStartColor( valStart );
    mValueSlider->setBarEndColor( valEnd );

    Color *satStart = Color::makeColorFromHSV( 
        mHVPicker->getSelectedHue(),
        0,
        mHVPicker->getSelectedValue() );
    

    Color *satEnd = Color::makeColorFromHSV( 
        mHVPicker->getSelectedHue(),
        1,
        mHVPicker->getSelectedValue() );

    mSaturationSlider->setBarStartColor( satStart );
    mSaturationSlider->setBarEndColor( satEnd );
    }



char ColorEditor::getDragging() {
    return 
        mHVPicker->mPressed ||
        mSaturationSlider->mDragging ||
        mValueSlider->mDragging;
    }



void ColorEditor::actionPerformed( GUIComponent *inTarget ) {

    if( mIgnoreEvent ) {
        return;
        }
    
    // superclass
    Editor::actionPerformed( inTarget );
    
    if( inTarget == mEditPaletteButton ) {
        // close ourself
        mEditPalettePressed = true;
        mCloseButton->fireActionPerformed( mCloseButton );
        mEditPalettePressed = false;
        }


    char colorChange = false;
    

    if( inTarget == mSaturationSlider ||
        inTarget == mHVPicker ) {

        float h = mHVPicker->getSelectedHue();  
        float s = mSaturationSlider->getThumbPosition();
        float v = mHVPicker->getSelectedValue();
        

        mIgnoreEvent = true;

        mValueSlider->setThumbPosition( (int)( v* 128 ) / 128.0f);
        
        mIgnoreEvent = false;
        

        Color *newWorking = Color::makeColorFromHSV( h, s, v  );        

        mWorkingColor->setValues( newWorking );
        delete newWorking;

        mIgnoreEvent = true;
        addColor();
        mIgnoreEvent = false;

        colorChange = true;
        }


    if( inTarget == mValueSlider ) {
        

        float h = mHVPicker->getSelectedHue();  
        float s = mSaturationSlider->getThumbPosition();
        float v = mValueSlider->getThumbPosition();

        //normalize( h, s, v, &h, &s, &v );
        

        mIgnoreEvent = true;
        mHVPicker->setValues( h, v );        
        mIgnoreEvent = false;


        
        Color *newWorking = Color::makeColorFromHSV( h, s, v );

        mWorkingColor->setValues( newWorking );
        delete newWorking;



        // ignore color change to prevent sliders from jumping
        // (each HSV is not a unique RGB)
        mIgnoreEvent = true;
        addColor();
        mIgnoreEvent = false;

        colorChange = true;
        }
    

    if( inTarget == mainColorStack ) {
        // new color picked on stack
        rgbaColor c = mainColorStack->getSelectedColor();

        mWorkingColor->r = c.comp.r / 255.0f;
        mWorkingColor->g = c.comp.g / 255.0f;
        mWorkingColor->b = c.comp.b / 255.0f;

        
        
        float h, s, v;
        mWorkingColor->makeHSV( &h, &s, &v );


        mIgnoreEvent = true;
        
        // round to increments of 128 pixels to avoid round-off artifacts
        mValueSlider->setThumbPosition( (int)( v* 128 ) / 128.0f);
        mSaturationSlider->setThumbPosition( (int)( s* 128 ) / 128.0f);
        mHVPicker->setValues( h, v );
        
        mIgnoreEvent = false;

        colorChange = true;
        }


    if( colorChange ) {
        adjustBarColors();
        }
    
    }



void ColorEditor::editorClosing() {
    addColor();
    }



void ColorEditor::addColor() {
    rgbaColor c;
    
    c.comp.r = (unsigned char)( mWorkingColor->r * 255 );
    c.comp.g = (unsigned char)( mWorkingColor->g * 255 );
    c.comp.b = (unsigned char)( mWorkingColor->b * 255 );
    c.comp.a = 255;
    
    // replace current well
    mainColorStack->pushColor( c, true );
    }


