#include "borderdelegate.h"

// draws a thick border between reults on results table
BorderDelegate::BorderDelegate(QObject *parent):QStyledItemDelegate (parent)
{

}
void BorderDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QPen pen;
    pen.setWidth(5);
    const QRect rect( option.rect );
    painter->setPen( pen );
    painter->drawLine( rect.topLeft(), rect.topRight() );
    //painter->drawLine( rect.bottomLeft(), rect.bottomRight() );

    // Draw left edge of left-most cell
    //   if ( index.column() == 0 )
    //      painter->drawLine( rect.topLeft(), rect.bottomLeft() );

    // Draw right edge of right-most cell
    // if ( index.column() == index.model()->columnCount() - 1 )
    //  painter->drawLine( rect.topRight(), rect.bottomRight() );

    QStyledItemDelegate::paint( painter, option, index );
}
//tableWidget->setItemDelegateForRow( row, new DrawBorderDelegate( this ) );
