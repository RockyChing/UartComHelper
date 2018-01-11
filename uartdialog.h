#ifndef UARTDIALOG_H
#define UARTDIALOG_H

#include <QDialog>
#include <windows.h>
#include "win_qextserialport.h"

namespace Ui {
class UartDialog;
}

class UartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UartDialog(QWidget *parent = 0);
    ~UartDialog();

private slots:
    void onOpenBtnClicked();
    void onCloseBtnClicked();
    void onSendBtnClicked();

    void onDataRecv();
private:
    void setup();
    void uartInit();
    QString getUartName(int index, QString keyValue);

    void openBtnClicked();
    void openPort();
    void closePort();
    void sendData();
private:
    Ui::UartDialog *ui;
    Win_QextSerialPort *mUartCom;

    /**
     * Automatic recognition of serial port
     */
    QStringList mUartNameList;
    HKEY mHkey;
    LPCWSTR mSubKey;
    wchar_t mKeyName[256];
    char mKeyValue[256];
    DWORD mKeySize,mType,mValueSize;
    int mIndex;




    bool mIsOpen;
    QString mUartName;
    BaudRateType mBaudRate;
};

#endif // UARTDIALOG_H
