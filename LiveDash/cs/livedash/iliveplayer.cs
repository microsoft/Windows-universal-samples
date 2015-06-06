using System;
using System.Threading.Tasks;
using Windows.UI.Xaml.Controls;

namespace LiveDash
{
    interface ILivePlayer
    {
        Task Initialize(Uri manifest, MediaElement mediaElement);

        void GoToLive();

        void DesiredLiveOffset(TimeSpan offset);
    }
}
