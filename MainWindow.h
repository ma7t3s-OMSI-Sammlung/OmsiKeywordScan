#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "ThreadScanner.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbSelectDir_clicked();

    void reciveStatusUpdate(int dirCount, int fileCount, QString currentFile);
    void finishScan();

    void on_pbStart_clicked();

    void on_lwResults_currentRowChanged(int currentRow);

private:
    Ui::MainWindow *ui;
    ThreadScanner *_scanner;

    QList<Command *> _commands;
};
#endif // MAINWINDOW_H
