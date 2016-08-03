//
// EventTests.xaml.cpp
// Implementation of the EventTests class
//

#include "pch.h"
#include "Functions.xaml.h"
#include "Strsafe.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Popups;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Functions::Functions()
{
    InitializeComponent();
    this->ColorEntries = ref new Platform::Collections::Vector<ColorEntry^>();
    this->Model = ref new xBindSampleModel::DataModel();
}

void Functions::InitializeValues()
{
    this->Model->InitializeValues();
}

void Functions::UpdateValuesClick(Object^ sender, ::Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->Model->UpdateValues();
}

void Functions::ResetValuesClick(Object^ sender, ::Windows::UI::Xaml::RoutedEventArgs^ e)
{
    this->Model->InitializeValues();
    this->InitializeValues();
}

Platform::String^ Functions::GetTime()
{
    Calendar^ calendar = ref new Calendar();
    return _longTimeFormatter->Format(calendar->GetDateTime());
}


Platform::String^ Functions::Display(Platform::String^ stringArg, int intArg, double floatArg, bool boolArg)
{
    WCHAR longString[1024];
    HRESULT hr = S_OK;
    hr = StringCchPrintf(longString, _countof(longString),
        L"String is: %s, Integer is: %d, Float is: %f, Boolean is %s",
        stringArg->Data(), intArg, floatArg, (boolArg ? L"true" : L"false"));
    if (FAILED(hr))
    {
        throw ref new COMException(hr);
    }
    return ref new String(longString);
}

Platform::String^  Functions::countVowels(Platform::String^  words)
{
    int count = 0;
    PCWSTR wordsData = words->Data();
    for (UINT i = 0; i < words->Length(); i++ )
    {
        WCHAR ch = wordsData[i];
        if (ch == 'a' || ch == 'A' ||
            ch == 'e' || ch == 'E' ||
            ch == 'i' || ch == 'I' ||
            ch == 'o' || ch == 'O' ||
            ch == 'u' || ch == 'U')
        {
            count++;
        }
    }
    return count.ToString();
}

int CountEmployeesInternal(xBindSampleModel::IEmployee^ e)
{
    int reports = 1;
    if (e->IsManager)
    {
        xBindSampleModel::Manager^ manager = safe_cast<xBindSampleModel::Manager^>(e);
        for (xBindSampleModel::IEmployee^ employee : manager->ReportsList)
        {
            reports += CountEmployeesInternal(employee);
        }
    }
    return reports;
}

Platform::String^ Functions::CountEmployees(xBindSampleModel::IEmployee^ e)
{
    return CountEmployeesInternal(e).ToString();
}

Windows::UI::Xaml::Media::SolidColorBrush^ Functions::MakeColor(double red, double green, double blue)
{
    return ref new SolidColorBrush(Windows::UI::ColorHelper::FromArgb(255, (byte)red, (byte)green, (byte)blue));
}

Platform::String^ Functions::ColorValue(double red, double green, double blue)
{
    WCHAR longString[1024];
    HRESULT hr = S_OK;
    hr = StringCchPrintf(longString, _countof(longString), L"#%02X%02X%02X",(int)red, (int)green, (int)blue);
    if (FAILED(hr))
    {
        throw ref new COMException(hr);
    }
    return ref new String(longString);
}

int GetHexDigit(const WCHAR hexDigit)
{
    int digit = 0;

    if( hexDigit >= 'a' && hexDigit <= 'f')
    {
        return 10 + hexDigit - 'a';
    }
    if (hexDigit >= 'A' && hexDigit <= 'F')
    {
        return 10 + hexDigit - 'A';
    }
    if (isdigit(hexDigit))
    {
        return hexDigit - '0';
    }
    throw ref new InvalidArgumentException();
}

int ParseColorDoubleByte(PCWSTR colorString, int start)
{
    int firstDigit = GetHexDigit(colorString[start]);
    int secondDigit = GetHexDigit(colorString[start +1]);

    return firstDigit * 16 + secondDigit;
}

void Functions::ProcessColor(Platform::String^ value)
{
    PCWSTR colorString = value->Data();
    if (value->Length() == 7 && colorString[0] == '#')
    {
        int red = ParseColorDoubleByte(colorString, 1);
        int green = ParseColorDoubleByte(colorString, 3);
        int blue = ParseColorDoubleByte(colorString, 5);

        RedSlider->Value = red;
        GreenSlider->Value = green;
        BlueSlider->Value = blue;
    }
    else
    {
        throw ref new InvalidArgumentException();
    }
}

Platform::String^ _colorNames[] =
{
    "AliceBlue", "AntiqueWhite", "Aqua", "Aquamarine", "Azure", "Beige", "Bisque", "Black", "BlanchedAlmond", "Blue", "BlueViolet", "Brown", "BurlyWood", "CadetBlue", "Chartreuse", "Chocolate", "Coral", "CornflowerBlue", "Cornsilk", "Crimson", "Cyan", "DarkBlue", "DarkCyan", "DarkGoldenrod",
    "DarkGray", "DarkGreen", "DarkKhaki", "DarkMagenta", "DarkOliveGreen", "DarkOrange", "DarkOrchid", "DarkRed", "DarkSalmon", "DarkSeaGreen", "DarkSlateBlue", "DarkSlateGray", "DarkTurquoise", "DarkViolet", "DeepPink", "DeepSkyBlue", "DimGray", "DodgerBlue", "Firebrick", "FloralWhite",
    "ForestGreen", "Fuchsia", "Gainsboro", "GhostWhite", "Gold", "Goldenrod", "Gray", "Green", "GreenYellow", "Honeydew", "HotPink", "IndianRed", "Indigo", "Ivory", "Khaki", "Lavender", "LavenderBlush", "LawnGreen", "LemonChiffon", "LightBlue", "LightCoral", "LightCyan", "LightGoldenrodYellow",
    "LightGray", "LightGreen", "LightPink", "LightSalmon", "LightSeaGreen", "LightSkyBlue", "LightSlateGray", "LightSteelBlue", "LightYellow", "Lime", "LimeGreen", "Linen", "Magenta", "Maroon", "MediumAquamarine", "MediumBlue", "MediumOrchid", "MediumPurple", "MediumSeaGreen", "MediumSlateBlue",
    "MediumSpringGreen", "MediumTurquoise", "MediumVioletRed", "MidnightBlue", "MintCream", "MistyRose", "Moccasin", "NavajoWhite", "Navy", "OldLace", "Olive", "OliveDrab", "Orange", "OrangeRed", "Orchid", "PaleGoldenrod", "PaleGreen", "PaleTurquoise", "PaleVioletRed", "PapayaWhip", "PeachPuff",
    "Peru", "Pink", "Plum", "PowderBlue", "Purple", "Red", "RosyBrown", "RoyalBlue", "SaddleBrown", "Salmon", "SandyBrown", "SeaGreen", "SeaShell", "Sienna", "Silver", "SkyBlue", "SlateBlue", "SlateGray", "Snow", "SpringGreen", "SteelBlue", "Tan", "Teal", "Thistle", "Tomato", "Transparent",
    "Turquoise", "Violet", "Wheat", "White", "WhiteSmoke", "Yellow", "YellowGreen"
};

Windows::UI::Color _colors[] = 
{
    Windows::UI::Colors::AliceBlue, Windows::UI::Colors::AntiqueWhite, Windows::UI::Colors::Aqua, Windows::UI::Colors::Aquamarine, Windows::UI::Colors::Azure, Windows::UI::Colors::Beige, Windows::UI::Colors::Bisque, Windows::UI::Colors::Black, Windows::UI::Colors::BlanchedAlmond,
    Windows::UI::Colors::Blue, Windows::UI::Colors::BlueViolet, Windows::UI::Colors::Brown, Windows::UI::Colors::BurlyWood, Windows::UI::Colors::CadetBlue, Windows::UI::Colors::Chartreuse, Windows::UI::Colors::Chocolate, Windows::UI::Colors::Coral, Windows::UI::Colors::CornflowerBlue,
    Windows::UI::Colors::Cornsilk, Windows::UI::Colors::Crimson, Windows::UI::Colors::Cyan, Windows::UI::Colors::DarkBlue, Windows::UI::Colors::DarkCyan, Windows::UI::Colors::DarkGoldenrod, Windows::UI::Colors::DarkGray, Windows::UI::Colors::DarkGreen, Windows::UI::Colors::DarkKhaki,
    Windows::UI::Colors::DarkMagenta, Windows::UI::Colors::DarkOliveGreen, Windows::UI::Colors::DarkOrange, Windows::UI::Colors::DarkOrchid, Windows::UI::Colors::DarkRed, Windows::UI::Colors::DarkSalmon, Windows::UI::Colors::DarkSeaGreen, Windows::UI::Colors::DarkSlateBlue,
    Windows::UI::Colors::DarkSlateGray, Windows::UI::Colors::DarkTurquoise, Windows::UI::Colors::DarkViolet, Windows::UI::Colors::DeepPink, Windows::UI::Colors::DeepSkyBlue, Windows::UI::Colors::DimGray, Windows::UI::Colors::DodgerBlue, Windows::UI::Colors::Firebrick,
    Windows::UI::Colors::FloralWhite, Windows::UI::Colors::ForestGreen, Windows::UI::Colors::Fuchsia, Windows::UI::Colors::Gainsboro, Windows::UI::Colors::GhostWhite, Windows::UI::Colors::Gold, Windows::UI::Colors::Goldenrod, Windows::UI::Colors::Gray, Windows::UI::Colors::Green,
    Windows::UI::Colors::GreenYellow, Windows::UI::Colors::Honeydew, Windows::UI::Colors::HotPink, Windows::UI::Colors::IndianRed, Windows::UI::Colors::Indigo, Windows::UI::Colors::Ivory, Windows::UI::Colors::Khaki, Windows::UI::Colors::Lavender, Windows::UI::Colors::LavenderBlush,
    Windows::UI::Colors::LawnGreen, Windows::UI::Colors::LemonChiffon, Windows::UI::Colors::LightBlue, Windows::UI::Colors::LightCoral, Windows::UI::Colors::LightCyan, Windows::UI::Colors::LightGoldenrodYellow, Windows::UI::Colors::LightGray, Windows::UI::Colors::LightGreen,
    Windows::UI::Colors::LightPink, Windows::UI::Colors::LightSalmon, Windows::UI::Colors::LightSeaGreen, Windows::UI::Colors::LightSkyBlue, Windows::UI::Colors::LightSlateGray, Windows::UI::Colors::LightSteelBlue, Windows::UI::Colors::LightYellow, Windows::UI::Colors::Lime,
    Windows::UI::Colors::LimeGreen, Windows::UI::Colors::Linen, Windows::UI::Colors::Magenta, Windows::UI::Colors::Maroon, Windows::UI::Colors::MediumAquamarine, Windows::UI::Colors::MediumBlue, Windows::UI::Colors::MediumOrchid, Windows::UI::Colors::MediumPurple,
    Windows::UI::Colors::MediumSeaGreen, Windows::UI::Colors::MediumSlateBlue, Windows::UI::Colors::MediumSpringGreen, Windows::UI::Colors::MediumTurquoise, Windows::UI::Colors::MediumVioletRed, Windows::UI::Colors::MidnightBlue, Windows::UI::Colors::MintCream, Windows::UI::Colors::MistyRose,
    Windows::UI::Colors::Moccasin, Windows::UI::Colors::NavajoWhite, Windows::UI::Colors::Navy, Windows::UI::Colors::OldLace, Windows::UI::Colors::Olive, Windows::UI::Colors::OliveDrab, Windows::UI::Colors::Orange, Windows::UI::Colors::OrangeRed, Windows::UI::Colors::Orchid,
    Windows::UI::Colors::PaleGoldenrod, Windows::UI::Colors::PaleGreen, Windows::UI::Colors::PaleTurquoise, Windows::UI::Colors::PaleVioletRed, Windows::UI::Colors::PapayaWhip, Windows::UI::Colors::PeachPuff, Windows::UI::Colors::Peru, Windows::UI::Colors::Pink, Windows::UI::Colors::Plum,
    Windows::UI::Colors::PowderBlue, Windows::UI::Colors::Purple, Windows::UI::Colors::Red, Windows::UI::Colors::RosyBrown, Windows::UI::Colors::RoyalBlue, Windows::UI::Colors::SaddleBrown, Windows::UI::Colors::Salmon, Windows::UI::Colors::SandyBrown, Windows::UI::Colors::SeaGreen,
    Windows::UI::Colors::SeaShell, Windows::UI::Colors::Sienna, Windows::UI::Colors::Silver, Windows::UI::Colors::SkyBlue, Windows::UI::Colors::SlateBlue, Windows::UI::Colors::SlateGray, Windows::UI::Colors::Snow, Windows::UI::Colors::SpringGreen, Windows::UI::Colors::SteelBlue,
    Windows::UI::Colors::Tan, Windows::UI::Colors::Teal, Windows::UI::Colors::Thistle, Windows::UI::Colors::Tomato, Windows::UI::Colors::Transparent, Windows::UI::Colors::Turquoise, Windows::UI::Colors::Violet, Windows::UI::Colors::Wheat, Windows::UI::Colors::White,
    Windows::UI::Colors::WhiteSmoke, Windows::UI::Colors::Yellow, Windows::UI::Colors::YellowGreen
};

void Functions::PopulateColorsClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    _ASSERT(_countof(_colors) == _countof(_colorNames));
    for (int i = 0; i < _countof(_colors); i++)
    {
        ColorEntry^ ce = ref new ColorEntry();
        ce->ColorName = _colorNames[i];
        ce->Color = _colors[i];
        ColorEntries->Append(ce);
    }
}

void Functions::ClearColorsClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ColorEntries->Clear();
}

Windows::UI::Xaml::Visibility Functions::ShowPlaceholder(unsigned int count)
{
    return count > 0 ? Windows::UI::Xaml::Visibility::Collapsed : Windows::UI::Xaml::Visibility::Visible;
}

SolidColorBrush^ Functions::TextColor(Windows::UI::Color c)
{
    return ref new SolidColorBrush(((c.R * 0.299 + c.G * 0.587 + c.B * 0.114) > 150) ? Colors::Black : Colors::White);
}