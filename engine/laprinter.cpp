#include "laprinter.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>

#include <QMessageBox>
#include <QStringRef>
#include <QtCore>
#include <QFile>
#include <QTextStream>
#include <QSettings>

LAPrinter::LAPrinter(QObject *parent) : QObject(parent)
{
	dp_ = NULL;
	if(0 == pthread_mutex_init(&printerMutex, NULL))
	{
		bmutexIsOK = true;
		strErrorInfo = tr("无错误");
	}
	else
	{
		bmutexIsOK = false;
		strErrorInfo = tr("打印机初始化互斥锁错误");
	}
}

LAPrinter::~LAPrinter()
{
	if(bmutexIsOK) pthread_mutex_destroy(&printerMutex);
	if(dp_ != NULL) dlclose(dp_);
}

bool LAPrinter::LoadDriverSympol(const char *const file)
{
	if(dp_ != NULL) dlclose(dp_);
	char *error = NULL;
	struct stat stat_buf;

	if(0 != stat(file, &stat_buf))
	{
		printf("%s not exsist\n", file);
		return false;
	}
	printf("open %s\n",file);
	dlerror();
	dp_ = dlopen(file, RTLD_LAZY);
	if( (error=dlerror()) != NULL)
	{
		printf("openso is %s\n",error);
		return false;
	}

//	PrinterUnsetPassword = (UnsetPassword)dlsym(dp_,"UnsetPassword");
//	if((error=dlerror()) != NULL)
//	{
//		printf("load PrinterUnsetPassword is %s\n",error);
//		return false;
//	}

//	PrinterSetPassword = (SetPassword)dlsym(dp_,"SetPassword");
//	if((error=dlerror()) != NULL)
//	{
//		printf("load PrinterSetPassword is %s\n",error);
//		return false;
//	}

//	PrinterGetComplement = (GetComplement)dlsym(dp_,"GetComplement");
//	if((error=dlerror()) != NULL)
//	{
//		printf("load PrinterGetComplement is %s\n",error);
//		return false;
//	}

//	PrinterUnlock = (Unlock)dlsym(dp_,"Unlock");
//	if((error=dlerror()) != NULL)
//	{
//		printf("load PrinterUnlock is %s\n",error);
//		return false;
//	}

	PrinterInit = (InitP)dlsym(dp_,"PInit");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterInit is %s\n",error);
		return false;
	}

    PrinterSetCutterMode = (SetCutterMode)dlsym(dp_, "PSetCutterMode");
    if((error=dlerror()) != NULL)
	{
		printf("load PrinterSetCutterMode is %s\n",error);
		return false;
	}

	PrinterSetFont = (SetFont)dlsym(dp_,"PSetFont");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterSetFont is %s\n",error);
		return false;
	}

	PrinterSetFontEmpha = (SetFontEmpha)dlsym(dp_,"PSetFontEmpha");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterSetFontEmpha is %s\n",error);
		return false;
	}

	PrinterSetLineSpace = (SetLineSpace)dlsym(dp_,"PSetLineSpace");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterSetLineSpace is %s\n",error);
		return false;
	}

	PrinterSetCharSpace = (SetCharSpace)dlsym(dp_,"PSetCharSpace");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterSetCharSpace is %s\n",error);
		return false;
	}

	PrinterSetLeftMargin = (SetLeftMargin)dlsym(dp_,"PSetLeftMargin");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterSetLeftMargin is %s\n",error);
		return false;
	}

	PrinterSetAreaWidth = (void (*)(int width))dlsym(dp_,"PSetAreaWidth");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterSetAreaWidth is %s\n",error);
		return false;
	}

	printerIsReady = (PrinterIsReady)dlsym(dp_,"PPrinterIsReady");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterIsReady is %s\n",error);
		return false;
	}

	PrinterPrintString = (int (*)(const char* str))dlsym(dp_,"PPrintString");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterPrintString is %s\n",error);
		return false;
	}

	PrinterFeedLine = (void (*)(unsigned char lineCount))dlsym(dp_,"PFeedLine");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterFeedLine is %s\n",error);
		return false;
	}

	PrinterCutPaper = (CutPaper)dlsym(dp_,"PCutPaper");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterCutPaper is %s\n",error);
		return false;
	}

//	PrinterPrint1DBar = (Print1DBar)dlsym(dp_,"Print1DBar");
//	if((error=dlerror()) != NULL)
//	{
//		printf("load PrinterPrint1DBar is %s\n",error);
//		return false;
//	}

	PrinterPrint2DBar = (Print2DBar)dlsym(dp_,"PrintPDF417");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterPrint2DBar is %s\n",error);
		return false;
	}

	PrinterSetUserChar = (SetUserChar)dlsym(dp_,"PSetUserChar");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterSetUserChar is %s\n",error);
		return false;
	}

	PrinterCancelUserDefChar = (UnsetUserChar)dlsym(dp_,"PUnsetUserChar");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterCancelUserDefChar is %s\n",error);
		return false;
	}

	PrinterPrintUserBitmap = (PrintUserBitmap)dlsym(dp_,"PPrintUserBitmap");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterPrintUserBitmap is %s\n",error);
		return false;
	}

	PrinterPrintMark = (PrintBlackMark)dlsym(dp_,"PPrintBlackMark");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterPrintMark is %s\n",error);
		return false;
	}

	PrinterGetTopMargin = (GetTopMargin)dlsym(dp_,"PGetTopMargin");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterGetTopMargin is %s\n",error);
		return false;
	}

	PrinterGetLastErrorCode= (GetLastErrorCode)dlsym(dp_,"PGetLastErrorCode");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterGetLastErrorCode is %s\n",error);
		return false;
	}

	PrinterGetLastErrorStr = (GetLastErrorStr)dlsym(dp_,"PGetLastErrorStr");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterGetLastErrorStr is %s\n",error);
		return false;
	}

	PrinterGetHWInformation = (GetHWInformation)dlsym(dp_,"PGetHWInformation");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterGetHWInformation is %s\n",error);
		return false;
	}

	PrinterGetDpi = (GetDpi)dlsym(dp_,"PGetDpi");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterGetDpi is %s\n",error);
		return false;
	}

	PrinterQueryCapability = (QueryCapability)dlsym(dp_,"PQueryCapability");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterQueryCapability is %s\n",error);
		return false;
	}

	PrinterGetSWVersion = (GetSWVersion)dlsym(dp_,"PGetSWVersion");
	if((error=dlerror()) != NULL)
	{
		printf("load GetSWVersion is %s\n",error);
		return false;
	}
#if 0
        PrinterPrintBigMark = (PrintBigMark)dlsym(dp_,"PrintBMark");
        if((error=dlerror()) != NULL)
        {
                printf("load PrinterPrintBigMark is %s\n",error);
                return false;
        }
	PrinterPrintSmallMark = (PrintSmallMark)dlsym(dp_,"PrintSMark");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterPrintSmallMark is %s\n",error);
		return false;
	}

	PrinterCutPaperByType = (CutPaperByType)dlsym(dp_,"CutPaperByType");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterCutPaperByType is %s\n",error);
		return false;
	}

	PrinterPrintIsComplete = (PrintIsComplete)dlsym(dp_,"PPrintIsComplete");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterPrintIsComplete is %s\n",error);
		return false;
	}

	PrinterGetStatus = (PGetStatus)dlsym(dp_,"GetStatus");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterGetStatus is %s\n",error);
		return false;
	}
#endif
	PrinterSetAngle = (SetAngle)dlsym(dp_,"PSetAngle");
	if((error=dlerror()) != NULL)
	{
		printf("load SetAngle is %s\n",error);
		return false;
	}

	printDiskImage = (PrintDiskImage)dlsym(dp_,"PPrintDiskImage");
	if((error=dlerror()) != NULL)
	{
		printf("load PrintDiskImage is %s\n",error);
		return false;
	}

	PrinterSetPageMode = (SetPageMode)dlsym(dp_,"PSetPageMode");
	if((error=dlerror()) != NULL)
	{
		printf("load SetPageMode is %s\n",error);
		return false;
	}

	PrinterSetLineMode = (SetLineMode)dlsym(dp_,"PSetLineMode");
	if((error=dlerror()) != NULL)
	{
		printf("load SetLineMode is %s\n",error);
		return false;
	}

	PrinterPrintPage = (PrintPage)dlsym(dp_,"PPrintPage");
	if((error=dlerror()) != NULL)
	{
		printf("load PrintPage is %s\n",error);
		return false;
	}
#if 0
	PrinterWriteSerial = (WriteSerial)dlsym(dp_, "WriteSerial");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterWriteSerial is %s\n",error);
		return false;
	}

	PrinterPrintStatus = (PrinterStatusPrint)dlsym(dp_, "PrinterStatusPrint");
	if((error=dlerror()) != NULL)
	{
		printf("load PrinterStatusPrint is %s\n",error);
		return false;
	}

	PrinterSetDensity = (PSetPrintDensity)dlsym(dp_, "PSetPrintDensity");
	if((error=dlerror()) != NULL)
	{
		printf("load PSetPrintDensity is %s\n",error);
		return false;
	}
#endif
	return true;
}

bool LAPrinter::PrintTicketHeader()
{
    if(TRUE != printerIsReady())
    {
		qDebug("PrinterIsReady error\n");
		return false;
    }
    //unsigned int status = PrinterGetStatus();
    //if(status != 0) return false;

    PrinterSetLeftMargin(0x0);
    PrinterSetAreaWidth(0x0300);
    PrinterSetLineSpace(10);
    PrinterSetCharSpace(0);

    PrinterSetFont(0x00,0x10,0x01);
    PrinterPrintString(U2G("体彩<测试样票>").data());
    PrinterFeedLine(1);

    PrinterSetLineSpace(0);
    PrinterFeedLine(1);
    PrinterSetFont(0x00,0x00,0x01);
    PrinterPrintString(U2G("35073110603460810054   d2WIvi   64319331").data());
    PrinterFeedLine(1);

    PrinterPrintString(U2G("销售点:06034      06073期 2006/03/22开奖").data());
    PrinterFeedLine(1);

    PrinterPrintString(U2G("倍:1  合计:10元      2006/03/22 12:27:49\n").data());
    PrinterFeedLine(1);

    PrinterSetLineSpace(10);
    PrinterFeedLine(1);
    PrinterSetFont(0x00,0x10,0x01);
    PrinterPrintString(U2G("直 选 票").data());
    PrinterFeedLine(1);

    PrinterSetFont(0x01,0x10,0x01);
    PrinterSetLineSpace(0);
	qsrand((uint)time(NULL));
	char buf[256] = {0};
	snprintf(buf, sizeof(buf), "\n\n①%02d %02d %02d %02d %02d+%02d %02d",
			 qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%12+1, qrand()%12+1);
	PrinterPrintString(U2G(buf).data());
	PrinterFeedLine(1);
	snprintf(buf, sizeof(buf), "\n\n②%02d %02d %02d %02d %02d+%02d %02d",
			 qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%12+1, qrand()%12+1);
	PrinterPrintString(U2G(buf).data());
	PrinterFeedLine(1);
	snprintf(buf, sizeof(buf), "\n\n③%02d %02d %02d %02d %02d+%02d %02d",
			 qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%12+1, qrand()%12+1);
	PrinterPrintString(U2G(buf).data());
	PrinterFeedLine(1);
	snprintf(buf, sizeof(buf), "\n\n④%02d %02d %02d %02d %02d+%02d %02d",
			 qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%12+1, qrand()%12+1);
	PrinterPrintString(U2G(buf).data());
	PrinterFeedLine(1);
	snprintf(buf, sizeof(buf), "\n\n⑤%02d %02d %02d %02d %02d+%02d %02d",
			 qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%12+1, qrand()%12+1);
	PrinterPrintString(U2G(buf).data());
	PrinterFeedLine(1);
    //打印广告
    PrinterSetLineSpace(15);
    PrinterSetFont(0x00,0x00,0x01);
    PrinterPrintString(U2G("电话查询:123456短信查询:输入1234发至4321").data());
    PrinterFeedLine(1);
    PrinterPrintString(U2G("********胡同****号****大厦").data());
    PrinterFeedLine(3);

    return true;
}

bool LAPrinter::PrintPDFTicketHeader(QString &pstr)
{
    if(TRUE != printerIsReady())
    {
		qDebug("PrinterIsReady error\n");
		return false;
    }
    //unsigned int status = PrinterGetStatus();
    //if(status != 0) return false;

    PrinterSetLeftMargin(0x0);
    PrinterSetAreaWidth(0x0300);
    PrinterSetLineSpace(10);
    PrinterSetCharSpace(0);

    PrinterSetFont(0x00,0x10,0x01);
    PrinterPrintString(U2G("体彩<测试样票>").data());
    PrinterFeedLine(1);

    PrinterSetLineSpace(0);
    PrinterFeedLine(1);
    PrinterSetFont(0x00,0x00,0x01);
    PrinterPrintString(U2G("35073110603460810054   d2WIvi   64319331").data());
    PrinterFeedLine(1);

    PrinterPrintString(U2G("销售点:06034      06073期 2006/03/22开奖").data());
    PrinterFeedLine(1);

    PrinterPrintString(U2G("倍:1  合计:10元      2006/03/22 12:27:49\n").data());
    PrinterFeedLine(1);

    PrinterSetLineSpace(10);
    PrinterFeedLine(1);
    PrinterSetFont(0x00,0x10,0x01);
    PrinterPrintString(U2G("直 选 票").data());
    PrinterFeedLine(1);

    PrinterSetFont(0x01,0x10,0x01);
    PrinterSetLineSpace(0);

	pstr = "35073110603460810054   d2WIvi   64319331\n" +
			QString("06034 06073 2006/03/22\n");

	qsrand((uint)time(NULL));
	char buf[256] = {0};

        snprintf(buf, sizeof(buf), "\n\n①%02d %02d %02d %02d %02d+%02d %02d",
			 qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%12+1, qrand()%12+1);
	pstr += QString(buf+5) + "\n";
	PrinterPrintString(U2G(buf).data());
	PrinterFeedLine(1);
	snprintf(buf, sizeof(buf), "\n\n②%02d %02d %02d %02d %02d+%02d %02d",
			 qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%12+1, qrand()%12+1);
	pstr += QString(buf+5) + "\n";
	PrinterPrintString(U2G(buf).data());
	PrinterFeedLine(1);
	snprintf(buf, sizeof(buf), "\n\n③%02d %02d %02d %02d %02d+%02d %02d",
			 qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%12+1, qrand()%12+1);
	pstr += QString(buf+5) + "\n";
	PrinterPrintString(U2G(buf).data());
	PrinterFeedLine(1);
	snprintf(buf, sizeof(buf), "\n\n④%02d %02d %02d %02d %02d+%02d %02d",
			 qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%12+1, qrand()%12+1);
	pstr += QString(buf+5) + "\n";
	PrinterPrintString(U2G(buf).data());
	PrinterFeedLine(1);
	snprintf(buf, sizeof(buf), "\n\n⑤%02d %02d %02d %02d %02d+%02d %02d",
			 qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%35+1, qrand()%12+1, qrand()%12+1);
        pstr += QString(buf+5) + "\n";


	PrinterPrintString(U2G(buf).data());
	PrinterFeedLine(1);
    //打印广告
    PrinterSetLineSpace(15);
    PrinterSetFont(0x00,0x00,0x01);
    PrinterPrintString(U2G("电话查询:123456短信查询:输入1234发至4321").data());
    PrinterFeedLine(1);
    PrinterPrintString(U2G("********胡同****号****大厦").data());
    PrinterFeedLine(3);

    return true;
}

bool LAPrinter::Init(const char* const input_dir, const char* const output_dir)
{
	if(!bmutexIsOK)
	{
		qDebug("thread mutex is bad!\n");
		return false;
	}
	strMarkData = "";
	strPDF417Data = "";

	strErrorInfo = tr("无错误");
    if(NULL != PrinterInit)
    {
		if(!pthread_mutex_lock(&printerMutex))
		{
			if(NO_ERROR != PrinterInit(input_dir, output_dir))
	        {
				bPrinterInit = false;
	            qDebug("PrinterInit error!\n");
	        }
	        else
	        {
	            bPrinterInit = true;
                    qDebug("Printer Init ok!\n");
				//PrinterSetCutterMode(0);
                                //PrinterCutPaper();
	        }
			pthread_mutex_unlock(&printerMutex);
		}
		return bPrinterInit;
    }
	strErrorInfo = tr("初始化打印机失败");
	return false;
}

bool LAPrinter::GetInitStatus()
{
	return bPrinterInit;
}

bool LAPrinter::PrintSampleTicket()//打印样票
{
    char buf[1024] = {0};

	strErrorInfo = tr("无错误");
    if(!bPrinterInit)
    {
                qDebug("打印机未初始化成功.");
		strErrorInfo = tr("初始化打印机失败");
		return false;
    }
	if(pthread_mutex_trylock(&printerMutex))
	{
		qDebug("打印机繁忙！");
		strErrorInfo = tr("打印机繁忙！");
		return false;
	}

        if(TRUE != printerIsReady())
    {
		pthread_mutex_unlock(&printerMutex);
		strErrorInfo = tr("打印机状态异常");
		qDebug("PrinterIsReady error\n");
		return false;
    }
        CreateMark(buf,96);
	strMarkData = tr(buf);

	//打印大黑标票
	PrinterSetCutterMode(0);
    if(PrintTicketHeader())
    {
        PrinterFeedLine(3);
//        PrinterPrintBigMark(buf, 96);
         PrinterPrintMark(buf, 96);
                PrinterCutPaper();

    }
    else
    {
		pthread_mutex_unlock(&printerMutex);
		strErrorInfo = tr("打印样票出错");
        return false;
    }
    memset(buf, 0, sizeof(buf));

	//打印二维条码
	QString pstr;
    if(PrintPDFTicketHeader(pstr))
	{
		strcpy(buf, U2G(pstr).data());
		strPDF417Data = pstr;
		PrinterPrint2DBar(15, 30, 20, 6, 3, buf, strlen(buf), 1);

        PrinterCutPaper();//PrinterCutPaper();
    }
    else
    {
		pthread_mutex_unlock(&printerMutex);
		strErrorInfo = tr("打印样票出错");
        return false;
    }

	if(PrintTicketHeader())
    {
        PrinterFeedLine(3);
//        PrinterPrint1DBar(2,56, U2G("123456789ABa%%^&*(*").data(),73,0x42);//打印一维条码
        PrinterCutPaper();//PrinterCutPaper();
    }
    else
    {
		pthread_mutex_unlock(&printerMutex);
		strErrorInfo = tr("打印样票出错");
        return false;
    }

	pthread_mutex_unlock(&printerMutex);
	return true;
}

void LAPrinter::PrintTestStr()
{
    PrinterPrintString(U2G("打印机测试abCD123(&*@@@\n").data());
}

bool LAPrinter::PrintTest() //打印测试
{
	char buf[128];
	if(!bPrinterInit)
	{
		qDebug("打印机未初始化成功!");
		//QMessageBox::information(NULL,QObject::tr("打印机信息"),QObject::tr("打印机未初始化成功，请重新初始化!"),QObject::tr("确定"));
		return false;
	}
	if(pthread_mutex_trylock(&printerMutex))
	{
		qDebug("打印机繁忙!");
		//QMessageBox::information(NULL,QObject::tr("打印机信息"),QObject::tr("打印机繁忙!"),QObject::tr("确定"));
		return false;
	}

        if( TRUE != printerIsReady())
	{
		qDebug("PrinterIsReady error\n");
		//QMessageBox::information(NULL,QObject::tr(""),QObject::tr("打印机状态异常！\n"),QObject::tr("知道了！"));
		pthread_mutex_unlock(&printerMutex);
		return false;
	}

	PrinterSetCutterMode(1);
	PrinterFeedLine(1);
	PrinterSetLeftMargin(0x0);
	PrinterSetAreaWidth(0x0300);
	PrinterSetLineSpace(10);
	PrinterSetCharSpace(0);
	PrinterSetFont(0x00,0x00,0x01);//1号字,靠中
	PrinterPrintString(U2G("1号字,靠中").data());

	PrinterFeedLine(1);
	PrinterSetFont(0x00,0x10,0x00);//1号字,大字体,靠左
	PrinterPrintString(U2G("1号字,大字体,靠左").data());

	PrinterFeedLine(1);
	PrinterSetFont(0x01,0x00,0x00);//2号字,靠左
	PrinterPrintString(U2G("2号字,靠左").data());

	PrinterFeedLine(1);
	PrinterSetFont(0x01,0x00,0x01);//2号字,靠中
	PrinterPrintString(U2G("2号字,靠中").data());

	PrinterFeedLine(1);
	PrinterSetFont(0x01,0x00,0x02);//2号字,靠右
	PrinterPrintString(U2G("2号字,靠右").data());

	PrinterFeedLine(1);
	PrinterSetFontEmpha(0x0001);//粗体
	PrinterPrintString(U2G("粗体").data());

	PrinterFeedLine(1);
	PrinterSetFontEmpha(0x0000);//取消粗体
	PrinterPrintString(U2G("取消粗体").data());

	PrinterFeedLine(1);
	PrinterSetLineSpace(10);//设置行间距
	PrinterPrintString(U2G("设置行间距10").data());
	PrintTestStr();

	PrinterFeedLine(1);
	PrinterSetCharSpace(3);//设置字符间距
	PrinterPrintString(U2G("设置字符间距为3，").data());
	PrintTestStr();

	PrinterFeedLine(1);
	PrinterSetFont(0x01,0x00,0x00);//2号字,靠左
	PrinterSetLeftMargin(10);//设置左边界
	PrinterPrintString(U2G("设置左边界为10，").data());
	PrintTestStr();

	PrinterSetFont(0x00,0x00,0x01);//恢复字体
	PrinterFeedLine(1);
	PrinterSetAreaWidth(0x0100);//置有效打印区域宽度
	PrinterPrintString(U2G("设置有效打印区域宽度为0x100，").data());
	PrintTestStr();

	PrinterSetLeftMargin(0x0);
	PrinterSetAreaWidth(0x0300);
	PrinterSetLineSpace(10);
	PrinterSetCharSpace(0);
	PrinterSetFont(0x00,0x00,0x01);//恢复字体
	PrinterFeedLine(1);
	PrinterSetAreaWidth(0x0300);//置有效打印区域宽度
	PrinterPrintString(U2G("设置有效打印区域宽度为0x300，").data());
	PrintTestStr();

	PrinterFeedLine(1);
	PrinterPrintString(U2G("用户自定义字符").data());
	PrinterFeedLine(1);
	unsigned char bar[37]=//user-define characters
	{
		0x0c, //font size: 12*24
		0xff,0x80,0x00,
		0xff,0x80,0x00,
		0xff,0x80,0x00,
		0xff,0x80,0x00,
		0xff,0x80,0x00,
		0xff,0x80,0x00,
		0xff,0x80,0x00,
		0xff,0x80,0x00,
		0xff,0x80,0x00,
		0xff,0x80,0x00,
		0xff,0x80,0x00,
		0xff,0x80,0x00
	};
	PrinterSetUserChar('@','@',9,bar,37);
	PrintTestStr();
	PrinterCancelUserDefChar('@','@');


	PrinterFeedLine(15);
	PrinterPrintString(U2G("打印位图").data());
	PrinterFeedLine(3);
	strcpy(buf, U2G(LATECH_PATH"/demo.bmp").data());
	PrinterPrintString(buf);
	PrinterFeedLine(3);
	printDiskImage(0, 0, buf);
	//PrintUserBitmap(buf);

	PrinterFeedLine(15);
	PrinterPrintString(U2G("打印一维条码").data());
	PrinterFeedLine(3);
	strcpy(buf, "123456789ABa%%^&*(*");
	PrinterPrintString(buf);
	PrinterFeedLine(3);
//	PrinterPrint1DBar(2,56, U2G("123456789ABa%%^&*(*").data(),73,0x42);//打印一维条码

	PrinterFeedLine(15);
	PrinterPrintString(U2G("打印二维条码").data());
	PrinterFeedLine(3);
	strcpy(buf, U2G("123456789ABa%%^&*(*中文1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ").data());
	PrinterPrintString(buf);
	PrinterFeedLine(3);
	qDebug("打印二维条码");
	PrinterPrint2DBar(15, 30, 20, 6, 3, buf, strlen(buf), 1);


	PrinterFeedLine(15);
	PrinterPrintString(U2G("打印小黑标").data());
	PrinterFeedLine(3);
	CreateMark(buf, 96);
//	PrinterPrintSmallMark(buf, 96);
        PrinterPrintMark(buf, 96);

	PrinterFeedLine(15);
	PrinterPrintString(U2G("打印大黑标").data());
	PrinterFeedLine(3);
	CreateMark(buf, 96);
//	PrinterPrintBigMark(buf, 96);
        PrinterPrintMark(buf, 96);
	PrinterSetCutterMode(0);
	pthread_mutex_unlock(&printerMutex);
	return true;
}

bool LAPrinter::PrintLengthTicket()//打印非定长票
{
	int i;
	if(pthread_mutex_trylock(&printerMutex))
	{
		qDebug("打印机繁忙!");
		return false;
	}

        if( TRUE != printerIsReady())
	{
		qDebug("PrinterIsReady error\n");
		pthread_mutex_unlock(&printerMutex);
		return false;
	}

	PrinterSetFont(0x00,0x00,0x01);
	PrinterSetLineSpace(10);
	PrinterSetLeftMargin(0x0);
	PrinterSetCharSpace(0);

	PrinterSetCutterMode(1);
	for(i = 0; i < 10; i++)
	{
		PrinterPrintString(U2G("非定长短票").data());
		PrinterFeedLine(1);
	}
        PrinterCutPaper();//PrinterCutPaper();

	for(i = 0; i < 42; i++)
	{
		PrinterPrintString(U2G("非定长长票").data());
		PrinterFeedLine(1);
	}
        PrinterCutPaper();//PrinterCutPaper();

	PrinterSetCutterMode(1);
	PrinterFeedLine(1);
        PrinterCutPaper();//PrinterCutPaper();
	pthread_mutex_unlock(&printerMutex);
	return true;
}

bool LAPrinter::PrintBitmap()//打印位图
{
	if(pthread_mutex_trylock(&printerMutex))
	{
		qDebug("打印机繁忙!");
		return false;
	}

        if(TRUE != printerIsReady())
	{
		qDebug("PrinterIsReady error\n");
		pthread_mutex_unlock(&printerMutex);
		return false;
	}

	PrinterSetCutterMode(0);
    PrinterSetFont(0x00,0x00,0x01);
	printDiskImage(0, 0, LATECH_PATH"/demo.bmp");
    PrinterCutPaper();//PrinterCutPaper();
	pthread_mutex_unlock(&printerMutex);
	return true;
}

bool LAPrinter::AnglePrint()//旋转打印
{
	if(pthread_mutex_trylock(&printerMutex))
	{
		qDebug("打印机繁忙!");
		return false;
	}

        if(TRUE != printerIsReady())
	{
		qDebug("PrinterIsReady error\n");
		pthread_mutex_unlock(&printerMutex);
		return false;
	}

	PrinterSetCutterMode(0);

	PrinterSetPageMode(400, 800, 40, 40);
	PrinterSetFont(0x00,0x10,0x00);
	PrinterSetLineSpace(60);
    PrinterSetAngle(90);//SetAngle(x),当x不为0,都是默认为页模式，为0时是行模式。
    PrinterFeedLine(1);
	PrinterPrintString(U2G("旋转90度打印 旋转90度打印 旋转90度打印 旋转90度打印 ").data());
	PrinterFeedLine(1);
	PrinterPrintPage();
    PrinterCutPaper();//PrinterCutPaper();

	PrinterSetPageMode(400, 800, 40, 40);
	PrinterSetFont(0x00,0x10,0x00);
	PrinterSetLineSpace(60);
	PrinterSetAngle(180);//SetAngle(x),当x不为0,都是默认为页模式，为0时是行模式。
    PrinterFeedLine(1);
	PrinterPrintString(U2G("旋转180度打印 旋转180度打印 旋转180度打印 旋转180度打印 ").data());
	PrinterFeedLine(1);
	PrinterPrintPage();
    PrinterCutPaper();//PrinterCutPaper();

	PrinterSetPageMode(400, 800, 40, 40);
	PrinterSetFont(0x00,0x10,0x00);
	PrinterSetLineSpace(60);
	PrinterSetAngle(270);//SetAngle(x),当x不为0,都是默认为页模式，为0时是行模式。
    PrinterFeedLine(1);
	PrinterPrintString(U2G("旋转270度打印 旋转270度打印 旋转270度打印 旋转270度打印 ").data());
	PrinterFeedLine(1);
	PrinterPrintPage();
    PrinterCutPaper();//PrinterCutPaper();

	PrinterSetPageMode(400, 800, 40, 40);
	PrinterSetFont(0x00,0x10,0x00);
	PrinterSetLineSpace(60);
	PrinterSetAngle(0);//SetAngle(x),当x不为0,都是默认为页模式，为0时是行模式。
    PrinterFeedLine(1);
	PrinterPrintString(U2G("旋转0度打印 旋转0度打印 旋转0度打印 旋转0度打印 ").data());
	PrinterFeedLine(1);
	PrinterPrintPage();
    PrinterCutPaper();//PrinterCutPaper();

	PrinterSetLineMode();
	PrinterSetFont(0x00,0x00,0x01);
	PrinterSetLineSpace(10);
	pthread_mutex_unlock(&printerMutex);
	return true;
}

bool LAPrinter::FullCutAndHalfCut()//全切/半切测试
{
	if(pthread_mutex_trylock(&printerMutex))
	{
		qDebug("打印机繁忙!");
		return false;
	}

        if(TRUE != printerIsReady())
	{
		qDebug("PrinterIsReady error\n");
		pthread_mutex_unlock(&printerMutex);
		return false;
	}

    PrinterSetCutterMode(0);
    if(NULL !=  PrinterCutPaper)
    {
        PrinterFeedLine(5);
        PrinterSetFont(0, 0x22, 1);
        PrinterPrintString(U2G("全切").data());
        PrinterFeedLine(1);
        PrinterCutPaper();

        PrinterFeedLine(5);
        PrinterPrintString(U2G("半切").data());
        PrinterFeedLine(1);
        PrinterSetFont(0x00,0x00,0x01);
        PrinterFeedLine(2);
        PrinterCutPaper();

    }
	pthread_mutex_unlock(&printerMutex);
    return true;
}

bool LAPrinter::PrintNTickets(char *buf, int len, int Num, bool bCut)//打印N张
{
	const int paper = Num;//张数
    long count=0;
	bool bStop = false;

	if(!pthread_mutex_trylock(&printerMutex))
	{
		qDebug("打印机忙!");
		return false;
	}
    while(!bStop)
    {

                if(TRUE != printerIsReady())
        {
			qDebug("PrinterIsReady error\n");
			bStop = true;
			pthread_mutex_unlock(&printerMutex);
			return false;
        }
        if(PrintTicketHeader())
        {
            PrinterFeedLine(3);
            PrinterPrintMark(buf, len);
            PrinterFeedLine(22);//22
                        if(bCut) PrinterCutPaper();
            count++;
			QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
        else
        {
			//QMessageBox::information(NULL,QObject::tr(""),QObject::tr("打印机打印数据出错！\n此次速度测试不可作为参考数据!\n"),QObject::tr("关闭"));
            pthread_mutex_unlock(&printerMutex);
			return false;
        }
        if(count >= paper)
        {
            break;
        }
    }
	pthread_mutex_unlock(&printerMutex);
	return true;
}

bool LAPrinter::PrintNLines(char *buf, unsigned lines)//打印N行
{
    unsigned long count = 0;
	if(pthread_mutex_trylock(&printerMutex))
	{
		qDebug("打印机繁忙!");
		//QMessageBox::information(this,trUtf8("打印机信息"),trUtf8("打印机繁忙!"),trUtf8("确定"));
		return false;
	}
    while(count < lines)
    {

        if(NO_ERROR == PrinterPrintString(buf))
        {
            count++;
            PrinterFeedLine(1);
			QCoreApplication::processEvents();
        }
		else
		{
			QMessageBox::information(NULL,QObject::tr(""),QObject::tr("打印机打印数据出错！\n此次速度测试不可作为参考数据!\n"),QObject::tr("关闭"));
            pthread_mutex_unlock(&printerMutex);
			return false;
		}
    }
	pthread_mutex_unlock(&printerMutex);
	return true;
}

long LAPrinter::PrintNSeconds(char *buf, int len, int sec, bool bCut)//打印N秒样票
{
	bool bStop = false;
    const int t = sec*1000;//打印时间（s）
    unsigned long count = 0;
	int t2;

    QTime time1 = QTime::currentTime();
    QTime time2;
	if(pthread_mutex_trylock(&printerMutex))
	{
		qDebug("打印机忙!");
		//QMessageBox::information(NULL,QObject::tr(""),QObject::tr("打印机忙!\n"),QObject::tr("关闭"));
		return -1;
	}
	bPrintInterupt = false;
    while(!bStop)
    {
		if(bPrintInterupt)
		{
			break;
		}

                if( TRUE != printerIsReady())
        {
                qDebug("PrinterIsReady error\n");
				bStop = true;
				pthread_mutex_unlock(&printerMutex);
				return -1;
        }

        if(PrintTicketHeader())
        {
            PrinterFeedLine(3);
            //PrinterPrintBigMark(buf, len);
            PrinterPrintMark(buf, len);
            PrinterFeedLine(22);//22
            if(bCut) PrinterCutPaper();
            count++;
        }
        else
        {
                        PrinterCutPaper();//PrinterCutPaper();
			pthread_mutex_unlock(&printerMutex);
			return -1;
        }
        //usleep(500000);//防止过多的命令，打印速度跟不上发送命令的速度。
		PrinterPrintIsComplete(10);
        time2 = QTime::currentTime();
		t2 = time1.msecsTo(time2);
        if(t2 >= t)
        {
            break;
        }
		else
		{
			QCoreApplication::processEvents();
		}
    }
        PrinterCutPaper();
	pthread_mutex_unlock(&printerMutex);
    return count;
}

unsigned int LAPrinter::GetStatus()
{
	unsigned int ret;

        ret =1;
	if(pthread_mutex_trylock(&printerMutex))
	{
		qDebug("打印机繁忙!");
		return -1;
	}

	pthread_mutex_unlock(&printerMutex);
        return ret;
}

const QString& LAPrinter::GetStatusString()
{
	static QString statusStr;
	unsigned int printerStatus;


	if(pthread_mutex_trylock(&printerMutex))
	{
		statusStr = tr("获取打印机状态出错, 打印机繁忙!");
		return statusStr;
	}
        printerStatus = (unsigned int)PrinterIsReady();

	pthread_mutex_unlock(&printerMutex);

	statusStr = "";
	if(printerStatus == 0)
	{
		statusStr = "正常";
		return statusStr;
	}

	if(FLAG_ISSET(printerStatus, ST_CUTER_ERR))
	{
		statusStr += tr("卡纸 ");
	}
	if(FLAG_ISSET(printerStatus, ST_PAPER_NEAR_END))
	{
		statusStr += tr("纸将尽 ");
	}
	if(FLAG_ISSET(printerStatus, ST_PAPER_END))
	{
		statusStr += tr("缺纸 ");
	}
	if(FLAG_ISSET(printerStatus, ST_PLATEN_OPEN))
	{
		statusStr += tr("上盖打开 ");
	}
	if(!GetInitStatus())
	{
		statusStr = tr("打印机未初始化");
	}
	if(statusStr.length() == 0)
	{
		statusStr = tr("打印机出错(未知错误)");
	}

          char errStr[1024];
          PrinterGetLastErrorStr(errStr,sizeof(errStr));
          statusStr=QString(errStr);
           //statusStr=QString::number(err);

	return statusStr;
}

QString LAPrinter::GetMarkData()
{
	return strMarkData;
}

QString LAPrinter::GetPDF417Data()
{
	return strPDF417Data;
}

bool LAPrinter::GetPrinterHWInformation(char *buf, int length)
{
	bool ret;
	if(PrinterGetHWInformation == NULL) return false;


	if(pthread_mutex_trylock(&printerMutex))
	{
		qDebug("打印机繁忙!");
		return false;
	}
	ret = PrinterGetHWInformation(buf, length);
	pthread_mutex_unlock(&printerMutex);
	return ret;
}

void LAPrinter::PrintTestBlock()
{
	if(pthread_mutex_trylock(&printerMutex))
	{
		qDebug("打印机繁忙!");
		return ;
	}
	printDiskImage(0, 0, "./block2.bmp");
//	PrinterPrintStatus();
	sleep(3);
        PrinterCutPaper();
	pthread_mutex_unlock(&printerMutex);
}

void LAPrinter::PrintDensityBlock()
{
//	if(pthread_mutex_trylock(&printerMutex))
//	{
//		qDebug("打印机繁忙!");
//		return ;
//	}
//	PrinterSetDensity(6); //130%
//	sleep(2);
//	qApp->processEvents();
//	PrinterInit(SO_PATH, SO_PATH);
//	printDiskImage(0, 0, "./block0.bmp");
//	PrinterSetFont(0, 0, 1);
//	PrinterPrintString("130%");
//	PrinterFeedLine(2);

//	sleep(2);
//	qApp->processEvents();
//	PrinterSetDensity(4); //120%
//	sleep(2);
//	qApp->processEvents();
//	PrinterInit(SO_PATH, SO_PATH);
//	printDiskImage(0, 0, "./block0.bmp");
//	PrinterSetFont(0, 0, 1);
//	PrinterPrintString("120%");
//	PrinterFeedLine(2);

//	sleep(2);
//	qApp->processEvents();
//	PrinterSetDensity(2); //110%
//	sleep(2);
//	qApp->processEvents();
//	PrinterInit(SO_PATH, SO_PATH);
//	printDiskImage(0, 0, "./block0.bmp");
//	PrinterPrintString("110%");
//	PrinterFeedLine(2);

//	sleep(2);
//	qApp->processEvents();
//	PrinterSetDensity(0); //100%
//	sleep(2);
//	qApp->processEvents();
//	PrinterInit(SO_PATH, SO_PATH);
//	printDiskImage(0, 0, "./block0.bmp");
//	PrinterPrintString("100%");
//	PrinterFeedLine(2);

//	sleep(2);
//	qApp->processEvents();
//	PrinterSetDensity(65534); //90%
//	sleep(2);
//	qApp->processEvents();
//	PrinterInit(SO_PATH, SO_PATH);
//	printDiskImage(0, 0, "./block0.bmp");
//	PrinterPrintString("90%");
//	PrinterFeedLine(2);

//	sleep(2);
//	qApp->processEvents();
//	PrinterSetDensity(4); //120%
//	sleep(2);
//	qApp->processEvents();
//	PrinterInit(SO_PATH, SO_PATH);

//	printDiskImage(0, 0, "./block1.bmp");
//	PrinterPrintString("    0.2     0.3       0.4      0.5");
//	PrinterFeedLine(5);

//	PrinterCutPaper();
//	pthread_mutex_unlock(&printerMutex);
}

bool LAPrinter::PrintBigMarkTicket()
{
        //char buf[1024] = {0};

	strErrorInfo = tr("无错误");
	if(!bPrinterInit)
	{
		qDebug("打印机未初始化成功.");
		strErrorInfo = tr("初始化打印机失败");
		return false;
	}
	if(pthread_mutex_trylock(&printerMutex))
	{
		qDebug("打印机繁忙！");
		strErrorInfo = tr("打印机繁忙！");
		return false;
	}
        //unsigned int status = PrinterGetStatus();
        if( TRUE != printerIsReady())
	{
		pthread_mutex_unlock(&printerMutex);
		strErrorInfo = tr("打印机状态异常");
		qDebug("PrinterIsReady error\n");
		return false;
	}
        //CreateMark(buf,96);
        char buf[]="11000111011101001110000001100100010000011011101111011000001011110100110110110010010000011000100";
	PrinterSetCutterMode(0);
        strMarkData = QString(buf);

	if(PrintTicketHeader())
	{
		PrinterFeedLine(3);
//                PrinterPrintBigMark(buf, 96);//长度不超过96,并且要求是12的倍数
                PrinterPrintMark(buf, 96);
                PrinterCutPaper();
	}
	else
	{
		pthread_mutex_unlock(&printerMutex);
		strErrorInfo = tr("打印样票出错");
		return false;
	}
	pthread_mutex_unlock(&printerMutex);
	return true;
}

bool LAPrinter::PrintSmallMarkTicket()
{
        //char buf[1024] = {0};

	strErrorInfo = tr("无错误");
	if(!bPrinterInit)
	{
                qDebug("打印机未初始化成功");
		strErrorInfo = tr("初始化打印机失败");
		return false;
	}
	if(pthread_mutex_trylock(&printerMutex))
	{
		qDebug("打印机繁忙！");
		strErrorInfo = tr("打印机繁忙！");
		return false;
	}

        if(TRUE != printerIsReady())
	{
		pthread_mutex_unlock(&printerMutex);
		strErrorInfo = tr("打印机状态异常");
		qDebug("PrinterIsReady error\n");
		return false;
	}
        //CreateMark(buf,96);
        char buf[]="010101110000101000011111100001000100110111011011011101010111011001010100101000000010010101110000";
	PrinterSetCutterMode(0);

	if(PrintTicketHeader())
	{
		PrinterFeedLine(3);
//		PrinterPrintSmallMark(buf, 96);
                PrinterPrintMark(buf, 96);
                PrinterCutPaper();
	}
	else
	{
		pthread_mutex_unlock(&printerMutex);
		strErrorInfo = tr("打印样票出错");
		return false;
	}
	pthread_mutex_unlock(&printerMutex);
	return true;
}

bool LAPrinter::PrintCodeTicket()
{
    QString pstr;
    if(TRUE != printerIsReady())
    {
                qDebug("PrinterIsReady error\n");
                return false;
    }
    //unsigned int status = PrinterGetStatus();
    //if(status != 0) return false;

    PrinterSetLeftMargin(0x0);
    PrinterSetAreaWidth(0x0300);
    PrinterSetLineSpace(10);
    PrinterSetCharSpace(0);

    PrinterSetFont(0x00,0x10,0x01);
    PrinterPrintString(U2G("体彩<测试样票>").data());
    PrinterFeedLine(1);

    PrinterSetLineSpace(0);
    PrinterFeedLine(1);
    PrinterSetFont(0x00,0x00,0x01);
    PrinterPrintString(U2G("35073110603460810054   d2WIvi   64319331").data());
    PrinterFeedLine(1);

    PrinterPrintString(U2G("销售点:06034      06073期 2006/03/22开奖").data());
    PrinterFeedLine(1);

    PrinterPrintString(U2G("倍:1  合计:10元      2006/03/22 12:27:49\n").data());
    PrinterFeedLine(1);

    PrinterSetLineSpace(10);
    PrinterFeedLine(1);
    PrinterSetFont(0x00,0x10,0x01);
    PrinterPrintString(U2G("直 选 票").data());
    PrinterFeedLine(1);

    PrinterSetFont(0x01,0x10,0x01);
    PrinterSetLineSpace(0);

        pstr = "35073110603460810054   d2WIvi   64319331\n" +
                        QString("06034 06073 2006/03/22\n");

        qsrand((uint)time(NULL));
        char buf[256] = {0};
        snprintf(buf, sizeof(buf), "\n\n①%02d %02d %02d %02d %02d+%02d %02d",
                         15, 21, 35, 13, 28, 5, 5);
        pstr += QString(buf+5) + "\n";
        PrinterPrintString(U2G(buf).data());
//        PrinterFeedLine(1);
        snprintf(buf, sizeof(buf), "\n\n②%02d %02d %02d %02d %02d+%02d %02d",
                         22, 20, 03, 13, 18, 29, 7);
        pstr += QString(buf+5) + "\n";
        PrinterPrintString(U2G(buf).data());
//        PrinterFeedLine(1);
        snprintf(buf, sizeof(buf), "\n\n③%02d %02d %02d %02d %02d+%02d %02d",
                         32, 11, 4, 15, 3, 21, 9);
        pstr += QString(buf+5) + "\n";
        PrinterPrintString(U2G(buf).data());
//        PrinterFeedLine(1);
        snprintf(buf, sizeof(buf), "\n\n④%02d %02d %02d %02d %02d+%02d %02d",
                         22, 34, 5, 16, 27, 19, 24);
        pstr += QString(buf+5) + "\n";
        PrinterPrintString(U2G(buf).data());
//        PrinterFeedLine(1);
        snprintf(buf, sizeof(buf), "\n\n⑤%02d %02d %02d %02d %02d+%02d %02d",
                         4, 13, 18, 26, 33, 17, 7);
        pstr += QString(buf+5) + "\n";
        PrinterPrintString(U2G(buf).data());
        PrinterFeedLine(1);
    //打印广告
    PrinterSetLineSpace(15);
    PrinterSetFont(0x00,0x00,0x01);
    PrinterPrintString(U2G("电话查询:123456短信查询:输入1234发至4321").data());
    PrinterFeedLine(1);
    PrinterPrintString(U2G("********胡同****号****大厦").data());
    PrinterFeedLine(3);

    strcpy(buf, U2G(pstr).data());
    strPDF417Data = pstr;
    PrinterPrint2DBar(15, 30, 20, 6, 3, buf, strlen(buf), 1);

    PrinterCutPaper();//PrinterCutPaper();

    //第二张
    {
        QString pstr2;
        PrinterSetLeftMargin(0x0);
        PrinterSetAreaWidth(0x0300);
        PrinterSetLineSpace(10);
        PrinterSetCharSpace(0);

        PrinterSetFont(0x00,0x10,0x01);
        PrinterPrintString(U2G("中国体育彩票<排列3>").data());
        PrinterFeedLine(1);

        PrinterSetLineSpace(0);
        PrinterFeedLine(1);
        PrinterSetFont(0x00,0x00,0x01);
        PrinterPrintString(U2G("467853028972603721200  53WOLiiq   990162").data());

        PrinterFeedLine(1);

        PrinterPrintString(U2G("销售点:06034      02013期 2017/03/22开奖").data());
        PrinterFeedLine(1);

        PrinterPrintString(U2G("倍:1  合计:10元      2017/03/22 12:27:49\n").data());
        PrinterFeedLine(1);

        PrinterSetLineSpace(10);
        PrinterFeedLine(1);
        PrinterSetFont(0x00,0x10,0x01);
        PrinterPrintString(U2G("直 选 票").data());
        PrinterFeedLine(1);

        PrinterSetFont(0x01,0x10,0x01);
        PrinterSetLineSpace(0);

            pstr2 = "467853028972603721200  53WOLiiq   990162\n" +
                            QString("06034 02013 2017/03/22\n");

            qsrand((uint)time(NULL));
            char buf[256] = {0};
            snprintf(buf, sizeof(buf), "\n\n①%02d %02d %02d %02d %02d+%02d %02d",
                             42, 11, 25, 33, 58, 5, 5);
            pstr2 += QString(buf+5) + "\n";
            PrinterPrintString(U2G(buf).data());
//            PrinterFeedLine(1);
            snprintf(buf, sizeof(buf), "\n\n②%02d %02d %02d %02d %02d+%02d %02d",
                             32, 50, 07, 13, 18, 29, 7);
            pstr2 += QString(buf+5) + "\n";
            PrinterPrintString(U2G(buf).data());
//            PrinterFeedLine(1);
            snprintf(buf, sizeof(buf), "\n\n③%02d %02d %02d %02d %02d+%02d %02d",
                             82, 11, 4, 15, 3, 21, 2);
            pstr2 += QString(buf+5) + "\n";
            PrinterPrintString(U2G(buf).data());
//            PrinterFeedLine(1);
//            snprintf(buf, sizeof(buf), "\n\n④%02d %02d %02d %02d %02d+%02d %02d",
//                             12, 34, 2, 16, 17, 39, 54);
//            pstr += QString(buf+5) + "\n";
//            PrinterPrintString(U2G(buf).data());
//            PrinterFeedLine(1);
//            snprintf(buf, sizeof(buf), "\n\n⑤%02d %02d %02d %02d %02d+%02d %02d",
//                             1, 13, 18, 36, 33, 17, 8);
//            pstr += QString(buf+5) + "\n";
//            PrinterPrintString(U2G(buf).data());
//            PrinterFeedLine(1);
        //打印广告
         PrinterFeedLine(3);
        PrinterSetLineSpace(15);
        PrinterSetFont(0x00,0x00,0x01);
        PrinterPrintString(U2G("手机查询:888888短信查询:输入6666发至9876").data());
        PrinterFeedLine(1);
        PrinterPrintString(U2G("********胡同****号****大厦").data());
        PrinterFeedLine(3);

        strcpy(buf, U2G(pstr2).data());
        strPDF417Data = pstr;
        PrinterPrint2DBar(15, 30, 20, 6, 3, buf, strlen(buf), 1);

        PrinterCutPaper();//PrinterCutPaper();
    }
    return true;
}
