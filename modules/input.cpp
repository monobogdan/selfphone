#include <monohome.h>
#include <linux/input.h>

/* Dunno why it's not defined in kernel headers, so define it here */
#define ABS_MT_SLOT		0x2f	/* MT slot being modified */
#define ABS_MT_TOUCH_MAJOR	0x30	/* Major axis of touching ellipse */
#define ABS_MT_TOUCH_MINOR	0x31	/* Minor axis (omit if circular) */
#define ABS_MT_WIDTH_MAJOR	0x32	/* Major axis of approaching ellipse */
#define ABS_MT_WIDTH_MINOR	0x33	/* Minor axis (omit if circular) */
#define ABS_MT_ORIENTATION	0x34	/* Ellipse orientation */
#define ABS_MT_POSITION_X	0x35	/* Center X touch position */
#define ABS_MT_POSITION_Y	0x36	/* Center Y touch position */
#define ABS_MT_TOOL_TYPE	0x37	/* Type of touching device */
#define ABS_MT_BLOB_ID		0x38	/* Group a set of packets as a blob */
#define ABS_MT_TRACKING_ID	0x39	/* Unique ID of initiated contact */
#define ABS_MT_PRESSURE		0x3a	/* Pressure on contact area */
#define ABS_MT_DISTANCE		0x3b	/* Contact hover distance */
#define ABS_MT_TOOL_X		0x3c	/* Center X tool position */
#define ABS_MT_TOOL_Y		0x3d	/* Center Y tool position */

CInput::CInput()
{
    LOG("Initializing input subsystem\n");
    // Open input device
	evDev = open("/dev/input/event3", O_RDWR | O_NONBLOCK);

    TouchState = tsIdle;
}

CInput::~CInput()
{
    close(evDev);
}

void CInput::Update()
{
    input_event ev;
	int ret = 0;
    bool gotEvent = false; // Touchscreen driver sends us events each input "frame". So, if we don't have BTN_TOUCH event, we can track releasing finger when there are no events in current frame.

	while((ret = read(evDev, &ev, sizeof(input_event)) != -1))
	{
		if(ev.code == ABS_MT_POSITION_X)
			TouchX = ev.value;
		
		if(ev.code == ABS_MT_POSITION_Y)
			TouchY = ev.value;

        gotEvent = true;
	}

    bool pressed = gotEvent;

    if(pressed && TouchState == tsIdle)
        TouchState = tsTouching;

    if(TouchState == tsReleased)
        TouchState = tsIdle;

    if(!pressed && TouchState == tsTouching)
        TouchState = tsReleased;
}

bool CInput::IsTouchedAt(int x, int y, int w, int h)
{
    return TouchX > x && TouchY > y && TouchX < x + w && TouchY < y + h && TouchState == tsReleased;
}

CGestureDetector::CGestureDetector()
{
    FPX = -999;
    FPY = -999;
}

CGestureDetector::~CGestureDetector()
{

}

void CGestureDetector::Update()
{
    if(gLauncher->Input->TouchState == tsTouching)
    {
        if(FPX == -999)
        {
            FPX = gLauncher->Input->TouchX;
            FPY = gLauncher->Input->TouchY;
        }

        ScrollX = gLauncher->Input->TouchX - FPX;
        ScrollY = gLauncher->Input->TouchY - FPY;
    }

    if(gLauncher->Input->TouchState == tsReleased)
    {
        FPX = -999;
        FPY = -999;
    }


}