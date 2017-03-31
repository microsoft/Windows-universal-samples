using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using Windows.UI;
using Windows.UI.Xaml;

namespace xBindSampleModel
{
    public sealed class DataModel : DependencyObject, INotifyPropertyChanged
    {
        public DataModel()
        {
            InitializeValues();
        }

        public string StringPropNoINPC { get; set; }
        public string StringPropAsInteger { get; set; }
        public int IntPropNoINPC { get; set; }
        public IEmployee EmployeePropNoINPC { get; set; }

        private string _StringPropWithINPC;
        public string StringPropWithINPC
        {
            get { return _StringPropWithINPC; }
            set
            {
                if (value != _StringPropWithINPC)
                {
                    _StringPropWithINPC = value;
                    NotifyPropertyChanged();
                }
            }
        }

        private int _IntPropWithINPC;
        public int IntPropWithINPC
        {
            get { return _IntPropWithINPC; }
            set
            {
                if (value != _IntPropWithINPC)
                {
                    _IntPropWithINPC = value;
                    NotifyPropertyChanged();
                }
            }
        }

        private Color _ColorPropWithINPC;
        public Color ColorPropWithINPC
        {
            get { return _ColorPropWithINPC; }
            set
            {
                if (value != _ColorPropWithINPC)
                {
                    _ColorPropWithINPC = value;
                    NotifyPropertyChanged();
                }
            }
        }

        private IEmployee _EmployeePropWithINPC;
        public IEmployee EmployeePropWithINPC
        {
            get { return _EmployeePropWithINPC; }
            set
            {
                if (value != _EmployeePropWithINPC)
                {
                    _EmployeePropWithINPC = value;
                    NotifyPropertyChanged();
                }
            }
        }

        private EmployeeCollection _Employees;
        public EmployeeCollection Employees
        {
            get { return _Employees; }
            set
            {
                if (value != _Employees)
                {
                    _Employees = value;
                    NotifyPropertyChanged();
                }
            }
        }

        private Manager _ManagerProp;
        public Manager ManagerProp
        {
            get { return _ManagerProp; }
            set
            {
                if (value != _ManagerProp)
                {
                    _ManagerProp = value;
                    NotifyPropertyChanged();
                }
            }
        }

        private bool _BoolPropWithINPC;
        public bool BoolPropWithINPC
        {
            get { return _BoolPropWithINPC; }
            set
            {
                if (value != _BoolPropWithINPC)
                {
                    _BoolPropWithINPC = value;
                    NotifyPropertyChanged();
                }
            }
        }

        public int IntPropertyDP
        {
            get { return (int)GetValue(IntPropertyDPProperty); }
            set { SetValue(IntPropertyDPProperty, value); }
        }
        #region IntPropertyDP DP
        private const string IntPropertyDPName = "IntPropertyDP";
        private static readonly DependencyProperty _IntPropertyDPProperty =
            DependencyProperty.Register(IntPropertyDPName, typeof(int), typeof(DataModel), new PropertyMetadata(0));
        public static DependencyProperty IntPropertyDPProperty { get { return _IntPropertyDPProperty; } }
        #endregion

        public string StringPropertyDP
        {
            get { return (string)GetValue(StringPropertyDPProperty); }
            set { SetValue(StringPropertyDPProperty, value); }
        }
        #region StringPropertyDP DP
        private const string StringPropertyDPName = "StringPropertyDP";
        private static readonly DependencyProperty _StringPropertyDPProperty =
            DependencyProperty.Register(StringPropertyDPName, typeof(string), typeof(DataModel), new PropertyMetadata(""));
        public static DependencyProperty StringPropertyDPProperty { get { return _StringPropertyDPProperty; } }
        #endregion

        public IEmployee EmployeePropertyDP
        {
            get { return (IEmployee)GetValue(EmployeePropertyDPProperty); }
            set { SetValue(EmployeePropertyDPProperty, value); }
        }
        #region EmployeePropertyDP DP
        private const string EmployeePropertyDPName = "EmployeePropertyDP";
        private static readonly DependencyProperty _EmployeePropertyDPProperty =
            DependencyProperty.Register(EmployeePropertyDPName, typeof(IEmployee), typeof(DataModel), new PropertyMetadata(0));
        public static DependencyProperty EmployeePropertyDPProperty { get { return _EmployeePropertyDPProperty; } }

        #endregion


        public void InitializeValues()
        {
            ManagerProp = CreateManagerEmployeeTree();
            Employees = ManagerProp.ReportsOC;

            StringPropNoINPC = "String Property without INPC";
            StringPropWithINPC = "String Property with INPC";
            ColorPropWithINPC = Colors.Red;
            IntPropNoINPC = 42;
            IntPropWithINPC = 2;
            IntPropertyDP = 42;
            StringPropertyDP = "String Property as a DP";
            StringPropAsInteger = "65";
            EmployeePropNoINPC = Employees[0];
            EmployeePropWithINPC = Employees[1];

        }
        public void UpdateValues()
        {
            this.BoolPropWithINPC = !this.BoolPropWithINPC;
            this.IntPropNoINPC = IntPropNoINPC + 10 % 100;
            this.IntPropWithINPC = IntPropWithINPC + 1 % 10;
            this.IntPropertyDP = IntPropertyDP + 10 % 100;

            string name = ", " + lnames[(new Random()).Next(lnames.Length)];
            this.StringPropNoINPC += name;
            this.StringPropWithINPC += name;
            this.StringPropertyDP += name;

            EmployeePropWithINPC = Employees[IntPropWithINPC % Employees.Count];
            EmployeePropertyDP = Employees[(IntPropWithINPC + 1) % Employees.Count];
            // Swap Manager Reports [0], [2]
            var temp = ManagerProp.ReportsOC[0];
            ManagerProp.ReportsOC[0] = ManagerProp.ReportsOC[2];
            ManagerProp.ReportsOC[2] = temp;
        }


        #region Employee Tree Creation
        private static string[] fnames = { "Alice", "Brian", "Chris", "David", "Emily", "Freda", "Garry", "Harriet", "Isobel", "John", "Keith", "Lisa", "Mark", "Nathan", "Olivia", "Penelope", "Quentin", "Robert", "Simon", "Tom", "Veronica", "Will", "Xavier", "Yvette", "Zach" };
        private static string[] lnames = { "Smith", "Johnson", "Williams", "Jones", "Brown", "Davis", "Miller", "Wilson", "Moore", "Taylor", "Anderson", "Thomas", "Jackson", "White", "Harris", "Martin", "Thompson", "Garcia", "Martinez", "Robinson", "Clark", "Rodriguez", "Lewis", "Lee", "Walker" };

        public static Manager CreateManagerEmployeeTree()
        {
            Random rnd = new Random(4524); // Passing in a seed guarantees the same values each time
            Manager CEO = new Manager() { FirstName = fnames[rnd.Next(fnames.Length)], LastName = lnames[rnd.Next(lnames.Length)] };
            CreateReports(5, CEO, rnd);
            return CEO;
        }

        private static void CreateReports(int depth, Manager m, Random rnd)
        {
            int count = rnd.Next(10) + 3;
            for (int i = 0; i < count; i++)
            {
                int j = rnd.Next(10);
                IEmployee e = (j > 2 && depth > 1) ? (IEmployee)new Manager() : (IEmployee)new Employee();
                e.LastName = lnames[rnd.Next(lnames.Length)];
                e.FirstName = fnames[rnd.Next(fnames.Length)];
                e.DirectManager = m;
                m.ReportsList.Add(e);
                m.ReportsDict.Add(e.FirstName + i, e);
                if (e.IsManager) CreateReports(depth - 1, (Manager)e, rnd);
            }
        }

        #endregion
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
