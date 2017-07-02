#ifndef MYDEBUG_H
#define MYDEBUG_H

QString aliteraciua(QString s);

void messageToFile(QtMsgType type,
                   const QMessageLogContext& context,
                   const QString& msg);

#endif // MYDEBUG_H
