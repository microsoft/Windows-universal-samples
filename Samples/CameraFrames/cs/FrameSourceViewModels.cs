//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.Devices.Enumeration;
using Windows.Media.Capture.Frames;
using Windows.UI.Core;

namespace SDKTemplate
{
    /// <summary>
    /// View model for MediaFrameSourceGroup used in XAML ContentControl.
    /// </summary>
    public class FrameSourceGroupModel
    {
        public string Id { get; }
        public string DisplayName { get; }
        public MediaFrameSourceGroup SourceGroup { get; }
        public IReadOnlyList<FrameSourceInfoModel> SourceInfos { get; }

        public FrameSourceGroupModel(MediaFrameSourceGroup group)
        {
            SourceGroup = group;
            this.Id = group.Id;
            this.DisplayName = group.DisplayName;
            this.SourceInfos = group.SourceInfos.Select(
                sourceInfo => new FrameSourceInfoModel(sourceInfo)).ToArray();
        }

        public override string ToString()
        {
            return this.DisplayName;
        }
    }

    /// <summary>
    /// View model for MediaFrameSourceInfo used in XAML ContentControl.
    /// </summary>
    public class FrameSourceInfoModel
    {
        public string DisplayName { get; }
        public string Id { get; }
        public MediaFrameSourceGroup SourceGroup { get; }
        public MediaFrameSourceInfo SourceInfo { get; }

        public FrameSourceInfoModel(MediaFrameSourceInfo sourceInfo)
        {
            this.SourceInfo = sourceInfo;
            this.SourceGroup = this.SourceInfo.SourceGroup;

            this.Id = sourceInfo.Id;

            var kind = sourceInfo.SourceKind.ToString();
            var type = sourceInfo.MediaStreamType.ToString();
            this.DisplayName = $"{kind} {type}";
        }

        public override string ToString()
        {
            return this.DisplayName;
        }
    }

    /// <summary>
    /// View model for MediaFrameFormat used in XAML ContentControl.
    /// </summary>
    public class FrameFormatModel
    {
        public MediaFrameFormat Format { get; }

        public string DisplayName { get; }

        public FrameFormatModel(MediaFrameFormat format)
        {
            this.Format = format;
            this.DisplayName = string.Format("{0} | {1} | {2} x {3} | {4:#.##}fps",
                format.MajorType,
                format.Subtype,
                format.VideoFormat?.Width,
                format.VideoFormat?.Height,
                Math.Round((double)format.FrameRate.Numerator / format.FrameRate.Denominator, 2));
        }

        public override string ToString()
        {
            return this.DisplayName;
        }

        /// <summary>
        /// Compares the Format contained by this view model to the given format for equivalency.
        /// </summary>
        /// <param name="otherFormat">The MediaFrameFormat to compare to the MediaFrameFormat in this view model.</param>
        /// <returns></returns>
        public bool HasSameFormat(MediaFrameFormat otherFormat)
        {
            if (otherFormat == null)
            {
                return (Format == null);
            }

            return this.Format.MajorType == otherFormat.MajorType &&
                this.Format.Subtype == otherFormat.Subtype &&
                this.Format.FrameRate.Numerator == otherFormat.FrameRate.Numerator &&
                this.Format.FrameRate.Denominator == otherFormat.FrameRate.Denominator &&
                this.Format.VideoFormat?.Width == otherFormat.VideoFormat?.Width &&
                this.Format.VideoFormat?.Height == otherFormat.VideoFormat?.Height;
        }
    }

    /// <summary>
    /// View model for a collection of MediaFrameSourceGroup used in XAML ItemsControl.
    /// </summary>
    public class SourceGroupCollection : IDisposable
    {
        private CoreDispatcher _dispatcher;
        private DeviceWatcher _watcher;
        private ObservableCollection<FrameSourceGroupModel> _sourceCollection;

        public SourceGroupCollection(CoreDispatcher uiDispatcher)
        {
            _dispatcher = uiDispatcher;
            _sourceCollection = new ObservableCollection<FrameSourceGroupModel>();

            // Only listen to devices with type of MediaFrameSourceGroup
            var deviceSelector = MediaFrameSourceGroup.GetDeviceSelector();
            _watcher = DeviceInformation.CreateWatcher(deviceSelector);
            _watcher.Added += Watcher_Added;
            _watcher.Removed += Watcher_Removed;
            _watcher.Updated += Watcher_Updated;
            _watcher.Start();
        }

        public void Dispose()
        {
            _watcher.Stop();
            _watcher.Updated -= Watcher_Updated;
            _watcher.Removed -= Watcher_Removed;
            _watcher.Added -= Watcher_Added;
        }

        public IReadOnlyList<FrameSourceGroupModel> FrameSourceGroups
        {
            get { return _sourceCollection; }
        }

        /// <summary>
        /// Updates a device when a change occurs.
        /// </summary>
        private async void Watcher_Updated(DeviceWatcher sender, DeviceInformationUpdate args)
        {
            await RemoveDevice(args.Id);
            await AddDeviceAsync(args.Id);
        }

        /// <summary>
        /// Removes a device from the collection when one disconnected.
        /// </summary>
        private async void Watcher_Removed(DeviceWatcher sender, DeviceInformationUpdate args)
        {
            await RemoveDevice(args.Id);
        }

        /// <summary>
        /// Adds a device to the collection when one connected.
        /// </summary>
        private async void Watcher_Added(DeviceWatcher sender, DeviceInformation args)
        {
            await AddDeviceAsync(args.Id);
        }

        /// <summary>
        /// Adds a SourceGroup with given Id to the collection.
        /// </summary>
        private async Task AddDeviceAsync(string id)
        {
            var group = await MediaFrameSourceGroup.FromIdAsync(id);
            if (group != null)
            {
                await _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        _sourceCollection.Add(new FrameSourceGroupModel(group));
                    });
            }
        }

        /// <summary>
        /// Removes a SourceGroup with given id from the collection if it exists
        /// </summary>
        private async Task RemoveDevice(string id)
        {
            await _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    var existing = _sourceCollection.FirstOrDefault(item => item.Id == id);
                    if (existing != null)
                    {
                        _sourceCollection.Remove(existing);
                    }
                });
        }
    }
}
