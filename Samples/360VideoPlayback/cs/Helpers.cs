using System.Numerics;

namespace _360VideoPlayback
{
    class Helpers
    {
        static public Quaternion CreateFromHeadingPitchRoll(double heading, double pitch, double roll)
        {
            Quaternion result;
            double headingPart = DegreetoRadians(heading) * 0.5;
            double sin1 = System.Math.Sin(headingPart);
            double cos1 = System.Math.Cos(headingPart);

            double pitchPart = DegreetoRadians(-pitch) * 0.5;
            double sin2 = System.Math.Sin(pitchPart);
            double cos2 = System.Math.Cos(pitchPart);

            double rollPart = DegreetoRadians(roll) * 0.5;
            double sin3 = System.Math.Sin(rollPart);
            double cos3 = System.Math.Cos(rollPart);

            result.W = (float)(cos1 * cos2 * cos3 - sin1 * sin2 * sin3);
            result.X = (float)(cos1 * cos2 * sin3 + sin1 * sin2 * cos3);
            result.Y = (float)(sin1 * cos2 * cos3 + cos1 * sin2 * sin3);
            result.Z = (float)(cos1 * sin2 * cos3 - sin1 * cos2 * sin3);

            return result;
        }

        static private double DegreetoRadians(double degree)
        {
            return degree * System.Math.PI / 180.0;
        }

    }
}
