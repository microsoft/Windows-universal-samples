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
$firewallRuleName = "BackgroundTransferSample - HTTP 80"
$settingsFile = "$scriptPath\BackgroundTransferSampleScriptSettings"

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

# Get content of the config file.
$content = $(Get-Content $settingsFile).Trim()
$featuresToDisable = @()
if ($content -ne "")
{
    $featuresToDisable = $content -split ","
}

# Config file is not needed anymore so remove it.
Remove-Item $settingsFile

# Remove the firewall rule added by setup.
"Removing firewall rule `'$firewallRuleName`'."
Remove-NetFirewallRule -DisplayName $firewallRuleName > $null

# Remove features enabled by setup.
if ($featuresToDisable.Count -gt 0)
{
    "Following features were enabled by SetupServer.ps1 script: $($featuresToDisable -join ", ")."
    "Should they be disabled?"
    $answer = Read-Host '[Y] Yes [N] No (default is "Y")'
    if ($answer -eq "" -or $answer -eq "Y")
    {
        Disable-WindowsOptionalFeature -Online -FeatureName $featuresToDisable > $null
    }
}
