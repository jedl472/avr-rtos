TODO:
    
    - rewrite queue.c for it to require only one function to call for initialization 
      and also there should not be type mess with queue item
    
    - clean up tests so they are not all over the place
    
    - fix messy naming of core functions
    
    - add system ticks data type and counting (also add a macro to calculate ticks to suspend from queue);

    - document where higher priority tasks can get ready

    - revision of debug.c

    - add documentation inside header files

    - add queue ring buffer

    - add higher_priority_task_woken to queue_send

    - move large constants into PROGMEM

    - make small functions inline
