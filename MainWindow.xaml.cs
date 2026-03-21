
using System.Collections.ObjectModel;
using System.Windows;

namespace ToolTest;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();

        TextUpdate();

        var tool = (ToolViewModel)DataContext;
        tool.OnValueChanged += TextUpdate;
    }

    private void TextUpdate()
    {
        string text = "";
        var vm = (ToolViewModel)DataContext;

        foreach (var category in vm.Value)
        {
            text += "[" + category.Name + "]";
            text += "\n";

            ItemsToText(ref text, category.Items);
        }

        Data.Text = text;
    }
    private void ItemsToText(ref string text, ObservableCollection<Item> items, string header = "・", string tab = "")
    {
        foreach (var item in items)
        {
            text += tab + header + item.Name;

            if (item.Time.Length != 0)
            {
                text += "[" + item.Time + "h]";
            }

            text += "\n";

            ItemsToText(ref text, item.Items, "-", tab + "  ");
        }
    }

    private void Btn_Copy_TextCopy(object sender, RoutedEventArgs e) =>  Clipboard.SetData(DataFormats.Text, (Object)Data.Text);
    private void Btn_Update_TextUpdate(object sender, RoutedEventArgs e) => TextUpdate();
    
}