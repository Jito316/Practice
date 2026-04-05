// C#側: プロキシクラスの定義
using System.Windows;

namespace ToolTest;

public class BindingProxy : Freezable
{
    protected override Freezable CreateInstanceCore() => new BindingProxy();

    public object Data
    {
        get => (object)GetValue(DataProperty);
        set { SetValue(DataProperty, value); sData = Data; }
    }

    public static readonly DependencyProperty DataProperty =
        DependencyProperty.Register("Data", typeof(object), typeof(BindingProxy), new PropertyMetadata(null));

    public static object? sData;
}