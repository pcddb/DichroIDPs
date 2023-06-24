#include "interface.h"
#include "ui_interface.h"

interFace::interFace(QWidget *parent) :
    QWidget(parent)

{
    setupUi(this);
    setupPlot();
    setupTable();
    setparameters();
    setupResultsTable();
    setupSummmaryTabel();
    readSettings();


    connect(datasetCombo,SIGNAL(currentIndexChanged(const QString&)),
            this,SLOT(datasetchanged(const QString&)));

    //connect(RandDBox, SIGNAL(stateChanged(int )),
          //  this, SLOT(RDboxchanged(int )));


}
interFace::~interFace()
{
    //delete ui;
    saveSettings();
}
void interFace::setupPlot()
{
    plot->setInteractions( QCP::iSelectPlottables );
}
void interFace::setupTable()
{

    connect(listWidget, SIGNAL(itemSelectionChanged()),
            SLOT(slotSelectionChange())
            );


    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(listWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(ShowContextMenu(const QPoint&)));

    listWidget->viewport()->installEventFilter(this);
    listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
}
void interFace::setparameters()
{
    //save parameters when program closed

    frontlabel->setVisible(FALSE);

    QStringList datasetList;
    datasetList << "SP175"<< "MP180"<< "IDP190"<<"test";
    datasetCombo->addItems(datasetList);

    progressBar->setVisible(false);
    datasetCombo->setEditable(false);
    resultLabel->setVisible(false);


}
void interFace::setupSummmaryTabel()
{

    summaryTable->horizontalHeader()->setVisible(true);
    QFont font;// = resultsTable->horizontalHeader()->font();
    font.setPointSize(11);
    font.setBold(true);
    summaryTable->horizontalHeader()->setFont( font );
//     summaryTable->horizontalHeader()->

    summaryTable->verticalHeader()->setVisible(false);
    summaryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    summaryTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(summaryTable, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(summaryContextMenu(const QPoint&)));
    summaryTable->viewport()->installEventFilter(this);
    summaryTable->setShowGrid(false);
   // summaryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   //makes result (apart from NRMSD 2 dp)


}

 // for combobox settings
void interFace::readSettings()
{
    // read save parameters from previous instance

    QSettings settings("bbk","DichroApp");

    QStringList Datasets = settings.value("set").toStringList();
    datasetCombo->clear();
    datasetCombo->addItems(Datasets);

    // dataset directory strored in global
    Global *glob;// glob is a singleton
    glob = Global::getInstance();

    glob->globalFile=settings.value("dir").toString();



}
void interFace::saveSettings()
{

    QSettings settings("bbk","DichroApp");

    QStringList Datasets;
    for(int index = 0; index < datasetCombo->count();index++)
        Datasets.append(datasetCombo->itemText(index));
    settings.setValue("set", Datasets);

    // dataset directory saved when prog closed
    Global *glob;// Spectrum is a singleton
    glob = Global::getInstance();

    settings.setValue("dir",glob->globalFile);


}

void interFace::setupResultsTable()
{

    QStringList headers;
    headers<< "Initial"<<"HJ5"<<"Sel1"<<"Sel2"<<"Sel3"<<"Closest proteins"<<" File name "<<"NRMSD ";

    resultsTable->setColumnCount(8);
    resultsTable->setHorizontalHeaderLabels(headers);

    // resultsTable->horizontalHeader()->setStyleSheet("color: blue");

    QFont font;// = resultsTable->horizontalHeader()->font();
    font.setPointSize(12);
    font.setBold(true);
    resultsTable->horizontalHeader()->setFont( font );
    resultsTable->horizontalHeader()->setStyleSheet(  "QHeaderView::section{"
                                                      "border-top:0px solid #D8D8D8;"
                                                      "border-left:0px solid #D8D8D8;"
                                                      "border-right:1px solid #D8D8D8;"
                                                      "border-bottom: 1px solid #D8D8D8;"
                                                      "background-color:white;"
                                                      "padding:4px;"
                                                      "}");
    QFont font2;// = resultsTable->verticalHeader()->font();
    font2.setPointSize( 12 );
    font2.setBold(true);
    resultsTable->verticalHeader()->setFont( font2 );

    resultsTable->resizeRowsToContents();
    resultsTable->resizeColumnsToContents();

    // resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultsTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

    resultsTable->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(resultsTable, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(contextMenu(const QPoint&)));

     // resultsTable->viewport()->installEventFilter(this);
}


bool interFace::eventFilter(QObject *object, QEvent *event) //works inside widget
{

    //***********clear selection in table view and plot upon clicking left button in table view**********

    if (object == listWidget->viewport() && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *pMouseEvent = static_cast<QMouseEvent *>(event);

        if (pMouseEvent->button() == Qt::LeftButton)
        {
            QModelIndex item=listWidget->indexAt(pMouseEvent->pos());

            if (!item.isValid())
            {
                listWidget->clearSelection();
                for(int i =0; i<plot->graphCount(); ++i)
                    plot->graph( i)->setSelected(FALSE);
                plot->replot();
            }
        }
    }

    return false;
}
void interFace::contextMenu(const QPoint& pos)
{

    QMenu myMenu;
    if (menubool==true)
    {
        // makes sure the context menu vanished when action selected.
        //this wasnt happening for some reason
        //qDebug()<<"menubool true return??";
        menubool=false;
      // resultsTable->blockSignals(true);
        return;
    }


    QPoint globalPos = resultsTable->mapToGlobal(pos);



    QAction action1("copy", this);
    //QAction action2("clear", this);


    connect(&action1, SIGNAL(triggered()), this, SLOT(copy()));
  //  connect(&action2, SIGNAL(triggered()), this, SLOT(on_clearButton_clicked()));

    myMenu.addAction(&action1);
   // myMenu.addAction(&action2);

    if(resultsTable->rowCount()==0)
    {
        action1.setEnabled(false);
        //action2.setEnabled(false);
    }else
    {
        action1.setEnabled(true);
        //action2.setEnabled(true);
    }

    myMenu.exec(globalPos);
  //  myMenu.deleteLater();

}
void interFace::summaryContextMenu(const QPoint& pos)
{


    QMenu myMenu2;

    QPoint globalPos = summaryTable->mapToGlobal(pos);



    QAction action1("copy, Ctrl+ C", this);
   // QAction action2("clear", this);


    connect(&action1, SIGNAL(triggered()), this, SLOT(copysummary()));
   // connect(&action2, SIGNAL(triggered()), this, SLOT(on_clearButton_clicked()));

    myMenu2.addAction(&action1);
   // myMenu.addAction(&action2);

    if(summaryTable->rowCount()==0)
    {
        action1.setEnabled(false);
       // action2.setEnabled(false);
    }else
    {
        action1.setEnabled(true);
       // action2.setEnabled(true);
    }

    myMenu2.exec(globalPos);

}
void interFace::keyPressEvent(QKeyEvent *event)
{
    if(event->matches(QKeySequence::Copy))
    {
        copysummary();
    }
}

void interFace::on_openButton_clicked()
{
    //################################################################
    //    function opens spectra files one at a  time
    //##############################################################
    datasetCombo->setEditable(false);

    if(genButton->isChecked())
        ftype =1;
    else
    {
        ftype =0;
    }
    // show analysis interface
    QStringList paths;
    paths.append(path);
    QDir d;

    fname = QFileDialog::getOpenFileName(this, tr("Select one or more files to open"),
                                         paths.last(), tr("*.txt *.gen"));

    if (fname.isEmpty())
        return;
    scaleBox->setValue(1.00);
    //*********** stores the file path for the next time we open a file**


    d = QFileInfo(fname).absoluteDir();
    paths.clear();
    path=d.absolutePath();
    paths.append(path);


    //**********************************************************************

    // Read_file *F=new(Read_file) ;

    Read_file *f = new Read_file;


    QObject::connect( f,SIGNAL(send_data(QVector<double> ,QVector<double>)),this, SLOT (plotdata(QVector<double>,QVector<double>)),Qt::AutoConnection);

    if(!fname.isEmpty() )
    {
        f->openSpectrum(fname, ftype);
    }

}
void interFace::plotdata(QVector<double>  WL1, QVector<double>  CD1)
{
    //###############################################################
    //  plot spectrum and add entry to file list

    //#######################################################

    QVector<double> CD;
    QVector<double> WL;

    if(WL1.first()<240)
    {
        QMessageBox msgBox;
        msgBox.setText("spectrum must have a high wavelength of 240 nm or above!");
        msgBox.exec();
        return;

    }
    //ensure data starts at a whole number
    if(floor(WL1.first())!=WL1.first())
    {
         QString highWL =QString::number(WL1.first(),'f', 1);
        QMessageBox msgBox;
        msgBox.setText("high wavelength is " + highWL +" High wavelength must be a whole number!");
        msgBox.exec();
        return;

    }

    //ensure interval of 1 or 0.5 nm

    double interval =WL1.at(0)-WL1.at(1);
    int roundedInterval = (int)(interval * 10.0);// do this beause you cant compare doubles properly
    QString stepsize =QString::number(interval,'f', 1);

    Rmsd *R =new Rmsd;
    QList<QVector<double>> vectorlist;

    if(roundedInterval==10||roundedInterval==5)
    {

        //if not 1nm interval (ie 0.5 nm) we make interval 1nm
        //and crucially remove all data above 240nm
        vectorlist = R->sortOutInterval(WL1, CD1);
    }
    else
    {
        QMessageBox msg;
        msg.setText("Wavelength interval is "+ stepsize+" only wavelength intervals of 1.0 nm and 0.5 nm accepted!");
        msg.exec();
        return;
    }


    // QList<QVector<double>> vectorlist = R->sortOutInterval(WL1, CD1);


//########################################################################
    WL =vectorlist.first();
    CD= vectorlist.last();


    getNamefromPath();

    //ui->widget->graph()->setName(numberstring);
    new QListWidgetItem(fname, listWidget);

    //############   make each graph a different color ######################
    QColor RGBcolor;
    int a = rand() % 254+1;
    int b= rand() % 254+1;
    int c= rand()%254+1;


    // dissalow colors that are too light
    if (a>220 )
        a=0;
    if(b>220)
        b=0;
    if(c>220)
        c=0;

    RGBcolor.setRgb( a,  b,  c,   255);

    QPen pen( RGBcolor,1.25);
  //  QPen pen2( RGBcolor, 0.75);
    //########################################################################################

    plot->addGraph(plot->xAxis, plot->yAxis);

    plot->graph(plotindex)->setData(WL, CD);

    plot->graph(plotindex)->setPen(pen);
    plot->graph(plotindex)->setVisible(TRUE);
    plot->rescaleAxes(1);

    plot->replot();

    plotindex++;

}


void interFace::getNamefromPath()
{
    //#################################################################
    //  extract the file name from the file path to put in table
    //################################################################
    QString s = "/";
    QString e = "";
    int start = fname.lastIndexOf(s); //index of last / character
    int end = fname.lastIndexOf(e); //index of last null character
    int lengthstr = end - start; // difference is the string length
    fname = fname.right(lengthstr);


}
void interFace::deletspectrum()
{

    int currentRow = listWidget->currentRow();


    listWidget->blockSignals(true);//stops slot selectionchanged being called
    listWidget->takeItem(currentRow);
    plot->removeGraph(currentRow);//cd plot
    // plot->removeGraph(currentRow);//ht plot

    plot->rescaleAxes(1);
    plot->replot();
    // plotindex-=2;
    //tindex-=2;*/
    plotindex--;

    //#### clear selection ########
    listWidget->clearSelection();
    for(int i =0; i<plot->graphCount(); ++i)
        plot->graph( i)->setSelected(FALSE);
    plot->replot();
    // qDebug()<<"plotindexdelete"<<plotindex;

    //  }
    listWidget->blockSignals(false);
}
void interFace::deleteAll()
{
    listWidget->clear();
    plot->clearGraphs();
    plotindex=0;
    plot->replot();
}

void interFace::on_pushButton_clicked()
{
    datasetCombo->setEditable(false);
    if (Xvalbool==1)
    {
        resultsTable->clear();
        solutionRowIndex=0;
        protInd=0;
        RMSDIndex=0;
        // plotindex=0;
        // plot->clearGraphs();
        // plot->replot();
    }
    setupResultsTable();
    Xvalbool=0;
    //###########################################################################
    //  analysis button clicked get data from graph
    //############################################################################
    VectorXd CD;
    VectorXd WL;
    int j=0;

    Read_file *RF =new Read_file;

    connect (RF,SIGNAL(sendSolution(QList<VectorXd>)),this,SLOT(outPut(QList<VectorXd>)),Qt::AutoConnection);
    connect (RF,SIGNAL(sendlabels(QStringList)),this,SLOT(outPutlabels(QStringList)),Qt::AutoConnection);
    connect (RF,SIGNAL(sendProtInd(MatrixXi)),this,SLOT(protIndex(MatrixXi)),Qt::AutoConnection);
    connect (RF,SIGNAL(sendRefit(VectorXd)),this,SLOT(plotRefit(VectorXd)),Qt::AutoConnection);
    connect (RF,SIGNAL(sendNMRSD(double)),this,SLOT(showNRMSD(double)),Qt::AutoConnection);
    connect (RF, SIGNAL(sendProg(int)), this, SLOT(progress(int)), Qt::AutoConnection);


    if(plot->selectedGraphs().count() != 0)
    {
        const QCPDataMap *dataMap =plot->selectedGraphs().first()->data();
        CD.resize(plot->selectedGraphs().first()->data()->size());
        WL.resize(plot->selectedGraphs().first()->data()->size());
        QMap<double, QCPData>::const_iterator i = dataMap->constBegin();
        while (i != dataMap->constEnd()) {
            CD(j) = i.value().value;
            WL(j)=i.value().key;
            ++j;
            ++i;
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("No spectrum selected");
        msgBox.exec();
        return;
    }

    //############## get low wavelength cutoff ##########################

    double lowW = LowWaveBox->value();
    VectorXd CD1(0);
    for(int i=0; i<WL.size();++i)
    {
        //  qDebug()<<"ere";
        if(WL(i)==lowW)
        {
            int cut= (WL.size()-i);

            CD1.resize(CD.tail(cut).size());
            CD1 =CD.tail(cut);

        }

    }
    if(lowW< WL.head(1).value())
    {
        cout<<"wltailvalue "<<WL.tail(1).value()<<endl;        QMessageBox msgBox;
        msgBox.setText("Low wavelength cut-off below the lowest wavelength of data\n"
                       " Using lowest wavelength of data");

        msgBox.exec();


    }
    // if no resizing is done above put CD into CD1
    if(CD1.size()==0)
    {
       // qDebug()<<"cd=cd1";
        CD1.resize(CD.size());
        CD1=CD;

    }
    //##############################################################

    QListWidgetItem *item =listWidget->item(listWidget->currentRow());
    QString itemString =item->text();
    fname =itemString;


    progressBar->setVisible(true);
    progressBar->setValue(progIndex);
    //reverse the CD vector: comes out of the plot the wrong way!!
    VectorXd CDrev(CD1.size());
    CDrev= CD1.colwise().reverse();


    QString set = datasetCombo->currentText();

    RF->openDataSet(set, CDrev);
    // need to return a bool to say if datasets were open or not. if not then dont do the following.
    stackedWidget->setCurrentIndex(1);
    frontlabel->setVisible(TRUE);
    backlabel->setVisible(FALSE);
    pushButton->setEnabled(false);
    progressBar->setVisible(FALSE);
    progIndex=25;
}
void interFace::outPut(QList<VectorXd> output)
{

    //########################################################
    //   solutions are displayed in the results table
    //#########################################################

    int solutionIndex=0;
    VectorXd OP(output.first().size());
    resultsTable->setRowCount(output.first().rows()+solutionRowIndex);
    for(int j=0; j<output.size(); ++j)
    {
        OP=output.at(j);
        for(int i=0; i<OP.size(); ++i)
        {
            // add row headers
            QTableWidgetItem *lblItem =new QTableWidgetItem;
            lblItem->setData(Qt::EditRole, labels.at(i));
            resultsTable->setVerticalHeaderItem(i+solutionRowIndex, lblItem );

            double d;

            d = OP(i);
            //qDebug()<<"d "<<d;

            d = roundf(d*100)/100;

            QTableWidgetItem *solItem = new QTableWidgetItem;
            solItem->setData(Qt::EditRole, d);
            resultsTable->setItem(i+solutionRowIndex,solutionIndex,solItem);
            //  solItem->setTextAlignment(Qt::AlignLeft);
            solItem->setTextAlignment(Qt::AlignVCenter);
        }
        solutionIndex++;
    }
    //give trailing zeros if nessessary
    TableItemDelegate *zeroDelegate = new TableItemDelegate(resultsTable);
    for (int i=0; i<5; i++)
    {
        resultsTable->setItemDelegateForColumn( i,zeroDelegate);
    }

    // getNamefromPath();
    QTableWidgetItem *fileItem = new QTableWidgetItem;
    fileItem->setData(Qt::EditRole, fname);
    //qDebug()<<"fname"<<fname;
    resultsTable->setItem(solutionRowIndex,6, fileItem);

    //draw thick border
    resultsTable->setItemDelegateForRow( solutionRowIndex, new BorderDelegate( this ) );

    solutionRowIndex+=OP.size();//allows multiple analyses to appear on table with gap between each.
    rmsdplacementindex=OP.size();


    VectorXd summary(output.first().size());
    if(output.size()<5)
    {
        summaryTable->clear();
        return;
    }
    summary =output.at(4);

    outputSummary(summary);

}
void interFace::outputSummary(VectorXd &summry)
{
//hack so that i can modify summary results without affecting the headers
    labels.removeLast();
    labels.removeLast();
    labels<<"NRMSD";

    summaryTable->clear();
    summaryTable->setRowCount(1);
    summaryTable->setColumnCount(summry.size()+1);
    summaryTable->setHorizontalHeaderLabels(labels);
   //qDebug()<<"labels"<<labels;
    for(int i=0; i< summry.size(); ++i)
    {
        double d;

        d = summry(i);

        d = roundf(d*100)/100;

        QTableWidgetItem *solItem = new QTableWidgetItem;

        solItem->setData(Qt::EditRole, d);

        summaryTable->setItem(0,i,solItem);

        solItem->setTextAlignment(Qt::AlignVCenter);
        solItem->setTextAlignment(Qt::AlignCenter);


    }
//give trailing zeros if nessessary
    TableItemDelegate *zeroDelegate = new TableItemDelegate(summaryTable);
       for (int i=0; i<labels.size()-1; i++)
       {
           summaryTable->setItemDelegateForColumn( i,zeroDelegate);
       }

}
void interFace::outPutlabels(QStringList label)
{
    //########### structure labels###################

    labels=label;

}
void interFace::protIndex(MatrixXi protI)
{
    int pn =protI.rows();// for checking if lbl2 can be opened
    protNumber =protI;

    // cout<<endl<<"protnumber"<<endl<<protNumber<<endl;
    QString set = datasetCombo->currentText();
    Read_file *R = new Read_file;
    QString prots =R->openProtLabel(set, pn);
    QStringList protlist = prots.split("\t");


    for(int i=0; i<9; ++i)
    {

        int index =protNumber(i,1);
     //  qDebug()<<"index"<<index;
        QString protein = protlist.at(index);
        //   qDebug()<<"proteins  "<<i<<" "<<protein;
        QTableWidgetItem *protItem = new QTableWidgetItem;
        protItem->setData(Qt::EditRole, protein);
        resultsTable->setItem(i+protInd,5,protItem);


    }

   //  resultsTable->item(protInd,5)->setBackgroundColor(Qt::green);
    protInd= resultsTable->rowCount();
    resultsTable->scrollToBottom();

}
void interFace::plotRefit(VectorXd refitCD)
{

   // cout<<"refit in refit"<<refitCD<<endl;
    int size = refitCD.size();
    QVector<double> refittedCDdata;
    QVector<double> refittedWLdata;
   // QVector<double> refittedWLdata2;
    for(int i =240; i>size-1; --i)
    {

        refittedWLdata<<i;
    }
    for(int i =0; i<size; ++i)
    {
        refittedCDdata<<refitCD(i);

    }
    fname = fname +": Refit";
    plotdata(refittedWLdata,refittedCDdata);
    fname.clear();
    QPen newpen(Qt::black);
    newpen.setWidth(2);
    QVector<qreal> dashes;
    qreal space = 4;
    dashes << 1 << space << 3 << space << 9 << space
           << 27 << space << 9 << space;
    newpen.setDashPattern(dashes);
    int p=plot->graphCount()-1;
    plot->graph(p)->setPen(newpen);

}
void interFace::showNRMSD(double error)
{
    int count =resultsTable->rowCount();
    int summarycount =summaryTable->columnCount();
    qDebug()<<"Scount"<<summarycount;
    RMSDIndex=count-rmsdplacementindex;
    //########### NMRSD for table ###################
    rmsdplacementindex=0;
    QTableWidgetItem *NRMSDItem = new QTableWidgetItem;
    NRMSDItem->setData(Qt::EditRole,error);  
    resultsTable->setItem(RMSDIndex,7,NRMSDItem);

    // color if below critical level

    if(error<0.0001)
    {
        //colour amber
        resultsTable->item(RMSDIndex, 7)->setBackgroundColor(Qt::yellow);
    }

   resultsTable->scrollToBottom();
    // NRMSD entry in summary table
    QTableWidgetItem *NRMSDItem2 = new QTableWidgetItem;
    NRMSDItem2->setData(Qt::EditRole,error);
    summaryTable->setItem(0,summarycount-1,NRMSDItem2);

    NRMSDItem2->setTextAlignment(Qt::AlignVCenter);

   // NRMSDItem2->setTextAlignment(Qt::AlignHCenter);
    if(error<0.0001)
    {
        //colour red
        summaryTable->item(0,summarycount-1)->setBackgroundColor(Qt::yellow);
    }

    summaryTable->resizeColumnsToContents();


    qDebug()<<"rmsdcount"<<RMSDIndex;
}
void interFace::ShowContextMenu(const QPoint & pos)
{
    QPoint globalPos = listWidget->mapToGlobal(pos);
    QMenu Menu;


    int row=listWidget->currentRow();
    //  qDebug()<<"row"<<row;



        QAction action1("delete row", this);
        QAction action2("delete all",this);

        connect(&action1, SIGNAL(triggered()), this, SLOT(deletspectrum()));
        connect(&action2, SIGNAL(triggered()), this, SLOT(deleteAll()));

        Menu.addAction(&action1);
        Menu.addAction(&action2);



            if (row!=-1)
            {
                action1.setEnabled(true);
                action1.setEnabled(true);

            }
            else {
                action1.setEnabled(false);
                action1.setEnabled(false);
            }
  Menu.exec(globalPos);
    return;
}
void interFace::slotSelectionChange()
{

    //ensure analysis button i active when 1 row is selected
    //  and show selected spectrum in plot
    QModelIndexList selection = listWidget->selectionModel()->selectedRows();
    if (selection.size()==1)
    {
        pushButton->setDisabled(FALSE);

    }
    else
    {
        pushButton->setDisabled(TRUE);
    }

    for(int i =0; i<plot->graphCount(); ++i)
        plot->graph( i)->setSelected(FALSE);

    int Selectedrow=listWidget->currentRow();

    plot->graph( Selectedrow)->setSelected(TRUE);
    plot->replot();

}
void interFace::on_backButton_clicked()
{
    resultLabel->setVisible(false);
    datasetCombo->setEditable(false);
    if(stackedWidget->currentIndex()==1)
    {
        stackedWidget->setCurrentIndex(0);
        frontlabel->setVisible(FALSE);
        backlabel->setVisible(TRUE);
        pushButton->setEnabled(true);
    }
    else
    {
        stackedWidget->setCurrentIndex(1);
        frontlabel->setVisible(TRUE);
        backlabel->setVisible(FALSE);
        pushButton->setEnabled(false);
    }
    return;

}
void interFace::stayOntab1(bool & open)
{
    // need to send a signal here
    stackedWidget->setCurrentIndex(0);
    frontlabel->setVisible(FALSE);
    backlabel->setVisible(TRUE);

}
void interFace::progress(int i)
{
    // ###################### ipdate progress bar ################

    progressBar->setValue(i+progIndex);
    qApp->processEvents();
    progIndex+=i;

}

void interFace::on_clearButton_clicked()
{
    //myMenu.hide();
    //for (int i=1; i<resultsTable->rowCount(); ++i)
    resultsTable->clear();
    summaryTable->clear();

    solutionRowIndex=0;
    resultsTable->setRowCount(0);
    setupResultsTable();
    protInd=0;
   // menubool=1;

}
void interFace::copy()
{
    //#### copy selection in results table using context menu ###########
    //  datasetCombo->setEditable(false);
    //myMenu.hide();
    QString text;
    QItemSelectionRange range = resultsTable->selectionModel()->selection().first();
    for (auto i = range.top(); i <= range.bottom(); ++i)
    {
        QStringList rowContents;
        for (auto j = range.left(); j <= range.right(); ++j)
            rowContents << resultsTable->model()->index(i,j).data().toString();
        text += rowContents.join("\t");
        text += "\n";
    }
    QApplication::clipboard()->setText(text);
    menubool=true;
}
void interFace::copysummary()
{
    //#### copy selection in summmary table using context menu ###########

    QString text;
    if(summaryTable->selectionModel()->selection().isEmpty())
    {
       // qDebug()<<"in summary copy";
        return;
    }
    QItemSelectionRange range = summaryTable->selectionModel()->selection().first();
    if(range.isEmpty())
    {
        return;

    }
    for (auto i = range.top(); i <= range.bottom(); ++i)
    {
        QStringList rowContents;
        for (auto j = range.left(); j <= range.right(); ++j)
            rowContents << summaryTable->model()->index(i,j).data().toString();
        text += rowContents.join("\t");
        text += "\n";
    }
    QApplication::clipboard()->setText(text);
    // menubool=1;
}
void interFace::on_scaleButton_clicked()
{
    datasetCombo->setEditable(false);

    QVector<double> CD,CD1;
    QVector<double> WL,WL1;
    int j=0;
    double scalefactor = scaleBox->value();
    if(plot->selectedGraphs().count() != 0)
    {
        const QCPDataMap *dataMap =plot->selectedGraphs().first()->data();
        QMap<double, QCPData>::const_iterator i = dataMap->constBegin();
        while (i != dataMap->constEnd()) {
            CD << i.value().value*scalefactor;
            WL<<i.value().key;
            ++j;
            ++i;
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("No spectrum selected");
        msgBox.exec();
        return;
    }

    QListWidgetItem *item =listWidget->item(listWidget->currentRow());
    QString itemString =item->text();
    QString Sf;
    Sf = QString("%1").arg(scalefactor);
    fname =itemString +": scaled X " + Sf;

    //reverse data befor plotting
    for(int i=WL.size();i>0;--i)
    {
        WL1<<WL.at(i-1);
        CD1<<CD.at(i-1);
    }
    plotdata(WL1, CD1);
    fname.clear();

    // select the scaled spectrum
    listWidget->clearSelection();
    int row =listWidget->count();
    listWidget->setCurrentRow(row-1);

}

/*void interFace::on_xValbutton_clicked()
{
    //enable();
    // stackedWidget->setCurrentIndex(0)
    resultLabel->setVisible(true);
    // datasetCombo->setEditable(false);
    QMessageBox msgBox;
    msgBox.setText("This may take several mins");

    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret =msgBox.exec();
    switch (ret) {

    case QMessageBox::Ok:
        // Don't Save was clicked
        break;
    case QMessageBox::Cancel:
        // Cancel was clicked
        return;
    default:
        // should never be reached
        break;
    }
    disable();

    QString set = datasetCombo->currentText();
    Read_file *datasetFile =new Read_file;

    connect (datasetFile,SIGNAL(sendXVal(QList<QList<double>>)),this,SLOT(XvaloutPut(QList<QList<double>>)),Qt::AutoConnection);
    connect (datasetFile, SIGNAL(xValNMRSDlist(QList<double>)), this, SLOT(NMRSDlist(QList<double>)), Qt::AutoConnection);
    connect (datasetFile,SIGNAL(sendlabels(QStringList)),this,SLOT(outPutlabels(QStringList)),Qt::AutoConnection);
    connect (datasetFile,SIGNAL(proteinList(QStringList)),this,SLOT(outPutProteins(QStringList)),Qt::AutoConnection);

    // progressBar->setVisible(true);
    Xvalbool =1;

    datasetFile->XValfiles(set, RandD, Xvalbool);
    enable();
}*/
/*void interFace::XvaloutPut(QList<QList<double>> Xvalstats)
{
    // for(int i =0; i<matlist.size(); ++i)
  //  qDebug()<<"r"<<endl<< Xvalstats.last();
  //  qDebug()<<"z"<<endl<< Xvalstats.at(1);
//    qDebug()<<"d"<<endl<<Xvalstats.first();
    // Rmsd *r = new Rmsd;
    //  r->rmsdXval(matlist);
    stackedWidget->setCurrentIndex(1);
    pushButton->setEnabled(false);
    frontlabel->setVisible(TRUE);
    backlabel->setVisible(FALSE);


    int solutionIndex=0;
    QList<double> Val;
    resultsTable->clear();
    setupXValTable();
    resultsTable->setRowCount(Xvalstats.first().size());
    //resultsTable->setRowCount(protlist.size());
    for(int j=0; j<Xvalstats.size(); ++j)
    {
        Val=Xvalstats.at(j);
        for(int i=0; i<Val.size(); ++i)
        {
            // add row headers
            QTableWidgetItem *lblItem =new QTableWidgetItem;
            lblItem->setData(Qt::EditRole, labels.at(i));
            resultsTable->setVerticalHeaderItem(i+solutionRowIndex, lblItem );
            //add data
            double d;

            d = Val.at(i);


            d = roundf(d*10000)/10000;

            QTableWidgetItem *solItem = new QTableWidgetItem;
            solItem->setData(Qt::EditRole, d);
            resultsTable->setItem(i,solutionIndex,solItem);
            solItem->setTextAlignment(Qt::AlignLeft);
            solItem->setTextAlignment(Qt::AlignVCenter);
        }
        solutionIndex++;
    }

    enable();
}
void interFace::setupXValTable()
{
    QStringList headers;
    headers<< "r"<<"RMSD"<<"zeta"<<"proteins"<<"NRMSD";

    resultsTable->setColumnCount(5);
    resultsTable->setHorizontalHeaderLabels(headers);
    // resultsTable->horizontalHeader()->setStyleSheet("color: blue");

    QFont font;// = resultsTable->horizontalHeader()->font();
    font.setPointSize(10);
    font.setBold(true);
    resultsTable->horizontalHeader()->setFont( font );
    resultsTable->horizontalHeader()->setStyleSheet(  "QHeaderView::section{"
                                                      "border-top:0px solid #D8D8D8;"
                                                      "border-left:0px solid #D8D8D8;"
                                                      "border-right:1px solid #D8D8D8;"
                                                      "border-bottom: 1px solid #D8D8D8;"
                                                      "background-color:white;"
                                                      "padding:4px;"
                                                      "}");
    QFont font2 = resultsTable->verticalHeader()->font();
    font2.setPointSize( 9 );
    font2.setBold(true);
    resultsTable->verticalHeader()->setFont( font2 );
    resultsTable->resizeColumnsToContents();
    //  resultsTable->setSortingEnabled(true);
    //  resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);


}*/
void interFace::NMRSDlist(QList<double> nmrsdList)
{
   // QColor red;

    qDebug()<<"nmrsd"<<nmrsdList;
    int count =resultsTable->rowCount()-1;
    qDebug()<<"nmrsdlist"<<nmrsdList;

    for(int i=0 ;i<nmrsdList.size(); ++i)
    {
         double checkNmrsd= nmrsdList.at(i);

        QTableWidgetItem *solItem = new QTableWidgetItem;
        solItem->setData(Qt::DisplayRole, nmrsdList.at(i));//,intValue
        resultsTable->setItem(i,4,solItem);



        if(i>count)
        {
            QTableWidgetItem *solItem = new QTableWidgetItem;
            solItem->setData(Qt::DisplayRole, nmrsdList.at(i));

            resultsTable->insertRow(resultsTable->rowCount());

            resultsTable->setItem(i,4,solItem);
        }
        if(checkNmrsd==0)
        {
            //colur red
            resultsTable->item(i, 4)->setBackgroundColor(Qt::red);
            // solItem->setData(Qt::BackgroundRole,Qt::red);
        }
        if(checkNmrsd<0.0001 && checkNmrsd>0)
        {
            //colour amber
            resultsTable->item(i, 4)->setBackgroundColor(Qt::yellow);
        }

    }
    resultsTable->resizeColumnsToContents();
}
void interFace::outPutProteins(QStringList prots)
{
    // int count =resultsTable->rowCount();
    qDebug()<<"protlist"<<prots;
    // protlist=prots;
    for(int i=0 ;i<prots.size(); ++i)
    {
        QTableWidgetItem *solItem = new QTableWidgetItem;
        solItem->setData(Qt::EditRole, prots.at(i));

        resultsTable->setItem(i,3,solItem);
    }
    resultsTable->resizeColumnsToContents();
}
void interFace::on_addDatasetButton_clicked()
{
    //#### add a datset to the dataset list #############################

    DialogDatset *dlg = new DialogDatset;
    dlg->setModal(true);
    if(dlg->exec()==1)
    {
        QString dataset =dlg->getDataset();
        datasetCombo->setEditable(true);
        datasetCombo->setInsertPolicy(QComboBox::InsertAtTop);
        datasetCombo->addItem(dataset);
        datasetCombo->setEditable(false);

        datasetCombo->setCurrentIndex(datasetCombo->count()-1);
        return;
    }
    else {
        return;
    }

}
void interFace::on_pushButton_2_clicked()
{

    //############# remove a dataset from the list #############################

    QMessageBox msg;
    msg.setText("You are about to remove a dataset");
    msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

    int ret =msg.exec();
    switch (ret) {

    case QMessageBox::Ok:
        // delete
        break;
    case QMessageBox::Cancel:
        // Cancel was clicked
        return;
    default:
        // should never be reached
        break;
    }
    int index= datasetCombo->currentIndex();
    datasetCombo->removeItem( index);
}


void interFace::enable()
{
    resultsBox->setEnabled(true);
    analysisBox->setEnabled(true);
    //xValBox->setEnabled(true);
    fileBox->setEnabled(true);
}
void interFace::disable()
{
    resultsBox->setEnabled(false);
    analysisBox->setEnabled(false);
   // xValBox->setEnabled(false);
    fileBox->setEnabled(false);
}
