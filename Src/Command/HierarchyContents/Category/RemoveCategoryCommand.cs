namespace ToolTest.Command.HierarchyContents;

public class RemoveCategoryCommand(ToolViewModel a_view) : HierarchyContentsCommnad(a_view)
{
    protected override void CategoryExecute(Category parameter)
    {
        _view.Value.Remove(parameter);
    }

    protected override void ItemExecute(Item parameter)
    {
        foreach (var category in _view.Value)
        {
            if (category.Items.Contains(parameter))
            {
                _view.Value.Remove(category);
                break;
            }
        }
    }
}