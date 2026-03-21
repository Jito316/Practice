using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

namespace ToolTest;

public class ItemLeaf : StackPanel
{
    // 1. 依存関係プロパティの定義 (Itemがセットされたら表示を更新する)
    public static readonly DependencyProperty ItemProperty =
        DependencyProperty.Register(
            nameof(Item),
            typeof(Item),
            typeof(ItemLeaf),
            new PropertyMetadata(null, OnItemChanged)); // 値が変わった時のコールバックを指定


    // TextBoxスタイルのプロパティ
    public static readonly DependencyProperty TexBoxStyleProperty =
        DependencyProperty.Register(nameof(TextBoxStyle), typeof(Style), typeof(ItemLeaf),
            new PropertyMetadata(null, OnStyleChanged));

    public Item Item { get => (Item)GetValue(ItemProperty); set => SetValue(ItemProperty, value); }
    public Style TextBoxStyle { get => (Style)GetValue(TexBoxStyleProperty); set => SetValue(TexBoxStyleProperty, value); }


    // 各コントロールをフィールドとして保持
    private InsertionBorder _dropBorder;
    private Border _itemBorder;
    private EditableTextBox _header;
    private TextBox _nameTextBox;
    private TextBox _timeTextBox;

    public ItemLeaf()
    {
        // 2. レイアウトの基本構造だけコンストラクタで作っておく
        _dropBorder = new InsertionBorder { Height = 2, Margin = new Thickness(20, 0, 0, 0) };
        _itemBorder = new Border { Padding = new Thickness(4) };

        var grid = new Grid();
        grid.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });
        grid.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });
        grid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });

        _header = new EditableTextBox { Text = "⠿", IsReadOnly = true, Margin = new Thickness(5, 0, 5, 0) };
        Grid.SetColumn(_header, 0);

        _nameTextBox = new TextBox { HorizontalAlignment = HorizontalAlignment.Stretch };
        Grid.SetColumn(_nameTextBox, 1);

        _timeTextBox = new TextBox { MinWidth = 80, Margin = new Thickness(10, 0, 0, 0), TextAlignment = TextAlignment.Right };
        Grid.SetColumn(_timeTextBox, 2);

        grid.Children.Add(_header);
        grid.Children.Add(_nameTextBox);
        grid.Children.Add(_timeTextBox);
        _itemBorder.Child = grid;

        this.Children.Add(_dropBorder);
        this.Children.Add(_itemBorder);
    }

    // 3. Itemプロパティがセットされた時に呼ばれる処理
    private static void OnItemChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        if (d is ItemLeaf leaf && e.NewValue is Item newItem)
        {
            // ここで初めてバインディングを繋ぐ
            leaf._nameTextBox.SetBinding(TextBox.TextProperty, new Binding("Name") { Source = newItem, Mode = BindingMode.TwoWay });
            leaf._timeTextBox.SetBinding(TextBox.TextProperty, new Binding("Time") { Source = newItem, Mode = BindingMode.TwoWay });

            // 例：フラグに応じて表示を切り替える設定もここで行う
            leaf._timeTextBox.SetBinding(TextBox.VisibilityProperty, new Binding("IsVisibleTime")
            {
                Source = newItem,
                Converter = new BooleanToVisibilityConverter()
            });
        }
    }

    private static void OnStyleChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        var leaf = (ItemLeaf)d;
        var style = (Style)e.NewValue;
        leaf._header.Style = style;
        leaf._nameTextBox.Style = style;
        leaf._timeTextBox.Style = style;
    }
}