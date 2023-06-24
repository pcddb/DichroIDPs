#include "tableitemdelegate.h"

//TableItemDelegate::TableItemDelegate(QWidget *parent) : QStyledItemDelegate (parent)

//{
//}
void TableItemDelegate::paintzero(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
{
    if (index.column() == 1)
    {
       //??????????????????????????////
    }
    else // it's just a common column. Live it in default way
        TableItemDelegate::paint(painter, option, index);
}
