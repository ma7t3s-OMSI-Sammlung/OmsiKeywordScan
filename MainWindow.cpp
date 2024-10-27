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
    _scanner->setExtensions({"cfg", "bus", "sco", "sli"});
}

MainWindow::~MainWindow() {
    delete ui;
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

    for(Command *c : _commands) {
        ui->lwResults->addItem(c->command);
    }
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

