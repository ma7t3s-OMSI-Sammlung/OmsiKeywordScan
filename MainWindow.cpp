#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _scanner(new ThreadScanner(this)) {
    ui->setupUi(this);

    ui->progressBar->hide();

    connect(_scanner, &ThreadScanner::statusUpdate, this, &MainWindow::reciveStatusUpdate);
    connect(_scanner, &QThread::finished, this, &MainWindow::finishScan);

    // set extensions here
    _scanner->setExtensions({"cfg", "bus", "ovh", "cti", "dsc", "hof", "hum", "map", "ocu", "odr", "oft", "oop", "osn", "otp", "owt", "sco", "sli", "ttp", "ttl", "ttr"});
}

MainWindow::~MainWindow() {
    delete ui;
    qDeleteAll(_commands);
}

void MainWindow::on_pbSelectDir_clicked() {
    QString path = QFileDialog::getExistingDirectory(this, "", "");
    if(path.isEmpty())
        return;

    ui->leDirPath->setText(path);
    _scanner->setDir(path);
}

void MainWindow::reciveStatusUpdate(int dirCount, int fileCount, QString currentFile) {
    ui->lCurrentProgress->setText(tr("%1 directories and %2 files scanned.").arg(QString::number(dirCount)).arg(QString::number(fileCount)));
    ui->lCurrentFile->setText(currentFile);
}

void MainWindow::finishScan() {
    ui->progressBar->hide();
    _commands = _scanner->result();

    updateLists();
}

void MainWindow::updateLists() {
    ui->lwResults->clear();
    ui->lwFiles->clear();
    for(Command *c : _commands)
        ui->lwResults->addItem(c->command);
}

void MainWindow::on_pbStart_clicked() {
    ui->progressBar->show();
    _scanner->start();
}

void MainWindow::on_lwResults_currentRowChanged(int currentRow) {
    ui->lwFiles->clear();

    if(currentRow == -1)
        return;

    Command *c = _commands[currentRow];
    for(QString &path : c->files) {
        ui->lwFiles->addItem(path);
    }
}

void MainWindow::on_actionSaveJson_triggered() {
    QString path = QFileDialog::getSaveFileName(this, "", "", "JSON File (*.json)");
    if(path.isEmpty())
        return;

    QFile f(path);
    if(!f.open(QFile::WriteOnly)) {
        QMessageBox::critical(this, tr("Couldn't save file"), tr("Couldn't save file - no writing access."));
        return;
    }

    QJsonArray jArr;
    for(Command *c : _commands)
        jArr.append(c->toJson());

    QJsonDocument doc;
    doc.setArray(jArr);
    QByteArray str = doc.toJson();
    f.write(str);
    f.close();
}

void MainWindow::on_actionOpenJson_triggered() {
    QString path = QFileDialog::getOpenFileName(this, "", "", "JSON File (*.json)");
    if(path.isEmpty())
        return;

    QFile f(path);
    if(!f.open(QFile::ReadOnly)) {
        QMessageBox::critical(this, tr("Couldn't open file"), tr("Couldn't open file - no read access."));
        return;
    }

    QByteArray str = f.readAll();

    QJsonDocument doc = QJsonDocument::fromJson(str);
    QJsonArray jArr = doc.array();

    for(QJsonValue val : jArr) {
        QJsonObject jObj = val.toObject();

        Command *cmd = new Command(jObj.value("command").toString());
        QStringList files;
        for(QJsonValue fileVal : jObj.value("files").toArray())
            files << fileVal.toString();

        cmd->files = files;
        _commands << cmd;
    }

    f.close();
    updateLists();
}
