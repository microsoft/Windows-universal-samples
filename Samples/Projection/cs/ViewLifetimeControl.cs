//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

// The objects defined here demonstrate how to make sure each of the views created remains alive as long as 
// the app needs them, but only when they're being used by the app or the user. Many of the scenarios contained in this
// sample use these functions to keep track of the views available and ensure that the view is not closed while
// the scenario is attempting to show it.
//
// As you can see in scenario 1, the ApplicationViewSwitcher.TryShowAsStandaloneAsync and 
// ProjectionManager.StartProjectingAsync methods let you show one view next to another. The Consolidated event
// is fired when a view stops being visible separately from other views. Common cases where this will occur
// is when the view falls out of the list of recently used apps, or when the user performs the close gesture on the view.
// This is a good time to close the view, provided the app isn't trying to show the view at the same time. This event
// is fired on the thread of the view that becomes consolidated.
//
// Each view lives on its own thread, so concurrency control is necessary. Also, as you'll see in the sample,
// certain objects may be bound to UI on given threads. Properties of those objects should only be updated
// on that UI thread.

using System;
using System.ComponentModel;
using Windows.UI.Core;
using Windows.UI.ViewManagement;

namespace SecondaryViewsHelpers
{
    // A custom event that fires whenever the secondary view is ready to be closed. You should
    // clean up any state (including deregistering for events) then close the window in this handler
    public delegate void ViewReleasedHandler(Object sender, EventArgs e);

    // A ViewLifetimeControl is instantiated for every secondary view. ViewLifetimeControl's reference count
    // keeps track of when the secondary view thinks it's in usem and when the main view is interacting with the secondary view (about to show
    // it to the user, etc.) When the reference count drops to zero, the secondary view is closed.
    public sealed class ViewLifetimeControl : INotifyPropertyChanged
    {
        // Dispatcher for this view. Kept here for sending messages between this view and the main view.
        CoreDispatcher dispatcher;

        // Window for this particular view. Used to register and unregister for events
        CoreWindow window;

        // The title for the view shown in the list of recently used apps (by setting the title on 
        // ApplicationView)
        string title;

        // This class uses references counts to make sure the secondary views isn't closed prematurely.
        // Whenever the main view is about to interact with the secondary view, it should take a reference
        // by calling "StartViewInUse" on this object. When finished interacting, it should release the reference
        // by calling "StopViewInUse". You can see examples of this throughout the sample, especially in
        // scenario 1.
        int refCount = 0;

        // Each view has a unique Id, found using the ApplicationView.Id property or
        // ApplicationView.GetApplicationViewIdForCoreWindow method. This id is used in all of the ApplicationViewSwitcher
        // and ProjectionManager APIs. 
        int viewId;

        // Tracks if this ViewLifetimeControl object is still valid. If this is true, then the view is in the process
        // of closing itself down
        bool released = false;

        // Keeps track of if the consolidated event has fired yet. A view is consolidated with other views
        // when there's no way for the user to get to it (it's not in the list of recently used apps, cannot be
        // launched from Start, etc.) A view stops being consolidated when it's visible--at that point
        // the user can interact with it, move it on or off screen, etc. 
        bool consolidated = true;

        // Tracks whether the projected view has been made visible for the first time yet. If it has not yet been shown for the
        // first time yet, we should not signal for it to be released yet. This protects against the fact that visibilityChanged
        // is not guaranteed to fire until after the StartProjectingAsync task has completed, so refCount of the ViewLifeTimeControl
        // may temporarily drop to 0 until the visibilityChanged event fires for the projected view first becoming visible.
        bool madeVisible = false;

        // Used to store pubicly registered events under the protection of a lock
        event ViewReleasedHandler InternalReleased;

        // Instantiate views using "CreateForCurrentView"
        private ViewLifetimeControl(CoreWindow newWindow)
        {
            dispatcher = newWindow.Dispatcher;
            window = newWindow;
            viewId = ApplicationView.GetApplicationViewIdForWindow(window);

            // This class will automatically tell the view when its time to close
            // or stay alive in a few cases
            RegisterForEvents();
        }

        // Register for events on the current view
        private void RegisterForEvents()
        {
            // A view is consolidated with other views hen there's no way for the user to get to it (it's not in the list of recently used apps, cannot be
            // launched from Start, etc.) A view stops being consolidated when it's visible--at that point the user can interact with it, move it on or off screen, etc. 
            // It's generally a good idea to close a view after it has been consolidated, but keep it open while it's visible.
            ApplicationView.GetForCurrentView().Consolidated += ViewConsolidated;
            window.VisibilityChanged += VisibilityChanged;
        }

        // Unregister for events. Call this method before closing the view to prevent leaks.
        private void UnregisterForEvents()
        {
            ApplicationView.GetForCurrentView().Consolidated -= ViewConsolidated;
            window.VisibilityChanged -= VisibilityChanged;
        }

        private void VisibilityChanged(object sender, VisibilityChangedEventArgs e)
        {

            if (e.Visible)
            {
                // A view is consolidated with other views hen there's no way for the user to get to it (it's not in the list of recently used apps, cannot be
                // launched from Start, etc.) A view stops being consolidated when it's visible--at that point the user can interact with it, move it on or off screen, etc. 
                // It's generally a good idea to close a view after it has been consolidated, but keep it open while it's visible.
                Consolidated = false;
            }
        }

        // A view is consolidated with other views hen there's no way for the user to get to it (it's not in the list of recently used apps, cannot be
        // launched from Start, etc.) A view stops being consolidated when it's visible--at that point the user can interact with it, move it on or off screen, etc. 
        // It's generally a good idea to close a view after it has been consolidated, but keep it open while it's visible.
        private void ViewConsolidated(ApplicationView sender, ApplicationViewConsolidatedEventArgs e)
        {
            Consolidated = true;
        }

        // Called when a view has been "consolidated" (no longer accessible to the user) 
        // and no other view is trying to interact with it. This should only be closed after the reference
        // count goes to 0 (including being consolidated). At the end of this, the view is closed. 
        private void FinalizeRelease()
        {
            bool justReleased = false;
            lock (this)
            {
                if (refCount == 0)
                {
                    justReleased = true;
                    released = true;
                }
            }

            // This assumes that released will never be made false after it
            // it has been set to true
            if (justReleased)
            {
                UnregisterForEvents();
                InternalReleased(this, null);
            }
        }

        // Creates ViewLifetimeControl for the particular view.
        // Only do this once per view.
        public static ViewLifetimeControl CreateForCurrentView()
        {
            return new ViewLifetimeControl(CoreWindow.GetForCurrentThread());
        }

        // For purposes of this sample, the collection of views
        // is bound to a UI collection. This property is available for binding
        public string Title
        {
            get
            {
                return title;
            }
            set
            {
                if (title != value)
                {
                    title = value;
                    if (PropertyChanged != null)
                    {
                        PropertyChanged(this, new PropertyChangedEventArgs("Title"));
                    }
                }
            }
        }

        // Necessary to communicate with the window
        public CoreDispatcher Dispatcher
        {
            get
            {
                // This property never changes, so there's no need to lock
                return dispatcher;
            }
        }

        // Each view has a unique Id, found using the ApplicationView.Id property or
        // ApplicationView.GetApplicationViewIdForCoreWindow method. This id is used in all of the ApplicationViewSwitcher
        // and ProjectionManager APIs. 
        public int Id
        {
            get
            {
                // This property never changes, so there's no need to lock
                return viewId;
            }
        }

        // Keeps track of if the consolidated event has fired yet. A view is consolidated with other views
        // when there's no way for the user to get to it (it's not in the list of recently used apps, cannot be
        // launched from Start, etc.) A view stops being consolidated when it's visible--at that point
        // the user can interact with it, move it on or off screen, etc. 
        public bool Consolidated
        {
            get
            {
                // This property should only be accessed by the thread on which the view lives, so there's no need to lock
                return consolidated;
            }
            set
            {
                if (consolidated != value)
                {
                    consolidated = value;
                    if (consolidated)
                    {
                        // The view isn't accessible to the user, so it's OK to close it.
                        StopViewInUse();
                    }
                    else
                    {
                        // The view has become visible, so do not close it until it's consolidated
                        StartViewInUse();
                        madeVisible = true;
                    }
                }
            }
        }

        // Signals that the view is being interacted with by another view,
        // so it shouldn't be closed even if it becomes "consolidated"
        public int StartViewInUse()
        {
            bool releasedCopy = false;
            int refCountCopy = 0;

            // This method is called from several different threads
            // (each view lives on its own thread)
            lock (this)
            {
                releasedCopy = this.released;
                if (!released)
                {
                    refCountCopy = ++refCount;
                }
            }

            if (releasedCopy)
            {
                throw new InvalidOperationException("This view is being disposed");
            }

            return refCountCopy;
        }

        // Should come after any call to StartViewInUse
        // Signals that the another view has finished interacting with the view tracked
        // by this object
        public int StopViewInUse()
        {
            int refCountCopy = 0;
            bool releasedCopy = false;

            lock (this)
            {
                releasedCopy = this.released;
                if (!released)
                {
                    refCountCopy = --refCount;
                    if ((refCountCopy == 0) && madeVisible)
                    {
                        dispatcher.RunAsync(CoreDispatcherPriority.Low, FinalizeRelease);
                    }
                }
            }

            if (releasedCopy)
            {
                throw new InvalidOperationException("This view is being disposed");
            }
            return refCountCopy;
        }

        // Signals to consumers that its time to close the view so that
        // they can clean up (including calling Window.Close() when finished)
        public event PropertyChangedEventHandler PropertyChanged;
        public event ViewReleasedHandler Released
        {
            add
            {
                bool releasedCopy = false;
                lock (this)
                {
                    releasedCopy = released;
                    if (!released)
                    {
                        InternalReleased += value;
                    }
                }

                if (releasedCopy)
                {
                    throw new InvalidOperationException("This view is being disposed");
                }
            }

            remove
            {
                lock (this)
                {
                    InternalReleased -= value;
                }
            }
        }
    }
}
