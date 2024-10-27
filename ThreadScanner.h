#ifndef THREADSCANNER_H
#define THREADSCANNER_H

#include <QObject>
#include <QThread>
#include <QDir>
#include <QFile>
#include <QFileInfoList>

#include "Command.h"

class ThreadScanner : public QThread {
    Q_OBJECT
public:
    explicit ThreadScanner(QObject *parent = nullptr);

    ~ThreadScanner();

    void run() override;

    void scanDir(const QString &dirPath);
    void scanFile(const QString &filePath);

    void setDir(const QString &);
    void setExtensions(const QStringList &);
    QList<Command *> result() const;

protected:
    Command *findCommand(const QString &command);

signals:
    void statusUpdate(int dirCount, int fileCount, QString currentFile);

private:
    QString _dir;
    QStringList _extensions;
    QList<Command *> _commands;

    int _dirCounter, _fileCounter;
};

#endif // THREADSCANNER_H
