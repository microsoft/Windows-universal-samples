#pragma once
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Interop;

namespace NavigationMenuSample
{
    /// <summary>
    /// Data to represent an item in the nav menu.
    /// </summary>
    public ref class NavMenuItem sealed : public INotifyPropertyChanged
    {
    public:
        NavMenuItem(String^, Symbol, TypeName);
        NavMenuItem(String^, Symbol, TypeName, Object^);
        property String^ Label;
        property Symbol Symbol;
        property TypeName DestPage;
        property String^ SymbolAsChar
        {
            String^ get();
        }
        property Object^ Arguments;
        property bool IsSelected
        {
            bool get();
            void set(bool value);
        }

        property Visibility SelectedVis
        {
            Visibility get();
            void set(Visibility value);
        }

        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

    protected:
        void NotifyPropertyChanged(Platform::String^ prop);

    private:
        bool m_isSelected;
        Visibility m_selectedVisibility;
    };
}
