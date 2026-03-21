namespace ToolTest.Command.HierarchyContents;

public class AddCategoryCommand(ToolViewModel a_view) : ToolCommand(a_view)
{
    public override bool CanExecute(object? parameter) => true;
    public override void Execute(object? parameter) => _view.Value.Add(new());
}