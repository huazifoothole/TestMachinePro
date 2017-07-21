#include "testmachine.h"
#include "ui_testmachine.h"
#include <QTime>
#include<QMessageBox>
#include <QKeyEvent>
#define netPath "/etc/sysconfig/network-scripts/"
TestMachine::TestMachine(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TestMachine)
{
	ui->setupUi(this);

        qDebug("V4.1");
	laPrinter = new LAPrinter();
	laScanner = new LAScanner();
	laBCR = new LABCR();
        socketthread_A =new SocketThread();
        laHwInfo =new LAHW();
        choice_otherPro = new choiceOtherPro();
        printerFlag = true;
        scannerFlag= true;
        connectTestFlag = false;
        system("insmod /home/oneu/ccpos/dist/driver/la/modules/usbreader_fc14.ko");
        system("insmod /home/oneu/ccpos/dist/driver/la/modules/scanner_f14.ko");
        system("insmod /home/oneu/ccpos/dist/driver/la/modules/usb_datapipe-14.ko");
        system("insmod /home/oneu/ccpos/dist/driver/la/modules/cdc-acm.ko");
        system("sh /root/TestSystem/voice.sh");

	if(laScanner)
	{
		connect(laScanner, SIGNAL(ScanFinished(int,QString)), this, SLOT(ScannerScanFinish(int,QString)));
		connect(laScanner, SIGNAL(ScannerError(int,QString)), this, SLOT(ScannerError(int,QString)));
	}
	if(laBCR)
	{
                connect(laBCR, SIGNAL(ScanFinished(int,QString)), this, SLOT(BCRScanFinished(int, QString)));
		connect(laBCR, SIGNAL(BCRError(int,QString)), this, SLOT(BCRError(int,QString)));
	}

        //connect(&printThread, SIGNAL(PrintTicket()), this, SLOT(PrintTicket()));
          connect(&networkTimer, SIGNAL(timeout()), this, SLOT(NetWorkTimeoutEvent()));
        //connect(&printerStatusTimer, SIGNAL(timeout()), this, SLOT(PrinterStatusTimeoutEvent()));
          connect(&scanStatusTimer,SIGNAL(timeout()),this,SLOT(ScannerTimeout()));
         // connect(&BCRStatusTimer, SIGNAL(timeout()), this, SLOT(BCRStatusTimeout()));
      // connect(&printThread,SIGNAL(PrintErrStr()),this,SLOT(PrinterErrinfoShow()));
          connect(&prtInfoTimer,SIGNAL(timeout()),this,SLOT(PrintTicket()));
          connect(socketthread_A,SIGNAL(getConfInfo(QByteArray)),this, SLOT(readConfInfo(QByteArray)));//获取服务端配置信息
          connect(&monitorTimer,SIGNAL(timeout()),this,SLOT(monitorReadInfo()));
          connect(ui->pushButton_ip,SIGNAL(clicked()),this,SLOT(connectIP()));
          connect(&rebootTimer,SIGNAL(timeout()),this,SLOT(rebootTimeout()));
          connect(ui->pushButton_choice,SIGNAL(clicked()),this,SLOT(showChoiceDialog()));
          connect(choice_otherPro,SIGNAL(endPro(QString)),this,SLOT(choicePro(QString)));
          connect(&upsTimer,SIGNAL(timeout()),this,SLOT(upsTest()));
        //  connect(socketthread_A,SIGNAL(unconnectSer()),this,SLOT(localTest()));
        //  connect(ui->pushButton_ip_2,SIGNAL(clicked()),this,SLOT(localIpCfg()));
          connect(this,SIGNAL(sendBcrTestTime(uint)),laBCR,SLOT(getBcrTestTime(uint)));



	InitDevices();
	laScanner->SetOnlyScanning(true);
	laScanner->start();
        laBCR->SetStop(false);
        laBCR->start();


	networkTimer.start(1000);
	PlayVideo("/etc/LATECH/Videos/demo.mpg", false);

         devStaInit();
         QFont font;
         font.setPointSize(18);
         ui->lineEdit_ip->setFont(font);
         ui->lineEdit_ip->setText(socketthread_A->serverIP);
         ui->lineEdit_localip->setFont(font);
         ui->lineEdit_localip->setText(socketthread_A->localIP);
         QRegExp rx("((2[0-4]//d|25[0-5]|[01]?//d//d?)//.){3}(2[0-4]//d|25[0-5]|[01]?//d//d?)");
         QRegExpValidator v(rx, 0);
         ui->lineEdit_localip->setValidator(&v);
        //ui->lineEdit_localip->setInputMask("000.000.0.000;0");
         if(!socketthread_A->local())
         localTest();

        /* QDateTime time0 = QDateTime::currentDateTime();//获取系统现在的时间
         QString preTime = time0.toString("hh:mm:ss");//设置系统时间显示格式
         ui->preTimelcdNumber->display(preTime);//在lcdNumber上显示时间
         ui->preTimelcdNumber->hide();//不显示测试开始时间*/

         ui->Timelabel->setAlignment(Qt::AlignHCenter);
         ui->Timelabel->setStyleSheet("color:red");
          startRecord();//测试计时
         BigMark =
                 "\n_ X _ X _ X _ X _ X _ X _ _ _ _ \n"
                  "X X _ _ _ X X X _ X X X _ _ _ _ \n"
                  "_ X _ _ X X X _ _ _ _ _ _ _ _ _ \n"
                  "_ X X _ _ X _ _ _ X _ _ _ _ _ _ \n"
                  "_ _ _ X X _ X X X _ X X _ _ _ _ \n"
                  "X X _ X X _ _ _ _ _ X _ _ _ _ _ \n"
                  "X X X X _ X _ _ X X _ X _ _ _ _ \n"
                  "X _ X X _ _ X _ _ X _ _ _ _ _ _ \n"
                  "_ _ _ X X _ _ _ X _ _ _ _ _ _ _ ";
         Pdfinfo ="35073110603460810054   d2WIvi   64319331\n"
                 "06034 06073 2006/03/22\n"
                         "15 21 35 13 28+05 05\n"
                 "22 20 03 13 18+29 07\n"
                 "32 11 04 15 03+21 09\n"
                 "22 34 05 16 27+19 24\n"
                 "04 13 18 26 33+17 07\n";
         Pdfinfo2 =  "467853028972603721200  53WOLiiq   990162\n"
             "06034 02013 2017/03/22\n"
            "42 11 25 33 58+05 05\n"
            "32 50 07 13 18+29 07\n"
            "82 11 04 15 03+21 02\n";
         ui->scrollArea->setFocusPolicy(Qt::NoFocus);
         ui->pushButton_choice->setFocusPolicy(Qt::NoFocus);
         installFilter();

}

TestMachine::~TestMachine()
{
         printThread.PrintStop();
         videoProcess.kill();
	if(laPrinter) delete laPrinter;
	if(laScanner) delete laScanner;
	if(laBCR) delete laBCR;
        if(socketthread_A)
            delete socketthread_A;
	delete ui;
}

void TestMachine::keyPressEvent(QKeyEvent *e)
{
    if(e->key() ==Qt::Key_Return)
    {
        if(ui->pushButton_ip->hasFocus())
         {
           ui->pushButton_ip->setStyleSheet("background:url(:/images/connect.png)");
          // connectIP();
        }
    }

}

void TestMachine::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() ==Qt::Key_Return)
    {
        if(ui->pushButton_ip->hasFocus())
         {
           ui->pushButton_ip->setStyleSheet("");

        }
    }
}


void TestMachine::on_btnExit_clicked()
{
	printThread.PrintStop();
	videoProcess.kill();
	sleep(1);
	qApp->quit();
}

void TestMachine::InitDevices()
{
	//init printer
	while(laPrinter != NULL)
	{
		if(!laPrinter->LoadDriverSympol(SO_PATH"/HWILatechPrinter.so"))
		{
                        ui->labelDeviceStatus->setStyleSheet("color:red");
                        ui->labelDeviceStatus->setText(tr("加载驱动失败; "));
			break;
		}
                 sleep(2);
                bool Pinitflag=false;
		if(!laPrinter->Init(SO_PATH, SO_PATH))
		{
                    char errinfo1[1024]={0};
                    laPrinter->PrinterGetLastErrorStr(errinfo1,1024);
                    qDebug("Printer init = %s\n",errinfo1);

                        for(int i=0;i<3;i++){
                            if(laPrinter->Init(SO_PATH, SO_PATH)){
                                Pinitflag =  true;
                                break;
                            }
                            sleep(1);
                        }

                }else {
                    Pinitflag = true;
                }

                if(Pinitflag){
                    ui->labelDeviceStatus->setStyleSheet("color:black");
                    ui->labelDeviceStatus->setText(tr("正常"));      
                }else {
                    ui->labelDeviceStatus->setStyleSheet("color:red");
                    ui->labelDeviceStatus->setText(tr("异常"));
                }
                break;

	}

	//init scanner
	while(laScanner != NULL)
	{
		if(!laScanner->LoadDriverSympol(SO_PATH"/HWILatechScanner.so"))
		{
                        //strPortStatus += tr("加载驱动失败");
                    ui->labelScannerStatus->setStyleSheet("color:red");
                    ui->labelScannerStatus->setText(tr("加载驱动失败"));
			break;
		}
		if(!laScanner->Init(SO_PATH, SO_PATH))
		{
                     ui->labelScannerStatus->setStyleSheet("color:red");
                     ui->labelScannerStatus->setText(tr("异常"));
			break;
		}
                    ui->labelScannerStatus->setStyleSheet("color:black");
                    ui->labelScannerStatus->setText(tr("正常 "));
                        break;
	}

	//init bcr
	while(laBCR != NULL)
	{
		if(!laBCR->LoadDriverSympol(SO_PATH"/HWILatechBCR.so"))
                {
                        ui->labelHscanStatus->setStyleSheet("color:red");
                        ui->labelHscanStatus->setText(tr("加载驱动失败 "));
			break;
		}

              bool initflag=false;
              while(!initflag)
                {
                    if(laBCR->Init(SO_PATH, SO_PATH))
                    {
                       initflag=true;
                       break;
                    }
                    else{
                        char errinfo1[1024]={0};
                        laPrinter->PrinterGetLastErrorStr(errinfo1,1024);
                        qDebug("bcr init = %s\n",errinfo1);
                        qDebug("bcr init false");
                        break;
                    }
                }

                if(initflag == false)
		{
                        ui->labelHscanStatus->setStyleSheet("color:red");
                        ui->labelHscanStatus->setText(tr("异常"));
			break;
		}
              //  laBCR->HscannerUserLEDOff();
              //  laBCR->HscannerAimOff();
              //  laBCR->BarcodeScannerStopScan();
                ui->labelHscanStatus->setStyleSheet("color:black");
                ui->labelHscanStatus->setText(tr(" 正常 "));
                qDebug()<<"Init BCR ok";
		break;
	}

        while(laHwInfo!= NULL)
        {
            if(!laHwInfo->LoadDriverSympol(SO_PATH"/HWILatechHWINFO.so"))
              {
                  break;
              }
            qDebug("load HWILatechHWInfo.so ok");
            break;
         }
}

void TestMachine::BCRScanFinished(int errCode, QString info)
{
        if(errCode == 0)
	{
                ui->labelScanInfo->setText(info);
                qDebug()<<"info"<<info;
              //  if(errCode==0){
                if(Pdfinfo.contains(info) || Pdfinfo2.contains(info))
                {

                    ui->label_scan->setStyleSheet("color:black");
                    ui->label_scan->setText("识别匹配");
                }
                 else
                {
                    ui->label_scan->setStyleSheet("color:red");
                    ui->label_scan->setText("识别不匹配");
                }
           //     }
        }
        else if(errCode ==1||errCode ==2)
        {
            ui->labelScanInfo->setText(tr("扫描中"));
        }
        else
        {
            ui->labelScanInfo->setText(info);
            ui->labelHscanStatus->setText(info);
            qDebug()<<"*******errcode***************"<<errCode;
        }

        if(laBCR->testStartFlag)
        {

        if(errCode == 2)
         {
             sendBCRInfo.count_info.test_count += 1;
             qDebug("BCR test_count = %d\n",sendBCRInfo.count_info.test_count);
             sendBCRInfo.dev_sta = STA_NORMAL;
             strcpy(sendBCRInfo.error_info," ");
             strcpy(sendBCRInfo.read_info," ");
             strcpy(sendBCRInfo.time_str,QTime::currentTime().toString("HH:mm:ss").toLatin1().data());
             socketthread_A->sendInfo(sendBCRInfo);

             qDebug("sendBCRInfor test_count = %d\n",sendBCRInfo.count_info.test_count);
             qDebug("sendBCRInfo type = %d\n",sendBCRInfo.dev_type);
             return;
         }
        else if(errCode == -1)
          {

              sendBCRInfo.count_info.test_count += 1;
              qDebug("BCR test_count = %d\n",sendBCRInfo.count_info.test_count);
              sendBCRInfo.count_info.error_count += 1;
              sendBCRInfo.dev_sta = STA_ERROR;
              qDebug()<<"bcr errstr is "<<info;
              strcpy(sendBCRInfo.error_info,info.toLatin1().data());
              strcpy(sendBCRInfo.read_info," ");
              strcpy(sendBCRInfo.time_str,QTime::currentTime().toString("HH:mm:ss").toLatin1().data());
              socketthread_A->sendInfo(sendBCRInfo);
               return;
          }
        }

}

void TestMachine::ScannerScanFinish(int errCode, QString info)
{
	if(errCode == 0)
	{
		ui->labelScanInfo->setText(info);
                QString pdfStr = laPrinter->GetPDF417Data();
                qDebug()<<"scan info"<<info;
                qDebug()<<"pdfStr"<<pdfStr;
                  ui->label_scan->setStyleSheet("color:black");
                  if(Pdfinfo.contains(info) || Pdfinfo2.contains((info)))
                    ui->label_scan->setText("识别匹配");
                else if(BigMark.compare(info) == 0)
                    ui->label_scan->setText("识别匹配");
                 else
                {
                    ui->label_scan->setStyleSheet("color:red");
                    ui->label_scan->setText("识别不匹配");
                }

	}

}

void TestMachine::ScannerError(int errCode, QString info)
{
	if(errCode == TYPE_STATUS)
	{
		QString text = QTime::currentTime().toString("HH:mm:ss") + tr("  读票机未就绪: ") + info + tr("\n");
                text += ui->labelScannerStatus->text();
                ui->labelScannerStatus->setStyleSheet("color:red");
                ui->labelScannerStatus->setText(text);
#if 0
		laScanner->SetStop();
		sleep(1);
		qApp->processEvents();
		laScanner->Init(SO_PATH, SO_PATH);
		laScanner->start();
#endif
	}
}

void TestMachine::BCRError(int errCode, QString info)
{
	if(errCode == TYPE_STATUS)
	{
		QString text = QTime::currentTime().toString("HH:mm:ss") + tr("  扫描抢未就绪: ") + info + tr("\n");
                text += ui->labelHscanStatus->text();
                 ui->labelHscanStatus->setStyleSheet("color:red");
                ui->labelHscanStatus->setText(text);
#if 0
		laBCR->SetStop(true);
		sleep(1);
		qApp->processEvents();
		laBCR->Init(SO_PATH, SO_PATH);
		laBCR->start();
#endif
	}
}

void TestMachine::PrintTicket()
{
         sendPrinterInfo.count_info.test_count +=1;
         qDebug("printer test count is %d\n",sendPrinterInfo.count_info.test_count );
         qDebug("printer errtest count is %d\n",sendPrinterInfo.count_info.error_count );
//        if(laPrinter->PrintBigMarkTicket())
        if(laPrinter->PrintCodeTicket())
        {
               if(printerFlag == false)
               {
                     ui->labelDeviceStatus->setText(tr("正常"));
                     ui->labelDeviceStatus->setStyleSheet("color:black");
                     ui->labelScanInfo->setText("");
                     ui->labelScanInfo->setStyleSheet("background-color:rgb(200,211, 217)");
                     printerFlag = true;
               }
             sendPrinterInfo.dev_sta = STA_NORMAL;
             strcpy(sendPrinterInfo.error_info," ");
             strcpy(sendPrinterInfo.read_info," ");
             strcpy(sendPrinterInfo. time_str ,QTime::currentTime().toString("HH:mm:ss").toLatin1().data());
             socketthread_A->sendInfo(sendPrinterInfo);
             return;
        }
       else
            {
                   printerFlag = false;
                    PrinterErrinfoShow();
            }
        return;

}
void TestMachine::PrinterErrinfoShow()
{
    qDebug("it is PrinterErrinfoShow\n");
    laPrinter->PrinterPrintIsComplete(1);
    char errStr[1024];
    laPrinter->PrinterGetLastErrorStr(errStr,1024);
    qDebug("GetLastErrStr is %s\n",errStr);
    QString Str="PRINTER:";
    Str += QString(QLatin1String(errStr));
    if(Str.contains("PAPER_EMPTY"))
        Str=tr("打印机缺纸!");
    else if(Str.contains(("cover")))
        Str=tr("打印机上盖被打开");
    ui->labelDeviceStatus->setStyleSheet("color:red");
    ui->labelDeviceStatus->setText(Str);

    /*QMessageBox msgBox;
    int msg=msgBox.information(this,"warning", Str);*/
    ui->labelScanInfo->setStyleSheet("background-color:rgb(255,0,0)");

    ui->labelScanInfo->setText(Str);



    sendPrinterInfo.count_info.error_count +=1;
    sendPrinterInfo.dev_sta = STA_ERROR;
    strcpy(sendPrinterInfo.error_info ,Str.toLatin1().data());
    strcpy(sendPrinterInfo. time_str , QTime::currentTime().toString("HH:mm:ss").toLatin1().data());
    socketthread_A->sendInfo(sendPrinterInfo);

     return;
   // ui->lbe_printerInfo->setText(Str);

}

void TestMachine::on_sBoxPrintInterval_valueChanged(int arg1)
{
     ui->sBoxPrintInterval->setValue(arg1);
     prtInfoTimer.start(arg1*1000);
}

void TestMachine::NetWorkTimeoutEvent()
{
	QString networkInfo = LAHW::GetLinkStatus();
	ui->labelNetworkStatus->setText(networkInfo);
}


void TestMachine::PlayVideo(QString videoFile, bool isMute)
{
        if(videoProcess.state()) videoProcess.kill();
	WId wid;
	wid = ui->widgetVideo->winId();
	QString cmd = "mplayer";
	if(isMute)
	{
		cmd += " -nosound";
	}
        cmd +=      + " -loop 0 -slave -quiet -vf scale="
			+ QString::number(ui->widgetVideo->width())
			+ ":" + QString::number(ui->widgetVideo->height())
                        + " -wid " + QString::number(wid)
                        + " " + videoFile;

	qDebug()<<cmd;
	this->videoProcess.start(cmd);
        this->videoProcess.write("osd\n");//键盘控制的可视反馈
}

void TestMachine::PrinterStatusTimeoutEvent()
{
      /*  QString statusStr = laPrinter->GetStatusString();
        if(statusStr != tr("正常"))
        {
              //  ui->labelPrinterStatus->setText(tr("打印机状态: 不正常"));
                QString text = QTime::currentTime().toString("HH:mm:ss") + tr("  打印机未就绪: ") + statusStr + tr("\n");
                text += ui->labelDeviceStatus->text();
                ui->labelDeviceStatus->setText(text);
        }
        else
        {
              //  ui->labelPrinterStatus->setText(tr("打印机状态: 正常"));
        }*/
}

void TestMachine::BCRStatusTimeout()
{
    sendBCRInfo.count_info.test_count += 1;
    qDebug("BCR test_count = %d\n",sendBCRInfo.count_info.test_count);
     bool ret = laBCR->HscannerEnable();
    qDebug("BCR status is %d\n",ret);
    if(!ret)
     {
         char errstr[1024] = {0};
         sendBCRInfo.count_info.error_count += 1;
         sendBCRInfo.dev_sta = STA_ERROR;
         laBCR->HscannerGetLastErrorStr(errstr,1024);
         qDebug("bcr errstr is %s\n",errstr);
         strcpy(sendBCRInfo.error_info,errstr);
         strcpy(sendBCRInfo.read_info," ");
         strcpy(sendBCRInfo.time_str,QTime::currentTime().toString("HH:mm:ss").toLatin1().data());
         socketthread_A->sendInfo(sendBCRInfo);

          return;
     }
     else
     {
         sendBCRInfo.dev_sta = STA_NORMAL;
         strcpy(sendBCRInfo.error_info," ");
         strcpy(sendBCRInfo.read_info," ");
         strcpy(sendBCRInfo.time_str,QTime::currentTime().toString("HH:mm:ss").toLatin1().data());
         socketthread_A->sendInfo(sendBCRInfo);

         qDebug("sendBCRInfor test_count = %d\n",sendBCRInfo.count_info.test_count);
         qDebug("sendBCRInfo type = %d\n",sendBCRInfo.dev_type);
         return;
     }
}

void TestMachine::ScannerTimeout()
{
    sendReaderInfo.count_info.test_count += 1;
    bool status=laScanner->ScannerIsReady();
    qDebug("scanner status is %d\n",status);
   if(!status)//ok返回true
  {
        scannerFlag = false;
        char errStr[1024];
        laScanner->ScannerGetLastErrorStr(errStr,1024);
        QString Str=tr("读票机未就绪: ")+QString(QLatin1String(errStr));
        qDebug("Str=%s",errStr);
     //   ui->labelScannerStatus->setStyleSheet("color:red");
    //    ui->labelScannerStatus->setText(Str);

        sendReaderInfo.count_info.error_count += 1;
        sendReaderInfo.dev_sta =  STA_ERROR;
        strcpy(sendReaderInfo.error_info,errStr);
        strcpy(sendReaderInfo.time_str,QTime::currentTime().toString("HH:mm:ss").toLatin1().data());
        socketthread_A->sendInfo(sendReaderInfo);
        return;
    }
    else
   {
       if(scannerFlag == false)
          {
              ui->labelScannerStatus->setStyleSheet("color:red");
              ui->labelScannerStatus->setText(tr("异常 "));
              scannerFlag = true;
          }
          sendReaderInfo.dev_sta = STA_NORMAL;
          strcpy(sendReaderInfo.error_info," ");
          strcpy(sendReaderInfo.time_str,QTime::currentTime().toString("HH:mm:ss").toLatin1().data());
          socketthread_A->sendInfo(sendReaderInfo);
          ui->labelScannerStatus->setStyleSheet("color:black");
          ui->labelScannerStatus->setText("正常");
          return;
   }
}



void TestMachine::readConfInfo(QByteArray info)
{
    connectTestFlag = true;
    memset(&devconf , 0, sizeof(devconf));
    memcpy(&devconf,info,sizeof(devconf));
    qDebug("reboot_time  is %d\n",devconf.reboot_time);
    qDebug("image_time   is %d\n",devconf.img_time);
    if(devconf.printer_conf.printer_time >= 0 && devconf.printer_conf.printer_enable != 0)
    {
         ui->sBoxPrintInterval->setValue(devconf.printer_conf.printer_time);
         qDebug("priner time is %d s\n",devconf.printer_conf.printer_time);
         prtInfoTimer.start(devconf.printer_conf.printer_time*1000);
         QTimer::singleShot(10, this, SLOT(PrintTicket()));

    }

    if(devconf.reader_conf.reader_time >= 0 && devconf.reader_conf.reader_enable != 0)
    {
        int readerTime = devconf.reader_conf.reader_time;
        qDebug("reader time is %d\n",readerTime);
        scanStatusTimer.start(readerTime*1000);

    }
    if(devconf.screen_conf.screen_time>=0)
    {
        qDebug("screen(BCR) time is %d\n",devconf.screen_conf.screen_time);   
        BCRStatusTimer.start(devconf.screen_conf.screen_time*1000);
        sendBcrTestTime(devconf.screen_conf.screen_time);//将测试时间传给bcr.cpp
    }

    if(devconf.monitor_conf.monitor_time >= 0 && devconf.monitor_conf.monitor_enable != 0)
    {
        qDebug("monitor time is %d\n",devconf.monitor_conf.monitor_time);
        monitorTimer.start(devconf.monitor_conf.monitor_time*1000);
     }
    if(devconf.reboot_time >= 0)
    {
        qDebug("reboot time is %d\n",devconf.reboot_time);
        rebootTimer.start(devconf.reboot_time*1000);
    }
    if(devconf.ups_conf.ups_time >=0 )
    {
         qDebug("UPS time is %d\n",devconf.ups_conf.ups_time);
         upsTimer.start(devconf.ups_conf.ups_time*1000);
    }
}

void TestMachine::readerReadInfo()//获取软件版本
{
    ReaderReadInfo readerInfo;
    memset(&readerInfo,0,sizeof(readerInfo));
    sendReaderInfo.dev_type = READER;
    sendReaderInfo.dev_sta = STA_NORMAL;
    strcpy(sendReaderInfo.time_str,QTime::currentTime().toString("HH:mm:ss").toLatin1().data());
    char swVersion[40] = {0};
    char hwInfo[200] = {0};
    laScanner->ScannerGetSWVersion(swVersion,40);
    bool ret = laScanner->GetScannerHWInformation(hwInfo,200);
    qDebug("swVersion = %s\n",swVersion);
    qDebug("gethwinfo ret = %d\n reader hwInfo is %s\n",ret,hwInfo);
    strcpy(readerInfo.har_ver,swVersion);
    strcpy(readerInfo.s_version.szDescription,hwInfo);
    memcpy(sendReaderInfo.read_info,&readerInfo,sizeof(readerInfo));
}

void TestMachine::monitorReadInfo()
{
    sendMonitorInfo.count_info.test_count += 1;

    QDateTime  time=QDateTime::currentDateTime();
    time=QDateTime::currentDateTime();
    QString str=time.toString("yyyy-MM-dd hh:mm:ss");
    qDebug("time is %s\n",str.toLatin1().data());
    strcpy(sendMonitorInfo.time_str,str.toLatin1().data());

    sendMonitorInfo.dev_sta = STA_NORMAL;
    sendMonitorInfo.count_info.error_count =0;
    sendMonitorInfo.count_info.acc_error = 0;

    char mbinfo[4096]={0};
   if(laHwInfo->hwGetMainboardInfo(mbinfo, sizeof(mbinfo)))//获取bios信息
           {

               QString tmpstr = mbinfo;
               QStringList tmpList = tmpstr.split('\n');

               for(int i=0; i<tmpList.size(); i++)
               {
                   //qDebug()<<tmpList.at(i);
                   QStringList list2;
                   list2 = tmpList.at(i).split('=');

                                   if(list2.size() == 2 && list2.at(0) == "BIOSVersion")
                                   {
                                       strcpy( monitorInfo.bios_ver, list2.at(1).toLatin1().data());

                                   }
                                   if(list2.size() == 2 && list2.at(0) == "UUID")
                                   {
                                       strcpy(monitorInfo.monitor_id,list2.at(1).toLatin1().data());

                                   }
               }
           }
    else {
            qDebug("GetMainboardInfo false\n");
           }

    FILE  *stream;
    char   buf[4096];
    memset( buf, '\0', sizeof(buf) );//初始化buf,以免后面写如乱码到文件中

    stream = popen( "sensors | grep temp2 |awk '{print $2}'|cut -c 2-3", "r" ); //将“终端”命令的输出 通过管道读取（“r”参数）到FILE* stream
    fread( buf, sizeof(char), sizeof(buf), stream); //将刚刚FILE* stream的数据流读取到buf中
 //   qDebug("******the systemp info is******** %s",buf);
    MonitorStr =QString(buf);
    monitorInfo.sys_temp = MonitorStr.toInt();
    memset( buf, '\0', sizeof(buf) );
    pclose( stream );

    stream = popen( "sensors | grep temp3 |awk '{print $2}'|cut -c 2-3", "r" );
    fread( buf, sizeof(char), sizeof(buf), stream);
    MonitorStr =QString(buf);
    monitorInfo.cpu_temp = MonitorStr.toInt();
  //  qDebug("cpu_temp =%d\n",MonitorStr.toInt());
    memset( buf, '\0', sizeof(buf) );
    pclose( stream );

    stream = popen( "sensors | grep fan3 |awk '{print $2,$3;}'|cut -c 1-4", "r" );
    fread( buf, sizeof(char), sizeof(buf), stream);
    MonitorStr =QString(buf);
    monitorInfo.cpu_fan_speed = MonitorStr.toInt();
  // qDebug("cpu_fan_speed =%d\n",MonitorStr.toInt());
    memset( buf, '\0', sizeof(buf) );
    pclose( stream );

    stream = popen( "sensors | grep fan2 |awk '{print $2,$3;}'|cut -c 1-4", "r" );
    fread( buf, sizeof(char), sizeof(buf), stream);
    MonitorStr =QString(buf);
    monitorInfo.sys_fan_speed = MonitorStr.toInt();
    //qDebug("systemp_fan_speed =%d\n",MonitorStr.toInt());
    memset( buf, '\0', sizeof(buf) );
    pclose( stream );


    // QStringList arguments;
    // arguments <<" "<<"\|"<<"grep"<<" "<<"sh";
   /*  QString cmd = "./systemp.sh";
     monitorProcess.start(cmd);
    bool waitFlag = monitorProcess.waitForFinished(3000);
    qDebug()<<"waitFlag ="<<waitFlag;
    QByteArray output = monitorProcess.readAllStandardOutput();
     MonitorStr =QString(output);
       qDebug()<<MonitorStr;
     monitorInfo.sys_temp = MonitorStr.toInt();
     qDebug()<<"systemp ="<<MonitorStr;
     qDebug("systemp =%d\n",MonitorStr.toInt());

     cmd = "./cputemp.sh";
     monitorProcess.start(cmd);
     bool waitFlag2 =  monitorProcess.waitForFinished(3000);
     qDebug()<<"waitFlag2 ="<<waitFlag2;
     monitorInfo.cpu_temp = MonitorStr.toInt();
     qDebug()<<"cputemp = "<<MonitorStr;
    qDebug("cputemp =%d\n",MonitorStr.toInt());

    cmd = "./monitorFan.sh";
    monitorProcess.start(cmd);
    monitorProcess.waitForFinished(3000);
    monitorInfo.sys_fan_speed = MonitorStr.toInt();
    qDebug()<<"monitorFan = "<<MonitorStr;
    qDebug("monitorFan =%d\n",MonitorStr.toInt());

    cmd = "./cpuFan.sh";
    monitorProcess.start(cmd);
    monitorProcess.waitForFinished(3000);
    monitorInfo.cpu_fan_speed = MonitorStr.toInt();
    qDebug()<<"cpuFan = "<<MonitorStr;
    qDebug("cpuFan =%d\n",MonitorStr.toInt());*/

   // strcpy(monitorInfo.s_version.szDateTime,"2001-9-11");
   // strcpy(monitorInfo.s_version.szDescription,"It is monitorInfo");
   // strcpy(monitorInfo.s_version.szVersion,"2.01");

    memcpy(sendMonitorInfo.read_info,&monitorInfo,sizeof(monitorInfo));
   socketthread_A->sendInfo(sendMonitorInfo);
}

void TestMachine::upsTest()
{
    sendUPSInfo.count_info.test_count +=1;
    sendUPSInfo.dev_sta = STA_NOT_TEST;
    socketthread_A->sendInfo(sendUPSInfo);
}

void TestMachine::devStaInit()
{
    memset(&sendReaderInfo,0,sizeof(sendReaderInfo));
    memset(&sendMonitorInfo,0,sizeof(sendMonitorInfo));
    memset(&sendPrinterInfo,0,sizeof(sendPrinterInfo));
    memset(&sendBCRInfo,0,sizeof(sendBCRInfo));
    memset(&sendUPSInfo,0,sizeof(sendUPSInfo));

    memset(&monitorInfo,0,sizeof(monitorInfo));
    memset(&devconf , 0, sizeof(devconf));
    sendReaderInfo.dev_type = READER;
    sendMonitorInfo.dev_type = MONITOR;
    sendPrinterInfo.dev_type = PRINTER;
    sendBCRInfo.dev_type = SCREEN;
    sendUPSInfo.dev_type = UPS;

    char pSW[20]={0};
    char HscanSW[20]={0};
    char ScanVersion[20] = {0};
    laPrinter->PrinterGetSWVersion(pSW,20);
    laBCR->HscannerGetSWVersion(HscanSW,20);
    laScanner->ScannerGetSWVersion(ScanVersion,20);
    QString PSW = "Printer s_version :"+QString(pSW);
    QString BCRSW= "BCR s_version :"+QString(HscanSW);
    QString ScanSW= "Scanner s_version :"+QString(ScanVersion);
    strcpy(monitorInfo.s_version.szVersion,PSW.toLatin1().data());
    strcpy(monitorInfo.s_version.szDescription,BCRSW.toLatin1().data());
    strcpy(monitorInfo.s_version.szDateTime ,ScanSW.toLatin1().data());
    qDebug()<<"monitorInfo.s_version.szVersion"<<monitorInfo.s_version.szVersion;
    qDebug()<<"monitorInfo.s_version.szDescription"<<monitorInfo.s_version.szDescription;
    readerReadInfo();

}

void TestMachine::getMonitorInfo_slot()
{
    qDebug("**********it is output**********");
    QByteArray output = monitorProcess.readAllStandardOutput();
     MonitorStr =QString(output);
    qDebug()<<MonitorStr;

}

void TestMachine::connectIP()
{
    localIpCfg();

    if(!connectTestFlag)//在已连接情况下，若只改变本机地址101～200，将不重连
    {
        //QString serverIP= ui->lineEdit_ip->toPlainText();
        QString serverIP= ui->lineEdit_ip->text();
        socketthread_A->connectServer(serverIP);
     }
}

void TestMachine::rebootTimeout()
{
    if(!socketthread_A->rebootDev())
        return;
    usleep(50000);
    qDebug("ready to reboot");
    printThread.PrintStop();
    videoProcess.kill();
    sleep(1);
    qApp->quit();
    system("reboot");

}

void TestMachine::showChoiceDialog()
{
      choice_otherPro->show();
}

void TestMachine::choicePro(QString str)
{
   // QProcess::startDetached("./choiceDialog",QStringList());

     qApp->closeAllWindows();

    if(str == "sc") {
       qDebug("setup shenchan Pro");
//       QProcess::startDetached("/root/TestSystem/startSystem.sh",QStringList());
//       qApp->processEvents();
        exit(3);
   }
    else if(str == "kaoji") {
        qDebug("setup kaoji Pro");
        //QProcess::execute("./Machinestart.sh");
//        QProcess::startDetached("/root/TestSystem/start.sh",QStringList());
//        qApp->processEvents();
         exit(1);
    }
    else if(str == "LATECH") {
        qDebug("setup LATECH Pro");
//        QProcess::startDetached("/root/LATECH/start.sh",QStringList());
//        qApp->processEvents();
           exit(2);
    }
    else
    {
          QMessageBox msgBox;
          msgBox.information(this,tr("出错"), "未找到相应程序", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }



}

void TestMachine::localTest()
{
    qDebug()<<"*****it is localTest***";
     int ptime= ui->sBoxPrintInterval->value();
      prtInfoTimer.start(ptime*1000);
      BCRStatusTimer.start(8*1000);
      scanStatusTimer.start(7*1000);

}

void TestMachine::localIpCfg()
{
    int NetCount = 0;   //网卡个数
    int Neti = 0;
    QNetworkInterface thisNet;  //所要使用的网卡
    QList<QNetworkInterface> NetList = QNetworkInterface::allInterfaces();//获取所有网卡信息
    NetCount = NetList.count();   //统计网卡个数
    for(Neti = 0;Neti < NetCount; Neti++){ //遍历所有网卡
        if(NetList[Neti].flags().testFlag(NetList[Neti].CanMulticast) &&NetList[Neti].flags().testFlag(NetList[Neti].IsRunning) )
        { //判断该网卡是否是有效
                    {thisNet = NetList[Neti]; //将该网卡置为当前网卡
                    qDebug()<<"网卡"<<Neti<<NetList[Neti];
                    break;
                     }
       }

           }

    QSettings setting(QString("./networkIP.ini"), QSettings::IniFormat);
    setting.beginGroup("IP");
    QString ipaddr("");
    setting.setValue("localIP",ui->lineEdit_localip->text());
    setting.setValue("netInterface",thisNet.name());
    setting.setValue("serverIP",ui->lineEdit_ip->text());
    ipaddr = setting.value(QString("loacalIP")).toString();
    setting.endGroup();
    qDebug()<<"ipaddr2"<<ipaddr;

    QProcess netProcess;
    QString ifconfigcmd= "ifconfig "+thisNet.name()+" "+ui->lineEdit_localip->text();
    qDebug()<<"ifconfigcmf"<<ifconfigcmd;
    QByteArray s = ifconfigcmd.toLatin1();
    int ret1=system(s.data());
    int ret2 = system("ifconfig");
    qDebug()<<"ret1=="<<ret1<<"ret2=="<<ret2;
   // netProcess.start(ifconfigcmd);
    //sleep(1);
    netProcess.close();





   /*QString FileStr =QString(netPath)+"ifcfg-Auto_"+thisNet.name();
    QFile file(FileStr);
    if(!file.open(QIODevice::ReadOnly)){
    qDebug()   << "Cannot open file for Reading";
    return ;
        }
    QString str (file.readAll());
    if(str.contains("IPADDR=", Qt::CaseInsensitive)){
        str.replace(QRegExp("IPADDR=.*"),QString("IPADDR=")+ui->lineEdit_localip->toPlainText())+"\n";
           qDebug()<<str;
        }
    file.close();
    if(!file.open(QIODevice::WriteOnly)){
         qDebug()   << "Cannot open file for Writing";
              return ;
        }
    file.write(str.toUtf8());
    file.close();

    qDebug()<<"NetCount"<<NetCount;
    qDebug()<<"thisnet_name"<<thisNet.name();
    QProcess netProcess;
    QString ifconfigcmd= "ifconfig "+thisNet.name()+" "+ui->lineEdit_localip->toPlainText();
    qDebug()<<"ifconfigcmf"<<ifconfigcmd;
    netProcess.start(ifconfigcmd);
     netProcess.close();*/


     /*  QString localIPAddress = " "; //获取ip地址
       QList<QHostAddress> listAddress=QNetworkInterface::allAddresses();
         for(int j = 0; j < listAddress.size(); j++){
         if(!listAddress.at(j).isNull()
            && listAddress.at(j).protocol() ==  QAbstractSocket::IPv4Protocol
            && listAddress.at(j) != QHostAddress::LocalHost)
            {
                localIPAddress = listAddress.at(j).toString();
                qDebug()<<"localIPAddress"<<localIPAddress;

            }
         }
         qDebug()<<"localIPAddress"<<localIPAddress;*/

}


void TestMachine::startRecord()
{
    this->recordTimer = new QTimer;
    connect(recordTimer,SIGNAL(timeout()),this,SLOT(recordTiming()));
    hour = 0;
    minute = 0;
    second = 0;
    recordTimer->start(1000);
}

void TestMachine::recordTiming()
{
    if(++second>59)
    {
         second=0;

         if(++minute>59)
         {
             minute=0;
             hour++;

         }
    }

    QString str,tmp;
    tmp.setNum(hour);
    if(hour <10)
    {
        str+='0';
    }
    str+=tmp;
    str+=':';

    tmp.setNum(minute);
    if(minute<10)
    {
        str+='0';
    }
    str+=tmp;
    str+=':';

    tmp.setNum(second);
    if(second<10)
    {
        str+='0';
    }
    str+=tmp;

    ui->Timelabel->setText(str);
}


void TestMachine::on_pushButtonExit_clicked()
{
    printThread.PrintStop();
    if(videoProcess.state()) videoProcess.kill();
    if(monitorProcess.state() )monitorProcess.kill();
    if(socketthread_A) delete socketthread_A;

    sleep(1);
    qApp->quit();
}

void TestMachine::on_pushButtonReboot_clicked()
{
    QMessageBox msgBox;
    int msg=msgBox.information(this,"warning", "确认重启？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(msg == QMessageBox::Yes)
    {
        system("reboot");
        qApp->quit();

    }
    else
        qDebug()<<"no";
}

void TestMachine::on_pushButtonShutdown_clicked()
{
    QMessageBox msgBox;
    int msg=msgBox.information(this,"warning", "确认关机？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(msg == QMessageBox::Yes)
    {

        qDebug()<<"yes";
        system("shutdown -h now");
        qApp->quit();

    }
    else
        qDebug()<<"no";
}

void TestMachine::on_btn_toSCSystem_clicked()
{
    QMessageBox msgBox;
    int msg=msgBox.information(this,"warning", "下次开机将进入<<生产测试程序>>", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(msg == QMessageBox::Yes)
    {
        qApp->closeAllWindows();
//        system("ln -s /root/TestSystem/startTestSystem.sh /root/TestSystem/localsetup  -f");
//        QProcess::startDetached("/root/TestSystem/startSystem.sh",QStringList());
//        qApp->processEvents();
         system("ln -s /root/TestSystem/changeTestSystem.sh /root/TestSystem/localsetup  -f");
         exit(3);
    }
    else
        qDebug()<<"no";
}

void TestMachine::on_pushButton_toChoieDlg_clicked()
{
                  QMessageBox msgBox;
                  int msg=msgBox.information(this,"warning", "下次开机将进入<<选择引导界面>>", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                  if(msg == QMessageBox::Yes)
                  {
                      qApp->closeAllWindows();
//                      system("ln -s /root/TestSystem/choiceDialog  /root/TestSystem/localsetup  -f");
//                      printThread.PrintStop();
//                      videoProcess.kill();
//                     if(laPrinter) delete laPrinter;
//                     if(laScanner) delete laScanner;
//                     if(laBCR) delete laBCR;
//                     if(socketthread_A)
//                         delete socketthread_A;
//                     QProcess::startDetached("/root/TestSystem/choiceDialog",QStringList());
//                     qApp->processEvents();
                      system("ln -s /root/TestSystem/changeChoice.sh /root/TestSystem/localsetup  -f");
                     exit(4);

                  }
                  else
                      qDebug()<<"no";
}

void TestMachine::on_pushButton_toLATECHPro_clicked()
{
                                QMessageBox msgBox;
                                int msg=msgBox.information(this,"warning", "下次开机将进入<<市场演示程序>>", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                                if(msg == QMessageBox::Yes)
                                {
                                    qApp->closeAllWindows();
//                                    system("ln -s /root/LATECH/start.sh /root/TestSystem/localsetup  -f");
//                                   QProcess::startDetached("/root/LATECH/start.sh",QStringList());
//                                   qApp->processEvents();
                                    system("ln -s /root/TestSystem/changeLATECH.sh /root/TestSystem/localsetup  -f");
                                    exit(2);

                                }
                                else
                                    qDebug()<<"no";
}

void TestMachine::on_pushButtontimeup_clicked()
{
    int value=ui->sBoxPrintInterval->value();
    ui->sBoxPrintInterval->setValue(value+10);
}

void TestMachine::on_pushButtontimedown_clicked()
{
    int value=ui->sBoxPrintInterval->value();
    ui->sBoxPrintInterval->setValue(value-10);
}

void TestMachine::installFilter()
{
    QPushButton *btnUiTable[]=
     {
          ui->pushButtonExit,ui->pushButtonReboot,ui->pushButtonShutdown,ui->pushButton_ip
     };
    for(unsigned int i=0;i<sizeof(btnUiTable)/sizeof(btnUiTable[0]);i++)
  {
      btnUiTable[i]->installEventFilter(this);
  }
}

bool TestMachine::eventFilter(QObject *watched, QEvent *event)
{
    QPushButton *btnUiTable[]=
    {
        ui->pushButtonExit,ui->pushButtonReboot,ui->pushButtonShutdown,ui->pushButton_ip
    };

    for(unsigned int i=0;i<sizeof(btnUiTable)/sizeof(btnUiTable[0]);i++)
  {
       if(watched == btnUiTable[i])
       {
           if(event->type() == QEvent::FocusIn)
           {
               if(i ==0 )
                         btnUiTable[i]->setStyleSheet("background-image:url(:/images/exit_checked.png);border-style:inset;");
               else if( i == 1)
                          btnUiTable[i]->setStyleSheet("background-image:url(:/images/reboot_checked.png);border-style:inset;");
               else if(i == 2)
                      btnUiTable[i]->setStyleSheet("background-image:url(:/images/shutdown_checked.png);border-style:inset;");
               else if( i ==3)
                   btnUiTable[i]->setStyleSheet("background-image:url(:/images/connect.png);border-style:inset;");
           }


           if(event->type() == QEvent::FocusOut)
          {
              if(i ==0 )
                        btnUiTable[i]->setStyleSheet("background-image:url(:/images/exit.png);border-style:inset;");
              else if( i == 1)
                         btnUiTable[i]->setStyleSheet("background-image:url(:/images/reboot.png);border-style:inset;");
              else if(i == 2)
                     btnUiTable[i]->setStyleSheet("background-image:url(:/images/shutdown.png);border-style:inset;");
              else if( i ==3)
                  btnUiTable[i]->setStyleSheet(" ");
           }
       }
  }


  return QWidget::eventFilter(watched,event);


}
