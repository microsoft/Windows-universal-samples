//
// OtherBindings.xaml.h
// Declaration of the OtherBindings class
//

#pragma once

#include "OtherBindings.g.h"

namespace xBindSampleCX
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class OtherBindings sealed
	{
    private:
        const Platform::String^ DPOnPageName = "DPOnPage";
        static Windows::UI::Xaml::DependencyProperty^ _DPOnPage;

    public:
		OtherBindings();
        property int MyInt;
        property xBindSampleModel::DataModel^ Model;

        property Platform::String^ DPOnPage
        {
            Platform::String^ get()
            {
                return dynamic_cast<Platform::String^>(GetValue(DPOnPageProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(DPOnPageProperty, value);
            }
        }

        static property Windows::UI::Xaml::DependencyProperty^ DPOnPageProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get() { return _DPOnPage; }
        }

        property xBindSampleModel::IEmployee^ NullEmployee;
        property Platform::String^ NullStringProperty;

    private:
        void InitializeValues();
        void UpdateValuesClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ResetValuesClick(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UndeferElementClick(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
