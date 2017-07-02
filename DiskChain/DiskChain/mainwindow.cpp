#include "pch.h"
#include "dcfileinfo.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QObject>
#include <QStandardItem>
#include <QTreeWidgetItem>


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
    });
    connect(ui->toLocalButton, &QPushButton::clicked, [&](){
        d->toLocal();
    });
    ui->treeView->setModel(d->model);
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
    model->setRootPath("/");
    proxy = new QSortFilterProxyModel(parent);
    fileModel = new QStandardItemModel(parent);
    dcModel = new QStandardItemModel(parent);

    DcFileInfo dci;
    dci.Init(dcModel);
    ui->treeViewDc->setModel(dcModel);
}

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
    if (dci.Send(curFileInfo, dcModel))
    {
        QDir dir(fileName);
        dir.remove(fileName);
    }
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

