#pragma once

namespace TreeViewControl {
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class IntegerToIndentationConverter sealed : Windows::UI::Xaml::Data::IValueConverter
    {
    public:
        property int IndentMultiplier
        {
            int get() { return indentMultiplier; };
            void set(int i) { indentMultiplier = i; };
        }

        IntegerToIndentationConverter();

        virtual Platform::Object^ Convert(Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language);

        virtual Platform::Object^ ConvertBack(Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language);
    private:
        int indentMultiplier;
    };
}

