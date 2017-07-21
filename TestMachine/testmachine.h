#ifndef TESTMACHINE_H
#define TESTMACHINE_H

#include <QWidget>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include "engine/laprinter.h"
#include "engine/lascanner.h"
#include "engine/labcr.h"
#include "engine/lahw.h"
#include "printthread.h"
#include "socketthread.h"
#include"choiceotherpro.h"

//开发时无各个部件,请定义V_TEST
//#define V_TEST

namespace Ui {
	class TestMachine;
}

class TestMachine : public QWidget
{
	Q_OBJECT
	
public:
	explicit TestMachine(QWidget *parent = 0);
	~TestMachine();

public:
    DevConf  devconf;
    SendInfo sendReaderInfo,sendMonitorInfo,sendPrinterInfo,sendBCRInfo,sendUPSInfo;
    MonitorReadInfo monitorInfo;
protected:
	void InitDevices();
	void PlayVideo(QString videoFile, bool isMute);
        void readerReadInfo();


        void devStaInit();
        QString MonitorStr;
        bool printerFlag,scannerFlag;
        bool connectTestFlag;
        QDateTime time;
        QString strTime;
        QString BigMark;
        QString Pdfinfo,Pdfinfo2;

        void keyPressEvent(QKeyEvent *);
        void keyReleaseEvent(QKeyEvent *);
        bool eventFilter(QObject *, QEvent *);
        void installFilter();


	
protected slots:
	void ScannerScanFinish(int errCode, QString info);
	void ScannerError(int errCode, QString info);
	void BCRScanFinished(int errCode, QString info);
	void BCRError(int errCode, QString info);
	void PrintTicket();
	void NetWorkTimeoutEvent();
	void PrinterStatusTimeoutEvent();
	void BCRStatusTimeout();
        void ScannerTimeout();
         void monitorReadInfo();
	//void timerEvent(QTimerEvent *e);
         void upsTest();


private slots:
	void on_btnExit_clicked();
	void on_sBoxPrintInterval_valueChanged(int arg1);
        void PrinterErrinfoShow();
        void readConfInfo(QByteArray);
        void getMonitorInfo_slot();
        void connectIP();
        void rebootTimeout();
        void showChoiceDialog();
        void choicePro(QString);
        void localTest();
        void localIpCfg();
        void recordTiming();


        void on_pushButtonExit_clicked();

        void on_pushButtonReboot_clicked();

        void on_pushButtonShutdown_clicked();

        void on_btn_toSCSystem_clicked();

        void on_pushButton_toChoieDlg_clicked();

        void on_pushButton_toLATECHPro_clicked();

        void on_pushButtontimeup_clicked();

        void on_pushButtontimedown_clicked();

private:
	Ui::TestMachine *ui;
	LAPrinter *laPrinter;
	LAScanner *laScanner;
	LABCR *laBCR;
        SocketThread *socketthread_A;
	PrintThread printThread;
        QTimer networkTimer, printerStatusTimer, BCRStatusTimer,scanStatusTimer,
                    prtInfoTimer,monitorTimer,rebootTimer,upsTimer;
        QProcess videoProcess,monitorProcess;
        LAHW *laHwInfo;
        int hour;
        int minute;
        int second;
        QTimer *recordTimer;
        void startRecord();
        choiceOtherPro *choice_otherPro;

signals:
       void sendBcrTestTime(unsigned int);

};

#endif // TESTMACHINE_H
