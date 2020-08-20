#pragma once
// This class provides an IAudioEffectDefinition which can be used
// to configure and create a MixedRealityCaptureAudioEffect
// object. See https://developer.microsoft.com/en-us/windows/holographic/mixed_reality_capture_for_developers#creating_a_custom_mixed_reality_capture_.28mrc.29_recorder
// for more information about the effect definition properties.

#define RUNTIMECLASS_MIXEDREALITYCAPTURE_AUDIO_EFFECT L"Windows.Media.MixedRealityCapture.MixedRealityCaptureAudioEffect"

//
// MixerMode
// Type: AudioMixerMode as UINT32
//  0: Mic audio only
//  1: System audio only
//  2: Mixing mic and system audio (default)
//
#define PROPERTY_MIXERMODE  L"MixerMode"

//
// LoopbackGain: Gain to apply to system audio volume.
// Type : double (0.0 to 5.0)
// Default: 1.0
//
#define PROPERTY_LOOPBACKGAIN  L"LoopbackGain"

//
// MicrophoneGain: Gain to apply to mic volume.
// Type : double (0.0 to 5.0)
// Default: 1.0
//
#define PROPERTY_MICROPHONEGAIN  L"MicrophoneGain"

namespace MrcEffectDefinitions
{
    public enum class AudioMixerMode
    {
        Mic = 0,
        Loopback = 1,
        MicAndLoopback = 2
    };

    public ref class MrcAudioEffectDefinition sealed : public Windows::Media::Effects::IAudioEffectDefinition
    {
    public:
        MrcAudioEffectDefinition();

        //
        // IAudioEffectDefinition
        //
        property Platform::String^ ActivatableClassId
        {
            virtual Platform::String^ get()
            {
                return m_activatableClassId;
            }
        }

        property Windows::Foundation::Collections::IPropertySet^ Properties
        {
            virtual Windows::Foundation::Collections::IPropertySet^ get()
            {
                return m_propertySet;
            }
        }

        //
        // Mixed Reality Capture effect properties
        //
        property AudioMixerMode MixerMode
        {
            AudioMixerMode get()
            {
                return GetValueFromPropertySet<uint32_t>(m_propertySet, PROPERTY_MIXERMODE, DefaultAudioMixerMode);
            }

            void set(AudioMixerMode newValue)
            {
                m_propertySet->Insert(PROPERTY_MIXERMODE, static_cast<uint32_t>(newValue));
            }
        }

        property double LoopbackGain
        {
            double get()
            {
                return GetValueFromPropertySet<double>(m_propertySet, PROPERTY_LOOPBACKGAIN, DefaultLoopBackGain);
            }

            void set(double newValue)
            {
                m_propertySet->Insert(PROPERTY_LOOPBACKGAIN, static_cast<double>(newValue));
            }
        }

        property double MicrophoneGain
        {
            double get()
            {
                return GetValueFromPropertySet<double>(m_propertySet, PROPERTY_MICROPHONEGAIN, DefaultMicrophoneGain);
            }

            void set(double newValue)
            {
                m_propertySet->Insert(PROPERTY_MICROPHONEGAIN, static_cast<double>(newValue));
            }
        }

    private:
        static constexpr AudioMixerMode DefaultAudioMixerMode = AudioMixerMode::MicAndLoopback;
        static constexpr double DefaultLoopBackGain = 1.0f;
        static constexpr double DefaultMicrophoneGain = 1.0f;
    private:
        Platform::String^ m_activatableClassId = ref new Platform::String(RUNTIMECLASS_MIXEDREALITYCAPTURE_AUDIO_EFFECT);
        Windows::Foundation::Collections::PropertySet^ m_propertySet = ref new Windows::Foundation::Collections::PropertySet();
    };
}
