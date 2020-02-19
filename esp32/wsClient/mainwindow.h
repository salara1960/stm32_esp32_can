#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <inttypes.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __WIN32
#include <windows.h>
#else
#include <arpa/inet.h>
#include <endian.h>
#endif

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QByteArray>
#include <QUrl>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QtWebSockets/QWebSocket>


//********************************************************************************

#define SET_DEBUG

#define max_buf 2048

//********************************************************************************

namespace Ui {
class MainWindow;
}

//********************************************************************************
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    class TheError {
        public :
            int code;
            TheError(int);
    };

    explicit MainWindow(QWidget *parent = nullptr, QString *srv = nullptr);
    ~MainWindow();
    void timerEvent(QTimerEvent *event);

public slots:
    void LogSave(const char *, const QByteArray &, bool, bool);
    void About();
    void clrLog();

private slots:
    void on_answer_clicked();
    void on_connect();
    void on_disconnect();
    void on_error(QAbstractSocket::SocketError SErr);
    void onTextMessageReceived(QString message);
    void CliDone();

private:
    Ui::MainWindow *ui;
    int tmr_sec, MyError;
    QWebSocket wsCli;
    QString url;
};

#endif // MAINWINDOW_H
