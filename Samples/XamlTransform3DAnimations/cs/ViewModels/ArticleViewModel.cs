using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;

namespace Transform3DAnimations.ViewModels
{
    public class ArticleViewModel
    {
        private static Random s_random = new Random();
        private string _headline;
        private Uri _imageUri;

        public string Headline
        {
            get { return _headline; }
            set { _headline = value; }
        }

        public ImageSource ImageSource
        {
            get { return new BitmapImage(_imageUri); }
        }

        public string FullText
        {
            get
            {
                return @"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam sed nibh ac nisl porta ornare. Vivamus a mollis leo. Proin eu leo at justo aliquam porttitor id nec arcu. Aenean ornare ligula a orci vulputate, sit amet hendrerit leo commodo. Suspendisse interdum porta ante, sit amet tristique enim fringilla sed. Fusce pulvinar eget sem nec iaculis. In euismod malesuada sem eget dignissim. Nulla interdum vulputate mi vel tempor. Aliquam tempor efficitur mi at dictum. 

Vestibulum sollicitudin arcu at elementum tempor. Sed suscipit luctus massa, et cursus arcu malesuada vitae. Donec venenatis elit ex, quis iaculis elit vehicula aliquam. Suspendisse sed purus nisl. Aliquam aliquet leo sed erat ultricies consequat. Aliquam ac pellentesque urna, id fringilla sapien. Fusce imperdiet nisl nec lectus placerat sagittis. Sed nibh ipsum, cursus at ultrices molestie, ornare id augue. Quisque augue tortor, rutrum et sodales vestibulum, elementum ut nunc. Nunc eu nunc a nisi laoreet laoreet. Nunc nec diam non eros tempor tristique. Maecenas a sollicitudin felis. In tortor nisi, bibendum non lectus id, malesuada tincidunt nibh. Ut consequat elit nulla, vitae maximus odio ornare a. 

Praesent placerat velit vel eleifend tristique. Sed vulputate aliquet risus, ac ultrices sapien volutpat in. Donec blandit congue dictum. In in sapien id augue elementum condimentum. Pellentesque maximus felis eu efficitur convallis. Vestibulum augue leo, elementum ac urna at, luctus finibus leo. Sed venenatis augue et laoreet vehicula. Nulla facilisi. Nulla eu mauris nec augue congue semper vel in ipsum. Aenean gravida tempus lobortis. Sed sodales ac lectus pellentesque tempor. Nulla ut ipsum sit amet magna euismod porttitor condimentum fringilla urna. Curabitur sit amet hendrerit metus. Proin gravida accumsan urna, ut porta est placerat efficitur. Nullam iaculis libero ex, ac accumsan tortor placerat at. 
";
            }
        }

        public ArticleViewModel()
        {
            _headline = new[]
                {
                    "Lorem Ipsum",
                    "Dolor Sit",
                    "Consectetus Adipiscing",
                    "Etiam Sed Nibh",
                    "Vivamus a Mollis Leo",
                    "Vestibulum Sollicitudin Arcu",
                    "Sed Suscipit Luctus",
                    "Arcu Malesuada Vitae",
                    "Sed Vulputate Aliquet",
                    "Donec Blandit Congue Dictum"
                }[s_random.Next(10)];

            var imagePath = "Images/circle_image" + (s_random.Next(3) + 1).ToString() + ".jpg";
            _imageUri = new Uri("ms-appx:///" + imagePath);
        }
    }
}
