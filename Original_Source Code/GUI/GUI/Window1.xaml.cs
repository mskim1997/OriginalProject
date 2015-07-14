using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Control;

namespace GUI
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        private Controller control;

        public Window1()
        {
            InitializeComponent();
            this.control = new Controller();
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            this.control.Close();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            GridView myGridView = new GridView();
            myGridView.AllowsColumnReorder = true;

            //Collapse the header
            Style collapsedHeaderStyle = new Style();
            Setter collapsedHeaderSetter = new Setter();

            collapsedHeaderSetter.Property = VisibilityProperty;
            collapsedHeaderSetter.Value = Visibility.Collapsed;
            collapsedHeaderStyle.Setters.Add(collapsedHeaderSetter);

            myGridView.ColumnHeaderContainerStyle = collapsedHeaderStyle;

            for (int i = 0; i < control.XSize; i++)
            {
                GridViewColumn col = new GridViewColumn();
                col.DisplayMemberBinding = new Binding("Column[" + i.ToString() + "]");
                col.Width = 30;
                myGridView.Columns.Add(col);
            }

            listView.View = myGridView;
            listView.DataContext = control.AgentCollection;
        }
    }
}
