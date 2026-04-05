namespace ToolTest.Command.HierarchyContents;

public class AddItemCommand(ToolViewModel tool) : HierarchyContentsCommnad(tool)
{
    protected override void CategoryExecute(Category parameter)
    {
        parameter.Items.Add(new(parameter.Items));
        base.CategoryExecute(parameter);
    }

    protected override void ItemExecute(Item parameter)
    {
        foreach (var category in _view.Value)
        {
            if (category.Items.Contains(parameter))
            {
                category.Items.Add(new(parameter.Items));
                break;
            }
        }
        base.ItemExecute(parameter);
    }
}