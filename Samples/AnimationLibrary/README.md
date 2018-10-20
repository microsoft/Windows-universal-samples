<!---
  category: GraphicsAndAnimation
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=620483
--->

# Animation library (HTML) sample

Shows how to use the Animation Library APIs. These functions provide you with the ability to use animations 
that are consistent with those used by Windows.

> **Note:** This sample is part of a large collection of UWP feature samples. 
> If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](https://github.com/Microsoft/Windows-universal-samples/archive/master.zip), but be 
> sure to unzip everything to access shared dependencies. For more info on working with the ZIP file, 
> the samples collection, and GitHub, see [Get the UWP samples from GitHub](https://aka.ms/ovu2uq). 
> For more samples, see the [Samples portal](https://aka.ms/winsamples) on the Windows Dev Center. 

The sample demonstrates the following animation scenarios:

-   Bringing an entire page onto the screen, all at one time or as separate parts (enterPage)
-   Transitioning between pages (enterPage/exitPage)
-   Bringing partial page content onto the screen (enterContent)
-   Transitioning partial page content (enterContent/exitContent)
-   Expanding a set of elements to make room for new element, and collapsing the original elements when the new element is removed (createExpandAnimation/createCollapseAnimation)
-   Animating a touch or mouse click response (pointerUp/pointerDown)
-   Adding and removing a list item (createAddToListAnimation/createDeleteToListAnimation)
-   Filtering a list of search results (createAddToSearchListAnimation/createDeleteFromSearchListAnimation)
-   Fading content in and out (fadeIn/fadeOut)
-   Crossfading an element in place (crossFade)
-   Moving an element to a new location (createRepositionAnimation)
-   Dragging and dropping an element (dragSourceStart/dragSourceEnd)
-   Repositioning elements to allow an element to be dropped and restoring those elements when it is not (dragBetweenEnter/dragBetweenLeave)
-   Showing and hiding pop-up UI (showPopup/hidePopup)
-   Showing and hiding edge-based UI (showEdgeUI/hideEdgeUI)
-   Showing and hiding a panel (showPanel/hidePanel)
-   Revealing whether an item supports swipe (swipeReveal)
-   Reacting to a swipe to select or deselect an item (swipeSelect/swipeDeselect)
-   Updating a badge in response to a badge notification (updateBadge)
-   Updating a tile in response to a tile notification (createPeekAnimation)
-   Running custom CSS animations and transitions (executeAnimation/executeTransition)
-   Enabling/disabling animations and checking whether animations are enabled (enableAnimations/disableAnimations/isAnimationEnabled)

This sample is written in HTML, CSS, and JavaScript. For the XAML version, see the [XAML personality animations sample](http://go.microsoft.com/fwlink/p/?linkid=242401) and the [XAML animations sample](http://go.microsoft.com/fwlink/p/?linkid=242404).

**Note** The Universal Windows app samples require Visual Studio 2017 to build and Windows 10 to execute.
 
To obtain information about Windows 10 development, go to the [Windows Dev Center](http://go.microsoft.com/fwlink/?LinkID=532421)

To obtain information about Microsoft Visual Studio and the tools for developing Windows apps, go to [Visual Studio](http://go.microsoft.com/fwlink/?LinkID=532422)

## Related topics

### Concepts

[Animating your UI with the Animation Library](https://msdn.microsoft.com/library/windows/apps/hh465165)

### Reference

[createAddToListAnimation](https://msdn.microsoft.com/library/windows/apps/br212653)  
[createAddToSearchListAnimation](https://msdn.microsoft.com/library/windows/apps/br212654)  
[createCollapseAnimation](https://msdn.microsoft.com/library/windows/apps/br212655)  
[createDeleteFromListAnimation](https://msdn.microsoft.com/library/windows/apps/br212656)  
[createDeleteFromSearchListAnimation](https://msdn.microsoft.com/library/windows/apps/br212657)  
[createExpandAnimation](https://msdn.microsoft.com/library/windows/apps/br212658)  
[createPeekAnimation](https://msdn.microsoft.com/library/windows/apps/br212659)  
[createRepositionAnimation](https://msdn.microsoft.com/library/windows/apps/br212660)  
[crossFade](https://msdn.microsoft.com/library/windows/apps/br212661)  
[disableAnimations](https://msdn.microsoft.com/library/windows/apps/hh779759)  
[dragBetweenEnter](https://msdn.microsoft.com/library/windows/apps/br212668)  
[dragBetweenLeave](https://msdn.microsoft.com/library/windows/apps/br212669)  
[dragSourceEnd](https://msdn.microsoft.com/library/windows/apps/br212670)  
[dragSourceStart](https://msdn.microsoft.com/library/windows/apps/br212671)  
[enableAnimations](https://msdn.microsoft.com/library/windows/apps/hh779760)  
[enterContent](https://msdn.microsoft.com/library/windows/apps/hh701582)  
[enterPage](https://msdn.microsoft.com/library/windows/apps/br212672)  
[executeAnimation](https://msdn.microsoft.com/library/windows/apps/hh779762)  
[executeTransition](https://msdn.microsoft.com/library/windows/apps/hh779763)  
[exitContent](https://msdn.microsoft.com/library/windows/apps/hh701585)  
[exitPage](https://msdn.microsoft.com/library/windows/apps/hh701586)  
[fadeIn](https://msdn.microsoft.com/library/windows/apps/br212673)  
[fadeOut](https://msdn.microsoft.com/library/windows/apps/br212674)  
[hideEdgeUI](https://msdn.microsoft.com/library/windows/apps/br212676)  
[hidePanel](https://msdn.microsoft.com/library/windows/apps/br212677)  
[hidePopup](https://msdn.microsoft.com/library/windows/apps/br212678)  
[isAnimationEnabled](https://msdn.microsoft.com/library/windows/apps/hh779793)  
[pointerDown](https://msdn.microsoft.com/library/windows/apps/br212680)  
[pointerUp](https://msdn.microsoft.com/library/windows/apps/br212681)  
[showEdgeUI](https://msdn.microsoft.com/library/windows/apps/br230466)  
[showPanel](https://msdn.microsoft.com/library/windows/apps/br230467)  
[showPopup](https://msdn.microsoft.com/library/windows/apps/br230468)  
[swipeDeselect](https://msdn.microsoft.com/library/windows/apps/br212662)  
[swipeReveal](https://msdn.microsoft.com/library/windows/apps/br212663)  
[swipeSelect](https://msdn.microsoft.com/library/windows/apps/br212664)  
[updateBadge](https://msdn.microsoft.com/library/windows/apps/br230471)  

## System requirements

**Client:** Windows 10

**Server:** Windows Server 2016 Technical Preview

**Phone:** Windows 10

## Build the sample

1. If you download the samples ZIP, be sure to unzip the entire archive, not just the folder with the sample you want to build. 
2. Start Microsoft Visual Studio 2017 and select **File** \> **Open** \> **Project/Solution**.
3. Starting in the folder where you unzipped the samples, go to the Samples subfolder, then the subfolder for this specific sample, then the subfolder for your preferred language (C++, C#, or JavaScript). Double-click the Visual Studio Solution (.sln) file.
4. Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.

## Run the sample

The next steps depend on whether you just want to deploy the sample or you want to both deploy and run it.

### Deploying the sample

- Select Build > Deploy Solution. 

### Deploying and running the sample

- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or select Debug > Start Without Debugging. 
