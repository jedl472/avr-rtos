#ifndef CONFIG_H
#define CONFIG_H

// -----------------------
// CORE config
// -----------------------

// cpu frequency set through fuses
#define configCPU_FREQ 16000000UL // for 16 mHz nano m328p
                      
// frequency of os tick interrupts
// value should be between 1000 Hz and 250 000 Hz for prescaler set to 64
#define configOS_TICK_FREQ 1000 // [Hz] 

// maximum number of registered tasks
#define configMAX_TASKS 8



// -----------------------
// QUEUE config
// -----------------------

// If queue_add is called on a full queue, the element is either voidded or kp is thrown
#define configVOID_ELEMENTS_ADDED_TO_FULL_QUEUE 0



#endif // !CONFIG_H
