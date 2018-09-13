//
// Scenario5_TriggerCFU.xaml.cpp
// Implementation of the Scenario5_TriggerCFU class
//

#include "pch.h"
#include "Scenario5_TriggerCFU.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::AccessCache;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Provider;
using namespace Windows::UI::Xaml;

Scenario5::Scenario5()
{
    InitializeComponent();
    UpdateButtons();
}

void Scenario5::UpdateButtons()
{
    WriteToFileButton->IsEnabled = (m_afterWriteFile != nullptr);
    WriteToFileWithCFUButton->IsEnabled = (m_afterWriteFile != nullptr);

    SaveToFutureAccessListButton->IsEnabled = (m_beforeReadFile != nullptr);
    GetFileFromFutureAccessListButton->IsEnabled = !m_faToken->IsEmpty();
}

void Scenario5::CreateFileButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("", NotifyType::StatusMessage);

    FileSavePicker^ savePicker = ref new FileSavePicker();
    savePicker->SuggestedStartLocation = PickerLocationId::DocumentsLibrary;

    savePicker->FileTypeChoices->Insert("Plain Text", ref new Vector<String^>{ ".txt" });
    savePicker->SuggestedFileName = "New Document";

    create_task(savePicker->PickSaveFileAsync()).then([this](StorageFile^ file)
    {
        m_afterWriteFile = file;
        if (m_afterWriteFile)
        {
            rootPage->NotifyUser("File created.", NotifyType::StatusMessage);
        }
        else
        {
            rootPage->NotifyUser("Operation cancelled.", NotifyType::ErrorMessage);
        }
        UpdateButtons();
    });
}

void Scenario5::WriteToFileButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    create_task(FileIO::WriteTextAsync(m_afterWriteFile, "The File Picker App just wrote to the file!")).then([this]()
    {
        rootPage->NotifyUser("File write complete. If applicable, a WriteActivationMode.AfterWrite activation will occur in approximately 60 seconds on desktop.", NotifyType::StatusMessage);
    });
}

void Scenario5::WriteToFileWithExplicitCFUButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    StorageFile^ file = m_afterWriteFile;
    CachedFileManager::DeferUpdates(file);
    create_task(FileIO::WriteTextAsync(file, "The File Picker App just wrote to the file!")).then([this, file]()
    {
        rootPage->NotifyUser("File write complete. Explicitly completing updates.", NotifyType::StatusMessage);
        return create_task(CachedFileManager::CompleteUpdatesAsync(file));
    }).then([this, file](FileUpdateStatus status)
    {
        switch (status)
        {
        case FileUpdateStatus::Complete:
            rootPage->NotifyUser("File " + file->Name + " was saved.", NotifyType::StatusMessage);
            break;

        case FileUpdateStatus::CompleteAndRenamed:
            rootPage->NotifyUser("File " + file->Name + " was renamed and saved.", NotifyType::StatusMessage);
            break;

        default:
            rootPage->NotifyUser("File " + file->Name + " couldn't be saved.", NotifyType::ErrorMessage);
            break;
        }
    });
}

void Scenario5::PickAFileButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("", NotifyType::StatusMessage);

    FileOpenPicker^ openPicker = ref new FileOpenPicker();
    openPicker->ViewMode = PickerViewMode::Thumbnail;
    openPicker->FileTypeFilter->Append(".txt");

    create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        m_beforeReadFile = file;
        if (file)
        {
            rootPage->NotifyUser("Picked file: " + file->Name, NotifyType::StatusMessage);
        }
        else
        {
            rootPage->NotifyUser("Operation cancelled.", NotifyType::ErrorMessage);
        }
        UpdateButtons();
    });
}

void Scenario5::SaveToFutureAccessListButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    m_faToken = StorageApplicationPermissions::FutureAccessList->Add(m_beforeReadFile);
    rootPage->NotifyUser("Saved to Future Access List", NotifyType::StatusMessage);
    m_beforeReadFile = nullptr;
    UpdateButtons();
}

void Scenario5::GetFileFromFutureAccessListButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Getting the file from the Future Access List. If applicable, a ReadActivationMode.BeforeAccess activation will occur in approximately 60 seconds on desktop.", NotifyType::StatusMessage);
    create_task(StorageApplicationPermissions::FutureAccessList->GetFileAsync(m_faToken)).then([this](StorageFile^ file)
    {
        if (file)
        {
            rootPage->NotifyUser("Retrieved file from Future Access List: " + file->Name, NotifyType::StatusMessage);
            m_beforeReadFile = file;
        }
        else
        {
            rootPage->NotifyUser("Unable to retrieve file from Future Access List.", NotifyType::ErrorMessage);
        }
        UpdateButtons();
    });
}

