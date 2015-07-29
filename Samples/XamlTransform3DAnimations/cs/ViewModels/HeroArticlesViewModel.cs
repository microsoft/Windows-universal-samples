using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml;

namespace Transform3DAnimations.ViewModels
{
    public class HeroArticlesViewModel : INotifyPropertyChanged
    {
        private ArticleViewModel _article0;
        public ArticleViewModel Article0
        {
            get
            {
                return _article0;
            }
            private set
            {
                _article0 = value;
                NotifyPropertyChanged("HeroArticle0");
            }
        }

        private ArticleViewModel _article1;

        public ArticleViewModel Article1
        {
            get
            {
                return _article1;
            }

            private set
            {
                _article1 = value;
                NotifyPropertyChanged("HeroArticle1");
            }
        }

        public HeroArticlesViewModel()
        {
            _article0 = new ArticleViewModel();
            _article1 = new ArticleViewModel();
        }

        private void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
    }
}
