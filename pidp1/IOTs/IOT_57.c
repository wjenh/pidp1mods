#include <stdio.h>
#include "common.h"
#include "pdp1.h"
#include "iotHandler.h"

static FILE *fP;

int
iotHandler(PDP1 *pdp1P, int pulse, int completion)
{
int chan;

    if( !fP )
    {
        fP = fopen("/tmp/iot", "w");
    }

    if( pulse )
    {
        fprintf(fP,"IOT 57 called, pulse 1.\n");
    }
    else
    {
        chan = (pdp1P->mb >> 6) & 077;      // we expect the 'control' bits to have a channel number
        fprintf(fP,"IOT 57 called, pulse 0, breaking on channel %d.\n", chan);
        initiateBreak(chan);
        fprintf(fP,"IOT 57 called, break issued.\n");
    }

    fflush(fP);
    return(1);
}

void
iotStart()
{
    if( !fP )
    {
        fP = fopen("/tmp/iot", "w");
    }

    if( fP )
    {
        fprintf(fP,"iotStart()\n");
    }
}

void
iotStop()
{
    if( fP )
    {
        fprintf(fP,"iotStop()\n");
        fclose(fP);
        fP = 0;
    }
}
