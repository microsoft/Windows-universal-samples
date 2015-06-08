//
// PhasingTests.xaml.h
// Declaration of the PhasingTests class
//

#pragma once

#include "PhasingTests.g.h"

namespace xBindSampleCX
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class PhasingTests sealed
	{
	public:
		PhasingTests();


    private:
        Windows::Foundation::EventRegistrationToken _CCToken;
        Windows::Foundation::EventRegistrationToken _CCCToken;
        xBindSampleModel::FileDataSource^ _dataSource;
        bool initialized;

        property xBindSampleModel::FileDataSource^ dataSource
        {
            xBindSampleModel::FileDataSource^ get()
            {
                if (_dataSource == nullptr)
                {
                    _dataSource = ref new xBindSampleModel::FileDataSource();
                    _CCToken= _dataSource->VectorChanged += ref new Windows::Foundation::Collections::VectorChangedEventHandler<xBindSampleModel::FileItem^>(this, &PhasingTests::DataSource_VectorChanged);
                }
                return _dataSource;
            }
            void set(xBindSampleModel::FileDataSource^ value)
            {
                if (value != _dataSource)
                {
                    _dataSource->CollectionChanged -= _CCToken;
                    _dataSource = value;
                    _CCToken = _dataSource->VectorChanged += ref new Windows::Foundation::Collections::VectorChangedEventHandler<xBindSampleModel::FileItem^>(this, &PhasingTests::DataSource_VectorChanged);
                }
            }
        }

        void CreateTestData();

        void myGridView_ContainerContentChanging(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs^ args);

        void DataSource_VectorChanged(Windows::Foundation::Collections::IObservableVector<xBindSampleModel::FileItem^>^ sender, Windows::Foundation::Collections::IVectorChangedEventArgs^ e);

        void Reset_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ChangeFolderClick(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SlowPhasing_UnChecked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SlowPhasing_Checked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PhasedTemplate_Checked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Sleep(int time);
	};
}
