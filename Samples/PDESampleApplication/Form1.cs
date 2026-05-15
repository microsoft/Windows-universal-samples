using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using Windows.Security.DataProtection;
using Windows.Storage;
using System.Runtime.CompilerServices;
using Windows.Security.Cryptography;

namespace PdeTest
{
    public partial class Form1 : Form
    {
        UserDataProtectionManager m_udpm;

        public Form1()
        {
            InitializeComponent();
        }

        private void LogLine(string msg)
        {
            if (InvokeRequired)
            {
                this.Invoke(new Action<string>(LogLine), new object[] { msg });
                return;
            }
            string ts = DateTime.Now.ToString("MM/dd/yy HH:mm:ss.fff");
            textBox1.Text += "[" + ts + "] " + msg + "\r\n";
            textBox1.Select(textBox1.TextLength, 0);
            textBox1.ScrollToCaret();
            Console.WriteLine(msg);
        }

        async void ProtectChosenFilesToLevel(UserDataAvailability level)
        {
            var udpm = UserDataProtectionManager.TryGetDefault();
            var fileNames = txtSelectedFiles.Text.Split(new char[] { '\r', '\n' });
            foreach (var fileName in fileNames)
            {
                if (fileName.Length > 0)
                {
                    IStorageItem item;
                    {
                        LogLine("Before");
                        item = await StorageFile.GetFileFromPathAsync(fileName);
                        LogLine("After");
                    }
                    var protectResult = await udpm.ProtectStorageItemAsync(item, level);
                    if (protectResult == UserDataStorageItemProtectionStatus.Succeeded)
                    {
                        LogLine("Protected " + fileName + " to level " + level);
                    }
                    else
                    {
                        LogLine("Protection failed for " + fileName + " to level " + level + ", status: " + protectResult);
                    }
                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            ProtectChosenFilesToLevel(UserDataAvailability.AfterFirstUnlock);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            ProtectChosenFilesToLevel(UserDataAvailability.WhileUnlocked);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            ProtectChosenFilesToLevel(UserDataAvailability.Always);
        }

        private void LogCurrentDataAvailability()
        {
            bool l1Avl = m_udpm.IsContinuedDataAvailabilityExpected(UserDataAvailability.AfterFirstUnlock);
            bool l2Avl = m_udpm.IsContinuedDataAvailabilityExpected(UserDataAvailability.WhileUnlocked);
            LogLine("IsContinuedDataAvailabilityExpected AfterFirstUnlock: " + l1Avl + ", WhileUnlocked: " + l2Avl);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            m_udpm = UserDataProtectionManager.TryGetDefault();
            if (m_udpm == null)
            {
                LogLine("Personal Data Encryption is not supported or enabled. Restart this app to check again.");
                button1.Enabled = false;
                button2.Enabled = false;
                button3.Enabled = false;
                button4.Enabled = false;
                btnProtectBufferL1.Enabled = false;
                btnUnprotectBuffer.Enabled = false;
            }
            else
            {
                LogLine("Personal Data Encryption is enabled.");
                m_udpm.DataAvailabilityStateChanged += M_udpm_DataAvailabilityStateChanged;
                LogCurrentDataAvailability();
                LogLine("Listening to DataAvailabilityStateChanged event");
            }
        }

        private void M_udpm_DataAvailabilityStateChanged(UserDataProtectionManager sender, UserDataAvailabilityStateChangedEventArgs args)
        {
            LogLine("DataAvailabilityStateChanged event received");
            LogCurrentDataAvailability();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            LogCurrentDataAvailability();
        }

        async void ProtectBuffer(UserDataAvailability level)
        {
            var buffer = CryptographicBuffer.ConvertStringToBinary(textBox2.Text, BinaryStringEncoding.Utf8);
            var protectedBuffer = await m_udpm.ProtectBufferAsync(buffer, level);
            var base64ProtectedBuffer = CryptographicBuffer.EncodeToBase64String(protectedBuffer);
            LogLine("Protected buffer: " + base64ProtectedBuffer);
        }

        private void btnProtectBufferL1_Click(object sender, EventArgs e)
        {
            ProtectBuffer(UserDataAvailability.AfterFirstUnlock);
        }

        private void btnProtectBufferL2_Click(object sender, EventArgs e)
        {
            ProtectBuffer(UserDataAvailability.WhileUnlocked);
        }

        async void UnprotectBuffer()
        {
            var protectedBuffer = CryptographicBuffer.DecodeFromBase64String(textBox3.Text);
            var result = await m_udpm.UnprotectBufferAsync(protectedBuffer);
            if (result.Status == UserDataBufferUnprotectStatus.Succeeded)
            {
                var msg = CryptographicBuffer.ConvertBinaryToString(BinaryStringEncoding.Utf8, result.UnprotectedBuffer);
                LogLine("Unprotected string: " + msg);
            }
            else
            {
                LogLine("This protected buffer is currently unavailable for unprotection");
            }
        }

        private void btnUnprotectBuffer_Click(object sender, EventArgs e)
        {
            UnprotectBuffer();
        }

        private void btnSelectFiles_Click(object sender, EventArgs e)
        {
            var dialogResult = openFileDialog1.ShowDialog();
            if (dialogResult == DialogResult.OK)
            {
                var udpm = UserDataProtectionManager.TryGetDefault();
                var fileNames = openFileDialog1.FileNames;
                var fileNamesConcat = new StringBuilder();
                foreach (var fileName in fileNames)
                {
                    fileNamesConcat.AppendLine(fileName);
                }
                txtSelectedFiles.Text = fileNamesConcat.ToString();
            }
        }
    }
}
