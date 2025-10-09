// Include to be used by IOT handler implementations
int iotHandler(PDP1 *, int, int);
void iotStart(void);
void iotStop(void);

// Hidden methods used for control, implemented here to hide details from handlers
typedef void (*IotSeqBreakP)(int);

static IotSeqBreakP _breakCallback;

// Called by pdp1.c during setup of this handler, not for direct use in a handler
void _setBreakCallback( IotSeqBreakP callback)
{
    _breakCallback = callback;
}

void initiateBreak(int chan)
{
    if( _breakCallback )
    {
        _breakCallback(chan);
    }
}
