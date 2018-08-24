Quick guide to creating new SDK Sample projects.  

The templates are designed to work with the linked files in the SharedContent folders as part of the Universal Windows Platform Samples repo.  As such, the sample must be created in the repository.

Setup - Install the UWP Sample project templates:
1) Shut down Visual Studio if it is running
2) Open an Administrator command prompt, and run "SharedContent\InstallSDKTemplates.cmd", this will copy the template files to the correct user locations.

Create a new Sample project:
3) Launch Visual Studio
4) File.New Project...Templates->[Visual C++|Visual C#|JavaScript].Windows.Universal.Windows SDK UWP Sample (XAML)
5) Uncheck the checkbox for "Create directory for solution" 
6) Create the sample in the Samples folder of your local repo, ie: "C:\repos\public\Samples"
7) Enter an appropriate name for the sample and click OK

Move the Sample project to the appropriate language folder.  This is not natural to VS project system, so we need to move it.
8) Right Click on the solution and choose "Open Folder in File Explorer"
9) Go back to VS and close VS
10) In File Explorer, create a language subfolder (ie: cs, cpp, js) and move all the solution files to the new subfolder, ie:
	<SampleName> -> <SampleName>\<lang>
11) Load the solution in VS and verify you can now BUILD it.
 Repeat this for every language you are going to create a sample for.

The default template has 3 scenarios, delete those that you don't need. If you need to add more follow these instructions.
1) With you project open in VS, choose 'Add..New Item.."
2) Choose 'SDK Sample Scenario Page'
