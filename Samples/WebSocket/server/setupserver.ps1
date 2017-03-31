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
$websitePath = "$scriptPath\website"
$wsfirewallRuleName = "WebSocketSample - HTTP 80"
$wssFirewallRuleName = "WebSocketSample - HTTPS 443"
$requiredFeatures = "IIS-WebServer", "IIS-WebServerRole", "NetFx4Extended-ASPNET45", "IIS-ApplicationDevelopment", "IIS-ASPNET45", "IIS-ISAPIExtensions", "IIS-ISAPIFilter", "IIS-NetFxExtensibility45", "IIS-WebSockets"
$settings = @{"featuresToEnable"=@(); "certificateThumbprint"=""; "webBindingAdded"=$false; "oldDefaultCert"=""}
$settingsFile = "$scriptPath\WebSocketSampleScriptSettings"

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
    return;
}

# Get features that should be enabled.
$featuresToEnable = @()
Get-WindowsOptionalFeature -Online | ?{ $_.State -eq [Microsoft.Dism.Commands.FeatureState]::Disabled -and $requiredFeatures -contains $_.FeatureName } | %{ $featuresToEnable += $_.FeatureName }
$settings.featuresToEnable = $featuresToEnable

# Enable features.
if ($featuresToEnable.Count -gt 0)
{
    "Enabling following features: $($featuresToEnable -join ", ")."
    Enable-WindowsOptionalFeature -Online -FeatureName $featuresToEnable > $null
}

# Copy the webpage files
if (-not (Test-Path $iisAppPath))
{
    mkdir $iisAppPath > $null
    Copy-Item $websitePath\* $iisAppPath -r
}

# Add web application.
if ($(Get-WebApplication $iisAppName) -eq $null)
{
    "Adding a `'$iisAppName`' web application at $iisAppPath."
    New-WebApplication -Site "Default Web Site" -Name $iisAppName -PhysicalPath $iisAppPath > $null
    Add-WebConfigurationProperty -pspath "MACHINE/WEBROOT/APPHOST/Default Web Site/$iisAppName" -filter "appSettings" -name "." -value @{key='aspnet:UseTaskFriendlySynchronizationContext';value='true'}
    Add-WebConfigurationProperty -pspath "MACHINE/WEBROOT/APPHOST/Default Web Site/$iisAppName" -filter "system.diagnostics/trace/listeners" -name "." -value `
    @{ `
        "name"="WebSocketSampleTraceListener"; `
        "initializeData"="$iisAppPath\trace.log"; `
        "type"="System.Diagnostics.TextWriterTraceListener" `
    }
    Set-WebConfigurationProperty -pspath "MACHINE/WEBROOT/APPHOST/Default Web Site/$iisAppName" -filter "system.diagnostics/trace" -name "autoflush" -value "true"

    # Allow IIS_IUSRS to write to this directory. This is necessary if a log file is created.
    $acl = Get-Acl $iisAppPath
    $writePermission = "BUILTIN\IIS_IUSRS", "Write", "Allow"
    $accessRule = New-Object System.Security.AccessControl.FileSystemAccessRule $writePermission
    $acl.SetAccessRule($accessRule)
    Set-Acl $iisAppPath $acl
}
else
{
    "Web application `'$iisAppName`' already exists."
}

# Creating self-signed certificate and assigning it to the default binding
"Creating self-signed certificate."
$cert = New-SelfSignedCertificate -DnsName www.fabrikam.com -CertStoreLocation cert:\LocalMachine\My

$binding = Get-WebBinding "Default Web Site" -Protocol "https" -Port 443 -IPAddress "*"
if ($binding -eq $null)
{
    "Creating SSL IIS binding"
    New-WebBinding -Name "Default Web Site" -IP "*" -Port 443 -Protocol https
    $settings.webBindingAdded = $true
}

if(Test-Path IIS:\SslBindings\0.0.0.0!443)
{
    $oldBinding = Get-Item IIS:\SslBindings\0.0.0.0!443
    "Removing existing default SSL binding"
    $settings.oldDefaultCert = $oldBinding.Thumbprint
    Remove-Item IIS:\SslBindings\0.0.0.0!443
}

"Assigning certificate to the default binding"

$cert | New-Item IIS:\SslBindings\0.0.0.0!443 -Force | out-null

# Add firewall rules.
"Adding firewall rule `'$wsfirewallRuleName`'."
New-NetFirewallRule -DisplayName $wsfirewallRuleName -Direction Inbound -Protocol TCP -LocalPort 80 -Action Allow > $null
"Adding firewall rule `'$wssfirewallRuleName`'."
New-NetFirewallRule -DisplayName $wssfirewallRuleName -Direction Inbound -Protocol TCP -LocalPort 443 -Action Allow > $null

"Saving settings for uninstall"
$settings.certificateThumbprint = $cert.Thumbprint
$settings | Export-Clixml -Path $settingsFile