#include <monohome.h>
#include <time.h>

CAnimator::CAnimator()
{
    SetDuration(1.0f);
}

CAnimator::~CAnimator()
{
    
}

void CAnimator::SetTranslation(int xFrom, int yFrom, int xTo, int yTo)
{
    this->xFrom = xFrom;
    this->yFrom = yFrom;
    this->xTo = xTo;
    this->yTo = yTo;
}

void CAnimator::SetRotation(float from, float to)
{
    rFrom = from;
    rTo = to;
}

void CAnimator::SetDuration(float speed)
{
    duration = speed;
}

float lerp(float a, float b, float f)
{
    return a * (1.0 - f) + (b * f);
}

bool CAnimator::Update()
{
    Time += 0.25f;

    if(Time > 1.0f)
        Time = 1.0f;

    X = (int)lerp((float)xFrom, (float)xTo, Time);
    Y = (int)lerp((float)yFrom, (float)yTo, Time);

    Rotation = lerp(rFrom, rTo, Time);
}

void CAnimator::Run()
{
    Time = 0;

    IsPlaying = true;
}