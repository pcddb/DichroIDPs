#ifndef TABLEITEMDELEGATE_H
#define TABLEITEMDELEGATE_H
#include<QObject>
#include<QStyledItemDelegate>
#include<QPainter>
#include<QModelIndex>
#include<QStyleOptionViewItem>

class TableItemDelegate : public QStyledItemDelegate
{

   Q_OBJECT

public:

   TableItemDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}

   QString displayText(const QVariant & value, const QLocale & locale)const
   {

     QString str = QString::number(value.toDouble(), 'f', 2);
     if (str.isEmpty())
         return "0.00";
     else
     return str;
   }
   void paintzero(QPainter *, const QStyleOptionViewItem &,
                          const QModelIndex &) const;

};

#endif // TABLEITEMDELEGATE_H
