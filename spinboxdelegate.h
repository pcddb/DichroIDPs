#ifndef SPINBOXDELEGATE_H
#define SPINBOXDELEGATE_H

#include <QObject>
#include<QStyleOptionViewItem>
#include<QStyledItemDelegate>
#include<QDoubleSpinBox>

class SpinBoxDelegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SpinBoxDelegate(QObject *parent = nullptr);
    const int DOUBLESP_PRECISION = 4;
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const Q_DECL_OVERRIDE
                          {
                              auto w = QStyledItemDelegate::createEditor(
                                  parent, option, index);

                              auto sp = qobject_cast<QDoubleSpinBox*>(w);
                              if (sp)
                              {
                                  sp->setDecimals(DOUBLESP_PRECISION);
                              }
                              return w;
                          }

};

#endif // SPINBOXDELEGATE_H
