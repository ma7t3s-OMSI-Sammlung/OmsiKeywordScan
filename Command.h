#ifndef COMMAND_H
#define COMMAND_H

#include <QString>
#include <QStringList>

#include <QJsonObject>
#include <QJsonArray>

class Command {
public:
    Command(QString command) : command(command) {}
    bool operator==(Command &other) {
        return command.toLower() == other.command.toLower();
    }

    bool operator<(Command &other) {
        return command.toLower() < other.command.toLower();
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj.insert("command", command);

        QJsonArray jFiles;
        for(QString file : files)
            jFiles.append(file);

        obj.insert("files", jFiles);
        return obj;
    }

    QString command;
    QStringList files;
};

#endif // COMMAND_H
