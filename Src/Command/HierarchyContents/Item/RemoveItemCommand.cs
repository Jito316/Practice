namespace ToolTest.Command.HierarchyContents;

public class RemoveItemCommand(ToolViewModel a_view) : ToolCommand(a_view)
{
    public override bool CanExecute(object? parameter) => parameter is Item;
    public override void Execute(object? parameter)
    {
        if (parameter != null)
        {
            var item = (Item)parameter;

            foreach (var category in _view.Value)
            {
                if (category.Items.Remove(item))
                {
                    break;
                }
            }
        }
    }
}