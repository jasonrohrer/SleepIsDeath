#ifndef RGBA_COLOR_INCLUDED
#define RGBA_COLOR_INCLUDED


#include "minorGems/graphics/Color.h"


typedef union rgbaColor {
        struct comp { 
                unsigned char r;
                unsigned char g;
                unsigned char b;
                unsigned char a;
            } comp;
        
        // access those bytes as an array
        unsigned char bytes[4];
        
        // reinterpret those bytes as an unsigned int
        unsigned int rgbaInt; 
    } rgbaColor;



inline char equal( rgbaColor inA, rgbaColor inB ) {
    return inA.rgbaInt == inB.rgbaInt;
    }


inline rgbaColor blend( rgbaColor inA, rgbaColor inB, double inBWeight ) {
    rgbaColor blendC = 
        {
            { (unsigned char)(
                inA.comp.r * (1-inBWeight) + inB.comp.r * inBWeight ),
              (unsigned char)(
                inA.comp.g * (1-inBWeight) + inB.comp.g * inBWeight ),
              (unsigned char)(
                inA.comp.b * (1-inBWeight) + inB.comp.b * inBWeight ),
              (unsigned char)(
                  inA.comp.a * (1-inBWeight) + inB.comp.a * inBWeight ) 
                }
            };
    return blendC;
    }



inline Color toColor( rgbaColor inC ) {
    Color c( inC.comp.r / 225.0f,
             inC.comp.g / 225.0f,
             inC.comp.b / 225.0f,
             inC.comp.a / 225.0f );
    return c;
    }

             
              
                



#include <stdio.h>

inline void print( rgbaColor inC ) {
    printf( "[%02x,%02x,%02x,%02x]", 
            inC.comp.r, inC.comp.g, inC.comp.b, inC.comp.a );
    }


#endif
