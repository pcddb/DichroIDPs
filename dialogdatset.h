#ifndef DIALOGDATSET_H
#define DIALOGDATSET_H

#include <QDialog>
#include<qfiledialog.h>
#include"global.h"

namespace Ui {
class DialogDatset;
}

class DialogDatset : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDatset(QWidget *parent = nullptr);
    ~DialogDatset();

public slots:
    QString getDataset();

private slots:
    void on_browseButton_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::DialogDatset *ui;
    QString dataset;
};

#endif // DIALOGDATSET_H
