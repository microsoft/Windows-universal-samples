using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Security.DataProtection;

namespace PDETestApp
{
    internal class Samples
    {
        async void OneDriveProtectSample()
        {
            {
                var udpm = UserDataProtectionManager.TryGetDefault();
                if (udpm != null)
                {
                    StorageFolder folder = await StorageFolder.GetFolderFromPathAsync(@"C:\users\bob\OneDrive - Microsoft\Documents");
                    var result = await udpm.ProtectStorageItemAsync(folder, UserDataAvailability.AfterFirstUnlock);
                }
            }
            {
                var udpm = UserDataProtectionManager.TryGetDefault();
                if (udpm != null)
                {
                    StorageFile file = await StorageFile.GetFileFromPathAsync(@"C:\users\bob\OneDrive - Microsoft\Documents\file.txt");
                    var result = await udpm.ProtectStorageItemAsync(file, UserDataAvailability.AfterFirstUnlock);
                }
            }
            {
                var udpm = UserDataProtectionManager.TryGetDefault();
                if (udpm != null)
                {
                    if (udpm.IsContinuedDataAvailabilityExpected(UserDataAvailability.WhileUnlocked))
                    {
                        // Data protected to WhileUnlocked level is not accessible right now.
                    }
                    else
                    {
                        // Data protected to WhileUnlocked level is accessible (and will be accessible for at least a minute from now).
                        // Once the user locks their device, keys for WhileUnlocked level are dropped 60 seconds after and only
                        // brought back when they unlock their device.
                        // Related: Keys for AfterFirstUnlock level are available throughout the user's signed in session even when
                        // the device is locked.
                    }
                }
            }
        }

        async void GeneralProtectSample()
        {
            {
                var udpm = UserDataProtectionManager.TryGetDefault();
                if (udpm != null)
                {
                    StorageFolder folder = await StorageFolder.GetFolderFromPathAsync(@"C:\Users\bob\AppData\Local\MyApp");
                    var result = await udpm.ProtectStorageItemAsync(folder, UserDataAvailability.AfterFirstUnlock);
                }
            }

            {
                var udpm = UserDataProtectionManager.TryGetDefault();
                if (udpm != null)
                {
                    StorageFile file = await StorageFile.GetFileFromPathAsync(@"C:\Users\bob\AppData\Local\MyApp\Passwords\pwd_a");
                    var result = await udpm.ProtectStorageItemAsync(file, UserDataAvailability.WhileUnlocked);
                }
            }

            {
                var udpm = UserDataProtectionManager.TryGetDefault();
                if (udpm != null)
                {
                    if (udpm.IsContinuedDataAvailabilityExpected(UserDataAvailability.WhileUnlocked))
                    {
                        // L2 protected data is available
                    }
                    else
                    {
                        // L2 protected data is not currently available, or will be unavailable soon (next 60 seconds).
                        // App expectation is to stop working with L2 protected data - flush / clean up anything in progress.
                    }
                }
            }
        }
    }
}
