#include <monohome.h>

bool CGUI::Button(CImage* img, int x, int y)
{
    if(img)
    {
        gLauncher->Graphics->DrawImage(img, x, y);

        return gLauncher->Input->IsTouchedAt(x, y, img->Width, img->Height);		
    }
}

void CNotifyDialog::Draw()
{
    if(Text && strlen(Text) > 0 && Duration > 0)
    {
        int h = 64;
        CColor col = { 225, 225, 225 };
        int baseY = 480 / 2 - (h / 2);

        // Right now, resolution is hardcoded here. We shouldn't do like that.
        gLauncher->Graphics->FillRect(20, baseY, 320 - 40, h, col, 1.0f);
        gLauncher->Graphics->DrawString(gLauncher->Font, Text, 35, baseY + (h / 2 - 8));
    }

    Duration -= 1;
}