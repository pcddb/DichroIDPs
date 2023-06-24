#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <QDialog>

namespace Ui {
class aboutDlg;
}

class aboutDlg : public QDialog
{
    Q_OBJECT

public:
    explicit aboutDlg(QWidget *parent = nullptr);
    ~aboutDlg();
private slots:
    void on_OKButton_clicked();

private:
    Ui::aboutDlg *ui;
};

#endif // ABOUTDLG_H
