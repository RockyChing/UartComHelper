#include <QMessageBox>
#include <QSettings>
#include <windows.h>
#include "portsettingdialog.h"
#include "ui_portsettingdialog.h"


PortSettingDialog::PortSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortSettingDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("串口设置"));
    mIsOpen = false;
    mIsHexShow = false;
    mUartCom = NULL;
    mBaudRate = BAUD9600;

    initUI();
    setup();
    portInit();
}

PortSettingDialog::~PortSettingDialog()
{
    if (mUartCom) {
        mUartCom->close();
        delete mUartCom;
    }
    delete ui;
}

void PortSettingDialog::initUI()
{
    mOpenCloseBtn = ui->uartOpenCloseBtn;
    mSendBtn = ui->uartSendBtn;
    mClearBtn = ui->recvClearBtn;
    mHexShowBtn = ui->hexShowRadioBtn;
    mUartRecvText = ui->uartRecvText;
}

void PortSettingDialog::setup()
{
    connect(mOpenCloseBtn, SIGNAL(clicked()), this, SLOT(onOpenCloseBtnClicked()));
    connect(mSendBtn, SIGNAL(clicked()), this, SLOT(onSendBtnClicked()));
    connect(mClearBtn, SIGNAL(clicked()), this, SLOT(onClearBtnClicked()));
    connect(mHexShowBtn, SIGNAL(toggled(bool)), this, SLOT(onHexShowRadioBtnToggled(bool)));

    mUartRecvText->setReadOnly(true);
}

/**
 * Automatic recognition of serial port
 */
void PortSettingDialog::portInit()
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

void PortSettingDialog::log(const char *msg)
{
    QMessageBox::warning(this, tr("错误"), tr(msg));
}

void PortSettingDialog::updateRecvUI(const char *buff, quint32 length)
{
    {
        char buffer[255] = {'\0'};
        unsigned int offset = 0l, shift;
        int i;
        const unsigned char *ptr = (const unsigned char *) buff;
        while (offset < length) {
            int off;
            off = strlen(buffer);
            shift = off;
            snprintf(buffer + off, sizeof(buffer) - off, "%08x: ", offset);
            off = strlen(buffer);
            for (i = 0; i < 16; i++) {
                if (offset + i < length) {
                    snprintf(buffer + off, sizeof(buffer) - off, "%02x%c", ptr[offset + i], i == 7 ? '-' : ' ');
                }
                else{
                    snprintf(buffer + off, sizeof(buffer) - off, " .%c", i == 7 ? '-' : ' ');
                }
                off = strlen(buffer);
            }

            offset += 16;
            QString str = QString::fromLocal8Bit((const char *)(buffer + shift), int (off - shift));
            mUartRecvText->append(str);
            //qDebug("== %s", buffer + shift);
        }
    }
}

/**
 * 访问Windows注册表，获取串口号
 * @index 索引号
 * @keyValue 选择返回值应该为key还是value
 */
QString PortSettingDialog::getUartName(int index, QString keyValue)
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

BaudRateType PortSettingDialog::getBaudRate(quint32 baudrate)
{
    BaudRateType br;
    switch(baudrate) {
    case 2400:
        br = BAUD2400;
        break;
    case 4800:
        br = BAUD4800;
        break;
    case 9600:
        br = BAUD9600;
        break;
    case 19200:
        br = BAUD19200;
        break;
    case 38400:
        br = BAUD38400;
        break;
    case 57600:
        br = BAUD57600;
        break;
    case 115200:
        br = BAUD115200;
        break;
    case 230400:
        br = BAUD230400;
        break;
    case 380400:
        br = BAUD380400;
        break;
    default:
        br = BAUD9600;
        break;
    }

    return br;
}

void PortSettingDialog::openCloseBtnHandler()
{
    if (mIsOpen) {
        closePort();
    } else {
        mUartName = "\\\\.\\" + ui->portNameCombo->currentText();
        int baudRate = ui->baudRateCombo->currentText().toInt();
        mBaudRate = getBaudRate(baudRate);
        openPort();
    }
}

void PortSettingDialog::openPort()
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
    mOpenCloseBtn->setText(tr("Close"));
    connect(mUartCom, SIGNAL(readyRead()), this, SLOT(onDataRecv()));
}

void PortSettingDialog::closePort()
{
    mOpenCloseBtn->setText(tr("Open"));
    mUartCom->close();
    delete mUartCom;
    mUartCom = NULL;
    mIsOpen = false;
}

void PortSettingDialog::sendData()
{
    if (mUartCom == NULL) {
        log("COM not opened!");
        return;
    }
    mUartCom->writeData("Write", strlen("Write"));
}

void PortSettingDialog::onDataRecv()
{
    QByteArray readData = mUartCom->readAll();
    if (readData.size() == 0)
        return;

    const char *rxBuffer = readData.data();
    if (mIsHexShow)
        updateRecvUI(rxBuffer, (quint32) readData.size());
    else {
        QString recvData = QString(QLatin1String(rxBuffer));
        //mUartRecvText->append(recvData);
        mUartRecvText->moveCursor(QTextCursor::End);
        mUartRecvText->insertPlainText(recvData);
    }
}

/**
 * SLOTS
 */
void PortSettingDialog::onOpenCloseBtnClicked()
{
    qDebug("onOpenBtnClicked");
    openCloseBtnHandler();
}

void PortSettingDialog::onCloseBtnClicked()
{
    qDebug("onCloseBtnClicked");
    closePort();
    mOpenCloseBtn->setText(tr("Open"));
}

void PortSettingDialog::onSendBtnClicked()
{
    qDebug("onSendBtnClicked");
    static const char *data = "Love is blind and lovers cannot see the pretty follies that themselves commit.\n";
    static const qint64 data_len = (qint64) strlen(data);
    if (NULL == mUartCom) {
        QMessageBox::warning(this, tr("警告"), tr("port not opened!"));
        return;
    }
    mUartCom->writeData(data, data_len);
}

void PortSettingDialog::onClearBtnClicked()
{
    qDebug("onClearBtnClicked");
    mUartRecvText->clear();
}

void PortSettingDialog::onHexShowRadioBtnToggled(bool checked)
{
    qDebug("onHexShowRadioBtnToggled, checked: %d", checked);
    mIsHexShow = checked;
}
