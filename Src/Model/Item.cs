using System.Collections.ObjectModel;

namespace ToolTest;

public class Item(ObservableCollection<Item>? _parent)
{
    public string Name { get; set; } = "Item";

    public bool IsTime { get; set; } = false;
    public string Time { get; set; } = "0.75";

    public ObservableCollection<Item>? Parent = _parent;
    public ObservableCollection<Item> Items { get; set; } = [];
}