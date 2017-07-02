#include "dcfileinfo.h"

#include <QFile>
#include <QDir>
#include <QCryptographicHash>
#include <QStandardItemModel>
#include <QFile>
#include <QTextStream>
#include <QDataStream>

const QString appDir = QDir::currentPath();
const QString defPath = "C:/diskchain/";

struct DcFileInfo::Impl
{
    Impl(){}
    ~Impl(){}
    QByteArray fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm);
    void LoadList(QStandardItemModel* model);
    QString FromByteArray(const QByteArray& data);
    void send(QFileInfo info, QStandardItemModel* model);
    void resv(const QString& sdir, QString hash);
    DcFileInfo readDci(QFileInfo info);
};

DcFileInfo::DcFileInfo()
    :d(new Impl())
{
    QDir def(defPath);
    if (!def.exists())
        def.mkdir(defPath);
}

DcFileInfo::DcFileInfo(DcFileInfo&& a)
    : d(new Impl())
    , size(a.size)
    , name(a.name)
    , hash(a.hash)
{
}

DcFileInfo::~DcFileInfo()
{

}

// search info
void DcFileInfo::SearchInfo()
{

}

// from local disk to DiskChain network
bool DcFileInfo::Send(const QFileInfo& info, QStandardItemModel* model)
{
    d->send(info, model);
    return true;
}

// from DiskChain network to local disk
bool DcFileInfo::Resv(const QString& sdir, const QString& hash)
{
    d->resv(sdir, hash);
    return true;
}

// load list of stored files
void DcFileInfo::Init(QStandardItemModel* model)
{
    QDir dir(defPath);
    foreach (QFileInfo info, dir.entryInfoList(QStringList() << "*")) {
        if (info.suffix() == "txt"){
            qDebug() << info.absoluteFilePath();
            DcFileInfo dci = d->readDci(info);

            QStandardItem* item = new QStandardItem(dci.name);
            item->setEditable(false);
            QString nameOnly = info.fileName().split(".")[0];
            item->setData(nameOnly);
            model->appendRow(item);
        }
    }
}

//======= Impl ========

// Returns empty QByteArray() on failure.
QByteArray DcFileInfo::Impl::fileChecksum(const QString &fileName,
                        QCryptographicHash::Algorithm hashAlgorithm)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    return QByteArray();
}

QString DcFileInfo::Impl::FromByteArray(const QByteArray& data)
{
    QMap<QString, int> enc {{"UTF-16",1015}, {"UTF-16LE",1014}, {"UTF-16BE",1013}, {"UTF-8",106}};
    return QTextCodec::codecForMib(enc["UTF-8"])->toUnicode(data);
}


void DcFileInfo::Impl::LoadList(QStandardItemModel *model)
{
}

void DcFileInfo::Impl::send(QFileInfo info, QStandardItemModel* model)
{
    const QString filename = info.absoluteFilePath();
    QByteArray hashMd5 = fileChecksum(filename, QCryptographicHash::Algorithm::Md5);
    QString strMd5 = FromByteArray(hashMd5.toHex());
    //QByteArray hashSha512 = fileChecksum(filename, QCryptographicHash::Algorithm::Sha3_512);
    //QString strSha512;// = FromByteArray(hashSha512.toHex());
    const QString hash = strMd5;
    const QString sendFileName = defPath + hash + "_" + QObject::tr("%1").arg(info.size());
    qDebug() << sendFileName;
    QFile::copy(filename, sendFileName + ".dat");
    QFile file(sendFileName + ".txt");
    if (!file.open(QFile::WriteOnly))
        return;
    QTextStream ts(&file);
    ts << filename << "\n";

    QStandardItem* item = new QStandardItem(filename);
    item->setEditable(false);
    item->setData(sendFileName);
    model->appendRow(item);
}

void DcFileInfo::Impl::resv(const QString& sdir, QString hash)
{
    QDir dir(defPath);
    qDebug() << "hash " << hash;
    foreach (QFileInfo info, dir.entryInfoList(QStringList() << "*"+hash+"*")) {
        qDebug() << "inside ";
        if (info.suffix() == "txt"){
            qDebug() << "resv " << info.absoluteFilePath();
            DcFileInfo dci = readDci(info);
            QFile::copy(defPath + hash + ".dat", dci.name);
        }
    }
}

DcFileInfo DcFileInfo::Impl::readDci(QFileInfo info)
{
    DcFileInfo dci;
    QString nameOnly = info.fileName().split(".")[0];
    QStringList hashlen = nameOnly.split("_");
    dci.hash = hashlen[0];
    bool ok = true;
    dci.size = hashlen[1].toLongLong(&ok);
    QFile file(info.absoluteFilePath());
    if (!file.open(QFile::ReadOnly))
        return DcFileInfo();
    QTextStream ts(&file);
    dci.name = ts.readLine();
    return dci;
}
