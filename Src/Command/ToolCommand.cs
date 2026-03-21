using System.Windows.Input;

namespace ToolTest.Command;

public abstract class ToolCommand(ToolViewModel a_view) : ICommand
{
    protected ToolViewModel _view = a_view;
    public event EventHandler? CanExecuteChanged
    {
        add { CommandManager.RequerySuggested += value; }
        remove { CommandManager.RequerySuggested -= value; }
    }

    public abstract bool CanExecute(object? parameter);
    public abstract void Execute(object? parameter);
}