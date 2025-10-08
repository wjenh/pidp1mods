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
        pdp1P->io = pdp1P->tw;       // copy the test word settings into the io register
        fprintf(fP,"IOT 57 called, pulse 0.\n");
    }

    return(1);
}

void
iotStop()
{
    fclose(fP);
    fP = 0;
}
