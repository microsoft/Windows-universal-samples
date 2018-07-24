using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using System.ComponentModel;
using System.ComponentModel.DataAnnotations;
using System.Threading.Tasks;
using System.Text;
using Windows.UI.Xaml.Data;
using System.Runtime.CompilerServices;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    /// 

    //Class for the Page
    public sealed partial class InputValidationPage : Page
    {
        public InputValidationPage()
        {
            this.InitializeComponent();

            ViewModel = new ViewModel();
        }

        public ViewModel ViewModel { get; set; }
        private void mailvalidation_TextChanged(object sender, TextChangedEventArgs e)
        {
            WinformsValidate(sender, e);
        }

        private void WinformsValidate(object sender, TextChangedEventArgs e)
        {
            var foo = sender as TextBox;
            var ErrorsCollection = Windows.UI.Xaml.Data.Validation.GetErrors(foo);
            ErrorsCollection.Clear();
            Windows.UI.Xaml.Data.Validation.AddErrorHandler(foo, OnValidationError);

            if (!new EmailAddressAttribute().IsValid(foo.Text))
            {
                ErrorsCollection = Windows.UI.Xaml.Data.Validation.GetErrors(foo);
                ErrorsCollection.Add(new ValidationError("Email is not a valid email address"));
            }
        }
        private void OnValidationError(object sender, ValidationErrorEventArgs e)
        {

        }

    }

    //ViewModel
    public class ViewModel
    {
        public ViewModel()
        {
            Person = new Person();
        }

        public Person Person { get; set; }
    }

    public class Person : ValidationWrapper
    {
        [MinLength(4, ErrorMessage = "Name must be at least 4 characters")]
        [MaxLength(6, ErrorMessage = "Name must be no more than 6 characters")]
        public string Name
        {
            get { return GetValue<string>("Name"); }
            set { SetValue("Name", value); }
        }
    }

    //ValidationWrapper
    public class ValidationWrapper : NotifyDataErrorInfoBase, INotifyPropertyChanged
    {
        private readonly Dictionary<string, object> _values = new Dictionary<string, object>();

        public event PropertyChangedEventHandler PropertyChanged;

        #region get/set
        protected T GetValue<T>(string propertyName)
        {
            if (string.IsNullOrEmpty(propertyName))
            {
                throw new ArgumentException("Invalid property name", propertyName);
            }

            object value;
            if (!_values.TryGetValue(propertyName, out value))
            {
                value = default(T);
                _values.Add(propertyName, value);
            }
            return (T)value;
        }

        // Validate() gets called from inside SetValue
        protected void SetValue<T>(string propertyName, T value)
        {
            if (string.IsNullOrEmpty(propertyName))
            {
                throw new ArgumentException("Invalid property name", propertyName);
            }

            var current = GetValue<T>(propertyName);

            if (!value.Equals(current))
            {
                _values[propertyName] = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
                Validate(propertyName);
            }
        }
        #endregion

        #region Populate errors (C# - DataAnnotations)
        private Task Validate(string propertyName)
        {
            ClearErrors();


            return Task.Run(() =>
            {
                var val = GetPropertyValue(propertyName);
                var results = new ObservableCollection<ValidationResult>();
                var context = new ValidationContext(this, null, null)
                { MemberName = propertyName };

                // Validator is from the .Net System.ComponentModel.DataAnnotations namespace
                Validator.TryValidateProperty(val, context, results);

                foreach (var result in results)
                {
                    Errors[propertyName].Add(new ValidationError(result.ErrorMessage));
                }

                OnErrorsChanged(propertyName);
            });
        }
        #endregion

        #region privates
        private object GetPropertyValue([CallerMemberName] string propertyName = "")
        {
            return _values.ContainsKey(propertyName) ? _values[propertyName] : null;
        }

        #endregion
    }

    //NotifyDataErrorInfoBase
    public class NotifyDataErrorInfoBase : Windows.UI.Xaml.Data.INotifyDataErrorInfo
    {
        protected readonly Dictionary<string, ObservableCollection<ValidationError>> Errors;

        protected NotifyDataErrorInfoBase()
        {
            Errors = new Dictionary<string, ObservableCollection<ValidationError>>();
        }

        public bool HasErrors => Errors.Any();

        public event EventHandler<Windows.UI.Xaml.Data.DataErrorsChangedEventArgs> ErrorsChanged;

        IEnumerable<object> Windows.UI.Xaml.Data.INotifyDataErrorInfo.GetErrors(string propertyName)
        {
            return propertyName != null && Errors.ContainsKey(propertyName)
                ? Errors[propertyName] as IEnumerable<ValidationError>
                : new ObservableCollection<ValidationError>() as IEnumerable<ValidationError>;
        }

        protected virtual void OnErrorsChanged(string propertyName)
        {
            ErrorsChanged?.Invoke(this, new Windows.UI.Xaml.Data.DataErrorsChangedEventArgs(propertyName));
        }

        protected void ClearErrors()
        {
            foreach (var propertyName in Errors.Keys.ToList())
            {
                Errors.Remove(propertyName);
                OnErrorsChanged(propertyName);
            }
        }

        //IEnumerable<object> Windows.UI.Xaml.Data.INotifyDataErrorInfo.GetErrors(string propertyName)
        //{
        //    throw new NotImplementedException();
        //}
    }
}
