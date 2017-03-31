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

    bool NavMenuItem::IsSelected::get()
    {
        return m_isSelected;
    }

    void NavMenuItem::IsSelected::set(bool value)
    {
        m_isSelected = value;
        NavMenuItem::SelectedVis = value ? Visibility::Visible : Visibility::Collapsed;
        NotifyPropertyChanged("IsSelected");
    }

    Visibility NavMenuItem::SelectedVis::get()
    {
        m_selectedVisibility = NavMenuItem::IsSelected ? Visibility::Visible : Visibility::Collapsed;
        return m_selectedVisibility;
    }

    void NavMenuItem::SelectedVis::set(Visibility value)
    {
        m_selectedVisibility = value;
        NotifyPropertyChanged("SelectedVis");
    }

    void NavMenuItem::NotifyPropertyChanged(String^ prop)
    {
        PropertyChanged(this, ref new PropertyChangedEventArgs(prop));
    }
}
