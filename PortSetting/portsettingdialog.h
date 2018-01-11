#ifndef ROBOT_CLEANER_DIALOG_H
#define ROBOT_CLEANER_DIALOG_H

#include <QDialog>
#include <windows.h>
#include "win_qextserialport.h"

namespace Ui {
class PortSettingDialog;
}

class PortSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PortSettingDialog(QWidget *parent = 0);
    ~PortSettingDialog();

private slots:
    void onOpenCloseBtnClicked();
    void onCloseBtnClicked();
    void onSendBtnClicked();

    void onDataRecv();
private:
    void setup();
    void portInit();
    QString getUartName(int index, QString keyValue);
    BaudRateType getBaudRate(quint32 baudrate);

    void openCloseBtnHandler();
    void openPort();
    void closePort();
    void sendData();

private:
    Ui::PortSettingDialog *ui;
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

    /**
     * Configuration uart
     */
    bool mIsOpen;
    QString mUartName;
    BaudRateType mBaudRate;
};

#endif // ROBOT_CLEANER_DIALOG_H
