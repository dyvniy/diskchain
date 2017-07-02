#ifndef DCFILEINFO_H
#define DCFILEINFO_H

#include <QString>
#include <QScopedPointer>
#include <QFileInfo>
#include <QStandardItemModel>

class DcFileInfo
{
public:
    // <hash>_<size>.txt
    long long size;
    QString name;
    QString hash; // md5 //&sha512
    // <hash>_<size>.dat
    //QByteArray data; - don't stay in memory

public:
    DcFileInfo();
    DcFileInfo(DcFileInfo&& a);
    ~DcFileInfo();

    void SearchInfo(QString name);
    bool Send(const QFileInfo& info, QStandardItemModel* model);
    bool Resv(const QString& sdir, const QString& hash);

    void Init(QStandardItemModel* model);

private:
    struct Impl;
    QScopedPointer<Impl> d;
};

#endif // DCFILEINFO_H
