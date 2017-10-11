#include "resourceManager.h"
#include "resourceDatabase.h"
#include "usageDatabase.h"

#include <stdlib.h>


int main() {

    // init this first so it creates dir where stringDB can go.
    initResourceManager();
    initDatabase();
    
    for( int i=0; i<5; i++ ) {    
        initUsageDatabase();
        freeUsageDatabase();
        }
    exit( 0 );

    }
