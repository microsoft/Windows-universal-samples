//
// Functions.xaml.h
// Declaration of the Functions class
//

#pragma once

#include "Functions.g.h"

namespace SDKTemplate
{
    using namespace ::Platform;
    using namespace ::Windows::Globalization;
    using namespace ::Windows::Globalization::DateTimeFormatting;
    using namespace ::Windows::UI::Xaml::Media;
    using namespace ::Windows::UI;
    using namespace ::Windows::UI::Xaml;

    public ref class ColorEntry sealed
    {
    public:
        property Platform::String^ ColorName;
        property Windows::UI::Color Color;

        Windows::UI::Xaml::Media::SolidColorBrush^ Brushify(Windows::UI::Color c)
        {
            return ref new SolidColorBrush(c);
        }
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Functions sealed
    {
    public:
        Functions();

        property xBindSampleModel::DataModel^ Model;

    private:
        void InitializeValues();
        void UpdateValuesClick(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ResetValuesClick(Object^, Windows::UI::Xaml::RoutedEventArgs^ e);
        DateTimeFormatter^ _longTimeFormatter = ref new DateTimeFormatter("longtime");

    public:
        property Windows::Foundation::Collections::IObservableVector<ColorEntry^>^ ColorEntries;

        String^ GetTime();
        String^ Display(String^ stringArg, int intArg, double floatArg, bool boolArg);
        String^  countVowels(String^  words);
        String^ CountEmployees(xBindSampleModel::IEmployee^ e);
        Windows::UI::Xaml::Media::SolidColorBrush^ MakeColor(double red, double green, double blue);
        String^  ColorValue(double red, double green, double blue);
        void ProcessColor(String^  value);
        void PopulateColorsClick(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ClearColorsClick(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        Windows::UI::Xaml::Visibility ShowPlaceholder(unsigned int count);
        static Windows::UI::Xaml::Media::SolidColorBrush^ TextColor(Windows::UI::Color c);
        property bool IsNew;
    };

}
