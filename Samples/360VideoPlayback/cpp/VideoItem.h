#pragma once
namespace _360VideoPlayback
{
    [Windows::UI::Xaml::Data::Bindable]
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class VideoItem sealed
    {
    public:

        VideoItem(Platform::String^ title, Windows::Foundation::Uri^ sourceUri)
            : m_title(title), m_sourceUri(sourceUri)
        {
        }

        property Platform::String^ Title
        {
            Platform::String^ get()
            {
                return m_title;
            }
        }

        property Windows::Foundation::Uri^ SourceUri
        {
            Windows::Foundation::Uri^ get()
            {
                return m_sourceUri;
            }
        }

    private:
        Platform::String^ m_title;
        Windows::Foundation::Uri^ m_sourceUri;

    };


}

