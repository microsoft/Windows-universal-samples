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

$scriptPath = $(Split-Path $MyInvocation.MyCommand.Path)
$iisAppName = "WebSocketSample"
$iisAppPath = "$env:systemdrive\inetpub\wwwroot\WebSocketSample"
$wsfirewallRuleName = "WebSocketSample - HTTP 80"
$wssFirewallRuleName = "WebSocketSample - HTTPS 443"
$settingsFile = "$scriptPath\WebSocketSampleScriptSettings"

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
$settings = Import-Clixml -Path $settingsFile
$featuresToDisable = $settings.featuresToEnable;


# Remove the firewall rule added by setup.
"Removing firewall rule `'$wsfirewallRuleName`'."
Remove-NetFirewallRule -DisplayName $wsfirewallRuleName > $null
"Removing firewall rule `'$wsssFirewallRuleName`'."
Remove-NetFirewallRule -DisplayName $wssFirewallRuleName > $null

"Removing certificate with thumbprint " + $settings.certificateThumbprint
gci cert:\ -Recurse | where {$_.Thumbprint -eq $settings.certificateThumbprint } | Remove-Item

if ($settings.webBindingAdded) 
{
    Remove-WebBinding -Name "Default Web Site" -IP "*" -Port 443 -Protocol https
}

Remove-Item IIS:\SslBindings\0.0.0.0!443
if ($settings.oldDefaultCert -ne "") 
{
    Join-Path "cert:\LocalMachine\My" $settings.oldDefaultCert | New-Item IIS:\SslBindings\0.0.0.0!443 -Force | out-null
}

if ($(Get-WebApplication $iisAppName) -ne $null)
{
    "Removing the `'$iisAppName`' web application at $iisAppPath."
    Remove-WebApplication -Site "Default Web Site" -Name $iisAppName > $null
}

Remove-Item $iisAppPath -Recurse

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

# Config file is not needed anymore so remove it.
Remove-Item $settingsFile