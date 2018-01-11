#include "mainwindow.h"
#include <QApplication>
#include "./PortSetting/PortSettingDialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    PortSettingDialog portSettingDlg;

    if (portSettingDlg.exec() == QDialog::Accepted) {
        w.show();

        return a.exec();
    }

    return 0;
}
