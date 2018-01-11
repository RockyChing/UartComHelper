#include "mainwindow.h"
#include <QApplication>
#include "uartdialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    UartDialog uartDlg;

    if (uartDlg.exec() == QDialog::Accepted) {
        w.show();

        return a.exec();
    }

    return 0;
}
