using System;
using System.Collections.Generic;
using Windows.Media.Effects;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Media;
using Windows.Media.MediaProperties;
using System.Runtime.InteropServices;

namespace CustomEffect
{
    // Using the COM interface IMemoryBufferByteAccess allows us to access the underlying byte array in an AudioFrame
    [ComImport]
    [Guid("5B0D3235-4DBA-4D44-865E-8F1D0E4FD04D")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    unsafe interface IMemoryBufferByteAccess
    {
        void GetBuffer(out byte* buffer, out uint capacity);
    }

    public sealed class AudioEchoEffect : IBasicAudioEffect
    {
        private AudioEncodingProperties currentEncodingProperties;
        private List<AudioEncodingProperties> supportedEncodingProperties;

        private float[] echoBuffer;
        private int currentActiveSampleIndex;
        private IPropertySet propertySet;

        // Mix does not have a set - all updates should be done through the property set.
        private float Mix
        {
            get
            {
                object val;
                if (propertySet != null && propertySet.TryGetValue("Mix", out val))
                {
                    return (float)val;
                }
                return .5f;
            }
        }

        public bool UseInputFrameForOutput { get { return false; } }

        // Set up constant members in the constructor
        public AudioEchoEffect()
        {
            // Support 44.1kHz and 48kHz mono float
            supportedEncodingProperties = new List<AudioEncodingProperties>();
            AudioEncodingProperties encodingProps1 = AudioEncodingProperties.CreatePcm(44100, 1, 32);
            encodingProps1.Subtype = MediaEncodingSubtypes.Float;
            AudioEncodingProperties encodingProps2 = AudioEncodingProperties.CreatePcm(48000, 1, 32);
            encodingProps2.Subtype = MediaEncodingSubtypes.Float;

            supportedEncodingProperties.Add(encodingProps1);
            supportedEncodingProperties.Add(encodingProps2);
        }
        
        public IReadOnlyList<AudioEncodingProperties> SupportedEncodingProperties
        {
            get
            {
                return supportedEncodingProperties;
            }
        }

        public void SetEncodingProperties(AudioEncodingProperties encodingProperties)
        {
            currentEncodingProperties = encodingProperties;

            // Create and initialize the echo array
            echoBuffer = new float[encodingProperties.SampleRate]; // exactly one second delay
            currentActiveSampleIndex = 0;
        }

        unsafe public void ProcessFrame(ProcessAudioFrameContext context)
        {
            AudioFrame inputFrame = context.InputFrame;
            AudioFrame outputFrame = context.OutputFrame;

            using (AudioBuffer inputBuffer = inputFrame.LockBuffer(AudioBufferAccessMode.Read),
                                outputBuffer = outputFrame.LockBuffer(AudioBufferAccessMode.Write))
            using (IMemoryBufferReference inputReference = inputBuffer.CreateReference(),
                                            outputReference = outputBuffer.CreateReference())
            {
                byte* inputDataInBytes;
                byte* outputDataInBytes;
                uint inputCapacity;
                uint outputCapacity;

                ((IMemoryBufferByteAccess)inputReference).GetBuffer(out inputDataInBytes, out inputCapacity);
                ((IMemoryBufferByteAccess)outputReference).GetBuffer(out outputDataInBytes, out outputCapacity);

                float* inputDataInFloat = (float*)inputDataInBytes;
                float* outputDataInFloat = (float*)outputDataInBytes;

                float inputData;
                float echoData;

                // Process audio data
                int dataInFloatLength = (int)inputBuffer.Length / sizeof(float);

                for (int i = 0; i < dataInFloatLength; i++)
                {
                    inputData = inputDataInFloat[i] * (1.0f - this.Mix);
                    echoData = echoBuffer[currentActiveSampleIndex] * this.Mix;
                    outputDataInFloat[i] = inputData + echoData;
                    echoBuffer[currentActiveSampleIndex] = inputDataInFloat[i];
                    currentActiveSampleIndex++;

                    if (currentActiveSampleIndex == echoBuffer.Length)
                    {
                        // Wrap around (after one second of samples)
                        currentActiveSampleIndex = 0;
                    }
                }
            }
        }

        public void Close(MediaEffectClosedReason reason)
        {
            // Clean-up any effect resources
            // This effect doesn't care about close, so there's nothing to do
        }

        public void DiscardQueuedFrames()
        {
            // Reset contents of the samples buffer
            Array.Clear(echoBuffer, 0, echoBuffer.Length - 1);
            currentActiveSampleIndex = 0;
        }

        public void SetProperties(IPropertySet configuration)
        {
            this.propertySet = configuration;
        }
    }
}