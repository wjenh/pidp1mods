#include <stdio.h>
#include "common.h"
#include "pdp1.h"
#include "iotHandler.h"

static FILE *fP;

int
iotHandler(PDP1 *pdp1P, int pulse, int completion)
{
    if( !fP )
    {
        fP = fopen("/tmp/iot", "a");
    }

    if( pulse )
    {
        fprintf(fP,"IOT 57 called, pulse 1.\n");
    }
    else
    {
        fprintf(fP,"IOT 57 called, pulse 0, breaking.\n");
        //initiateBreak(0);     currently broken, waiting on input from Angelo
    }

    return(1);
}

void
iotStop()
{
    fclose(fP);
    fP = 0;
}
