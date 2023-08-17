#include <monohome.h>
#include <time.h>

#define SMS_END 26

CLauncher* gLauncher;
CAnimator* anim;

APP_FACTORY(CDialerScreen)

CAppDesc _APhone = {
	"Phone",
	"ui/phone.tga",
	
	0,
	0,

	&__phone_factory_CDialerScreen
};

CAppDesc _ASMS = {
	"SMS",
	"ui/sms.tga"
};

CAppDesc _AGallery = {
	"Gallery",
	"ui/gallery.tga"
};

CAppDesc _ASettings = {
	"Settings",
	"ui/tools.tga"
};

CAppDesc* Apps[] = {
	&_APhone, &_ASMS, &_AGallery, &_ASettings
};

CLauncher::CLauncher()
{
	LOG("Starting...\n");
	Modem = new CModem();

	Graphics = new CGraphics();

	Wallpaper = CImage::FromFile("wallpaper.tga");
	Font = CFont::FromFile("ui/Courier");
	Status = new CStatusBar();
	Input = new CInput();
	PowerManager = new CPowerManager();
	
	Dialog = new CNotifyDialog();
	Dialog->Text = "Test";
	Dialog->Duration = 1000;

	drawerAnimator = new CAnimator();
	drawerAnimator->SetTranslation(-10, 0, 10, 52);
	drawerAnimator->Run();

	for(int i = 0; i < sizeof(Apps) / sizeof(CAppDesc*); i++)
	{
		LOGF("Preparing application \"%s\"\n", Apps[i]->Name);
		Apps[i]->Icon = CImage::FromFile(Apps[i]->IconPath);
	}
}

CLauncher::~CLauncher()
{
	delete Wallpaper;
	delete Font;
	delete Status;
	delete Input;
	delete PowerManager;

	delete drawerAnimator;

	delete Dialog;
	delete Graphics;
}

void CLauncher::DrawAppDrawer()
{
	for(int i = 0; i < sizeof(Apps) / sizeof(CAppDesc*); i++)
	{
		int x = drawerAnimator->X + (i * 75);
		int y = drawerAnimator->Y;
		Graphics->DrawImage(Apps[i]->Icon, x, y);

		if(Input->IsTouchedAt(x, y, Apps[i]->Icon->Width, Apps[i]->Icon->Height))
		{
			StartScreen(new CDialerScreen());
		}
	}
}

void CLauncher::StartScreen(CScreen* screen)
{
	if(screen)
	{
		currentScreen = screen;
		currentScreen->Show();
	}
}

int nextCheck = 0;

void CLauncher::Run()
{
	CImage* test = CImage::FromFile("ui/stFiller.tga");;

	while(true)
	{
		Input->Update();

		if(nextCheck < 0)
		{
			Modem->GetDialState();
			nextCheck = 50;
		}

		nextCheck--;

		Graphics->DrawImage(Wallpaper, 0, 0);

		if(currentScreen)
		{
			currentScreen->Update();	
			currentScreen->Draw();
		}
		else
		{
			drawerAnimator->Update();
			DrawAppDrawer();
		}

		Status->Update();
		Status->Draw();

		Dialog->Draw();

		Graphics->Flip();
	}
}

int main(int argc, char** argv)
{
	gLauncher = new CLauncher();
	gLauncher->Run();
	
	return 0;
}