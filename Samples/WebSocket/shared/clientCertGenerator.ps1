#*********************************************************
#
# Copyright (c) Microsoft. All rights reserved.
# This code is licensed under the MIT License (MIT).
# THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
# ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
# IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
# PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
#
#*********************************************************

$sampleRootPath = Split-Path $(Split-Path -Resolve $MyInvocation.MyCommand.Path)
$rootCertPath = "$sampleRootPath\server\RootCert.cer"
$pfxFilePath = "$sampleRootPath\shared\clientCert.pfx"

if ((Test-Path $rootCertPath) -And (Test-Path $pfxFilePath)) {
    "The necessary certificates for this sample have already been created."
    "If you want to regenerate them, delete the files below and rerun this script."
    $rootCertPath
    $pfxFilePath
    return
}

# Check if running as Administrator.
$windowsIdentity = [System.Security.Principal.WindowsIdentity]::GetCurrent()
$principal = New-Object System.Security.Principal.WindowsPrincipal($windowsIdentity)
$administratorRole = [System.Security.Principal.WindowsBuiltInRole]::Administrator
if ($principal.IsInRole($administratorRole) -eq $false)
{
    "This script must be run from an elevated PowerShell."
    return
}

# Create a Root Certificate with a Private Key
$rootCert = New-SelfSignedCertificate -DnsName "www.contoso.com" -KeySpec Signature -Subject "CN=www.contoso.com" -KeyExportPolicy Exportable -KeyUsage CertSign,CRLSign,DigitalSignature -CertStoreLocation "cert:\LocalMachine\My" -TextExtension @("2.5.29.19={text}CA=true&pathlength=0")

# Export the root certificate to a file
Export-Certificate -Cert $rootCert -FilePath $rootCertPath

# Verify if the root certificate was created successfully
if ($rootCert -eq $null) {
    Write-Error "Failed to create the root certificate."
    return
}

# Create the Client Certificate
$clientCert = New-SelfSignedCertificate -KeySpec Signature -Subject "CN=WebSocketSample" -KeyExportPolicy Exportable -KeyUsage DigitalSignature,KeyEncipherment -CertStoreLocation "cert:\LocalMachine\My" -FriendlyName "WebSocketSampleClientCert" -Signer $rootCert -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.2")

# Verify if the client certificate was created successfully
if ($clientCert -eq $null) {
    Write-Error "Failed to create the client certificate."
    return
} 

# Export the Client Certificate to a PFX File
$pfxPassword = ConvertTo-SecureString -String "1234" -Force -AsPlainText
Export-PfxCertificate -Cert $clientCert -FilePath $pfxFilePath -Password $pfxPassword

# Verify if the client certificate was exported successfully
if (-Not (Test-Path $pfxFilePath)) {
    Write-Error "Failed to export the client certificate to a PFX file."
    return
} 