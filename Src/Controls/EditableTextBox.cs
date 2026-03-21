using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace ToolTest
{
    public class EditableTextBox : TextBox
    {
        private ToolViewModel? vm;
        private string _originalText = "";

        public EditableTextBox()
        {
            AllowDrop = true;
            IsReadOnly = true;
            BorderThickness = new Thickness(0);
            Background = Brushes.Transparent;
        }

        protected override void OnPreviewMouseLeftButtonDown(MouseButtonEventArgs e)
        {
            // マウスの下にある要素の DataContext を取得
            if (e.OriginalSource is FrameworkElement el && el.DataContext != null)
            {
                Debug.WriteLine(el.DataContext);
                DragDrop.DoDragDrop(this, el.DataContext, DragDropEffects.Move);
            }

            if (!IsReadOnly)
            {
                e.Handled = true;
            }

            base.OnPreviewMouseLeftButtonDown(e);
        }

        protected override void OnDragOver(DragEventArgs e)
        {
            base.OnDragOver(e);
            // 運ばれてきたデータが自分のDataContext（ItemViewModel等）と同じ型かチェック
            if (e.Data.GetDataPresent(this.DataContext.GetType()))
            {
                e.Effects = DragDropEffects.Move;
            }
            else
            {
                e.Effects = DragDropEffects.None;
            }

        }

        protected override void OnDrop(DragEventArgs e)
        {
            if (vm == null)
            {
                vm = (this.DataContext as ToolViewModel) ?? FindParentViewModel(this);
            }

            base.OnDrop(e);

            // 1. ドラッグされたデータ (Category or Item)
            var sourceData = e.Data.GetData(e.Data.GetFormats()[0]);

            // 2. ドロップ先のデータ (Category or Item)
            // 視覚要素から DataContext を抜き出す
            var targetData = (e.OriginalSource as FrameworkElement)?.DataContext;

            if (sourceData == null || targetData == null || sourceData == targetData) return;

            // --- 入れ替えロジックの振り分け ---
            if (sourceData is Item sCat && targetData is Item tCat)
            {
                vm?.MoveItem_FromItem(sCat, tCat);
            }
        }

        protected override void OnMouseDoubleClick(MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                base.OnMouseDoubleClick(e);

                e.Handled = true;
                BeginEdit();
            }
        }

        protected override void OnLostFocus(RoutedEventArgs e)
        {
            base.OnLostFocus(e);
            EndEdit(true);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (!IsReadOnly)
            {
                if (e.Key == Key.Enter)
                {
                    EndEdit(true);
                    e.Handled = true;
                }
                else if (e.Key == Key.Escape)
                {
                    EndEdit(false);
                    e.Handled = true;
                }
            }

            base.OnKeyDown(e);
        }

        protected override void OnMouseEnter(MouseEventArgs e)
        {
            if (IsReadOnly)
            {
                Background = Brushes.LightGray;
            }
            base.OnMouseEnter(e);
        }

        protected override void OnMouseLeave(MouseEventArgs e)
        {
            if (IsReadOnly)
            {
                Background = Brushes.Transparent;
            }
            base.OnMouseLeave(e);
        }

        private void BeginEdit()
        {
            _originalText = Text;

            IsReadOnly = false;
            Background = Brushes.LightSkyBlue;

            BorderThickness = new Thickness(1);

            Focus();
            SelectAll();
        }

        private void EndEdit(bool commit)
        {
            if (!commit)
            {
                Text = _originalText;
            }

            BorderThickness = new Thickness(0);

            IsReadOnly = true;
            Background = Brushes.Transparent;
            Keyboard.ClearFocus();
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
}