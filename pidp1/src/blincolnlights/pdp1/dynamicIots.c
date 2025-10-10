/**
 * This implements dynamic loading and execution of custom IOT commands for the pidp-1.
 * When an unknown IOT is seen by the emulator, this will search for a handler compilied as a shared object.
 * The file must be named 'IOT_nn.so', where nn is the OCTAL IOT device number.
 * If found, it is registered and then used to handle the IOT.
 *
 * The handler will be called twice for every IOT instruction for it, once at the start of IOT 'hardware' pulse,
 * again at the end of the pulse.
 *
 * The handler is passed a pointer to the PDP structure which contains the entire state information of the emulator.
 * If the handler returns 0, then the IOT will be treated as an undefined IOT as if there was no handler.
 * Any other return value means the IOT was processed.
 *
 * An implemented IOT handler must implement an iotHandler() methood.
 * It can optionally implement:
 * void iotStart(void); - called when the emulator transitions to run state
 * void iotStop(void); - called when the emulator transitions to halt state
 *
 */

#include <unistd.h>
#include <dlfcn.h>

#include "common.h"
#include "pdp1.h"
#include "dynamicIots.h"

#include <stdio.h>

typedef struct
{
    int invalid;        // if 1, we tried to load already, nothing found
    void *dlHandleP;
    IotHandlerP handlerP;
    IotStartP startP;
    IotStopP stopP;
} IotEntry, *IotEntryP;

static int stopped = 1;         // assume we are halted initially
static IotEntry handles[64];

extern PDP1 *visiblePDP1P;      // from main.c
extern void dynamicReq(PDP1 *pdp, int chan);

void dynamicIotProcessBreak(int chan);

// Called from the emulator to try to invoke a dynamic IOT.
// It is called twice for each IOT, once on the IOT start pulse rising edge, once on the falling edge.
// The completion parameter is derived from bits 5 and 6 of the IOT instruction and can have the values:
// 0 - no completion pulse expected
// 1 - completion pulse enabled
// Returns 1 for success, 0 if none found.
int
dynamicIotProcessor(PDP1 *pdpP, int dev, int pulse, int completion)
{
int status;
char fname[128];

    if( dev > 63 )
    {
        return(0);              // bad dev value, treat as unknown
    }

    IotEntryP entryP = &handles[dev];

    if( entryP->invalid )       // been here before, nothing
    {
        return(0);
    }
    else if( !entryP->dlHandleP )    // it hasn't been resolved yet
    {
        sprintf(fname,"/opt/pidp1/IOTs/IOT_%2o.so", dev);

        if( !(entryP->dlHandleP = dlopen(fname, RTLD_LAZY)) )
        {
            // Not found, record that and fail
            entryP->invalid = 1;
            return(0);
        }

        entryP->handlerP = (IotHandlerP)dlsym(entryP->dlHandleP, "iotHandler");
        if( !entryP->handlerP )
        {
            dlclose(entryP->dlHandleP);
            entryP->invalid = 1;
            return(0);
        }

        // Should be implemented, but if not, ignore
        IotSeqBreakHandlerP breakP = (IotSeqBreakHandlerP)dlsym(entryP->dlHandleP, "_setBreakCallback");
        if( breakP )
        {
            breakP(dynamicIotProcessBreak);         // set the callback in the handler
        }

        // not required to be implemented
        entryP->startP = (IotStartP)dlsym(entryP->dlHandleP, "iotStart");
        entryP->stopP = (IotStopP)dlsym(entryP->dlHandleP, "iotStop");
    }

    stopped = 0;
    status = entryP->handlerP(pdpP, pulse, completion);
    return( status );
}

void
dynamicIotProcessBreak(int chan)
{
    if( chan > 0)
    {
        dynamicReq(visiblePDP1P, chan);               // signal a break, convoluted because of various unshared bits
    }
}

// Called when the emulator is started so IOTs that need to can clean up.
void
dynamicIotProcessorStart(void)
{
int i;
IotStartP startP;

    if( !stopped )
    {
        return;             // already done
    }

    for( i = 0; i < 65; ++i )
    {
        if( (startP = handles[i].startP) )
        {
            startP();
        }
    }

    stopped = 0;
}

// Called when the emulator is halted so IOTs that need to can clean up.
void
dynamicIotProcessorStop(void)
{
int i;
IotStopP stopP;

    if( stopped )
    {
        return;             // already done
    }

    for( i = 0; i < 65; ++i )
    {
        if( (stopP = handles[i].stopP) )
        {
            stopP();
        }
    }

    stopped = 1;
}
