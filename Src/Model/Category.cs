using System.Collections.ObjectModel;

namespace ToolTest;

public class Category()
{
    public string Name { get; set; } = "Category";
    public ObservableCollection<Item> Items { get; set; } = [];
}