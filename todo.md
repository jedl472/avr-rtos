TODO:
    - rewrite queue.c for it to require only one function to call for initialization 
      and also there should not be type mess with queue item
    
    - clean up tests so they are not all over the place
    
    - fix messy naming of core functions
    
    - fix task initialization for it to not require manual stack setup

    - add system ticks data type and counting (also add a macro to calculate ticks to suspend from queue);

    - higher priority task gets ready -> what will happen? (should be in documented)

    - add mutexes

    - revision of debug.c

    - add documentation inside header files

    - add queue ring buffer
