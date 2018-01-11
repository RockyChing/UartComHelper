#include "uartdialog.h"
#include "ui_uartdialog.h"
#include <QMessageBox>
#include <QSettings>

UartDialog::UartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UartDialog)
{
    ui->setupUi(this);
    mIsOpen = false;
    mUartCom = NULL;

    setup();
    uartInit();
}

UartDialog::~UartDialog()
{
    if (mUartCom) {
        mUartCom->close();
        delete mUartCom;
    }
    delete ui;
}

void UartDialog::setup()
{
    connect(ui->uartOpenBtn, SIGNAL(clicked()), this, SLOT(onOpenBtnClicked()));
    connect(ui->uartCloseBtn, SIGNAL(clicked()), this, SLOT(onCloseBtnClicked()));
    connect(ui->uartSendBtn, SIGNAL(clicked()), this, SLOT(onSendBtnClicked()));

    ui->uartRecvText->setReadOnly(true);
}

/**
 * Automatic recognition of serial port
 */
void UartDialog::uartInit()
{
    ui->baudRateCombo->setCurrentIndex(6);

    QString path = "HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM\\";
    QSettings *settings = new QSettings(path, QSettings::NativeFormat);
    QStringList keyList = settings->allKeys();
    const int num = (int) keyList.size();
    QString value;
    for (int i = 0; i < num; i ++) {
        value = getUartName(i, "value");
        mUartNameList << value;
    }

    ui->portNameCombo->addItems(mUartNameList);
    if (num >= 3)
        ui->portNameCombo->setCurrentIndex(2);
}

/**
 * 访问Windows注册表，获取串口号
 * @index 索引号
 * @keyValue 选择返回值应该为key还是value
 */
QString UartDialog::getUartName(int index, QString keyValue)
{
    QString result;
    if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),
                       0, KEY_READ, &mHkey) != 0) {
        QString err = "Cannot open regedit!"; // 无法打开注册表时返回error
        return err;
    }

    QString key;
    QString msg;
    QString keyVal;

    mIndex = index;
    mKeySize = sizeof(mKeyName);
    mValueSize = sizeof(mKeyValue);
    if(::RegEnumValue(mHkey, mIndex, mKeyName, &mKeySize, 0,
                      &mType, (BYTE*)mKeyValue, &mValueSize) == 0) { // 列举键名和值
        for (unsigned int i = 0; i < mKeySize; i ++) { // 读取键名
            msg = mKeyName[i];
            key.append(msg);
        }

        for(unsigned int j = 0; j < mValueSize; j ++) { // 读取键值
            if(mKeyValue[j] != 0x00) {
                keyVal.append(mKeyValue[j]);
            }
            if(keyValue == "key")
                result = key;
            else if(keyValue == "value")
            {
                result = keyVal;
            }
        }
    } else {
        result = "nokey";
    }

    ::RegCloseKey(mHkey); // 关闭注册表
    return result;
}

void UartDialog::openBtnClicked()
{
    if (mIsOpen) {
        // opened
        QMessageBox::warning(this, tr("Warning"),
                             tr("COM already opened!"),
                             QMessageBox::Yes);
    } else {
        mUartName = "\\\\.\\" + ui->portNameCombo->currentText();
        int baudRate = ui->baudRateCombo->currentText().toInt();
        switch(baudRate) {
        case 2400:
            mBaudRate = BAUD2400;
            break;
        case 4800:
            mBaudRate = BAUD4800;
            break;
        case 9600:
            mBaudRate = BAUD9600;
            break;
        case 19200:
            mBaudRate = BAUD19200;
            break;
        case 38400:
            mBaudRate = BAUD38400;
            break;
        case 57600:
            mBaudRate = BAUD57600;
            break;
        case 115200:
            mBaudRate = BAUD115200;
            break;
        case 230400:
            mBaudRate = BAUD230400;
            break;
        case 380400:
            mBaudRate = BAUD380400;
            break;
        default:
            break;
        }
        openPort();
    }
}

void UartDialog::openPort()
{
    mUartCom = new Win_QextSerialPort(mUartName, QextSerialBase::EventDriven);
    if (mUartCom->open(QIODevice::ReadWrite)) {
        mUartCom->setBaudRate(mBaudRate);
        mUartCom->setDataBits(DATA_8);
        mUartCom->setFlowControl(FLOW_OFF);
        mUartCom->setParity(PAR_NONE);
        mUartCom->setStopBits(STOP_1);
        mUartCom->setTimeout(500);
    }

    mIsOpen = true;
    connect(mUartCom, SIGNAL(readyRead()), this, SLOT(onDataRecv()));
}

void UartDialog::closePort()
{
    if (!mIsOpen) {
        // opened
        QMessageBox::warning(this, tr("Warning"),
                             tr("COM not opened"),
                             QMessageBox::Yes);
    } else {
        mUartCom->close();
        delete mUartCom;
        mUartCom = NULL;
    }
}

void UartDialog::sendData()
{
    qDebug("sendData");
    mUartCom->write("Write", strlen("Write"));
}

void UartDialog::onDataRecv()
{
    qDebug("onDataRecv");
    if (mUartCom->bytesAvailable() == 0)
        return;

    QByteArray readData = mUartCom->readAll();
    char  *rxBuffer;
    rxBuffer = readData.data();
    QString recvData = QString(QLatin1String(rxBuffer));
    ui->uartRecvText->append(recvData);
}

/**
 * SLOTS
 */
void UartDialog::onOpenBtnClicked()
{
    qDebug("onOpenBtnClicked");
    openBtnClicked();
}

void UartDialog::onCloseBtnClicked()
{
    qDebug("onCloseBtnClicked");
    closePort();
}

void UartDialog::onSendBtnClicked()
{
    qDebug("onSendBtnClicked");
    if (NULL == mUartCom) {
        qDebug("port not opened");
        return;
    }
    mUartCom->write("Test", 4);
}
