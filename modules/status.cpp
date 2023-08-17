#include <monohome.h>
#include <time.h>

CStatusBar::CStatusBar()
{
    imFiller = CImage::FromFile("ui/stFiller.tga");

    animator = new CAnimator();
    animator->SetTranslation(0, -imFiller->Height, 0, 0);
    animator->Run();

    imBattery[0] = CImage::FromFile("ui/batt0.tga");
    imBattery[1] = CImage::FromFile("ui/batt1.tga");
    imBattery[2] = CImage::FromFile("ui/batt2.tga");
    imBattery[3] = CImage::FromFile("ui/batt3.tga");
}

CStatusBar::~CStatusBar()
{
    delete animator;

    delete imFiller;
}

void CStatusBar::Update()
{
    animator->Update();
}

void CStatusBar::Draw()
{
    gLauncher->Graphics->DrawImage(imFiller, animator->X, animator->Y);
    gLauncher->Graphics->DrawImage(imBattery[(int)gLauncher->PowerManager->GetBatteryLevel()], imFiller->Width - imBattery[0]->Width - 5, animator->Y + 5);

    char timeFmt[64];
    time_t _time = time(0);
    tm* _localTime = localtime(&_time);
    strftime((char*)&timeFmt, sizeof(timeFmt), "%R", _localTime);

    gLauncher->Graphics->DrawString(gLauncher->Font, (char*)&timeFmt, 0, 0);
}