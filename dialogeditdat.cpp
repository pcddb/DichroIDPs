#include "dialogeditdat.h"
#include "ui_dialogeditdat.h"

DialogEditDat::DialogEditDat(QWidget *parent) :
    QDialog(parent),

    ui(new Ui::DialogEditDat),
    mModel(nullptr)
{
    ui->setupUi(this);
    initialiseTable();
}

DialogEditDat::~DialogEditDat()
{
    //delete ui;
}
void DialogEditDat::initialiseTable()
{
    deligate=new SpinBoxDelegate(this);
    mModel= new QStandardItemModel(this);
    pModel = new QSortFilterProxyModel(this);
    pModel->setSourceModel(mModel);
    ui->dataTable->setModel(pModel);
    ui->dataTable->setItemDelegate(deligate);//deligate allows editing to 4 decimal places

    ui->dataTable->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->dataTable, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(ShowContextMenu(const QPoint&)));

    ui->dataTable->viewport()->installEventFilter(this);
   // ui->dataTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
   ui->dataTable->installEventFilter(this);

    ui->dataTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
   // connect(ui->dataTable->horizontalHeader(),SIGNAL(sectionClicked(int)), this,SLOT(horizontalHeaderClicked(int)));
   // connect(ui->dataTable->verticalHeader(),SIGNAL(sectionClicked(int)), this,SLOT(verticalHeaderClicked(int)));
   // connect(ui->dataTable->horizontalHeader(),SIGNAL(sectiondoubleClicked(int)), this,SLOT(verticalHeaderDblClicked(int)));

}
/*void DialogEditDat::horizontalHeaderClicked(int column)
{
   if(column < 0) return; // can be used later for cols that can be selected

   ui->dataTable->setSelectionMode(QAbstractItemView::SingleSelection);
   ui->dataTable->setSelectionBehavior(QAbstractItemView::SelectColumns);
   ui->dataTable->selectColumn(column);
 //  ui->dataTable->setSelectionMode(QAbstractItemView::SingleSelection);
  // ui->dataTable->horizontalHeader()->blockSignals(true);
}
void DialogEditDat::verticalHeaderClicked(int row)
{
    if(row < 0) return; // use later for rows that are not selectable
   ui->dataTable->setSelectionMode(QAbstractItemView::SingleSelection);
   ui->dataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
   ui->dataTable->selectRow(row);
  // ui->dataTable->setSelectionMode(QAbstractItemView::SingleSelection);
    //ui->dataTable->verticalHeader()->blockSignals(true);
}
void DialogEditDat::verticalHeaderDblClicked(int i)
{

    //if(event->key() == Qt::CTRL)
   // {
      //  ui->dataTable->horizontalHeader()->blockSignals(true);
       // ui->dataTable->verticalHeader()->blockSignals(true);
        ui->dataTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

    //}
    //ui->dataTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
}
void DialogEditDat::keyReleaseEvent(QKeyEvent *Revent)
{
    /*if(Revent->key() == Qt::CTRL)
    {
        ui->dataTable->horizontalHeader()->blockSignals(false);
        ui->dataTable->verticalHeader()->blockSignals(false);
        ui->dataTable->setSelectionMode(QAbstractItemView::NoSelection);
    }
}*/

void DialogEditDat::on_openButton_clicked()
{
    Global *glob;// glob is a singleton
    glob = Global::getInstance();
    QString paths =glob->globalFile;
   // QStringList paths;
   // paths.append(path);
    QDir d;


    fname = QFileDialog::getExistingDirectory(this, tr("Select Dataset Folder"),
                                              paths,
                                              QFileDialog::ShowDirsOnly
                                              );

    if (fname.isEmpty())
        return;

    //*********** stores the file path for the next time we open a file**

    // QString file =fnames.first();
    d = QFileInfo(fname).absoluteDir();
   // paths.clear();
   // path=d.absolutePath();
  //  paths.append(path);

    ui->openButton->setEnabled(false);
    getData();

}
void DialogEditDat::getData()
{
    QStringList labelList;
    QStringList FList,AList;

    //###########  Get protein labels#############################################

    QString line;
    QFile file(fname +"/lbl2.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText(fname + "/lbl2.txt cant be found");
        msgBox.exec();
        ui->openButton->setEnabled(true);
        return;

    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        line = in.readLine();
    }
    QStringList proteinLablist =line.split("\t");
    // qDebug()<<"list"<<proteinLablist;
    cols=proteinLablist.size(); //get number of columns in tabel


    //add to table first row


    //#########################open lbl1 (structure labels ##################

    QFile fileL(fname + "/lbl1.txt");
    if (!fileL.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText(fname + "/lbl1.txt cant be found");
        msgBox.exec();
        ui->openButton->setEnabled(true);
        return;

    }
    //QString Lline;
    QStringList strucList;
    QTextStream inL(&fileL);
    while (!inL.atEnd()) {
        QString Lline = inL.readLine();
        strucList<<Lline;

    }
    strucList.prepend(" ");// so that first 2 row columns are empty
    strucList.prepend(" ");
    rows++;
    //add to first column
    //##########################open F matrix (structures) ############

    QFile fileF(fname + "/F.txt");
    if (!fileF.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText(fname +"/F.txt cant be found");
        msgBox.exec();
        ui->openButton->setEnabled(true);
        return;
    }

    QTextStream inF(&fileF);
    while (!inF.atEnd()) {
        QString line = inF.readLine();
        FList<<line;
        rows++;

    }

    //##############get sizes for F matrix ################################
    int fRows =FList.size();
    QStringList col =FList.first().split("\t");
    // int fCols = col.size();

    MatrixXd F(fRows, cols);
    for(int i=0; i<FList.size();++i)
    {
        QString line1 =FList.at(i);
        QStringList Firstline = line1.split("\t");

        int index=0;

        foreach (QString s, Firstline) {
            double d = s.toDouble();
            F(i,index) =d;
            ++index;
        }

    }

    rows++;
    row_offset = rows;
    //########################## openA matrix: spectra ###############

    QFile fileA(fname +"/A.txt");
    if (!fileA.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText(fname + "/A.txt cant be found");
        msgBox.exec();
        ui->openButton->setEnabled(true);
        return;
    }

    QTextStream inA(&fileA);
    while (!inA.atEnd()) {
        QString line = inA.readLine();
        AList<<line;
        rows++;

    }
    //##############get sizes for A matrix ################################
    int aRows =AList.size();
    QStringList col2 =AList.first().split("\t");
    // int aCols = col2.size();

    MatrixXd A(aRows, cols);
    for(int i=0; i<AList.size();++i)
    {
        QString line1 =AList.at(i);
        QStringList firstline = line1.split("\t");

        int index=0;

        foreach (QString s, firstline) {
            double d = s.toDouble();
            A(i,index) =d;
            ++index;
        }

    }

    setUpTable();
    protlistToTable(proteinLablist);
    structureLabelToTable(strucList);
    FmatrixToTable(F);
    AmatrixToTable(A);
}
void DialogEditDat::setUpTable()
{
    mModel->setColumnCount(cols);
    mModel->setRowCount(rows);

}
void DialogEditDat::protlistToTable(QStringList & protLabels)
{

    //##############   add to protein list to first row of table ############################
    if(protLabels.isEmpty())
    {
        ui->openButton->setEnabled(true);
        return;
    }
    for(int i=0; i<cols; ++i)
    {
        QStandardItem *protItem = new QStandardItem ;
        protItem->setText(protLabels.at(i));
        mModel->setItem(0,i,protItem);
    }
}
void DialogEditDat::structureLabelToTable(QStringList & strucList)
{
    qDebug()<<"struclist"<<strucList;
    mModel->setVerticalHeaderLabels(strucList);
}
void DialogEditDat::FmatrixToTable(MatrixXd &Fmat)
{
    for(int r=0; r<Fmat.rows(); ++r)
    {

        for(int c=0; c<Fmat.cols(); ++c)
        {
            QStandardItem  *FItem =  new QStandardItem ;
            //FItem->setData(Qt::EditRole, Fmat(r,c));
            FItem->setData(Fmat(r,c), Qt::DisplayRole);
            mModel->setItem(r+2,c,FItem);
        }
    }

}
void DialogEditDat::AmatrixToTable(MatrixXd &Amat)
{
    for(int r=0; r<Amat.rows(); ++r)
    {

        for(int c=0; c<Amat.cols(); ++c)
        {
            QStandardItem *AItem = new QStandardItem;
            AItem->setData(Amat(r,c), Qt::DisplayRole);
            mModel->setItem(r+row_offset,c,AItem);// sort getting row number
        }
    }
}

void DialogEditDat::on_clearButton_clicked()
{
    ui->openButton->setEnabled(true);
    mModel->clear();
    cols=0 ;
    rows=1;
    row_offset=0;
    fname.clear();
}

void DialogEditDat::on_closeButton_clicked()
{
    this->done(0);
    return ;
}
void DialogEditDat::ShowContextMenu(const QPoint & pos)
{
    QPoint globalPos = ui->dataTable->mapToGlobal(pos);
    QMenu myMenu;


    QAction action1("insert empty row above", this);
    QAction action2("insert empty column left",this);
   // QAction action3("insert empty column at end",this);
    QAction action4("delete row", this);
    QAction action5("delete column",this);
    QAction action6("copy",this);
    QAction action7("paste",this);
    QAction action8("delete",this);

    connect(&action1, SIGNAL(triggered()), this, SLOT(addRow()));
     connect(&action2, SIGNAL(triggered()), this, SLOT(addColumn()));
  //  connect(&action3, SIGNAL(triggered()), this, SLOT(addColumnEnd()));
    connect(&action4, SIGNAL(triggered()), this, SLOT(deleteRow()));
    connect(&action5, SIGNAL(triggered()), this, SLOT(deleteColumn()));
    connect(&action6, SIGNAL(triggered()), this, SLOT(copySelection()));
    connect(&action7, SIGNAL(triggered()), this, SLOT(paste()));
    connect(&action8, SIGNAL(triggered()), this, SLOT(deleteSelection()));

    myMenu.addAction(&action1);
    myMenu.addAction(&action2);
   // myMenu.addAction(&action3);
    myMenu.addAction(&action4);
    myMenu.addAction(&action5);
    myMenu.addAction(&action6);
    myMenu.addAction(&action7);
    myMenu.addAction(&action8);



     QModelIndexList Rindexes =  ui->dataTable->selectionModel()->selectedRows();

     if(Rindexes.isEmpty())
     {
         action1.setEnabled(false);
         action4.setEnabled(false);
     }
     else {
         action1.setEnabled(true);
         action4.setEnabled(true);
     }

     QModelIndexList Cindexes =  ui->dataTable->selectionModel()->selectedColumns();

     if(Cindexes.isEmpty())
     {
         action2.setEnabled(false);
       //  action3.setEnabled(false);
         action5.setEnabled(false);
     }
     else {
         action2.setEnabled(true);
        // action3.setEnabled(true);
         action5.setEnabled(true);
     }

  myMenu.exec(globalPos);
}
void DialogEditDat::deleteRow()
{
    QModelIndexList indexes =  ui->dataTable->selectionModel()->selectedRows();

    if (!indexes.isEmpty())
    {
        int r = indexes.first().row();

        mModel->removeRow(r);
    }
    else
    {
        return;
    }

    return;
}
void DialogEditDat::deleteColumn()
{
    QModelIndexList indexes =  ui->dataTable->selectionModel()->selectedColumns();

    if (!indexes.isEmpty())
    {
        int c = indexes.first().column();

        mModel->removeColumn(c);
    }
    else
    {
        return;
    }

    return;
}
void DialogEditDat::addRow()
{
    //################################################################
    //                Add empty row
    //#################################################################


    QModelIndexList indexes =  ui->dataTable->selectionModel()->selectedRows();

    if(!indexes.isEmpty())

    {
        mModel->insertRows(indexes.at(0).row(),
                           indexes.size());
        return;
    }
}
void DialogEditDat::addColumn()
{
    qDebug()<<"ere";

    QModelIndexList indexes =  ui->dataTable->selectionModel()->selectedColumns();
    if(!indexes.isEmpty())

    {
        mModel->insertColumns(indexes.at(0).column(),
                              indexes.size());
    }
}
/*void DialogEditDat::addColumnEnd()
{
    int cols =mModel->columnCount();


    QModelIndexList indexes =  ui->dataTable->selectionModel()->selectedColumns();
    if(!indexes.isEmpty())

    {

        mModel->setColumnCount(cols+1);

    }
}*/

void DialogEditDat::paste()
{

    QModelIndexList indexes =  ui->dataTable->selectionModel()->selectedIndexes();

    if(!indexes.isEmpty())
    {

        QString text = QApplication::clipboard()->text();
        QStringList rowContents = text.split("\n", QString::SkipEmptyParts);

        QModelIndex initIndex = indexes.at(0);
        auto initRow = initIndex.row();
        auto initCol = initIndex.column();

        for (auto i = 0; i < rowContents.size(); ++i) {
            QStringList columnContents = rowContents.at(i).split("\t");
            for (auto j = 0; j < columnContents.size(); ++j) {
                mModel->setData(mModel->index(
                                    initRow + i, initCol + j), columnContents.at(j));
            }

        }
    }
}
void DialogEditDat::deleteSelection()
{
    QModelIndexList indexes =  ui->dataTable->selectionModel()->selectedIndexes();


    int countRow = indexes.count();
    for( int i =0 ; i <countRow ; ++i)
    {
        int row = indexes.at(i).row();
        int col=indexes.at(i).column();

        mModel->takeItem(row,col);

    }


}
void DialogEditDat::copySelection()
{
    QString text;
    if(ui->dataTable->selectionModel()->selection().isEmpty())
    {
        qDebug()<<"in datatab copy";
                    return;
    }
    QItemSelectionRange range = ui->dataTable->selectionModel()->selection().first();
    for (auto i = range.top(); i <= range.bottom(); ++i)
    {
        QStringList rowContents;
        for (auto j = range.left(); j <= range.right(); ++j)
            rowContents << mModel->index(i,j).data().toString();
        text += rowContents.join("\t");
        text += "\n";
    }
    QApplication::clipboard()->setText(text);

}
bool DialogEditDat:: fileExists(QString path)
{
    QFileInfo check_file(path);

    return check_file.exists() && check_file.isFile();
}

/*void DialogEditDat::on_saveButton_clicked()
{
    // ui->dataTable->selectAll();
    //need to save matrices seperately

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),//
                                                    tr("/"), tr(".csv"));
    QFile f( fileName );

    if (fileExists(fileName))
    {
        QMessageBox msgBox;
        msgBox.setText("overwrite file?");
        msgBox.setStandardButtons(QMessageBox::Cancel|QMessageBox::Ok);

        if(msgBox.exec() == QMessageBox::Cancel)
            return;
    }

    if (!f.open(QIODevice::WriteOnly)) {


        return;
    }


    QTextStream data( &f );


    data.setRealNumberNotation(QTextStream::SmartNotation);//QDataStream::DoublePrecision
    data.setRealNumberPrecision(5);
    data.setFieldAlignment(QTextStream::AlignLeft);
    QStringList strList;

    for( int r = 0; r < mModel->rowCount()-1; ++r )
    {
        strList.clear();
        for( int c = 0; c < mModel->columnCount(); ++c )
        {
            if(mModel->item( r, c )==nullptr)//sort this. it is the space that fucking it up
                strList<<" ";
            else
                strList << mModel->item( r, c )->text();

        }
        data << strList.join( "," )<<endl;

    }
    f.close();
}*/
void DialogEditDat::on_saveButton_clicked()
{


    Global *glob;// glob is a singleton
    glob = Global::getInstance();
    QString paths =glob->globalFile;
    QInputDialog *dlg= new QInputDialog;
    bool ok;

         QString datasetName  = dlg->getText(this, paths,
                                              tr("Dataset name:"), QLineEdit::Normal,
                                              "", &ok);//dlg->setLabelText(paths);
        if (ok && datasetName.isEmpty())
           return;


        QString filePath=  paths +  "/" + datasetName;
        savedatafiles(filePath);

}
void DialogEditDat::savedatafiles(QString & filePath)
{
    // tr("/"), tr(".csv"));
    QDir dir(filePath);
    if (!dir.exists())
        dir.mkpath(".");

     qDebug()<<"daasetname"<<filePath;

     QString lbl1file = filePath +"/lbl1.txt";  //structure labels
     QString lbl2file = filePath +"/lbl2.txt";  //prot labels
     QString Ffile = filePath +"/F.txt";        //structure assignments
     QString Afile = filePath +"/A.txt";        //spectra

      QFile lbl2F( lbl2file);

   if (fileExists(lbl2file))
    {
        QMessageBox msgBox;
        msgBox.setText("overwrite file?");
        msgBox.setStandardButtons(QMessageBox::Cancel|QMessageBox::Ok);

        if(msgBox.exec() == QMessageBox::Cancel)
            return;
    }

    if (!lbl2F.open(QIODevice::WriteOnly)) {

   qDebug()<<"lbl2 not open";
        return;
    }


    QTextStream data( &lbl2F );


    data.setRealNumberNotation(QTextStream::SmartNotation);//QDataStream::DoublePrecision
    data.setRealNumberPrecision(5);
    data.setFieldAlignment(QTextStream::AlignLeft);
    QStringList strList;


        strList.clear();
        for( int c = 0; c < mModel->columnCount(); ++c )
        {
            if(mModel->item( 0, c )==nullptr)//sort this. it is the space that fucking it up
                strList<<" ";
            else
                strList << mModel->item( 0, c )->text();

        }
        data << strList.join( "\t" )<<endl;


    lbl2F.close();

    //############## save F file ####################################################
    //################################################################
   int headerlist_end;


    for(int r=2; r<20; ++r)//20 is far more rows than reqiired
    {
       if(mModel->item( r, 0 )==nullptr)
                headerlist_end=r;
    }

     QFile fF( Ffile);
    if (fileExists(Ffile))
     {
         QMessageBox msgBox;
         msgBox.setText("overwrite file?");
         msgBox.setStandardButtons(QMessageBox::Cancel|QMessageBox::Ok);

         if(msgBox.exec() == QMessageBox::Cancel)
             return;
     }

     if (!fF.open(QIODevice::WriteOnly)) {

    qDebug()<<"fF not open";
         return;
     }


     QTextStream fdata( &fF );


     fdata.setRealNumberNotation(QTextStream::SmartNotation);//QDataStream::DoublePrecision
     fdata.setRealNumberPrecision(5);
     fdata.setFieldAlignment(QTextStream::AlignLeft);
     QStringList FstrList;

     for(int r=2; r<headerlist_end; ++r)//20 is far more rows than reqiired
     {
         FstrList.clear();
         for( int c = 0; c < mModel->columnCount(); ++c )
         {
             if(mModel->item( r, c )==nullptr)
                 break;
             else
                 FstrList << mModel->item( r, c )->text();

         }
         fdata << FstrList.join( "\t" )<<endl;
     }

     fF.close();
     //#######################save lbl1 (structure labels ####################
     //#######################################################################

     QFile lbl1F( lbl1file);
    if (fileExists(lbl1file))
     {
         QMessageBox msgBox;
         msgBox.setText("overwrite file?");
         msgBox.setStandardButtons(QMessageBox::Cancel|QMessageBox::Ok);

         if(msgBox.exec() == QMessageBox::Cancel)
             return;
     }

     if (!lbl1F.open(QIODevice::WriteOnly)) {

         qDebug()<<"lbl1F not open";
         return;
     }
     QTextStream lbl1data( &lbl1F );


      lbl1data.setRealNumberNotation(QTextStream::SmartNotation);//QDataStream::DoublePrecision
     // lbl1data.setRealNumberPrecision(5);
     lbl1data.setFieldAlignment(QTextStream::AlignLeft);
    //qDebug()<<"headerlistend"<<headerlist_end;
     QStringList headerlist;
     for(int i=2; i< headerlist_end; ++i)
     {
         QStandardItem *vheaderItem = new QStandardItem;
         vheaderItem= mModel->verticalHeaderItem(i);
         QString  header =vheaderItem->text();
        // headerlist<<header;
          lbl1data<<header<<endl;

     }
     qDebug()<<"headerlist"<<headerlist;

       lbl1data<< headerlist.join( "\t" )<<endl;

     lbl1F.close();
     //################# save A data ####################################
     //###################################################################
     QFile aF( Afile);
    if (fileExists(Afile))
     {
         QMessageBox msgBox;
         msgBox.setText("overwrite file?");
         msgBox.setStandardButtons(QMessageBox::Cancel|QMessageBox::Ok);

         if(msgBox.exec() == QMessageBox::Cancel)
             return;
     }

     if (!aF.open(QIODevice::WriteOnly)) {

    qDebug()<<"aF not open";
    return;
     }


     QTextStream adata( &aF );


     adata.setRealNumberNotation(QTextStream::SmartNotation);//QDataStream::DoublePrecision
     adata.setRealNumberPrecision(5);
     adata.setFieldAlignment(QTextStream::AlignLeft);
     QStringList AstrList;

     for(int r=headerlist_end+1; r<mModel->rowCount(); ++r)//20 is far more rows than reqiired
     {
         AstrList.clear();
         for( int c = 0; c < mModel->columnCount(); ++c )
         {
             if(mModel->item( r, c )==nullptr)
                 break;
             else
                 AstrList << mModel->item( r, c )->text();

         }
         adata << AstrList.join( "\t" )<<endl;
     }

     aF.close();


     //############################################################

     QMessageBox msg;
     msg.setText(" datafiles created and saved to: "+ filePath);
     msg.exec();
}
