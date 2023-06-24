#ifndef BORDERDELEGATE_H
#define BORDERDELEGATE_H

#include <QObject>
#include<QStyledItemDelegate>
#include<QPainter>

class BorderDelegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BorderDelegate(QObject *parent = nullptr);

 // public: BorderDelegate( QObject* parent = nullptr ) : QStyledItemDelegate( parent ) {}
    void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;

};

#endif // BORDERDELEGATE_H
