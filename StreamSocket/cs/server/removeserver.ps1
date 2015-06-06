#*********************************************************
#
# Copyright (c) Microsoft. All rights reserved.
# THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
# ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
# IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
# PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
#
#*********************************************************

$scriptPath = $(Split-Path $MyInvocation.MyCommand.Path)
$firewallRuleName = "StreamSocketSample - HTTP 443"
$settingsFile = "$scriptPath\StreamSocketSampleScriptSettings.xml"

# Check if running as Administrator.
$windowsIdentity = [System.Security.Principal.WindowsIdentity]::GetCurrent()
$principal = New-Object System.Security.Principal.WindowsPrincipal($windowsIdentity)
$administratorRole = [System.Security.Principal.WindowsBuiltInRole]::Administrator
if ($principal.IsInRole($administratorRole) -eq $false)
{
    "Please run the script from elevated PowerShell"
    return
}

# Check if the config file was created. If not, then the setup script did not run.
if (-not $(Test-Path $settingsFile))
{
    "Please run SetupServer.ps1 before running this script."
    return
}

$settings = Import-Clixml $settingsFile

# Remove the firewall rule added by setup.
"Removing firewall rule `'$firewallRuleName`'."
Remove-NetFirewallRule -DisplayName $firewallRuleName > $null

# Remove features enabled by setup.
if ($settings.featuresToEnable.Count -gt 0)
{
    "Following features were enabled by SetupServer.ps1 script: $($settings.featuresToEnable -join ", ")."
    "Should they be disabled?"
    $answer = Read-Host '[Y] Yes [N] No (default is "Y")'
    if ($answer -eq "" -or $answer -eq "Y")
    {
        Disable-WindowsOptionalFeature -Online -FeatureName $settings.featuresToEnable > $null
    }
}

"Removing certificate with thumbprint " + $settings.certificateThumbprint
$certPath = Join-Path "cert:\LocalMachine\My" $settings.certificateThumbprint
Remove-Item $certPath

"Removing settings file"
Remove-Item $settingsFile

