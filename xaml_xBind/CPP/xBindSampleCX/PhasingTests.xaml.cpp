//
// PhasingTests.xaml.cpp
// Implementation of the PhasingTests class
//

#include "pch.h"
#include "PhasingTests.xaml.h"

using namespace xBindSampleCX;

using namespace concurrency;
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
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

PhasingTests::PhasingTests()
{
    InitializeComponent();
    CreateTestData();
    initialized = true;
}

void PhasingTests::CreateTestData()
{
    create_task(dataSource->SetupDataSourceUsingPicturesFolder()).then([this]()
    {
        this->LoadingPanel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        this->myGridView->ItemsSource = dataSource;
    });
}

void PhasingTests::Reset_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    dataSource->ResetCollection();
}


void PhasingTests::ChangeFolderClick(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    FolderPicker^ picker = ref new FolderPicker();
    picker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
    picker->FileTypeFilter->Append(".jpg");
    picker->FileTypeFilter->Append(".png");

    create_task(picker->PickSingleFolderAsync()).then([this](StorageFolder^ f)
    {
        dataSource = ref new xBindSampleModel::FileDataSource();
        this->myGridView->ItemsSource = dataSource;
        LoadingPanel->Visibility = Windows::UI::Xaml::Visibility::Visible;
        create_task(dataSource->SetupDataSource(f)).then([this]()
        {
            this->LoadingPanel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        });
    });
}

void PhasingTests::SlowPhasing_UnChecked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    SlowPhasing_Checked(sender, e);
}

void PhasingTests::SlowPhasing_Checked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (initialized)
    {
        if (SlowPhasing->IsChecked->Value)
        {
            _CCCToken = myGridView->ContainerContentChanging += ref new TypedEventHandler<ListViewBase^, ContainerContentChangingEventArgs^>(this, &PhasingTests::myGridView_ContainerContentChanging);
        }
        else
        {
            myGridView->ContainerContentChanging -= _CCCToken;
        }
    }
}

void PhasingTests::PhasedTemplate_Checked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (initialized)
    {
        if (RadioPhasedTempl->IsChecked->Value == true)
        {
            myGridView->ItemTemplate = dynamic_cast<DataTemplate^>(Resources->Lookup("PhasedFileTemplate"));
        }
        else if (RadioxBindTempl->IsChecked->Value)
        {
            myGridView->ItemTemplate = dynamic_cast<DataTemplate^>(Resources->Lookup("ClassicBindingFileTemplate"));
        }
        else if (RadioClassicTempl->IsChecked->Value)
        {
            myGridView->ItemTemplate = dynamic_cast<DataTemplate^>(Resources->Lookup("ClassicBindingFileTemplate"));
        }
    }
}

void PhasingTests::myGridView_ContainerContentChanging(ListViewBase^ sender, ContainerContentChangingEventArgs^ args)
{
    if (args->Phase < 10) args->RegisterUpdateCallback(ref new TypedEventHandler<ListViewBase^, ContainerContentChangingEventArgs^>(this, &PhasingTests::myGridView_ContainerContentChanging));
    Sleep(1);
}

void PhasingTests::DataSource_VectorChanged(Windows::Foundation::Collections::IObservableVector<xBindSampleModel::FileItem^>^ sender, Windows::Foundation::Collections::IVectorChangedEventArgs^ e)
{
    this->NoItemsPanel->Visibility = (dataSource->Count == 0) ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
}

void PhasingTests::Sleep(int timeout)
{
    //This function should not be used in real apps, it blocks the UI thread for a timeout. It is used in this sample to illustrate the effects of phasing.
    auto eventHandle = ::CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, 0);
    ::WaitForSingleObjectEx(eventHandle, timeout, FALSE);
}