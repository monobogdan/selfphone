#include <monohome.h>

CDialerScreen::CDialerScreen()
{
    dialerButton = CImage::FromFile("ui/dialer_btn.tga");

    memset(&number, 0, sizeof(number));
}

CDialerScreen::~CDialerScreen()
{
    delete dialerButton;
}

void CDialerScreen::Update()
{
    CScreen::Update();
}

bool DialerButton(CImage* img, int x, int y, char* str)
{
    bool state = CGUI::Button(img, x, y);
    gLauncher->Graphics->DrawString(gLauncher->Font, str, x + (img->Width / 2) - 8, y + (img->Height / 2) - 8);

    return state;
}

void CDialerScreen::Draw()
{
    CScreen::Draw();

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            int num = i * 3 + j + 1;
            char buf[16];
            memset(&buf, 0, sizeof(buf));
            sprintf((char*)&buf, "%i", num);

            if(DialerButton(dialerButton, j * dialerButton->Width + 15, 65 + (i * dialerButton->Height) + windowAnimator->Y, buf))
            {
                if(strlen((char*)&number) < 31)
                    strcat((char*)&number, (char*)&buf);
            }
        }
    }

    if(DialerButton(dialerButton, 1 * dialerButton->Width + 15, 65 + (3 * dialerButton->Height) + windowAnimator->Y, "0"))
            {
                if(strlen((char*)&number) < 31)
                    strcat((char*)&number, "0");
            }

    if(DialerButton(dialerButton, 0 * dialerButton->Width + 15, 65 + (3 * dialerButton->Height) + windowAnimator->Y, "C"))
    {
        gLauncher->Modem->Dial((char*)&number);
    }

    gLauncher->Graphics->DrawString(gLauncher->Font, (char*)&number, 10, 48);
}