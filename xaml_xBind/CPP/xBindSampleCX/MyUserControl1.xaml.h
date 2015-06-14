//
// MyUserControl1.xaml.h
// Declaration of the MyUserControl1 class
//

#pragma once

#include "MyUserControl1.g.h"

namespace xBindSampleCX
{
	[Windows::Foundation::Metadata::WebHostHidden]
    public ref class MyUserControl1 sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
	public:
		MyUserControl1();

        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

        property Platform::String^ NonDPProperty
        {
            Platform::String^ get()
            {
                return _nonDPProperty;
            }
            void set(Platform::String^ value)
            {
                if (NonDPProperty != value)
                {
                    _nonDPProperty = value;
                    PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("NonDPProperty"));
                }
            }
        }

    private:
        Platform::String^ _nonDPProperty;
	};
}
