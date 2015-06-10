#*********************************************************
#
# Copyright (c) Microsoft. All rights reserved.
# THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
# ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
# IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
# PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
#
#*********************************************************

param(
    [switch]$Force = $false
)

$scriptPath = $(Split-Path $MyInvocation.MyCommand.Path)
$firewallRuleName = "StreamSocketSample - HTTP 443"
$requiredFeatures = "IIS-WebServer", "IIS-WebServerRole"
$settingsFile = "$scriptPath\StreamSocketSampleScriptSettings.xml"

$settings = @{"featuresToEnable"=""; "certificateThumbprint"=""}

# Check if running as Administrator.
$windowsIdentity = [System.Security.Principal.WindowsIdentity]::GetCurrent()
$principal = New-Object System.Security.Principal.WindowsPrincipal($windowsIdentity)
$administratorRole = [System.Security.Principal.WindowsBuiltInRole]::Administrator
if ($principal.IsInRole($administratorRole) -eq $false)
{
    "Please run the script from elevated PowerShell."
    return
}

# Check if the config file was created. If yes, user should first run RemoveServer.ps1.
if (Test-Path $settingsFile)
{
    "The script has already been run. Please run RemoveServer.ps1 before running it again."
    return
}

if(!$Force)
{
    Write-Warning "This script will attempt to enable IIS and configure HTTPS."
    Write-Warning "The script may interfere with any existing IIS configuration on this machine."
    Write-Warning "Press N to abort if IIS is already configured on this PC."
    $answer = Read-Host '[Y] Yes [N] No (default is "N")'

    if ($answer -ne "Y")
    {
        Write-Host "Aborted by user."
        return
    }
}

# Get features that should be enabled.
$featuresToEnable = @()
Get-WindowsOptionalFeature -Online | ?{ $_.State -eq [Microsoft.Dism.Commands.FeatureState]::Disabled -and $requiredFeatures -contains $_.FeatureName } | %{ $featuresToEnable += $_.FeatureName }

# Save enabled features to the config file.
$settings.featuresToEnable = $featuresToEnable 
$settings | Export-Clixml -Path $settingsFile

# Enable features.
if ($featuresToEnable.Count -gt 0)
{
    "Enabling following features: $($featuresToEnable -join ", ")."
    Enable-WindowsOptionalFeature -Online -FeatureName $featuresToEnable > $null
}

# Add firewall rule.
"Adding firewall rule `'$firewallRuleName`'."
New-NetFirewallRule -DisplayName $firewallRuleName -Direction Inbound -Protocol TCP -LocalPort 443 -Action Allow > $null

"Creating self-signed certificate."
$cert = New-SelfSignedCertificate -DnsName www.fabrikam.com -CertStoreLocation cert:\LocalMachine\My
$settings.certificateThumbprint = $cert.Thumbprint
$settings | Export-Clixml -Path $settingsFile

"Creating SSL IIS binding"
New-WebBinding -Name "Default Web Site" -IP "*" -Port 443 -Protocol https

"Removing all existing SSL bindings"
Remove-Item IIS:\SslBindings\*

"Assigning certificate to the binding"
cd IIS:\SslBindings
$cert | New-Item 0.0.0.0!443 | out-null

"Done: https://localhost should now display an invalid-certificate web-page."

cd $scriptPath
