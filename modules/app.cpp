#include <monohome.h>

CScreen::CScreen()
{
    windowAnimator = new CAnimator();
    windowAnimator->SetTranslation(0, 480, 0, 0);
}

CScreen::~CScreen()
{
    delete windowAnimator;
}

void CScreen::Show()
{
    windowAnimator->Run();
}

void CScreen::Update()
{
    windowAnimator->Update();
}

void CScreen::Draw()
{
    CColor col = { 235, 235, 235 };
    gLauncher->Graphics->FillRect(0, windowAnimator->Y, 320, 480, col, 0.9f);
}

void CScreen::Hide()
{
    
}