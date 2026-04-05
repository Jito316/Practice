using System.Windows.Input;

namespace ToolTest.Command.HierarchyContents;

public class HierarchyContentsCommnad(ToolViewModel a_view) : ICommand
{
    protected ToolViewModel _view = a_view;
    protected virtual void ItemExecute(Item parameter) { }
    protected virtual void CategoryExecute(Category parameter) { }

    private enum Type { None, Item, Category }
    private Type type = Type.None;

    public event EventHandler? CanExecuteChanged
    {
        add { CommandManager.RequerySuggested += value; }
        remove { CommandManager.RequerySuggested -= value; }
    }
    public bool CanExecute(object? parameter)
    {
        if (parameter is Item) type = Type.Item;
        if (parameter is Category) type = Type.Category;
        return type != Type.None;
    }
    public void Execute(object? parameter)
    {
        if (parameter == null) return;

        switch (type)
        {
            case Type.Item:
                ItemExecute((Item)parameter);
                break;
            case Type.Category:
                CategoryExecute((Category)parameter);
                break;
        }
    }
}