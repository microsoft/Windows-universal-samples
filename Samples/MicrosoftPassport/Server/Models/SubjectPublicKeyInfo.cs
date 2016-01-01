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
// Please refer to the Microsoft Passport and Windows Hello
// whitepaper on the Windows Dev Center for a complete
// explanation of Microsoft Passport and Windows Hello
// implementation: 
// http://go.microsoft.com/fwlink/p/?LinkId=522066
//*********************************************************
using Microsoft.Win32.SafeHandles;
using System;
using System.Runtime.ConstrainedExecution;
using System.Runtime.InteropServices;
using System.Security.Cryptography;

namespace MicrosoftPassport.Models
{
    public sealed class SubjectPublicKeyInfo
    {
        const uint STATUS_SUCCESS = 0;

        private readonly byte[] _rawCngPublicKeyBlob;

        public SubjectPublicKeyInfo(byte[] subjectPublicKeyInfo)
        {
            if (subjectPublicKeyInfo == null)
            {
                throw new ArgumentNullException(nameof(subjectPublicKeyInfo));
            }
            if (subjectPublicKeyInfo.Length == 0)
            {
                throw new ArgumentException("Array cannot be empty.", nameof(subjectPublicKeyInfo));
            }

            // const definitions from wincrypt.h and bcrypt.h
            const uint X509_ASN_ENCODING = 0x00000001;
            const uint CRYPT_DECODE_ALLOC_FLAG = 0x8000;
            const int X509_PUBLIC_KEY_INFO = 8;
            const string BCRYPT_PUBLIC_KEY_BLOB = "PUBLICBLOB";

            // The incoming raw public key is an ASN.1-encoded subject public key info

            SafeLocalAllocHandle publicKeyInfoSafeHandle;
            uint cbPublicKeyInfo; // unused
            bool success = NativeMethods.CryptDecodeObjectEx(
                dwCertEncodingType: X509_ASN_ENCODING,
                lpszStructType: (IntPtr)X509_PUBLIC_KEY_INFO,
                pbEncoded: subjectPublicKeyInfo,
                cbEncoded: (uint)subjectPublicKeyInfo.Length,
                dwFlags: CRYPT_DECODE_ALLOC_FLAG,
                pDecodePara: IntPtr.Zero,
                pvStructInfo: out publicKeyInfoSafeHandle,
                pcbStructInfo: out cbPublicKeyInfo);

            if (!success)
            {
                throw new CryptographicException($"CryptDecodeObjectEx failed with error code 0x{Marshal.GetLastWin32Error():X8}.");
            }

            // The CERT_PUBLIC_KEY_INFO struct can be converted to a BCRYPT key handle

            SafeBCryptKeyHandle bcryptKeyHandle;
            success = NativeMethods.CryptImportPublicKeyInfoEx2(
                dwCertEncodingType: X509_ASN_ENCODING,
                pInfo: publicKeyInfoSafeHandle,
                dwFlags: 0,
                pvAuxInfo: IntPtr.Zero,
                phKey: out bcryptKeyHandle);

            if (!success)
            {
                throw new CryptographicException($"CryptImportPublicKeyInfoEx2 failed with error code 0x{Marshal.GetLastWin32Error():X8}.");
            }

            publicKeyInfoSafeHandle.Dispose(); // no longer need this

            // Export the BCRYPT key handle as raw CNG key material

            // first, get the required byte count
            uint numBytesRequired;
            int ntstatus = NativeMethods.BCryptExportKey(
                hKey: bcryptKeyHandle,
                hExportKey: IntPtr.Zero,
                pszBlobType: BCRYPT_PUBLIC_KEY_BLOB,
                pbOutput: null,
                cbOutput: 0,
                pcbResult: out numBytesRequired,
                dwFlags: 0);

            if (ntstatus != STATUS_SUCCESS)
            {
                throw new CryptographicException($"BCryptExportKey failed with error code 0x{ntstatus:X8}.");
            }

            // next, allocate the real buffer and populate it
            byte[] rawCngKey = new byte[checked((int)numBytesRequired)];
            ntstatus = NativeMethods.BCryptExportKey(
                hKey: bcryptKeyHandle,
                hExportKey: IntPtr.Zero,
                pszBlobType: BCRYPT_PUBLIC_KEY_BLOB,
                pbOutput: rawCngKey,
                cbOutput: (uint)rawCngKey.Length,
                pcbResult: out numBytesRequired,
                dwFlags: 0);

            if (ntstatus != STATUS_SUCCESS)
            {
                throw new CryptographicException($"BCryptExportKey failed with error code 0x{ntstatus:X8}.");
            }

            bcryptKeyHandle.Dispose(); // no longer need this

            // resize the CNG key buffer if we overallocated
            if (numBytesRequired != rawCngKey.Length)
            {
                byte[] tempBuffer = new byte[numBytesRequired];
                Buffer.BlockCopy(rawCngKey, 0, tempBuffer, 0, tempBuffer.Length);
                rawCngKey = tempBuffer;
            }

            // finally, set properties
            _rawCngPublicKeyBlob = rawCngKey;
        }

        public CngKey GetPublicKey()
        {
            return CngKey.Import(_rawCngPublicKeyBlob, CngKeyBlobFormat.GenericPublicBlob);
        }

        private sealed class SafeBCryptKeyHandle : SafeHandleZeroOrMinusOneIsInvalid
        {
            private SafeBCryptKeyHandle() : base(ownsHandle: true) { }

            protected override bool ReleaseHandle()
            {
                return (NativeMethods.BCryptDestroyKey(handle) == STATUS_SUCCESS);
            }
        }

        private sealed class SafeLocalAllocHandle : SafeHandleZeroOrMinusOneIsInvalid
        {
            private SafeLocalAllocHandle() : base(ownsHandle: true) { }

            public static SafeLocalAllocHandle Alloc(int cb)
            {
                SafeLocalAllocHandle handle = new SafeLocalAllocHandle();
                handle.AllocCore(cb);
                return handle;
            }

            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
            private void AllocCore(int cb)
            {
                SetHandle(Marshal.AllocHGlobal(cb));
            }

            protected override bool ReleaseHandle()
            {
                Marshal.FreeHGlobal(handle);
                return true;
            }
        }

        private static class NativeMethods
        {
            // https://msdn.microsoft.com/en-us/library/windows/desktop/aa379912(v=vs.85).aspx
            [DllImport("crypt32.dll", CallingConvention = CallingConvention.Winapi, SetLastError = true)]
            internal static extern bool CryptDecodeObjectEx(
                [In] uint dwCertEncodingType,
                [In] IntPtr lpszStructType,
                [In] byte[] pbEncoded,
                [In] uint cbEncoded,
                [In] uint dwFlags,
                [In] IntPtr pDecodePara,
                [Out] out SafeLocalAllocHandle pvStructInfo,
                [Out] out uint pcbStructInfo);

            // https://msdn.microsoft.com/en-us/library/windows/desktop/aa380211(v=vs.85).aspx
            [DllImport("crypt32.dll", CallingConvention = CallingConvention.Winapi, SetLastError = true)]
            internal static extern bool CryptImportPublicKeyInfoEx2(
                [In] uint dwCertEncodingType,
                [In] SafeLocalAllocHandle pInfo,
                [In] uint dwFlags,
                [In] IntPtr pvAuxInfo,
                [Out] out SafeBCryptKeyHandle phKey);

            // https://msdn.microsoft.com/en-us/library/windows/desktop/aa375404(v=vs.85).aspx
            [DllImport("bcrypt.dll", CallingConvention = CallingConvention.Winapi)]
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            internal static extern int BCryptDestroyKey(
                [In] IntPtr hKey);

            // https://msdn.microsoft.com/en-us/library/windows/desktop/aa375434(v=vs.85).aspx
            [DllImport("bcrypt.dll", CallingConvention = CallingConvention.Winapi)]
            internal static extern int BCryptExportKey(
                [In] SafeBCryptKeyHandle hKey,
                [In] IntPtr hExportKey,
                [In, MarshalAs(UnmanagedType.LPWStr)] string pszBlobType,
                [Out] byte[] pbOutput,
                [In] uint cbOutput,
                [Out] out uint pcbResult,
                [In] uint dwFlags);
        }
    }
}