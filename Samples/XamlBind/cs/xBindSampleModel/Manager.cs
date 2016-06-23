using System;
using System.Collections.Generic;
using System.Linq;
using Windows.UI.Xaml.Data;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Collections.ObjectModel;

using Windows.Foundation.Collections;
using System.Collections;
using System.Collections.Specialized;
using Windows.UI.Xaml;
using Windows.UI.Popups;

namespace xBindSampleModel
{
    public sealed class Manager : IEmployee, INotifyPropertyChanged
    {
        private Employee _employee;
        private EmployeeCollection _reports;
        private EmployeeDictionary _reportsDictionary;

        public Manager()
        {
            _employee = new Employee();
            _reports = new EmployeeCollection();
            _reportsDictionary = new EmployeeDictionary();
        }

        public IEnumerable<IEmployee> ReportsEnum { get { return _reports; } }
        public IList<IEmployee> ReportsList { get { return _reports; } }
        public EmployeeCollection ReportsOC { get { return _reports; } }
                public EmployeeDictionary ReportsDict {  get { return _reportsDictionary; } }

        public string NullStringProperty { get; set; }
        public Windows.UI.Xaml.Media.ImageSource NullImageSource
        {
            get { return null; }
            set { }
        }

        public bool Visibility
        {
            get { return _employee.Visibility; }
            set
            {
                if (_employee.Visibility != value)
                {
                    _employee.Visibility = value;
                    NotifyPropertyChanged("Visibility");
                }
            }
        }
        public string FirstName
        {
            get { return _employee.FirstName; }
            set
            {
                if (value != _employee.FirstName)
                {
                    _employee.FirstName = value;
                    NotifyPropertyChanged();
                    NotifyPropertyChanged("Name");
                }
            }
        }

        public string LastName
        {
            get { return _employee.LastName; }
            set
            {
                if (value != _employee.LastName)
                {
                    _employee.LastName = value;
                    NotifyPropertyChanged();
                    NotifyPropertyChanged("Name");
                }
            }
        }

        public Manager DirectManager
        {
            get { return _employee.DirectManager; }
            set
            {
                if (value != _employee.DirectManager)
                {
                    _employee.DirectManager = value;
                    NotifyPropertyChanged();
                }
            }
        }

        public void Poke(object sender, RoutedEventArgs e)
        {
            MessageDialog dlg = new MessageDialog("Employees of " + Name + " have been poked.");
            var t = dlg.ShowAsync();
        }

        public bool IsManager { get { return true; } }

        public String Title
        {
            get
            {
                return "Manager";
            }
        }

        public string Name { get { return FirstName + " " + LastName; } }

        public string FullName() {  return FirstName + " " + LastName; }

        #region INPC

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged([CallerMemberName] string propertyName = "")
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
        #endregion
    }
}
