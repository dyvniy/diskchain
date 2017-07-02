#include "pch.h"
#include "mydebug.h"

QString aliteraciya(QString s)
{
    const QString rus = QString::fromStdWString(L"абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЭИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ");
    const QVector<QString> ali = {
        "a","b","v", "g","d","ye","yo","zh","z","i",
        "y","k","l", "m","n", "o", "p",  "r", "s","t",
        "u","f","kh","c","ch","sh","csh","","i","",
        "e","yu","ya",
        "A","B","V","G","D","Ye","Yo","Zh","Z","I",
        "Y","K","L","M","N","O", "P",
        "R","S","T","U","F","Kh","C","Ch","Sh","Csh",
        "","i","","e","yu","ya"
    };
    QString s2;
    foreach (QChar ch, s)
    {
        int i=0;
        bool found = false;
        foreach (QChar chr, rus)
        {
           if (ch == chr)
           {
               s2 += ali[i];
               found = true;
               break;
           }
           i++;
        }
        if (!found)
            s2 += ch;
    }
    return s2;
}

void messageToFile(QtMsgType type,
                   const QMessageLogContext& context,
                   const QString& msg)
{
    QReadWriteLock lock;

    QFile file("protocol.log");
    if (!file.open(QIODevice::WriteOnly |
                   QIODevice::Text |
                   QIODevice::Append))
        return;

    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("UTF-8"));
    QString msg1 = QTime::currentTime().toString("hh:mm:ss ");
    switch (type) {
    case QtInfoMsg:
        msg1 += "Info: " + msg;
        break;
    case QtDebugMsg:
        msg1 += "Debug: " + msg;
        break;
    case QtWarningMsg:
        msg1 += "Warning: " + msg;
        break;
    case QtCriticalMsg:
        msg1 += "Critical: " + msg;
        break;
    case QtFatalMsg:
        msg1 += "Fatal: " + msg;
        break;
    }
    out << msg1 << ", " << context.file << " " << context.line << endl;

    QTextStream ts( stdout );
    ts << aliteraciya(msg1) << endl;
}

// how to use
//int main(int argc, char *argv[])
//{
//#ifdef QT_DEBUG
//    qInstallMessageHandler(messageToFile);
//#endif
//}

//CONFIG   += c++14 precompile_header j8
//# Use Precompiled headers (PCH)
//PRECOMPILED_HEADER  = pch.h


