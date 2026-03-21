using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows.Input;
using ToolTest.Command.HierarchyContents;

namespace ToolTest;

public class ToolViewModel : INotifyPropertyChanged
{
    public ObservableCollection<Category> Hierarchy { get; set; } = [];
    public event Action OnValueChanged;

    public event PropertyChangedEventHandler? PropertyChanged;
    public virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
    {
        if (this.PropertyChanged != null)
            this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        OnValueChanged.Invoke();
    }

    public AddCategoryCommand AddCategory { get; private set; }
    public RemoveCategoryCommand RemoveCategory { get; private set; }
    public AddItemCommand AddItem { get; private set; }
    public RemoveItemCommand RemoveItem { get; private set; }

    public ToolViewModel()
    {
        Hierarchy.Add(new() { Name = "Category1" });
        Hierarchy.Add(new() { Name = "Category2" });
        Hierarchy[0].Items.Add(new(Hierarchy[0].Items) { Name = "Item1", IsTime = true });
        Hierarchy[0].Items.Add(new(Hierarchy[0].Items) { Name = "Item2", IsTime = true });
        Hierarchy[1].Items.Add(new(Hierarchy[1].Items) { Name = "Item3", IsTime = true });
        Hierarchy[1].Items.Add(new(Hierarchy[1].Items) { Name = "Item4", IsTime = true });

        AddCategory = new(this);
        RemoveCategory = new(this);
        AddItem = new(this);
        RemoveItem = new(this);
    }

    public ObservableCollection<Category> Value
    {
        get
        {
            return Hierarchy;
        }
        set
        {
            Hierarchy = value;
            OnPropertyChanged();
        }
    }

    public void MoveItem_FromBorder(Item source, Item target)
    {
        if (target.Parent != null)
        {
            if (source.Parent == target.Parent)
            {
                var items = target.Parent;
                int oldIndex = items.IndexOf(source);
                int newIndex = items.IndexOf(target);
                source.Parent.Move(oldIndex, newIndex);
            }
            else
            {
                var items = target.Parent;
                int oldIndex = items.IndexOf(target);
                items.Insert(oldIndex, source);

                source.Parent?.Remove(source);
                source.Parent = target.Items;
            }
        }

        OnPropertyChanged();
    }

    public void MoveItem_FromItem(Item source, Item target)
    {
        source.Parent?.Remove(source);
        source.Parent = target.Items;

        target.Items.Add(source);
        OnPropertyChanged();
    }

    public void MoveCategory(Item sItem, Item tItem)
    {
        if (sItem.Parent != null)
        {
            var pItems = sItem.Parent;
            var index = pItems.IndexOf(sItem);
            pItems.Insert(index, tItem);
        }

        sItem.Parent?.Remove(sItem);
        sItem.Parent = tItem.Items;

        OnPropertyChanged();
    }

    /*
    class DragDropHelper<T>where T : class
    {
        private void MouseMove(object sender, MouseEventArgs e)
        {
            EditableTextBox? textBox = sender as EditableTextBox;
            if (textBox != null && e.LeftButton == MouseButtonState.Pressed)
            {
                DragDrop.DoDragDrop(textBox,
                                     textBox.Data,
                                     DragDropEffects.Copy);
            }
        }

        private T? _previousData;
        private void DragEnter(object sender, DragEventArgs e)
        {
            EditableTextBox? textBox = sender as EditableTextBox;
            if (textBox != null)
            {
                // Save the current Fill brush so that you can revert back to this value in DragLeave.
                _previousData = textBox.Data as T;

                if (_previousData != null)
                {
                    // If the DataObject contains string data, extract it.
                    if (e.Data.GetDataPresent(DataFormats.StringFormat))
                    {
                        T data = (T)e.Data.GetData(DataFormats.PenData);
                        textBox.Data = data;
                    }
                }
            }
        }

        private void DragOver(object sender, DragEventArgs e)
        {
            e.Effects = DragDropEffects.None;
            if (e.Data.GetDataPresent(DataFormats.PenData))
            {
                T category = (T)e.Data.GetData(DataFormats.PenData);
                e.Effects = DragDropEffects.Copy | DragDropEffects.Move;
            }
        }

        private void DragLeave(object sender, DragEventArgs e)
        {
            EditableTextBox? textBox = sender as EditableTextBox;
            if (textBox != null)
            {
                textBox.Data = _previousData;
            }
        }

        private void Drop(object sender, DragEventArgs e)
        {
            EditableTextBox? textBox = sender as EditableTextBox;
            if (textBox != null)
            {
                // If the DataObject contains string data, extract it.
                if (e.Data.GetDataPresent(DataFormats.PenData))
                {
                    T data = (T)e.Data.GetData(DataFormats.PenData);
                    textBox.Data = data;
                }
            }
        }
    }
     */
}


public class MoveCategoryCommand : ICommand
{
    private ToolViewModel _view { get; set; }

    public MoveCategoryCommand(ToolViewModel a_view)
    {
        _view = a_view;
    }

    public event EventHandler? CanExecuteChanged
    {
        add { CommandManager.RequerySuggested += value; }
        remove { CommandManager.RequerySuggested -= value; }
    }
    public bool CanExecute(object? parameter) => true;
    public void Execute(object? parameter)
    {

    }
}




public class MoveItemCommand : ICommand
{
    private ToolViewModel _view { get; set; }

    public MoveItemCommand(ToolViewModel a_view)
    {
        _view = a_view;
    }

    public event EventHandler? CanExecuteChanged
    {
        add { CommandManager.RequerySuggested += value; }
        remove { CommandManager.RequerySuggested -= value; }
    }
    public bool CanExecute(object? parameter) => true;
    public void Execute(object? parameter)
    {

    }
}
