#include "SelectionManager.h"



        
char SelectionManager::sSelection[P][P];
        
rgbaColor SelectionManager::sColor[P][P];
char SelectionManager::sTrans[P][P];

static rgbaColor black = { {0,0,0,255} };

void SelectionManager::clearSelection() {
    for( int y=0; y<P; y++ ) {
        for( int x=0; x<P; x++ ) {
            sSelection[y][x] = false;
            sColor[y][x] = black;
            sTrans[y][x] = false;
            }
        }
    }


char SelectionManager::isInSelection( int inX, int inY ) {
    return sSelection[inY][inX];
    }



rgbaColor SelectionManager::getColor( int inX, int inY ) {
    return sColor[inY][inX];
    }



char SelectionManager::getTrans( int inX, int inY ) {
    return sTrans[inY][inX];
    }



void SelectionManager::toggleSelection( int inX, int inY, char inSelected ) {
    sSelection[inY][inX] = inSelected;
    }



void SelectionManager::setColor( int inX, int inY, rgbaColor inColor ) {
    sColor[inY][inX] = inColor;
    }



void SelectionManager::setTrans( int inX, int inY, char inTrans ) {
    sTrans[inY][inX] = inTrans;
    }



intPair SelectionManager::getSelectionCenter( char inIgnoreTrans ) {
    int minX = P;
    int maxX = 0;
    int minY = P;
    int maxY = 0;
    
    for( int y=0; y<P; y++ ) {
        for( int x=0; x<P; x++ ) {
            if( sSelection[y][x] 
                &&
                ( !inIgnoreTrans || !sTrans[y][x] ) ) {
                
                if( x < minX ) {
                    minX = x;
                    }
                if( y < minY ) {
                    minY = y;
                    }

                if( x > maxX ) {
                    maxX = x;
                    }
                if( y > maxY ) {
                    maxY = y;
                    }
                }
            }
        }
    
    intPair result = { ( minX + maxX ) / 2,
                       ( minY + maxY ) / 2 };
    
    
    return result;
    }


