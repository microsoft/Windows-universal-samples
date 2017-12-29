@ECHO OFF
IF /I "%1"=="-UNINSTALL" GOTO Uninstall
	xcopy /f /y ".\UWPSDKSampleCS" "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\Visual C#\Windows\Universal\UWPSDKSampleCS\"
	xcopy /f /y ".\UWPSDKSampleCPP" "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\Visual C++ Project\Windows\Universal\UWPSDKSampleCPP\"
	REM xcopy /f /y ".\UWPSDKSampleJS" "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\JavaScript\Windows\Universal\UWPSDKSampleJS\"
	copy ".\Shared\README.md" "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\Visual C#\Windows\Universal\UWPSDKSampleCS"
	copy ".\Shared\README.MD" "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\Visual C++ Project\Windows\Universal\UWPSDKSampleCPP"
	REM copy ".\Shared\README.MD" "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\JavaScript\Windows\Universal\UWPSDKSampleJS"
	xcopy /f /y ".\UWPSDKSampleScenarioItemCPP" "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ItemTemplates\Visual C++ Project\UWPSDKSampleScenarioItem\"
	xcopy /f /y ".\UWPSDKSampleScenarioItemCS" "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ItemTemplates\Visual C#\UWPSDKSampleScenarioItem\"
	copy "..\media\windows-sdk.png" "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\Visual C#\Windows\Universal\UWPSDKSampleCS"
	copy "..\media\windows-sdk.png" "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\Visual C++ Project\Windows\Universal\UWPSDKSampleCPP"
	REM copy "..\media\windows-sdk.png" "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\JavaScript\Windows\Universal\UWPSDKSampleJS"
	copy "..\media\windows-sdk.png" "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ItemTemplates\Visual C++ Project\UWPSDKSampleScenarioItem"
	copy "..\media\windows-sdk.png" "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ItemTemplates\Visual C#\UWPSDKSampleScenarioItem"
	GOTO End
:Uninstall
	rd /s/q "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\Visual C#\Windows\Universal\UWPSDKSampleCS"
	rd /s/q "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\Visual C++ Project\Windows\Universal\UWPSDKSampleCPP"
	REM rd /s/q "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ProjectTemplates\JavaScript\Windows\Universal\UWPSDKSampleJS"
	rd /s/q "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ItemTemplates\Visual C++ Project\UWPSDKSampleScenarioItem"
	rd /s/q "%USERPROFILE%\Documents\Visual Studio 2017\Templates\ItemTemplates\Visual C#\UWPSDKSampleScenarioItem"
:End