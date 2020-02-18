#include "mainwindow.h"
#include "ui_mainwindow.h"


//******************************************************************************************************
//
//       web socket Client
//
//******************************************************************************************************

//const QString vers = "0.1";//18.02.2020
//const QString vers = "0.2";//18.02.2020
const QString vers = "0.3";//18.02.2020


const QString title = "webSocketClient";

const QString main_pic    = "png/linux.png";
const QString salara_pic  = "png/salara.png";


//******************************************************************************************************

MainWindow::MainWindow(QWidget *parent, QString *srv) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    url = *srv;
    ui->srv->setText(url);

    MyError = 0;

    this->setWindowIcon(QIcon(main_pic));
    this->setWindowOpacity(0.93);//set the level of transparency

    tmr_sec = startTimer(1000);// 1 sec.
    if (tmr_sec <= 0) {
        MyError |= 1;//start_timer error
        throw TheError(MyError);
    }

    //Menu
    connect(ui->actionVERSION,    &QAction::triggered, this, &MainWindow::About);
    connect(ui->actionCONNECT,    &QAction::triggered, this, &MainWindow::on_connect);
    connect(ui->actionDISCONNECT, &QAction::triggered, this, &MainWindow::on_disconnect);
    connect(ui->actionCLEAR,      &QAction::triggered, this, &MainWindow::clrLog);
    connect(ui->actionExit, &QAction::triggered, this, &QApplication::quit);

    ui->actionCONNECT->setEnabled(true);
    ui->actionDISCONNECT->setEnabled(false);

    ui->status->clear();
    ui->stx->setEnabled(false);

}
//-----------------------------------------------------------------------
MainWindow::~MainWindow()
{
    killTimer(tmr_sec);
    delete ui;
}
//--------------------------------------------------------------------------------

MainWindow::TheError::TheError(int err) { code = err; }//error class descriptor

//--------------------------------------------------------------------------------
void MainWindow::clrLog()
{
    ui->log->clear();
}
//--------------------------------------------------------------------------------
void MainWindow::About()
{
    QString st ="\nwsClient version " + vers + "\nused Qt v.";
    st.append(QT_VERSION_STR);

    QMessageBox box;
    box.setStyleSheet("background-color: rgb(208, 208, 208);");
    box.setIconPixmap(QPixmap(salara_pic));
    box.setText(st);
    box.setWindowTitle("About");
    box.exec();
}
//-----------------------------------------------------------------------
void MainWindow::LogSave(const char *func, const QByteArray & st, bool rxd, bool pr)
{
    QString fw;
    if (pr) {
        time_t ict = QDateTime::currentDateTime().toTime_t();
        struct tm *ct = localtime(&ict);
        fw.sprintf("%02d.%02d.%02d %02d:%02d:%02d | ", ct->tm_mday, ct->tm_mon+1, ct->tm_year+1900, ct->tm_hour, ct->tm_min, ct->tm_sec);
    }
    if (func) {
        fw.append("[");
        fw.append(func);
        fw.append("] ");
    }
    if (rxd) fw.append("> ");
        else fw.append("< ");
    fw.append(st);

    ui->log->append(fw);//to log screen
}
//-----------------------------------------------------------------------
void MainWindow::on_connect()
{
    url = ui->srv->text();
    wsCli.open(QUrl("ws://" + url));

    connect(&wsCli, &QWebSocket::textMessageReceived, this, &MainWindow::onTextMessageReceived);
    //connect(&wsCli, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbstractSocket::SocketError)));
    /**/
    connect(&wsCli, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), [=](QAbstractSocket::SocketError err) {
        on_error(err);
    });
    /**/

    ui->status->clear();
    ui->status->setText("Web client connected to server " + url);

    ui->actionCONNECT->setEnabled(false);
    ui->actionDISCONNECT->setEnabled(true);

    ui->stx->setEnabled(true);
    ui->stx->setText("{\"auth\":\"");

    ui->srv->setEnabled(false);
}
//-----------------------------------------------------------------------
void MainWindow::on_disconnect()
{

    ui->status->clear();
    ui->status->setText("Disconnect from server " + url);

    ui->actionCONNECT->setEnabled(true);
    ui->actionDISCONNECT->setEnabled(false);

    ui->stx->setEnabled(false);
    ui->srv->setEnabled(true);

    wsCli.close();
    wsCli.disconnect();
}
//-----------------------------------------------------------------------
void MainWindow::on_error(QAbstractSocket::SocketError SErr)
{

    ui->status->clear();
    QString m;
    m.sprintf("WebSocket error # %d ", SErr);
    m.append("(" + wsCli.errorString() + ")");
    ui->status->setText(m);

    ui->actionCONNECT->setEnabled(true);
    ui->actionDISCONNECT->setEnabled(false);

    ui->stx->setEnabled(false);
    ui->srv->setEnabled(true);

    wsCli.close();
    wsCli.disconnect();
}
//-----------------------------------------------------------------------
void MainWindow::on_answer_clicked()
{
    QString txt = ui->stx->text();
    wsCli.sendTextMessage(txt);

    QByteArray msg;
    msg.append(txt);
    LogSave(nullptr, msg, false, false);
}
//-----------------------------------------------------------------------
void MainWindow::onTextMessageReceived(QString message)
{
    QByteArray msg;
    msg.append(message);
    LogSave(nullptr, msg, true, false);
}
//-----------------------------------------------------------------------
void MainWindow::timerEvent(QTimerEvent *event)
{ 
    if (tmr_sec == event->timerId()) {
        time_t it_ct = QDateTime::currentDateTime().toTime_t();
        struct tm *ctimka = localtime(&it_ct);
        QString dt;
        dt.sprintf("%02d.%02d %02d:%02d:%02d",
                    ctimka->tm_mday,
                    ctimka->tm_mon + 1,
                    //ctimka->tm_year + 1900,
                    ctimka->tm_hour,
                    ctimka->tm_min,
                    ctimka->tm_sec);
        setWindowTitle(title +" ver. " + vers + "  |  " + dt);
    }
}
//-----------------------------------------------------------------------






