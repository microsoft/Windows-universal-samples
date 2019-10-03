@ECHO OFF
IF /I "%1"=="-UNINSTALL" GOTO Uninstall
	call :directory "ProjectTemplates\Visual C#\Windows Universal" UWPSDKSampleCS Shared\Project\*
	call :directory "ItemTemplates\Visual C#\Windows Universal" UWPSDKSampleScenarioItemCS Shared\Item\*

	call :directory "ProjectTemplates\Visual C++ Project\Windows Universal" UWPSDKSampleCX Shared\Project\*
	call :directory "ItemTemplates\Visual C++ Project\Windows Universal" UWPSDKSampleScenarioItemCX Shared\Item\*

	call :directory "ProjectTemplates\Visual C++ Project\Windows Universal" UWPSDKSampleCPP Shared\Project\*
	call :directory "ItemTemplates\Visual C++ Project\Windows Universal" UWPSDKSampleScenarioItemCPP Shared\Item\*

	GOTO :eof

:directory
	setlocal
	set SUBDIR=%1
	set SUBDIR=%SUBDIR:"=%
	set OUTPUTDIR=%USERPROFILE%\Documents\Visual Studio 2017\Templates\%SUBDIR%\%2
	xcopy /f /y %2 "%OUTPUTDIR%\"
	copy "..\media\windows-sdk.png" "%OUTPUTDIR%"
	shift
	shift
:copymore
	if "%1"=="" goto :enddirectory
	copy %1 "%OUTPUTDIR%"
	shift
	goto :copymore
:enddirectory
	endlocal
	goto :eof
:Uninstall
	rd /s/q "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\Visual C#\Windows Universal\UWPSDKSampleCS"
	rd /s/q "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ItemTemplates\Visual C#\Windows Universal\UWPSDKSampleScenarioItemCS"

	rd /s/q "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\Visual C++ Project\Windows Universal\UWPSDKSampleCX"
	rd /s/q "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ItemTemplates\Visual C++ Project\Windows Universal\UWPSDKSampleScenarioItemCX"

	rd /s/q "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\Visual C++ Project\Windows Universal\UWPSDKSampleCPPWINRT"
	rd /s/q "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ItemTemplates\Visual C++ Project\Windows Universal\UWPSDKSampleScenarioItemCPPWINRT"
