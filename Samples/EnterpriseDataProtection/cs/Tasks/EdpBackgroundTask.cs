// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

using System;
using System.Diagnostics;
using System.Threading;
using System.Runtime;
using Windows.ApplicationModel.Background;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.Security.EnterpriseData;
using Windows.Security.Cryptography;

namespace Tasks
{
    public sealed class EdpBackgroundTask : IBackgroundTask
    {
        volatile bool m_cancelRequested = false;
        private string m_taskName = "";
        private bool m_areKeysDropped = true;
        private string m_fileProtStatus = "";
        private string m_EnterpriseID = "";
        private string m_SecretMessage = "This is a secret message";
        private IBuffer m_protectedBuffer = null;

        public void Run(IBackgroundTaskInstance taskInstance)
        {
            Debug.WriteLine("EdpBackgroundTask " + taskInstance.Task.Name + " starting...");

            m_taskName = taskInstance.Task.Name;

            var settings = ApplicationData.Current.LocalSettings;
            var key = taskInstance.Task.Name;
            
            // Get EnterpriseID

            m_EnterpriseID = settings.Values[key].ToString();
            settings.Values[key] = "";

            // Associate a cancellation handler with the background task.
            taskInstance.Canceled += new BackgroundTaskCanceledEventHandler(OnCanceled);

            ProtectionPolicyManager.ProtectedAccessSuspending += ProtectedAccessSuspending;
            ProtectionPolicyManager.ProtectedAccessResumed += ProtectedAccessResumed;

            // Do background task activity.
            uint Progress;
            for (Progress = 0; Progress <= 100; Progress += 10)
            {
                // If the cancellation handler indicated that the task was canceled, stop doing the task.
                if (m_cancelRequested)
                {
                    break;
                }

                DoBufferWork(taskInstance);
                DoFileWork(taskInstance);

                // Sleep for 10 seconds for testing purposes
                 
                using (EventWaitHandle workEvent = new ManualResetEvent(false))
                {
                    workEvent.WaitOne(TimeSpan.FromSeconds(10));
                }
                // Indicate progress to foreground application.
                taskInstance.Progress = Progress;
            }

            settings.Values[key] += "\nWrote Protected File at: " + 
                                       Windows.Storage.ApplicationData.Current.LocalFolder.Path;
            settings.Values[key] += "\nFile protection Status:" + m_fileProtStatus;

            Debug.WriteLine("EdpBackgroundTask " + taskInstance.Task.Name + ((Progress < 100) ? " Canceled" : " Completed"));
        }

        private async void DoBufferWork(IBackgroundTaskInstance taskInstance)
        {
            string message = "Hello World!";
            string unprotectedMessage = "";
            string logFileName = "Bufferlog.txt";
            string logFileContent = "";

            StorageFolder localFolder = Windows.Storage.ApplicationData.Current.LocalFolder;
            StorageFile logFile = await localFolder.CreateFileAsync(logFileName, CreationCollisionOption.OpenIfExists);


            IBuffer inputBuffer = CryptographicBuffer.ConvertStringToBinary(message, BinaryStringEncoding.Utf8);
            BufferProtectUnprotectResult procBuffer = await DataProtectionManager.ProtectAsync(inputBuffer, m_EnterpriseID);
            logFileContent += "\r\n" + DateTime.Now + ":" + "ProtStatus:" + procBuffer.ProtectionInfo.Status + "\n";
            logFileContent += "\r\n" + "Protected Buffer:" + CryptographicBuffer.EncodeToHexString(procBuffer.Buffer).Substring(0, 5);

            // If keys are dropped under lock, unprotectBuffer will fail so don't unprotectbuffer under lock
            if (!m_areKeysDropped)
            {
                BufferProtectUnprotectResult unBuffer = await DataProtectionManager.UnprotectAsync(procBuffer.Buffer);
                unprotectedMessage = CryptographicBuffer.ConvertBinaryToString(BinaryStringEncoding.Utf8, procBuffer.Buffer);
                logFileContent += "\n Unprotected string:" + unprotectedMessage;
                if (message != unprotectedMessage)
                {
                    throw new Exception("Original string does not match with unprotectedMessage!");
                }
            }

            await FileIO.AppendTextAsync(logFile, logFileContent);

        }

        private async void DoFileWork(IBackgroundTaskInstance taskInstance)
        {
            string textBody = "\nHello World";
            string fileName = "EdpSample.txt";
            string fileCopyName = "EdpSampleCopy.txt";
            string logFileName = "Filelog.txt";

            StorageFolder localFolder = Windows.Storage.ApplicationData.Current.LocalFolder;

            StorageFile logFile = await localFolder.CreateFileAsync(logFileName, CreationCollisionOption.OpenIfExists);

            // Create a protected file allows the creation of new files even after keys are dropped so that application
            // can create and continue writing to files under lock

            var result = await FileProtectionManager.CreateProtectedAndOpenAsync(localFolder,
                                                                                 fileName,
                                                                                 m_EnterpriseID,
                                                                                 CreationCollisionOption.ReplaceExisting
                                                                                 );
            using (var stream = result.Stream)
            {
                m_fileProtStatus = result.ProtectionInfo.Status.ToString();

                // Write to File

                using (IOutputStream outputStream = result.Stream.GetOutputStreamAt(0))
                {
                    DataWriter writer = new DataWriter(outputStream);
                    for (int i = 0; i < 100; i++)
                    {
                        writer.WriteString(textBody);
                        await writer.StoreAsync();
                    }
                }
            }

            // Check and if Keys are not dropped, read and write to another file. 
            // Read from a protected file and copy it to another one

            if (!m_areKeysDropped)
            {
                StorageFile edpFileSource = await localFolder.CreateFileAsync(fileName, CreationCollisionOption.OpenIfExists);
                StorageFile edpFileCopy = await localFolder.CreateFileAsync(fileCopyName, CreationCollisionOption.ReplaceExisting);
                string sourceContent = await FileIO.ReadTextAsync(edpFileSource);
                await FileIO.WriteTextAsync(edpFileCopy, sourceContent);
            }
            else
            {
                await FileIO.AppendTextAsync(logFile, "\r\n" + DateTime.Now + ":" + " keys are dropped. Skip reading");
            }
        }

        private async void ProtectedAccessSuspending(object sender, ProtectedAccessSuspendingEventArgs args)
        {
            string logFileContent = "";

            Windows.Foundation.Deferral deferal = args.GetDeferral();

            // Protect any sensitive data in Memory so that it cannot be accessed while the device is  locked
            // You should encrypt or destory any copies of sensitve data while device is going to a locked state.

            IBuffer inputBuffer = CryptographicBuffer.ConvertStringToBinary(m_SecretMessage,
                                                                            BinaryStringEncoding.Utf8);
            BufferProtectUnprotectResult procBuffer = await DataProtectionManager.ProtectAsync(inputBuffer, m_EnterpriseID);
            m_protectedBuffer = procBuffer.Buffer;
           
            deferal.Complete();

            m_areKeysDropped = true;

            var settings = ApplicationData.Current.LocalSettings;
            settings.Values[m_taskName] += "\nApp got DPL suspend event";

            StorageFolder localFolder = Windows.Storage.ApplicationData.Current.LocalFolder;

            StorageFile logFile = await localFolder.CreateFileAsync("SuspendLog.txt",
                                           CreationCollisionOption.OpenIfExists);

            logFileContent += "\r\n" + DateTime.Now + ":" + "Got DPL Protected Suspended";
            logFileContent += "\r\n" + "Protection Status:" + procBuffer.ProtectionInfo.Status;

            await FileIO.AppendTextAsync(logFile, logFileContent);
        }

        private async void ProtectedAccessResumed(object sender, ProtectedAccessResumedEventArgs args)
        {
            // Keys are available so can read and write from a protected file
            
            m_areKeysDropped = false;
            
            BufferProtectUnprotectResult unBuffer = await DataProtectionManager.UnprotectAsync(m_protectedBuffer);
            m_SecretMessage = CryptographicBuffer.ConvertBinaryToString(BinaryStringEncoding.Utf8,
                                                                           m_protectedBuffer
                                                                           );
            string logFileContent = "\n Unprotected string:" + m_SecretMessage;

            var settings = ApplicationData.Current.LocalSettings;
            settings.Values[m_taskName] += "\nApp got DPL resumed event";
            StorageFolder localFolder = Windows.Storage.ApplicationData.Current.LocalFolder;
            StorageFile logFile = await localFolder.CreateFileAsync("ResumeLog.txt",
                                                                       CreationCollisionOption.OpenIfExists);
            await FileIO.AppendTextAsync(logFile, "\r\n" + DateTime.Now + ":" + "Got DPL Protected resumed");
        }

        private void OnCanceled(IBackgroundTaskInstance sender, BackgroundTaskCancellationReason reason)
        {
            m_cancelRequested = true;
            Debug.WriteLine("EdpBackgroundTask " + sender.Task.Name + " Cancel Requested...");
        }
    }
}

