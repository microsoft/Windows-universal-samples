//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Security.EnterpriseData;
using Windows.Storage.Pickers;
using System.Threading.Tasks;
using SDKTemplate;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace EdpSample
{
    public class StatusData
    {
        public bool result;
        public string outputStr;
    }

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario4 : Page
    {
        private MainPage rootPage;
        private string m_protectScenarioDesc = "App is an enterprise enlightened app.When it retrieves enterprise data in a file, it must protect the file " +
                                             "that contains said data.It does so by protecting the file to an enterprise identity. After this, the app " +
                                             "can use standard APIs to read or write from the file.The app generally gets the enterprise identity from an " +
                                             "external resource such as a mailbox";
        private string m_getProtectScenarioDesc = "\n\nAn app has attempted to access a resource that it previously protected. It is denied access and wants to "  +
                                                  "check the status of the file to see what went wrong.The app calls the GetProtectionInfoAsync API to query the status.";
        private string m_isRoamableScenarioDesc =  "\n\nAn app wants to roam its protected file to other devices.In order to do so, it must check if the file’s protection " +
                                                  "can be roamed.The app calls GetProtectionInfoAsync on the file to retrieve this information.";
        private string m_copyProtectionScenarioDesc = "\n\nAn app wants to create multiple copies of a protected file in which enterprise data is stored.It can use the CopyProtectionAsync" +
                                                      "API to replicate protection from one file to another.";



        private StorageFile m_FileHandle = null;
        private string m_FileCopyName =  "Edpcopy.txt";

        public Scenario4()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ScenarioText.Text = m_protectScenarioDesc + m_getProtectScenarioDesc + 
                               m_isRoamableScenarioDesc + m_copyProtectionScenarioDesc;
            rootPage = MainPage.Current;
        }

        private async void FilePickerBt_Click(object sender, RoutedEventArgs e)
        {
            string outputStr = "";

            FileOpenPicker openPicker = new FileOpenPicker();
            openPicker.ViewMode = PickerViewMode.Thumbnail;
            openPicker.SuggestedStartLocation = PickerLocationId.DocumentsLibrary;
            openPicker.FileTypeFilter.Add("*");
            m_FileHandle = await openPicker.PickSingleFileAsync();
            if (m_FileHandle != null)
            {
                // Application now has read/write access to the picked file
                outputStr += "\nPicked file: " + m_FileHandle.Path;
                rootPage.NotifyUser(outputStr, NotifyType.StatusMessage);
            }
            else
            {
                outputStr += "\nPlease pick a file";
                rootPage.NotifyUser(outputStr, NotifyType.ErrorMessage);
            }

        }

        private async void CreateFileAndProtect()
        {
            string outputStr = "";

            try
            {
                FileProtectionInfo procInfo =  await FileProtectionManager.GetProtectionInfoAsync(m_FileHandle);

                if (procInfo.Status != FileProtectionStatus.Protected)
                {
                    outputStr += "\nProtecting File: ";
                    await FileProtectionManager.ProtectAsync(m_FileHandle, Scenario1.m_EnterpriseIdentity);
                    procInfo =  await FileProtectionManager.GetProtectionInfoAsync(m_FileHandle);
                    outputStr += "\nProtected File: " + m_FileHandle.Path + "Status:" + procInfo.Status;
                }
                else
                {
                    outputStr += "\nFile protection status is: " + procInfo.Status;
                }

                rootPage.NotifyUser(outputStr, NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {

                rootPage.NotifyUser(outputStr +"\n" + "Exception thrown:" + ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        public async void CopyProtectionAsync()
        {
            bool result = false;
            string outputStr = "";

            try
            {
                if (m_FileHandle != null)
                {
                    StorageFolder localFolder =
                    Windows.Storage.ApplicationData.Current.LocalFolder;

                    StorageFile destFile =
                        await localFolder.CreateFileAsync(m_FileCopyName,
                            CreationCollisionOption.OpenIfExists);

                    if (destFile != null)
                    {
                        await destFile.DeleteAsync();
                    }

                    //Recreate file

                    destFile =
                        await localFolder.CreateFileAsync(m_FileCopyName,
                            CreationCollisionOption.ReplaceExisting);

                    outputStr += "Copying protection from source: " + m_FileHandle.Path + "\nTo destination: " + destFile.Path;
                    result = await FileProtectionManager.CopyProtectionAsync(m_FileHandle, destFile);
                    if (!result)
                    {
                        rootPage.NotifyUser(outputStr + " Copy protection Failed", NotifyType.ErrorMessage);
                    }
                    else
                    {
                        FileProtectionInfo sourceInfo = await FileProtectionManager.GetProtectionInfoAsync(m_FileHandle);
                        FileProtectionInfo destInfo = await FileProtectionManager.GetProtectionInfoAsync(destFile);
                        if (sourceInfo.Status != destInfo.Status)
                        {
                            outputStr += "\nsource and destination don't have same protection source status:" +
                                               sourceInfo.Status + " Destination status:" + destInfo.Status;
                            rootPage.NotifyUser(outputStr, NotifyType.ErrorMessage);
                        }
                        else
                        {
                            outputStr += "\nCopying protection succeeded";
                            rootPage.NotifyUser(outputStr, NotifyType.StatusMessage);
                        }

                    }

                }
                else
                {
                    rootPage.NotifyUser("Please pick a source file", NotifyType.ErrorMessage);
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Exception thrown:" + ex.ToString(), NotifyType.StatusMessage);
            }

        
        }

        public async Task<StatusData> CheckFileStatus()
        {
            bool result = false;
            string outputStr = "";

            StatusData resultData = new StatusData();

            try
            {

                FileProtectionInfo protectionInfo =
                    await FileProtectionManager.GetProtectionInfoAsync(m_FileHandle);

                if (protectionInfo.Status == FileProtectionStatus.Revoked)
                {
                    outputStr += "\nFile " + m_FileHandle.Path + " is revoked";
                }
                else
                {
                    outputStr += "\nFile protection status of: " + m_FileHandle.Path + " is " + protectionInfo.Status;
                }

                if (protectionInfo.IsRoamable)
                {
                    outputStr += "\nFile " + m_FileHandle.Path + " is roamable";
                }else
                {
                    outputStr += "\nFile " + m_FileHandle.Path + " is not roamable";
                }

                result = true;
            }
            catch (Exception ex)
            {
                outputStr += "\n" + "Exception thrown:" + ex.ToString();
            }

            resultData.result = result;
            resultData.outputStr = outputStr;


            return resultData;
        }

        private async void ProtectAsyncFileStatus_Click(object sender, RoutedEventArgs e)
        {
            if (m_FileHandle != null)
            {
                StatusData stData = await CheckFileStatus();
                if (stData.result)
                {
                    rootPage.NotifyUser(stData.outputStr, NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser(stData.outputStr, NotifyType.ErrorMessage);
                }
            }else
            {
                rootPage.NotifyUser("Please pick a file", NotifyType.ErrorMessage);
            }
        }

        private void ProtectAsyncFile_Click(object sender, RoutedEventArgs e)
        {
            if (m_FileHandle != null)
            {
                CreateFileAndProtect();
            }else
            {
                rootPage.NotifyUser("Please pick a file", NotifyType.ErrorMessage);
            }
        }

        private void CopyProtectionAsync_Click(object sender, RoutedEventArgs e)
        {
            CopyProtectionAsync();
        }


    }
}
