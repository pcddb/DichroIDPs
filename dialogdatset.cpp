#include "dialogdatset.h"
#include "ui_dialogdatset.h"

DialogDatset::DialogDatset(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDatset)
{
    ui->setupUi(this);
}

DialogDatset::~DialogDatset()
{
    delete ui;
}

void DialogDatset::on_browseButton_clicked()
{
    Global *glob;// glob is a singleton
    glob = Global::getInstance();
    QString paths =glob->globalFile;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                paths,
                                                QFileDialog::ShowDirsOnly
                                                );
    if (dir.isEmpty())
        return;

   int index= dir.lastIndexOf("/");
   int strlength =dir.length();
   int remove=strlength-index-1;
    dataset =dir.right(remove);
    ui->setlineEdit->setText(dataset);
}

void DialogDatset::on_buttonBox_accepted()
{
    this->done(1);
    return ;
}

void DialogDatset::on_buttonBox_rejected()
{
    this->done(0);
    return;
}
QString DialogDatset::getDataset()
{

    QString set =ui->setlineEdit->text();
    return set;
}
