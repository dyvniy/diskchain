#include "pch.h"
#include "dcfileinfo.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QObject>
#include <QStandardItem>
#include <QTreeWidgetItem>

const QString rootDir("../work/code/mainbugabr-114334");
//const QString rootPath("H:/prog/_myProg/work/acronis/work/code/mainbugabr-114334");
const QString rootPath("C:/work/code/mainlearn");

QDebug operator<<(QDebug debug, const QFileInfo infos)
{
    debug << infos.absoluteFilePath();
    return debug;
}

struct MainWindow::Impl
{
    QWidget *parent = nullptr;
    MainWindow* wnd = nullptr;
    Ui::MainWindow* ui = nullptr;

    QFileSystemModel* model = nullptr;
    QSortFilterProxyModel* proxy = nullptr;
    QStandardItemModel* fileModel = nullptr;
    QStandardItemModel* dcModel = nullptr;
    QLabel* statusLabel = nullptr;
    QLabel* statusLabelDc = nullptr;

    QFileInfo curFileInfo;
    QString curFileName;
    QString dcCurHash;
    QString dcCurFileName;

    void setup(QWidget *parent0, MainWindow* wnd0, Ui::MainWindow* ui0);

    //QFileInfoList loadFiles(const QString &startDir, QStringList filters);
    //void showFileInTree(QString fileName);

    QStringList onSelectChanged(const QModelIndex& index);
    QStringList onDcSelectChanged(const QModelIndex& index);
    void toDiskChain();
    void toLocal();

    ~Impl(){}
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , d(new Impl)
{
    ui->setupUi(this);
    d->setup(parent, this, ui);
    QLabel* lbl = new QLabel(".");
    ui->statusBar->addWidget(lbl);
    d->statusLabel = lbl;
    lbl = new QLabel(".");
    ui->statusBar->addWidget(lbl);
    d->statusLabelDc = lbl;

    auto msg = [](QString s0){
        auto s = s0 + tr(" действие");
        QMessageBox::warning(0, tr("Окно"), s);
        qDebug() << s;
    };

    connect(ui->toDcButton, &QPushButton::clicked, [&](){
        d->toDiskChain();
        //msg("toDC");
    });
    connect(ui->toLocalButton, &QPushButton::clicked, [&](){
        d->toLocal();
        //msg("toLocal");
    });
    /*
    QAction* act;
    act = new QAction(QIcon(":/icons/build.png"), tr("build"), this);
    ui->mainToolBar->addAction(act);
    connect(act, &QAction::triggered, [&](){
        qDebug() << QDir(rootDir).entryList(QStringList("*.*"));
        //ui->treeView->setModel(nullptr);
        //auto list = d->loadFiles(rootDir, QStringList("*.txt"));
        //qDebug() << list.count();
        msg("build");
    });
    */
    ui->treeView->setModel(d->model);
    if (!rootPath.isEmpty()) {
        const QModelIndex rootIndex = d->model->index(QDir::cleanPath(rootPath));
        if (rootIndex.isValid())
            ui->treeView->setRootIndex(rootIndex);
    }
    ui->treeView->setColumnWidth(0, 200);
    // double click
    connect(ui->treeView, &QTreeView::activated, [&](const QModelIndex& index){
        d->onSelectChanged(index);
    });
    // click
    connect(ui->treeView, &QTreeView::clicked, [&](const QModelIndex& index){
        QString fileName = d->onSelectChanged(index)[0];
        //d->showFileInTree(fileName);
    });

    ui->treeViewDc->setColumnWidth(0, 200);
    // double click
    connect(ui->treeViewDc, &QTreeView::activated, [&](const QModelIndex& index){
        d->onDcSelectChanged(index);
    });
    // click
    connect(ui->treeViewDc, &QTreeView::clicked, [&](const QModelIndex& index){
        d->onDcSelectChanged(index)[0];
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

//========= Impl =========//

void MainWindow::Impl::setup(QWidget *parent0, MainWindow *wnd0, Ui::MainWindow *ui0)
{
    parent = parent0;
    wnd = wnd0;
    ui = ui0;

    model = new QFileSystemModel(parent);
    model->setRootPath(rootPath);
    proxy = new QSortFilterProxyModel(parent);
    fileModel = new QStandardItemModel(parent);
    dcModel = new QStandardItemModel(parent);

    DcFileInfo dci;
    dci.Init(dcModel);
    ui->treeViewDc->setModel(dcModel);
}
/*
QFileInfoList MainWindow::Impl::loadFiles(const QString &startDir, QStringList filters)
{
    QDir dir(startDir);
    QFileInfoList list;

    foreach (QString file, dir.entryList(filters, QDir::Files))
    {
        auto info = QFileInfo(startDir + "/" + file);
        list += info;
    }

    foreach (QString subdir, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
        list += loadFiles(startDir + "/" + subdir, filters);
    return list;

}

void MainWindow::Impl::showFileInTree(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return;
    QTextStream ts(&file);//, QIODevice::ReadOnly);
    QString line;
    ui->treeViewDc->setModel(nullptr);
    fileModel->clear();
    //qDebug() << "before";
    QStandardItem* parentItem = new QStandardItem("");
    fileModel->appendRow(parentItem);
    QStandardItem* item = nullptr;
    while (!ts.atEnd())
    {
        line = ts.readLine();
        int iOpen = line.count('{');
        int iClose = line.count('}');
        if (iOpen < iClose)
            if (parentItem->parent() != fileModel->itemPrototype())
                parentItem = parentItem->parent();
        item = new QStandardItem(line);
        item->setEditable(false);
        parentItem->appendRow(item);
        if (iOpen > iClose)
            parentItem = item;
    }
    //qDebug() << "after";
    ui->treeViewDc->setModel(fileModel);
    ui->treeViewDc->expandAll();
}
*/
QStringList MainWindow::Impl::onSelectChanged(const QModelIndex &index)
{
    QModelIndexList list = ui->treeView->selectionModel()->selectedIndexes();
    QStringList sl;
    int row = -1;
    //foreach (QModelIndex index, list)
    {
        if (index.row()!=row && index.column()==0)
        {
            curFileInfo = model->fileInfo(index);
            curFileName = curFileInfo.absoluteFilePath();
            qDebug() << curFileName << '\n';
            sl << curFileName;
            statusLabel->setText(curFileName);
            row = index.row();
        }
    }
    return sl;
}

QStringList MainWindow::Impl::onDcSelectChanged(const QModelIndex &index)
{
    QModelIndexList list = ui->treeViewDc->selectionModel()->selectedIndexes();
    QStringList sl;
    int row = -1;
    //foreach (QModelIndex index, list)
    {
        if (index.row()!=row && index.column()==0)
        {
            dcCurFileName = dcModel->data(index).toString();
            dcCurHash = dcModel->data(index, Qt::UserRole+1).toString();
            qDebug() << dcCurFileName << '\n';
            sl << dcCurFileName;
            statusLabelDc->setText(dcCurFileName);
            row = index.row();
        }
    }
    return sl;
}

void MainWindow::Impl::toDiskChain()
{
    const QString fileName = curFileName;
    if (fileName=="" )
        return;
    if (curFileInfo.isDir())
        return;
    auto items = dcModel->findItems(fileName);
    if (items.count()>0)
        return;
    auto item = new QStandardItem(fileName);
    item->setEditable(false);
    DcFileInfo dci;
    dci.Send(curFileInfo, dcModel);
    ui->treeViewDc->setModel(dcModel);
}

void MainWindow::Impl::toLocal()
{
    const QString fileName = dcCurFileName;
    if (fileName=="" )
        return;
    QString curFileDir = curFileInfo.dir().absolutePath();
    DcFileInfo dci;
    dci.Resv(curFileDir, dcCurHash);
    ui->treeView->update();
}

