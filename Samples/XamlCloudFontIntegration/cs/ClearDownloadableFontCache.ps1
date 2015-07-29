$Identity = [Security.Principal.WindowsIdentity]::GetCurrent()
$Principal = [Security.Principal.WindowsPrincipal]$Identity
if (!$Principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Warning 'You must run this script as administrator.'
    Exit
}

$FontCachePath = "$env:WinDir\ServiceProfiles\LocalService\AppData\Local\FontCache\Fonts"
if (Test-Path $FontCachePath) {
    Get-Service 'FontCache' | Stop-Service
    Get-ChildItem -Path $FontCachePath -Filter *.bin | Remove-Item
    Get-Service 'FontCache' | Start-Service
}
