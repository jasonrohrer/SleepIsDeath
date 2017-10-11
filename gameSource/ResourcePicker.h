#ifndef RESOURCE_PICKER_INCLUDED
#define RESOURCE_PICKER_INCLUDED 

#include "minorGems/graphics/openGL/gui/GUIContainerGL.h"
#include "minorGems/ui/event/ActionListenerList.h"
#include "minorGems/graphics/openGL/gui/TextFieldGL.h"

#include "BorderPanel.h"
#include "Sprite.h"
#include "buttons.h"
#include "uniqueID.h"
#include "labels.h"


template <class ResourceType>
class ResourcePicker : public BorderPanel, public ActionListener,
                       public ActionListenerList {


    public:



        /**
         * Constructs a picker.
         *
         * @param inAnchorX the x position of the upper left corner
         *   of this component.
         * @param inAnchorY the y position of the upper left corner
         *   of this component.
         * @param inStack pointer to the stack shared by this picker.
         *   Must be destroyed externally.
         *
         * Sets its own width and height automatically.
         */
        ResourcePicker( double inAnchorX, double inAnchorY,
                        SimpleVector<uniqueID> *inStack );



        virtual ~ResourcePicker();


        
        ResourceType getSelectedResource();        
        uniqueID getSelectedResourceID();
        
        void setSelectedResource( ResourceType inResource,
                                  char inForceResultRefresh = false,
                                  char inFireEvent = true );
                
        
        void forceNewSearch();
        

        // used to check if DragAndDrop should be started by an action
        char wasLastActionFromPress();

        ResourceType getDraggedResource();


        
        char isSearchFieldFocused();
        
        
        // Forces this picker to recheck if selected resource is deleteable
        // (should be called after a usage database change)
        void recheckDeletable();
        
        
        // action listener
        virtual void actionPerformed( GUIComponent *inTarget );


        
        // copy the state from this picker onto another picker
        virtual void cloneState( ResourcePicker *inOtherPicker );

        
    protected:
        

        // force the state of this picker
        void setState( ResourceType inSelectedResource,
                       char *inSearchString,
                       int inResultOffset,
                       char inStackMode );



        static const int mNumResourcesDisplayedMax = 6;
        
        ResourceType mResourcesDisplayed[mNumResourcesDisplayedMax];
        
        //Sprite *mDisplaySprites[mNumResourcesDisplayedMax];
        
        static const int mGridW = 3;
        static const int mGridH = 2;
        
        DraggableTwoSpriteButtonGL *mButtonGrid[mGridH][mGridW];

        char mLastActionFromPress;

        ResourceType mCurrentDraggedResource;
        

        // defaults to NULL
        // can be overridden by subclasses to provide a background
        // behind transparent resources
        // if non-NULL, will be destroyed by caller
        virtual Sprite *getButtonGridBackground();
        


        ResourceType mSelectedResource;
        
        DraggableTwoSpriteButtonGL *mSelectedResourceButton;

        DeleteButtonGL *mDeleteButton;
        

        LeftButtonGL *mLeftButton;
        RightButtonGL *mRightButton;
        

        StackSearchButtonGL *mStackSearchButton;
        

        int mNumResourcesOnScreen;
        
        int mSearchResultOffset;
        int mNumSearchResults;

        TextFieldGL *mSearchField;

        EightPixelLabel *mSearchLabel;
        EightPixelLabel *mStackLabel;
        


        void newSearch( char inPreservePageOffset=false, 
                        char inMoveBackward=false );
        void getFreshResults( char inMoveBackward=false );

        void refreshSelectedResourceSprite();
        
        void lowerBoundSearchOffset();
        

        char mStackMode;

        // pointer passed in from outside (so that two pickers can share
        // the same stack)
        SimpleVector<uniqueID> *mStack;


        int countStackResults();


        // caller allocates spaces for inNumToGet and passes pointer as outIDs
        // returns the number returned
        int getStackResults( int inNumToSkip,
                             int inNumToGet,
                             uniqueID *outIDs );
    };



#endif



