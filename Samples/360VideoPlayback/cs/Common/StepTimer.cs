using System;
using System.Diagnostics;

namespace _360VideoPlayback.Common
{
    /// <summary>
    /// Helper class for animation and simulation timing.
    /// </summary>
    internal class StepTimer
    {
        // Integer format represents time using 10,000,000 ticks per second.
        private const long TicksPerSecond = 10000000;

        // Source timing data uses QPC units.
        private long qpcFrequency;
        private long qpcLastTime;
        private long qpcMaxDelta;

        // Derived timing data uses a canonical tick format.
        private long leftOverTicks = 0;

        // Members for tracking the framerate.
        private int framesThisSecond = 0;
        private long qpcSecondCounter = 0;

        public StepTimer()
        {
            qpcFrequency = Stopwatch.Frequency;
            qpcLastTime = Stopwatch.GetTimestamp();

            // Initialize max delta to 1/10 of a second.
            qpcMaxDelta = qpcFrequency / 10;
        }

        /// <summary>
        /// After an intentional timing discontinuity (for instance a blocking IO operation)
        /// call this to avoid having the fixed timestep logic attempt a set of catch-up 
        /// Update calls.
        /// </summary>
        public void ResetElapsedTime()
        {
            qpcLastTime = Stopwatch.GetTimestamp();

            leftOverTicks = 0;
            FramesPerSecond = 0;
            framesThisSecond = 0;
            qpcSecondCounter = 0;
        }

        /// <summary>
        /// Get elapsed time since the previous Update call.
        /// </summary>
        public long ElapsedTicks { get; private set; } = 0;

        /// <summary>
        /// Get elapsed time since the previous Update call.
        /// </summary>
        public double ElapsedSeconds => TicksToSeconds(ElapsedTicks);

        /// <summary>
        /// Get total time since the start of the program.
        /// </summary>
        public long TotalTicks { get; private set; } = 0;

        /// <summary>
        /// Get total time since the start of the program.
        /// </summary>
        public double TotalSeconds => TicksToSeconds(TotalTicks);

        /// <summary>
        /// Get total number of updates since start of the program.
        /// </summary>
        public int FrameCount { get; private set; } = 0;

        /// <summary>
        /// Get the current framerate.
        /// </summary>
        public int FramesPerSecond { get; private set; } = 0;

        /// <summary>
        /// Get/Set whether to use fixed or variable timestep mode.
        /// </summary>
        public bool IsFixedTimeStep { get; set; } = false;

        /// <summary>
        /// Get/Set how often to call Update when in fixed timestep mode.
        /// </summary>
        public long TargetElapsedTicks { get; set; } = TicksPerSecond / 60;

        /// <summary>
        /// Get/Set how often to call Update when in fixed timestep mode.
        /// </summary>
        public double TargetElapsedSeconds
        {
            get { return TicksToSeconds(TargetElapsedTicks); }
            set { TargetElapsedTicks = SecondsToTicks(value); }
        }

        /// <summary>
        /// // Update timer state, calling the specified Update function the appropriate number of times.
        /// </summary>
        public void Tick(Action update)
        {
            // Query the current time.
            long currentTime = Stopwatch.GetTimestamp();

            long timeDelta = currentTime - qpcLastTime;

            qpcLastTime = currentTime;
            qpcSecondCounter += timeDelta;

            // Clamp excessively large time deltas (e.g. after paused in the debugger).
            if (timeDelta > qpcMaxDelta)
            {
                timeDelta = qpcMaxDelta;
            }

            // Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
            timeDelta *= TicksPerSecond;
            timeDelta /= qpcFrequency;

            long lastFrameCount = FrameCount;

            if (IsFixedTimeStep)
            {
                // Fixed timestep update logic

                // If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
                // the clock to exactly match the target value. This prevents tiny and irrelevant errors
                // from accumulating over time. Without this clamping, a game that requested a 60 fps
                // fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
                // accumulate enough tiny errors that it would drop a frame. It is better to just round 
                // small deviations down to zero to leave things running smoothly.

                if (Math.Abs(timeDelta - TargetElapsedTicks) < (TicksPerSecond / 4000))
                {
                    timeDelta = TargetElapsedTicks;
                }

                leftOverTicks += timeDelta;

                while (leftOverTicks >= TargetElapsedTicks)
                {
                    ElapsedTicks = TargetElapsedTicks;
                    TotalTicks += TargetElapsedTicks;
                    leftOverTicks -= TargetElapsedTicks;
                    FrameCount++;

                    update();
                }
            }
            else
            {
                // Variable timestep update logic.
                ElapsedTicks = timeDelta;
                TotalTicks += timeDelta;
                leftOverTicks = 0;
                FrameCount++;

                update();
            }

            // Track the current framerate.
            if (FrameCount != lastFrameCount)
            {
                framesThisSecond++;
            }

            if (qpcSecondCounter >= qpcFrequency)
            {
                FramesPerSecond = framesThisSecond;
                framesThisSecond = 0;
                qpcSecondCounter %= qpcFrequency;
            }
        }

        private static double TicksToSeconds(long ticks)
        {
            return (double)ticks / TicksPerSecond;
        }

        private static long SecondsToTicks(double seconds)
        {
            return (long)(seconds * TicksPerSecond);
        }
    }
}
