#include "ThreadScanner.h"

ThreadScanner::ThreadScanner(QObject *parent) :
    QThread(parent) {

    _dirCounter  = 0;
    _fileCounter = 0;
}

ThreadScanner::~ThreadScanner() {
    qDeleteAll(_commands);
}

void ThreadScanner::run() {
    QDir dir(_dir);
    if(!dir.exists()) {
        qWarning() << "The directory" << _dir << "does not exist";
        return;
    }

    scanDir(_dir);

    std::sort(_commands.begin(), _commands.end(), [](Command *a, Command *b){return *a < *b;});
}

void ThreadScanner::scanDir(const QString &dirPath) {
    QDir dir(dirPath);

    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    QFileInfoList subdirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for(QFileInfo info : files) {
        QString filePath = info.absoluteFilePath();

        // exclude options.cfg
        if(filePath.endsWith("options.cfg"))
            continue;

        for(QString currentExtension : _extensions) {
            if(filePath.endsWith(currentExtension)) {
                scanFile(filePath);
                _fileCounter++;
                emit statusUpdate(_dirCounter, _fileCounter, filePath);
                break;
            }
        }
    }

    _dirCounter++;

    for(QFileInfo info : subdirs) {
        QString dirPath = info.absoluteFilePath();
        scanDir(dirPath);
    }
}

void ThreadScanner::scanFile(const QString &filePath) {
    QFile file(filePath);
    if(!file.open(QFile::ReadOnly))
        return;

    QTextStream s(&file);
    QString foundCommands;

    while(!s.atEnd()) {
        QString line = s.readLine();

        if(line.startsWith("[") && line.endsWith("]")) {
            Command *cmd = findCommand(line);
            if(!cmd) {
                cmd = new Command(line);
                _commands << cmd;
            }

            QString fixedFilePath = filePath;
            fixedFilePath.remove(_dir + "/");

            if(!foundCommands.contains(line)) {
                cmd->files.append(fixedFilePath);
                foundCommands.append(line);
            }
        }
    }

    file.close();
}

void ThreadScanner::setDir(const QString &dir) {
    _dir = dir;
}

void ThreadScanner::setExtensions(const QStringList &list) {
    for(QString current : list) {
        _extensions << "." + current;
    }
}

QList<Command *> ThreadScanner::result() const {
    return _commands;
}

Command *ThreadScanner::findCommand(const QString &command) {
    for(Command *c : _commands) {
        if(c->command == command)
            return c;
    }

    return nullptr;
}
