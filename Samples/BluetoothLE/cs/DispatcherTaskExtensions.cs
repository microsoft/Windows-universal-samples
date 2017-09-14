using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Core;

namespace SDKTemplate
{
    /// <summary>
    /// Helper class used to run functions on the UI thread
    /// </summary>
    public static class DispatcherService
    {
        /// <summary>
        /// Helper function to run task on UI thread
        /// </summary>
        /// <typeparam name="T">return value of task</typeparam>
        /// <param name="dispatcher"></param>
        /// <param name="func"></param>
        /// <param name="priority"></param>
        /// <returns>UI thread task</returns>
        public static async Task<T> RunTaskAsync<T>(
            this CoreDispatcher dispatcher,
            Func<Task<T>> func,
            CoreDispatcherPriority priority = CoreDispatcherPriority.Normal)
        {
            var taskCompletionSource = new TaskCompletionSource<T>();

            await dispatcher.RunAsync(
                priority,
                async () =>
                {
                    try
                    {
                        taskCompletionSource.SetResult(await func());
                    }
                    catch (Exception ex)
                    {
                        taskCompletionSource.SetException(ex);
                    }
                });
            return await taskCompletionSource.Task;
        }

        //// There is no TaskCompletionSource<void> so we use a bool that we throw away.

        /// <summary>
        /// Helper function to run task on UI thread
        /// </summary>
        /// <param name="dispatcher"></param>
        /// <param name="func"></param>
        /// <param name="priority"></param>
        /// <returns>UI thread task</returns>
        public static async Task RunTaskAsync(
            this CoreDispatcher dispatcher,
            Func<Task> func,
            CoreDispatcherPriority priority = CoreDispatcherPriority.Normal) =>
            await RunTaskAsync(
                dispatcher,
                async () =>
                {
                    await func();
                    return false;
                },
                priority);
    }
}
