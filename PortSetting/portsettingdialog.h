#ifndef PORTSETTINGDIALOG_H
#define PORTSETTINGDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QTextEdit>
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
    void onClearBtnClicked();

    void onDataRecv();
    void onHexShowRadioBtnToggled(bool checked);

private:
    void initUI();
    void setup();
    void portInit();
    void log(const char *msg);
    void updateRecvUI(const char *buff, quint32 length);
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
    volatile bool mIsOpen;
    volatile bool mIsHexShow;
    QString mUartName;
    BaudRateType mBaudRate;

    /**
     * UI
     */
    QPushButton *mOpenCloseBtn;
    QPushButton *mSendBtn;
    QPushButton *mClearBtn;
    QRadioButton *mHexShowBtn;

    QTextEdit *mUartRecvText;
};

#endif // PORTSETTINGDIALOG_H
