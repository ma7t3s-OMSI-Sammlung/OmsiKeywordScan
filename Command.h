#ifndef COMMAND_H
#define COMMAND_H

#include <QString>
#include <QStringList>

class Command {
public:
    Command(QString command) : command(command) {}
    bool operator==(Command &other) {
        return command.toLower() == other.command.toLower();
    }

    bool operator<(Command &other) {
        return command.toLower() < other.command.toLower();
    }

    QString command;
    QStringList files;
};

#endif // COMMAND_H
