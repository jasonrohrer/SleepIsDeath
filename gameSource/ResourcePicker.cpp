#ifndef RESOURCE_PICKER_IMPLEMENTATION_INCLUDED
#define RESOURCE_PICKER_IMPLEMENTATION_INCLUDED 


#include "ResourcePicker.h"
#include "resourceDatabase.h"
#include "resourceManager.h"
#include "usageDatabase.h"
#include "speechHints.h"
#include "Room.h"
#include "imageCache.h"


#include "minorGems/util/stringUtils.h"
#include "minorGems/util/log/AppLog.h"


#include <string.h>
#include <stdlib.h>
#include <stdio.h>


extern TextGL *largeTextFixed;


template <class ResourceType>
ResourcePicker<ResourceType>::ResourcePicker(
    double inAnchorX, double inAnchorY,
    SimpleVector<uniqueID> *inStack )
    : BorderPanel( inAnchorX, inAnchorY, 70, 93,
                   new Color( 0, 0, 0, 1 ),
                   new Color( 0.35, 0.35, 0.35, 1 ),
                   1 ),
      mNumResourcesOnScreen( 0 ),
      mSearchResultOffset( 0 ),
      mNumSearchResults( 0 ),
      mStackMode( false ),
      mStack( inStack ) {
    
    double tileSpace = P;
    
    // room for button borders
    double buttonSpace = tileSpace + 4;

    // use same separation for both x and y
    double buttonSeparation = 
        ( mWidth - buttonSpace * mGridW ) / (mGridW + 1);


    // put left and right at very bottom
    mLeftButton = new LeftButtonGL( mAnchorX + buttonSeparation,
                                    mAnchorY + buttonSeparation,
                                    8, 8 );
    add( mLeftButton );
    mLeftButton->setEnabled( false );
    mLeftButton->addActionListener( this );


    mRightButton = new RightButtonGL( mAnchorX + mWidth - buttonSeparation - 8,
                                      mAnchorY + buttonSeparation,
                                      8, 8 );
    add( mRightButton );
    mRightButton->setEnabled( false );
    mRightButton->addActionListener( this );


    // centered above picker
    mStackSearchButton = new StackSearchButtonGL( mAnchorX + (mWidth-8)/3 - 1,
                                                  mAnchorY + 1,
                                                  8, 8 );
    add( mStackSearchButton );
    mStackSearchButton->addActionListener( this );


    double baseY = mAnchorY + buttonSeparation + 8;
    

    
    double topButtonY = baseY + buttonSeparation +
        mGridH * ( buttonSpace + buttonSeparation ) - buttonSpace;
    

    for( int y=0; y<mGridH; y++ ) {
        for( int x=0; x<mGridW; x++ ) {
        
            mButtonGrid[y][x] = new DraggableTwoSpriteButtonGL( 
                NULL, NULL,
                1,
                mAnchorX + buttonSeparation + 
                  (buttonSpace + buttonSeparation) * x,
                topButtonY - buttonSeparation -
                  (buttonSpace + buttonSeparation) * y, 
                buttonSpace,
                buttonSpace );

            add( mButtonGrid[y][x] );
            
            mButtonGrid[y][x]->setEnabled( false );
            
            mButtonGrid[y][x]->addActionListener( this );
            }
        }
  
    mLastActionFromPress = false;
    


    int fieldHeight = 8;
    int fieldWidth = 8 * 8;
    
    const char *defaultSearch = "";

    double fieldAnchorY = topButtonY + buttonSpace + 1;
    
    mSearchField = new TextFieldGL( mAnchorX + 2,
                                    fieldAnchorY,
                                    fieldWidth,
                                    fieldHeight,
                                    1,
                                    defaultSearch,
                                    largeTextFixed,
                                    new Color( 0.75, 0.75, 0.75 ),
                                    new Color( 0.75, 0.75, 0.75 ),
                                    new Color( 0.15, 0.15, 0.15 ),
                                    new Color( 0.75, 0.75, 0.75 ),
                                    8,
                                    false );
    add( mSearchField );

    mSearchField->setFocus( false );
    //mSearchField->lockFocus( true );
    
    mSearchField->setCursorPosition( strlen( defaultSearch ) );
    
    mSearchField->addActionListener( this );

    
    

    mSelectedResourceButton = new DraggableTwoSpriteButtonGL( 
                NULL,
                NULL,
                1,  
                mAnchorX + mWidth - buttonSpace - 2,
                mAnchorY + mHeight - buttonSpace - 2,
                buttonSpace,
                buttonSpace );
    
    add( mSelectedResourceButton );
    mSelectedResourceButton->setEnabled( true );    
    mSelectedResourceButton->addActionListener( this );
    
    refreshSelectedResourceSprite();
    


    mDeleteButton = new DeleteButtonGL( 
        mSelectedResourceButton->getAnchorX() - 8,
        mAnchorY + mHeight - 10,
        8, 8 );
    add( mDeleteButton );
    mDeleteButton->addActionListener( this );
    mDeleteButton->setEnabled( false );

    // concatonate
    char *tipKey = autoSprintf( "tip_delete_%s", 
                                ResourceType::getResourceType() );
    
    mDeleteButton->setToolTip( tipKey );
    delete [] tipKey;
    


    EightPixelLabel *tileLabel = 
        new EightPixelLabel( mAnchorX + 2, 
                             mAnchorY + mHeight - 8 - 2,
                             (char*)( ResourceType::getResourceType() ) );
    add( tileLabel );


    mSearchLabel = new EightPixelLabel( mAnchorX + 2, 
                                        fieldAnchorY +
                                        fieldHeight + 1,
                                        "search" );
    add( mSearchLabel );

    mStackLabel = new EightPixelLabel( mAnchorX + 2, 
                                       fieldAnchorY +
                                       fieldHeight + 1,
                                       "stack" );
    }




template <class ResourceType>
ResourcePicker<ResourceType>::~ResourcePicker() {

    if( !contains( mSearchLabel ) ) {
        delete mSearchLabel;
        }
    if( !contains( mSearchField ) ) {
        delete mSearchField;
        }
    if( !contains( mStackLabel ) ) {
        delete mStackLabel;
        }
    }



template <class ResourceType>
ResourceType ResourcePicker<ResourceType>::getSelectedResource() {
    return mSelectedResource;
    }



template <class ResourceType>
uniqueID ResourcePicker<ResourceType>::getSelectedResourceID() {
    return mSelectedResource.getUniqueID();
    }



template <class ResourceType>
void ResourcePicker<ResourceType>::refreshSelectedResourceSprite() {
    Sprite *backgroundSprite = getButtonGridBackground();
            
    char useTrans = false;
    if( backgroundSprite != NULL ) {
        useTrans = true;
        }
    
    
    mSelectedResourceButton->setFrontSprite( 
        mSelectedResource.getSprite( useTrans ) );
    
    mSelectedResourceButton->setSprite( backgroundSprite );

    char *tip = mSelectedResource.getName();
    mSelectedResourceButton->setToolTip( tip, false );
    delete [] tip;
    }



template <class ResourceType>
void ResourcePicker<ResourceType>::recheckDeletable() {
    if( equal( mSelectedResource.getUniqueID(), 
               ResourceType::getDefaultResource().getUniqueID() ) ) {
            
        // can't delete blank
        mDeleteButton->setEnabled( false );
        }
    else if( isUsed( mSelectedResource.getUniqueID() ) ) {
        uniqueID user = getUser( mSelectedResource.getUniqueID() );
    
        char *userName = getResourceName( user );

        char *tip;
        
        char *resourceName = mSelectedResource.getName();
        
        if( userName != NULL ) {
            
            tip = autoSprintf( "%s  (used in %s)",
                               resourceName,
                               userName );
            }
        else {
            tip = autoSprintf( "%s  (used in current edit)",
                               resourceName );
            }
        delete [] resourceName;
        

        mSelectedResourceButton->setToolTip( tip, false );
        delete [] tip;
        
        // can't delete a resource used elsewhere
        mDeleteButton->setEnabled( false );
        }
    else {
        // not default, and not used anywhere.  Deletable!
        mDeleteButton->setEnabled( true );
        }
    }



template <class ResourceType>
void ResourcePicker<ResourceType>::setSelectedResource( 
    ResourceType inResource, char inForceResultRefresh, char inFireEvent ) {

    if( !equal( mSelectedResource.getUniqueID(), inResource.getUniqueID() ) ) {
        
        // a change
    
        mSelectedResource = inResource;

        refreshSelectedResourceSprite();
        

        // remove from lower in stack
        uniqueID id = mSelectedResource.getUniqueID();
        char found = false;
        int stackSize = mStack->size();
        for( int i=0; i<stackSize && !found; i++ ) {
            if( equal( id, *( mStack->getElement( i ) ) ) ) {
                mStack->deleteElement( i );
                found = true;
                }
            }
        mStack->push_back( id );
        
        if( mStack->size() > 256 ) {
            // size limit reached
            
            // delete oldest
            mStack->deleteElement( 0 );
            }
        

        recheckDeletable();
        

        if( mStackMode ) {
            // back to top whenever top changes
            mSearchResultOffset = 0;
            }
        
    
        if( inForceResultRefresh ) {
            newSearch( true );
            }
        else if( mStackMode ) {
            if( found ) {
                // just stack order changed
                getFreshResults();
                }
            else {
                // something new added to stack... maybe a size change
                newSearch( false );
                }
            }
        
        
        if( inFireEvent ) {
            mLastActionFromPress = false;
            fireActionPerformed( this );
            }
        }
        
    
    }



template <class ResourceType>
void ResourcePicker<ResourceType>::forceNewSearch() {
    newSearch( false );
    }



template <class ResourceType>
void ResourcePicker<ResourceType>::newSearch( char inPreservePageOffset,
                                              char inMoveBackward ) {

    char *search = stringDuplicate( mSearchField->getText() );
        
    int numResults;
    
    if( !mStackMode ) {    
        numResults = countSearchResults( ResourceType::getResourceType(),
                                         search );
        }
    else {
        numResults = countStackResults();
        }
    
    //printf( "New resource search, %d results\n", numResults );
        
    delete [] search;
        
    
    int oldOffset = mSearchResultOffset;
    

    mNumSearchResults = numResults;
    mSearchResultOffset = 0;


    if( inPreservePageOffset ) {
        if( oldOffset < numResults ) {
            // still in range
            mSearchResultOffset = oldOffset;
            }
        }
    
        
    if( numResults > mNumResourcesDisplayedMax ) {
        // scroll
        numResults = mNumResourcesDisplayedMax;
        mLeftButton->setEnabled( true );
        mRightButton->setEnabled( true );
        }
    else {
        // only one page
        mLeftButton->setEnabled( false );
        mRightButton->setEnabled( false );
        }
        
    getFreshResults( inMoveBackward );
    }



        
template <class ResourceType>
Sprite *ResourcePicker<ResourceType>::getButtonGridBackground() {
    return NULL;
    }






template <class ResourceType>
void ResourcePicker<ResourceType>::getFreshResults( char inMoveBackward ) {
    uniqueID ids[mNumResourcesDisplayedMax];


    // clear old sprites
    int i;
    for( i=0; i<mNumResourcesDisplayedMax; i++ ) {
        int y = i / mGridW;
        int x = i % mGridW;
        
        mButtonGrid[y][x]->setEnabled( false );
        }
        

    char *search = stringDuplicate( mSearchField->getText() );

    
    //printf( "Fresh results with offset %d from %d results\n", 
    //        mSearchResultOffset, mNumSearchResults );
    
    if( !mStackMode ) {
        mNumResourcesOnScreen = getSearchResults( 
            ResourceType::getResourceType(),
            search,
            mSearchResultOffset,
            mNumResourcesDisplayedMax,
            ids );
        

        }
    else {
        mNumResourcesOnScreen = getStackResults( mSearchResultOffset,
                                                 mNumResourcesDisplayedMax,
                                                 ids );
        }
    
    delete [] search;
    
    Sprite *backgroundSprite = getButtonGridBackground();
    
    char useTrans = false;
    if( backgroundSprite != NULL ) {
        useTrans = true;
        
        // delete this test sprite, because we need to add a copy
        // to every button below
        delete backgroundSprite;
        }
    

    char badResultCount = 0;

    for( i=0; i<mNumResourcesOnScreen; i++ ) {
        ResourceType r( ids[i] );
        
        mResourcesDisplayed[i] =  r;
        
        int y = i / mGridW;
        int x = i % mGridW;
        
        mButtonGrid[y][x]->setEnabled( true );
        mButtonGrid[y][x]->setFrontSprite( r.getSprite( useTrans ) );
        mButtonGrid[y][x]->setSprite( getButtonGridBackground() );        
        
        char *tip = r.getName();
        mButtonGrid[y][x]->setToolTip( tip, false );
        delete [] tip;

        if( ! equal( ids[i], r.getUniqueID() ) ) {
            // this resource failed to load (bogus search result in index,
            //   and it will be removed by resourceManager)
            
            
            // it will show up as a "default" resource with blank sprite,
            // even if the current search does not match the word "default",
            // which is confusing.
            badResultCount++;

            // abandon this search and start a brand new one, attempting
            // to maintain the page offset

            // however, finish loading rest of resources in this batch
            // in case it contains multiple bad ones (they are only removed
            //  by resourceManager when they are loaded), instead of removing
            // them one-by-one with several newSearches (slow)
            }
        }


    if( badResultCount > 0 ) {
        
        if( inMoveBackward ) {
            // when moving backward through results,
            // the removal of bad results can cause the left button
            // to appear to have no effect 

            // account for this
            mNumSearchResults -= badResultCount;
            mSearchResultOffset -= badResultCount;
            lowerBoundSearchOffset();
            }
        
        newSearch( true, inMoveBackward );
        }

    }


        

template <class ResourceType>
void ResourcePicker<ResourceType>::lowerBoundSearchOffset() {
    if( mSearchResultOffset < 0 ) {
        // roll back to last page
        int numOnLastPage = mNumSearchResults % mNumResourcesDisplayedMax;
        
        mSearchResultOffset = mNumSearchResults - numOnLastPage;
        if( numOnLastPage == 0 ) {
            // an even number of full pages
            
            // show last page
            mSearchResultOffset = 
                mSearchResultOffset - mNumResourcesDisplayedMax;
            }
        }
    }



template <class ResourceType>
void ResourcePicker<ResourceType>::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == mSearchField ) {
        // search changed
        newSearch();
        }
    else if( inTarget == mDeleteButton ) {
        // confirmed delete!

        uniqueID id = mSelectedResource.getUniqueID();
        
        deleteResource( ResourceType::getResourceType(), id );
        
        removeUsages( id );
        
        // delete from stack, too
        char found = false;
        int stackSize = mStack->size();
        // walk backward... most likely at top of stack (right?)
        for( int i=stackSize-1; i>=0 && !found; i-- ) {
            if( equal( id, *( mStack->getElement( i ) ) ) ) {
                AppLog::detail( "Found element to delete in stack\n" );
                mStack->deleteElement( i );
                found = true;
                }
            }
        
        // delete from image cache, too
        clearCachedImages( id );
        
        // also hint no longer needed
        clearSpeechHint( id );
        
        

        // refresh results (in case they show the deleted tile)
        newSearch( true );

        if( !mStackMode || mStack->size() == 0 ) {
            // set to default
            setSelectedResource( ResourceType::getDefaultResource() );
            }
        else {
            // take next top of stack
            ResourceType r( *( mStack->getElement( mStack->size() - 1 ) ) );
            setSelectedResource( r );
            }
        }
    else if( inTarget == mLeftButton ) {
        mSearchResultOffset -= mNumResourcesDisplayedMax;
        lowerBoundSearchOffset();
        getFreshResults( true );
        }
    else if( inTarget == mRightButton ) {
        mSearchResultOffset += mNumResourcesDisplayedMax;
        if( mSearchResultOffset > mNumSearchResults - 1 ) {
            // wrap back around
            mSearchResultOffset = 0;
            }
        getFreshResults();
        }
    else if( inTarget == mStackSearchButton ) {
        mStackMode = mStackSearchButton->getState();
        
        
        if( mStackMode ) {
            remove( mSearchField );
            remove( mSearchLabel );
            
            add( mStackLabel );
            }
        else {
            remove( mStackLabel );
            
            add( mSearchLabel );
            add( mSearchField );
            }

        newSearch();
        }
    else if( inTarget == mSelectedResourceButton ) {
        if( mSelectedResourceButton->mLastActionFromPress ) {
            // event from a press.... start drag and drop?
            mLastActionFromPress = true;
            mCurrentDraggedResource = mSelectedResource;
            
            fireActionPerformed( this );
            }
        }
    else {
        // consider a button press
        char found = false;
        for( int i=0; i<mNumResourcesOnScreen && !found; i++ ) {
            int y = i / mGridW;
            int x = i % mGridW;

            if( inTarget == mButtonGrid[y][x] ) {
                
                found = true;
                
                if( ! mButtonGrid[y][x]->mLastActionFromPress ) {
                    
                    
                
                    // don't let it fire an event...
                    // (because it only fires on item change)
                    setSelectedResource( mResourcesDisplayed[ i ], false,
                                         false );
                    // ...we fire our own, even if there's no change
                    mLastActionFromPress = false;
                    fireActionPerformed( this );
                

                    /*
                      if( mStackMode ) {
                      // show top of stack again
                      mSearchResultOffset = 0;
                      getFreshResults();
                      }
                    */
                    }
                else {
                    // event from a press.... start drag and drop?
                    mLastActionFromPress = true;
                    mCurrentDraggedResource = mResourcesDisplayed[ i ];
                    
                    fireActionPerformed( this );
                
                    }
                
                }
            }
        }
    
    
    }



template <class ResourceType>
void ResourcePicker<ResourceType>::setState( ResourceType inSelectedResource,
                                             char *inSearchString,
                                             int inResultOffset,
                                             char inStackMode ) {
    mSearchField->setText( inSearchString );
    mSearchResultOffset = inResultOffset;
    
    char stackModeChange = ( mStackMode != inStackMode );
    mStackMode = inStackMode;
    mStackSearchButton->setState( mStackMode );
    
    if( stackModeChange ) {    
        if( mStackMode ) {
            remove( mSearchField );
            remove( mSearchLabel );
            
            add( mStackLabel );
            }
        else {
            remove( mStackLabel );
            
            add( mSearchLabel );
            add( mSearchField );
            }
        }
    
    setSelectedResource( inSelectedResource );

    // always do new search after clone (in case selected resource doesn't
    // change, so no new search would be triggered)
    newSearch( true );
    }



template <class ResourceType>
void ResourcePicker<ResourceType>::cloneState( 
    ResourcePicker *inOtherPicker ) {

    inOtherPicker->setState( mSelectedResource,
                             mSearchField->getText(),
                             mSearchResultOffset, 
                             mStackMode );
    }





template <class ResourceType>
int ResourcePicker<ResourceType>::countStackResults() {
    return mStack->size();
    }



template <class ResourceType>
int ResourcePicker<ResourceType>::getStackResults( int inNumToSkip,
                                                   int inNumToGet,
                                                   uniqueID *outIDs ) {
    int numGotten = 0;
    
    int stackSize = mStack->size();

    int end = inNumToSkip + inNumToGet;
    if( end > stackSize ) {
        end = stackSize;
        }

    for( int i=inNumToSkip; i<end; i++ ) {
        // reverse order (end of vector is top of stack
        outIDs[ numGotten ] = *( mStack->getElement( stackSize - i - 1 ) );
        numGotten++;
        }

    return numGotten;
    }



template <class ResourceType>
char ResourcePicker<ResourceType>::wasLastActionFromPress() {
    return mLastActionFromPress;
    }



template <class ResourceType>
ResourceType ResourcePicker<ResourceType>::getDraggedResource() {
    
    return mCurrentDraggedResource;
    }



template <class ResourceType>
char ResourcePicker<ResourceType>::isSearchFieldFocused() {
    return mSearchField->isFocused();
    }




#endif
