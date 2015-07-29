#include "pch.h"
#include "DisplaySizeClass.h"
#include "sysinfoapi.h"

using namespace DisplaySizeHelper;
using namespace Platform;

DisplaySizeClass::DisplaySizeClass()
{
}

double DisplaySizeHelper::DisplaySizeClass::GetDisplaySizeInInches()
{
    double sizeInInches = 7;
    HRESULT result = GetIntegratedDisplaySize(&sizeInInches);
    if (result == S_OK)
        return sizeInInches;
    else
        return -1; //This will happen for non-integrated displays like monitors, projection surfaces etc.,
}
