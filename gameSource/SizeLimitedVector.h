#ifndef SIZE_LIMITED_VECTOR_INCLUDED
#define SIZE_LIMITED_VECTOR_INCLUDED


#include "minorGems/util/SimpleVector.h"


// vector that trims earlier elements if size passes double of maxSize

template <class Type>
class SizeLimitedVector : public SimpleVector<Type> {
        
    public:
        

        // inDeleteWhenCulling true to perform a "delete" operation
        // on culled elements before discarding them (should be set
        // when vector contains pointers)
        SizeLimitedVector( int inLimit, char inDeleteWhenCulling );
        


        // override
        void push_back( Type x );
        

    protected:
        
        int mLimit;
        char mDeleteWhenCulling;
        

        // must be implemented specially for each instantiation types
        // used to destroy elements when culling excess in vector
        //
        // can do nothing if no element destruction necessary
        // Only called if inDeleteWhenCulling is set in constructor
        void deleteElementOfType( Type inElement );
        

    };





template <class Type>
inline SizeLimitedVector<Type>::SizeLimitedVector( int inLimit,
                                                   char inDeleteWhenCulling )
        : mLimit( inLimit ),
          mDeleteWhenCulling( inDeleteWhenCulling ) {
    }




template <class Type>
inline void SizeLimitedVector<Type>::push_back( Type x ) {
    SimpleVector<Type>::push_back( x );
    
    int oldNum = SimpleVector<Type>::size();
    

    if( oldNum >= mLimit * 2 ) {
        printf( "Passed limit of %d with %d elements...\n",
                mLimit, oldNum );
        
        // cull back down to mLimit elements by removing excess
        
        int numToRemove = oldNum - mLimit;


        if( mDeleteWhenCulling ) {
            for( int i=0; i<numToRemove; i++ ) {
                // delete values being removed
                //delete SimpleVector<Type>::elements[i];
                deleteElementOfType( SimpleVector<Type>::elements[i] );
                }
            }
        
            

        for( int i=numToRemove; i<oldNum; i++ ) {
            // note that this will invoke assignment operators on elements
            // which is what we need
            
            SimpleVector<Type>::elements[i - numToRemove] = 
                SimpleVector<Type>::elements[i];
            }

        SimpleVector<Type>::numFilledElements -= numToRemove;


        printf( "...After culling, %d left\n", SimpleVector<Type>::size() );
        }
    
    }



#endif

        
