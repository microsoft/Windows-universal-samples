#include "pch.h"
#include "NavMenuItem.h"

using namespace Platform;
using namespace Windows::Foundation;

namespace NavigationMenuSample
{
    NavMenuItem::NavMenuItem(
        String^ label,
        Windows::UI::Xaml::Controls::Symbol symbol,
        Windows::UI::Xaml::Interop::TypeName destPage)
    {
        Label = label;
        Symbol = symbol;
        DestPage = destPage;
        Arguments = nullptr;
    }

    NavMenuItem::NavMenuItem(
        String^ label,
        Windows::UI::Xaml::Controls::Symbol symbol,
        Windows::UI::Xaml::Interop::TypeName destPage,
        Object^ arguments)
    {
        Label = label;
        Symbol = symbol;
        DestPage = destPage;
        Arguments = arguments;
    }

    String^ NavMenuItem::SymbolAsChar::get()
    {
        wchar_t c[] = { (wchar_t)((int)(Symbol)), '\0' };
        return ref new String(c);
    }
}
