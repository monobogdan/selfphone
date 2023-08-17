#include <monohome.h>

const int BattVoltage4 = 4100;
const int BattVoltage3 = 3900;
const int BattVoltage2 = 3700;
const int BattVoltage1 = 3500;

void CPowerManager::ReadSysfs(char* path, char* buf, int len)
{
    FILE* f = fopen(path, "r");

    if(!f)
    {
        LOGF("Failed to read %s\n", path);
        return;
    }

    fgets(buf, len, f);
    fclose(f);
}

void CPowerManager::WriteSysfs(char* path, char* buf)
{
    FILE* f = fopen(path, "w");

    if(!f)
    {
        LOGF("Failed to read %s\n", path);
        return;
    }

    fputs(buf, f);
    fclose(f);
}

CPowerManager::CPowerManager()
{
    LOG("Initializing power manager\n");

    SetBacklight(0.5f);
    LOGF("Battery voltage: %fv\n", (float)GetBatteryVoltage() / 1000);
}

CPowerManager::~CPowerManager()
{
    
}

void CPowerManager::SetCPUClock(int mhz)
{
    // No dynamic govenor in our kernel, so we can't scale CPU frequency now   
}

void CPowerManager::SetBacklight(float backLight)
{
    char buf[8];

    if(backLight < 0)
        backLight = 0;

    if(backLight > 1.0f)
        backLight = 1.0f;

    int adj = (int)(backLight * 255);

    sprintf((char*)&buf, "%i", adj);
    WriteSysfs("/sys/devices/platform/leds-mt65xx/leds/lcd-backlight/brightness", (char*)&buf);
    LOGF("Adjust backlight to %s\n", &buf);
}


EBatteryLevel CPowerManager::GetBatteryLevel()
{
    int voltage = GetBatteryVoltage();

    if(voltage >= BattVoltage4)
        return blFullyCharged;

    if(voltage >= BattVoltage3)
        return blHigh;

    if(voltage >= BattVoltage2)
        return blMedium;

    return blLow;
}

int CPowerManager::GetBatteryVoltage()
{
    char buf[16];
    int ret = 0;

    ReadSysfs("/sys/class/power_supply/battery/BatterySenseVoltage", (char*)&buf, sizeof(buf));
    sscanf((char*)&buf, "%i", &ret);

    return ret;
}

bool CPowerManager::IsCharging()
{
    char buf[16];
    int ret = 0;

    ReadSysfs("/sys/class/power_supply/battery/ChargerVoltage", (char*)&buf, sizeof(buf));
    sscanf((char*)&buf, "%i", &ret);

    return ret > 3000;
}
