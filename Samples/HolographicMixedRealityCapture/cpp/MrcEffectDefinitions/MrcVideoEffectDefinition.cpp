#include "pch.h"
#include "MrcVideoEffectDefinition.h"

using namespace MrcEffectDefinitions;

MrcVideoEffectDefinition::MrcVideoEffectDefinition()
{
    StreamType = DefaultStreamType;
    HologramCompositionEnabled = DefaultHologramCompositionEnabled;
    RecordingIndicatorEnabled = DefaultRecordingIndicatorEnabled;
    VideoStabilizationEnabled = DefaultVideoStabilizationEnabled;
    VideoStabilizationBufferLength = DefaultVideoStabilizationBufferLength;
    GlobalOpacityCoefficient = DefaultGlobalOpacityCoefficient;
}
