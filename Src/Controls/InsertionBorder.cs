using System.Windows.Controls;
using System.Windows.Media;
using System.Windows;

namespace ToolTest;
public class InsertionBorder : Border
{
    public InsertionBorder()
    {
        AllowDrop = true;
        Background = Brushes.Transparent; // 反応領域を確保
    }

    protected override void OnDragOver(DragEventArgs e)
    {
        base.OnDragOver(e);
        // Item または Category がドラッグされている場合のみ許可
        if (e.Data.GetDataPresent(typeof(Item)) || e.Data.GetDataPresent(typeof(Item)))
        {
            e.Effects = DragDropEffects.Move;
            this.Background = Brushes.SkyBlue; // ホバー中の視覚効果
            this.Height = 4;
        }
    }

    protected override void OnDragLeave(DragEventArgs e)
    {
        base.OnDragLeave(e);
        this.Background = Brushes.Transparent;
        this.Height = 2;
    }

    protected override void OnDrop(DragEventArgs e)
    {
        base.OnDrop(e);
        this.Background = Brushes.Transparent;
        this.Height = 2;

        var sourceData = e.Data.GetData(e.Data.GetFormats()[0]);
        // このボーダー自身のDataContext（ItemかCategory）をターゲットとする
        var targetData = this.DataContext;

        if (sourceData == null || targetData == null || sourceData == targetData) return;

        // ViewModelの取得（EditableTextBoxのロジックを流用）
        var vm = FindParentViewModel(this);

        if (sourceData is Item sCat && targetData is Item tCat)
        {
            vm?.MoveItem_FromBorder(sCat, tCat);
        }
    }

    private ToolViewModel? FindParentViewModel(DependencyObject child)
    {
        DependencyObject parent = VisualTreeHelper.GetParent(child);
        while (parent != null)
        {
            if (parent is FrameworkElement fe && fe.DataContext is ToolViewModel tvm)
                return tvm;
            parent = VisualTreeHelper.GetParent(parent);
        }
        return null;
    }
}