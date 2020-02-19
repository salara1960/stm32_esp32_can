/*
"Мы в фортеции живем,
Хлеб едим и воду пьем;
А как лютые враги
Придут к нам на пироги,
Зададим гостям пирушку:
Зарядим картечью пушку."
   А.С.Пушкин
*/


#include "mainwindow.h"
#include <QApplication>
#include <QLockFile>
#include <QDir>
#include <QMessageBox>


int main(int argc, char *argv[])
{
int cerr = 0;
QString errStr = "", cerrStr;
QString ws_srv = "192.168.0.103:8899";


    setlocale(LC_ALL,"UTF8");

    try {

        QLocale loc(QLocale::Russian, QLocale::RussianFederation);
        QLocale::setDefault(loc);

        QApplication app(argc, argv);

        QLockFile lockFile(QDir::temp().absoluteFilePath("wsClient.lock"));
        if (!lockFile.tryLock(100)){
            QMessageBox::warning(nullptr, "Attention !!!", "Program is already running");

            return 1;
        }

        if (argc > 1) {
            ws_srv.clear();
            ws_srv.append(argv[1]);
        }

        MainWindow wnd(nullptr, &ws_srv);
        wnd.show();

        app.exec();
    }

    catch (MainWindow::TheError(er)) {
        cerr = er.code;
        cerrStr.sprintf("%d", cerr);
        if (cerr > 0) {
            if (cerr & 1) errStr.append("Error starting timer_wait_data (" + cerrStr + ")\n");
        } else errStr.append("Unknown Error (" + cerrStr + ")\n");
        if (errStr.length() > 0) perror(reinterpret_cast<char *>(cerrStr.data()));

        return cerr;
    }
    catch (std::bad_alloc) {
        perror("Error while alloc memory via call function new\n");

        return -1;
    }

    return 0;
}
