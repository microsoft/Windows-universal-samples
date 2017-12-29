#include "pch.h"
#include "Fx2Driver.h"

using namespace SDKTemplate;
using namespace Platform;
using namespace Windows::Devices::Custom;

IOControlCode^ Fx2Driver::GetSevenSegmentDisplay = ref new IOControlCode(DeviceType,
                                                                         FunctionBase + 7,
                                                                         IOControlAccessMode::Read,
                                                                         IOControlBufferingMethod::Buffered);

IOControlCode^ Fx2Driver::SetSevenSegmentDisplay = ref new IOControlCode(DeviceType,
                                                                         FunctionBase + 8,
                                                                         IOControlAccessMode::Write,
                                                                         IOControlBufferingMethod::Buffered);

IOControlCode^ Fx2Driver::ReadSwitches = ref new IOControlCode(DeviceType,
                                                               FunctionBase + 6,
                                                               IOControlAccessMode::Read,
                                                               IOControlBufferingMethod::Buffered);

IOControlCode^ Fx2Driver::GetInterruptMessage = ref new IOControlCode(DeviceType,
                                                                      FunctionBase + 9,
                                                                      IOControlAccessMode::Read,
                                                                      IOControlBufferingMethod::DirectOutput);


byte Fx2Driver::DigitToSevenSegment(byte value)
{
    return SevenSegmentValues[value];
}

byte Fx2Driver::SevenSegmentToDigit(byte value)
{
    for (byte i = 0; i < ARRAYSIZE(SevenSegmentValues); i += 1)
    {
        if (SevenSegmentValues[i] == value)
        {
            return i;
        }
    }
    return 0xff;
}

const byte Fx2Driver::SevenSegmentValues[10] = { 
    0xD7, // 0
    0x06, // 1
    0xB3, // 2
    0xA7, // 3
    0x66, // 4
    0xE5, // 5
    0xF4, // 6
    0x07, // 7
    0xF7, // 8
    0x67, // 9
};


#include <initguid.h>
// 573E8C73-0CB4-4471-A1BF-FAB26C31D384
DEFINE_GUID(GUID_DEVINTERFACE_OSRUSBFX2,
    0x573e8c73, 0xcb4, 0x4471, 0xa1, 0xbf, 0xfa, 0xb2, 0x6c, 0x31, 0xd3, 0x84);

const Guid Fx2Driver::DeviceInterfaceGuid = Guid(GUID_DEVINTERFACE_OSRUSBFX2);