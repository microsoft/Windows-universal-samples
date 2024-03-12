using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection.Emit;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Windows.Security.Cryptography;
using Windows.Security.DataProtection;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml.Shapes;

namespace PDETestApp
{
    public partial class Form2 : Form
    {
        UserDataProtectionManager g_udpm;
        IBuffer g_protectedContent;
        String g_protectbase64EncodedContent;
        HashSet<string> g_fileList = new HashSet<string>();
        Queue<string> g_directoriesSubdirectories = new Queue<string>();   
        public Form2()
        {
            InitializeComponent();
        }

        private void Form2_load(object sender, EventArgs e)
        {
            g_udpm = UserDataProtectionManager.TryGetDefault();
            if (g_udpm == null)
            {
                LogLine("Personal Data Encryption is not supported or enabled. Restart this app to check again.");
            }
            else
            {
                LogLine("Personal Data Encryption is enabled.");
                g_udpm.DataAvailabilityStateChanged += M_udpm_DataAvailabilityStateChanged;
                LogCurrentDataAvailability();
                LogLine("Listening to DataAvailabilityStateChanged event");
            }
        }

        private void M_udpm_DataAvailabilityStateChanged(UserDataProtectionManager sender, UserDataAvailabilityStateChangedEventArgs args)
        {
            LogLine("DataAvailabilityStateChanged event received");
            LogCurrentDataAvailability();
        }
        private void LogLine(string msg)
        {
            if (InvokeRequired)
            {
                this.Invoke(new Action<string>(LogLine), new object[] { msg });
                return;
            }
            string ts = DateTime.Now.ToString("MM/dd/yy HH:mm:ss.fff");
            textBox2.Text += "[" + ts + "] " + msg + "\r\n";
            textBox2.Select(textBox1.TextLength, 0);
            textBox2.ScrollToCaret();
            Console.WriteLine(msg);
        }


        private void LogCurrentDataAvailability()
        {
            bool l1Avl = g_udpm.IsContinuedDataAvailabilityExpected(UserDataAvailability.AfterFirstUnlock);
            bool l2Avl = g_udpm.IsContinuedDataAvailabilityExpected(UserDataAvailability.WhileUnlocked);
            LogLine("IsContinuedDataAvailabilityExpected AfterFirstUnlock: " + l1Avl + ", WhileUnlocked: " + l2Avl);
        }

        private void ProtectChosenFilesToLevel(UserDataAvailability level)
        {
            var udpm = UserDataProtectionManager.TryGetDefault();

            // g_directoriesSubdirectories contains a list of the all the directories and subdirectories that need
            // to be protected
            foreach (String path in g_directoriesSubdirectories)
            {
                DirectoryInfo folder = new DirectoryInfo(path);

                // Protecting the folder first as a best practice
                ProtectFolder(folder, udpm, level);
                FileInfo[] files = folder.GetFiles();

                foreach (var file in files)
                {
                    // Protecting all files within the folder
                    ProtectFile(file, udpm, level);
                }
            }
        }

        private async void ProtectFolder(DirectoryInfo path, UserDataProtectionManager udm, UserDataAvailability level)
        {
            // Create a folder IStorageItem object to pass on to ProtectItem method
            IStorageItem item;
            {
                item = await StorageFolder.GetFolderFromPathAsync(path.FullName);
            }
            ProtectItem(item, udm, level);
        }

        private async void ProtectFile(FileInfo path, UserDataProtectionManager udm, UserDataAvailability level)
        {
            // Create a file IStorageItem object to pass on to ProtectItem method
            IStorageItem item;
            {
                item = await StorageFile.GetFileFromPathAsync(path.FullName);
            }
            ProtectItem(item, udm, level);
        }

        private async void ProtectItem(IStorageItem item, UserDataProtectionManager udm, UserDataAvailability level)
        {      
            // Protect the folder or file that gets passed into the method
            var protectResult = await udm.ProtectStorageItemAsync(item, level);
            if (protectResult == UserDataStorageItemProtectionStatus.Succeeded)
            {
                LogLine("Protected " + item.Path + " to level " + level);
            }
            else
            {
                LogLine("Protection failed for " + item.Path + " to level " + level + ", status: " + protectResult);
            }
        }

        async void UnprotectBuffer(String g_protectbase64EncodedContent)
        {
            var protectedBuffer = CryptographicBuffer.DecodeFromBase64String(g_protectbase64EncodedContent);
            var result = await g_udpm.UnprotectBufferAsync(protectedBuffer);
            if (result.Status == UserDataBufferUnprotectStatus.Succeeded)
            {
                LogLine("Result of Unprotecting the buffer:" + 
                    CryptographicBuffer.ConvertBinaryToString(BinaryStringEncoding.Utf8, result.UnprotectedBuffer));
                LogLine("Status of Unprotectng the buffer:" + result.Status);
            }
            else
            {
                LogLine("This protected buffer is currently unavailable for unprotection");
            }
        }

        async void ProtectBuffer(String text, UserDataAvailability level)
        {
            var buffer = CryptographicBuffer.ConvertStringToBinary(text, BinaryStringEncoding.Utf8);
            g_protectedContent = await g_udpm.ProtectBufferAsync(buffer, level);
            g_protectbase64EncodedContent = CryptographicBuffer.EncodeToBase64String(g_protectedContent);
            LogLine("Protected buffer: " + g_protectbase64EncodedContent);
        }
        private void button4_Click(object sender, EventArgs e)
        {
            if(folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                listView1.Items.Add(folderBrowserDialog1.SelectedPath);
                string selectedPath = folderBrowserDialog1.SelectedPath;
                if (File.Exists(selectedPath))
                {
                    g_fileList.Add(selectedPath);
                } else if (Directory.Exists(selectedPath))
                {
                    this.populateAllSubFolders(selectedPath);
                } else
                {
                    MessageBox.Show("Selected path is neither a file nor a directory please rectify");
                }
            } 
        }

        private void populateAllSubFolders(String selectedPath)
        {
            DirectoryInfo folder = new DirectoryInfo(selectedPath);
            g_directoriesSubdirectories.Enqueue(selectedPath);
            DirectoryInfo[] folders = folder.GetDirectories("*", SearchOption.AllDirectories);
            foreach (DirectoryInfo dir in folders)
            {
                g_directoriesSubdirectories.Enqueue(dir.FullName);
            }
        }



        private void button5_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                listView2.Items.Add(openFileDialog1.FileName);
                string selectedPath = openFileDialog1.FileName;
                if (File.Exists(selectedPath))
                {
                    g_fileList.Add(selectedPath);
                }
                else
                {
                    MessageBox.Show("Selected file is not a file. please rectify");
                }
            }
        }


        private void FolderL1_Click(object sender, EventArgs e)
        {
            this.ProtectChosenFilesToLevel(UserDataAvailability.AfterFirstUnlock);
        }

        private void FolderL2_Click(object sender, EventArgs e)
        {
            this.ProtectChosenFilesToLevel(UserDataAvailability.WhileUnlocked);
        }

        private void FolderUnprotect_Click(object sender, EventArgs e)
        {
            this.ProtectChosenFilesToLevel(UserDataAvailability.Always);
        }

        private void FileL1_Click(object sender, EventArgs e)
        {
            var udpm = UserDataProtectionManager.TryGetDefault();
            foreach (var file in g_fileList)
            {
                FileInfo fileInfo = new FileInfo(file);
                ProtectFile(fileInfo, udpm, UserDataAvailability.AfterFirstUnlock);
            }
        }

        private void FileL2_Click(object sender, EventArgs e)
        {
            var udpm = UserDataProtectionManager.TryGetDefault();
            foreach (var file in g_fileList)
            {
                FileInfo fileInfo = new FileInfo(file);
                ProtectFile(fileInfo, udpm, UserDataAvailability.WhileUnlocked);
            }
        }

        private void FileUnprotect_Click(object sender, EventArgs e)
        {
            var udpm = UserDataProtectionManager.TryGetDefault();
            foreach (var file in g_fileList)
            {
                FileInfo fileInfo = new FileInfo(file);
                ProtectFile(fileInfo, udpm, UserDataAvailability.Always);
            }
        }

        private void BufferL1_Click(object sender, EventArgs e)
        {
            ProtectBuffer(textBox1.Text, UserDataAvailability.AfterFirstUnlock);
        }

        private void BufferL2_Click(object sender, EventArgs e)
        {
            ProtectBuffer(textBox1.Text, UserDataAvailability.WhileUnlocked);
        }

        private void BufferUnprotect_Click(object sender, EventArgs e)
        {
            UnprotectBuffer(g_protectbase64EncodedContent);
        }
    }
}
