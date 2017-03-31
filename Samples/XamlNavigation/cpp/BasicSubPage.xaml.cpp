//
// BasicSubPage.xaml.cpp
// Implementation of the BasicSubPage class
//

#include "pch.h"
#include "BasicSubPage.xaml.h"

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

using namespace NavigationMenuSample::Views;

BasicSubPage::BasicSubPage()
{
    InitializeComponent();
}


void BasicSubPage::OnNavigatedTo(NavigationEventArgs^ e)
{
    auto param = dynamic_cast<String^>(e->Parameter);
    if (nullptr != param)
    {
        std::wstring msg(param->Data());
        const wchar_t* format = L"Clicked on %s";
        wchar_t buffer[75];
        swprintf_s(buffer, 75, format, msg.c_str());

        Message = ref new String(buffer);
    }
    else
    {
        Message = "Hi!";
    }

    Page::OnNavigatedTo(e);
}

String^ BasicSubPage::Message::get()
{
    return _string;
}

void BasicSubPage::Message::set(String^ value)
{
    _string = value;
}
