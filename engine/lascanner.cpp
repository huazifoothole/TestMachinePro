#include "lascanner.h"
#include "common.h"
#include <QImage>
#include <QPainter>
#include <QFont>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <dlfcn.h>
#include <dirent.h>


LAScanner::LAScanner(QObject *parent) : QThread(parent)
{
	dp_ = NULL;
	bStop = false;
	bScannerInit = false;
	bOnlyScanning = true;//默认只是扫描票
	bPrint = false;
	if(0 == pthread_mutex_init(&scannerMutex, NULL))
	{
		bmutexIsOK = true;
	}
	else
	{
		bmutexIsOK = false;
	}
	//connect(this,SIGNAL(ScanFinishedScanFinished(int , QString )), this, SLOT(ScanFinished(int errCode, QString str)));
}

LAScanner::~LAScanner()
{
	if(bmutexIsOK) pthread_mutex_destroy(&scannerMutex);
	if(dp_ != NULL) dlclose(dp_);
}

void LAScanner::SetStop()
{
	bStop = true;
}

bool LAScanner::LoadDriverSympol(const char *const file)
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
    dp_ = dlopen(file,RTLD_LAZY);
	if( (error=dlerror()) != NULL)
	{
		printf("openso is %s\n",error);
		return false;
	}

	ScannerInit = (InitS)dlsym(dp_,"SInit");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerInit is %s\n",error);
			return false;
	}
	/*
	ScannerClose = (CloseA)dlsym(dp_,"SClose");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerClose is %s\n",error);
			return false;
	}
	*/
	ScannerQueryCapability = (int (*)(void))dlsym(dp_,"SQueryCapability");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerQueryCapability is %s\n",error);
			return false;
	}

	ScannerGetHWInformation = (bool (*)(char* const hwInfo,unsigned int const length)) dlsym(dp_,"SGetHWInformation");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerGetHWInformation is %s\n",error);
			return false;
	}
	ScannerGetLastErrorCode = (int (*)(void))dlsym(dp_,"SGetLastErrorCode");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerGetLastErrorCode is %s\n",error);
			return false;
	}
	ScannerGetLastErrorStr = (GetLastErrorStr)dlsym(dp_,"SGetLastErrorStr");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerGetLastErrorStr is %s\n",error);
			return false;
	}
	ScannerGetScanDpi = (GetScanDpi)dlsym(dp_,"SGetScanDpi");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerGetScanDpi is %s\n",error);
			return false;
	}
	ScannerStart = (Start)dlsym(dp_,"SStart");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerStart is %s\n",error);
			return false;
	}
	ScannerIsComplete = (ScanIsComplete)dlsym(dp_,"ScanIsComplete");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerIsComplete is %s\n",error);
			return false;
	}
	/*
	ScannerDisable = (Disable)dlsym(dp_,"SDisable");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerDisable is %s\n",error);
			return false;
	}
	*/
	ScannerIsReady = (ScannerIsReadyS)dlsym(dp_,"ScannerIsReady");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerIsReady is %s\n",error);
			return false;
	}
	ScannerRollBack = (RollBack)dlsym(dp_,"SRollBack");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerRollBack is %s\n",error);
			return false;
	}
	ScannerGetOriginImageSize = (GetOriginImageSize)dlsym(dp_,"SGetOriginImageSize");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerGetOriginImageSize is %s\n",error);
			return false;
	}
	ScannerGetOriginImage = (GetOriginImage)dlsym(dp_,"SGetOriginImage");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerGetOriginImage is %s\n",error);
			return false;
	}
	ScannerGetTicketInfo = (GetTicketInfo)dlsym(dp_,"SGetTicketInfo");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerGetTicketInfo is %s\n",error);
			return false;
	}
	ScannerRecognizeItem = (RecognizeItem)dlsym(dp_,"SRecognizeItem");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerRecognizeItem is %s\n",error);
			return false;
	}
	ScannerGetBrandDpi = (GetBrandDpi)dlsym(dp_,"SGetBrandDpi");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerGetBrandDpi is %s\n",error);
			return false;
	}
	BrandPrintImage = (PrintBrandImage)dlsym(dp_,"SPrintBrandImage");
	if((error=dlerror()) != NULL)
	{
			printf("load BrandPrintImage is %s\n",error);
			return false;
	}
	ScannerGetSWVersion = (GetSWVersion)dlsym(dp_, "SGetSWVersion");
	if((error=dlerror()) != NULL)
	{
			printf("load ScannerGetSWVersion is %s\n",error);
			return false;
	}
	return true;
}

bool LAScanner::GetInitStatus()
{
	return bScannerInit;
}

bool LAScanner::Init(const char *const input_dir, const char *const output_dir)
{
	if(!bmutexIsOK) return false;
	if(NULL != ScannerInit)
    {
		while(1)
		{
			if(!pthread_mutex_trylock(&scannerMutex))
			{
				break;
			}
			usleep(200000);
		}
		if(NO_ERROR != ScannerInit(NULL, input_dir, output_dir))
        {
            bScannerInit = false;
            qDebug("ScannerInit error!\n");
			pthread_mutex_unlock(&scannerMutex);
			return false;
        }
        else
        {
            bScannerInit = true;
        }
		pthread_mutex_unlock(&scannerMutex);
    }
	return bScannerInit;
}

void LAScanner::ReaderPrnImage(int nXpos, int nYpos, int nIndex, const char *pPath)
{
#if 1
	char* aryTmp = GetBmpInfo(pPath);
	if(aryTmp != NULL)
	{
		BrandPrintImage(aryTmp, nIndex, nXpos, nYpos);
		free(aryTmp);
		aryTmp = NULL;
	}
#else
	char aryTmp[1024*64];
	int nWidth,nHeight;
	int nByteWidth;
	int dwVer;
	int nBmpIndex;

	nBmpIndex=(nIndex%4)+1;
	memset(aryTmp,0, sizeof(aryTmp));
	if(GetBmpInfo(pPath, &nWidth,&nHeight,&nByteWidth,(unsigned char *)aryTmp))
	{
		dwVer=BrandPrintImage(aryTmp, nIndex, nXpos, nYpos);
	}
#endif
}

bool LAScanner::SetBrandData(bool bprint, int xpos, int ypos, int index, QString printfile)
{
	bPrint = bprint;
	nXpos = xpos;
	nYpos = ypos;
	nIndex = index;
	printFile = printfile;
	return true;
}

void LAScanner::ParseTickerinfo(char *info, QString *strResult)
{
	int i,m,j;
    char ms[5000],mm[5000];
    int row,col;
    int len=0;
    char tab_p[]={0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80};
    char result[128] = { 0 };
	unsigned char type;

    //解压
    m=0;
    row=info[5]+(info[6]<<8);
    col=info[3]+(info[4]<<8);
    len=info[1]+(info[2]<<8)+7;

	type = (unsigned char)info[0];
	qDebug()<<"ticket type: "+QString::number(type);
    if(type == 0x98)/*彩票内容*///二维条码
    {
        qDebug()<<"彩票内容:";
        /*
        for(int b=7; b<len; b++)
        {
            qDebug("%02x", info[b]);
        }
        */
		//strResult->append("\n");
        strResult->append(info+7);
    }
	else if(type > 0x64)/*彩票内容*///一维条码
    {
        qDebug()<<"彩票内容:";
        /*
        for(int b=7; b<len; b++)
        {
            qDebug("%02x", info[b]);
        }
        */
		//strResult->append("\n");
        strResult->append(info+11);
    }
    else
    {
		if(info[0] == 3)/*彩票大黑标*/
		{
			qDebug("彩票大黑标");
		}
        else if(info[0] == 2)/*彩票小黑标*/
        {
            qDebug("彩票小黑标");
        }
        else if(info[0] == 1) /*投注单*/
        {
            qDebug("投注单");
        }
        for(i=7;i<len;i++)
        {
			for(j=0;j<8;j++)
			{
				if(info[i]&tab_p[j])
				{
					if(!bOnlyScanning)
					{
						ms[m] = '1';
					}
					else
					{
						ms[m]='X';
						ms[m+1] = ' ';
					}
				}
				else
				{
					if(!bOnlyScanning)
					{
						ms[m] = '0';
					}
					else
					{
						ms[m]='_';
						ms[m+1] = ' ';
					}
				}
				if(!bOnlyScanning) m+=1;
				else m+=2;
			}
		}

        for(i=0;i<row;i++)
        {
			for(j=0;j<(m/row);j++)
			{
				mm[j]=ms[i*(m/row)+j];
			}
			mm[m/row]=0;
			if(!bOnlyScanning)
			{
				if(i)
				{
					mm[m/row-4] = 0;
					strResult->append(mm);
				}
			}
			else
			{
				sprintf(result,"\n%s", mm);
				strResult->append(result);
			}
        }
    }
}

bool LAScanner::GetOnlyScanning()
{
	return bOnlyScanning;
}

void LAScanner::SetOnlyScanning(bool OnlyScanning)
{
	bOnlyScanning = OnlyScanning;
}

void LAScanner::RollBackTicket()
{
	if(ScannerRollBack == NULL) return;
	while(pthread_mutex_trylock(&scannerMutex))
	{
		usleep(100000);
		continue;
	}
	ScannerRollBack();//退票
	pthread_mutex_unlock(&scannerMutex);
}

void LAScanner::PrintBrand(int xpos, int ypos, int index, QString pfile)
{
	qDebug("BrandPrintDataReturn:x:%d y:%d index:%d printFile:%s", xpos, ypos, index, pfile.toLocal8Bit().constData());

	if(pthread_mutex_lock(&scannerMutex))
	{
		qDebug("读票机繁忙!");
		return ;
	}

	if(pfile.length() >0)
	{
		ReaderPrnImage(xpos, ypos, index, pfile.toLocal8Bit().constData());
	}
	else
	{
		BrandPrintImage(NULL, index, xpos, ypos);
	}

	pthread_mutex_unlock(&scannerMutex);
}

void LAScanner::MakeBrandToFile(QString content, QString filename)
{
	//自定义内容 标识
	QImage img(256, 60, QImage::Format_Mono);
	img.fill(1);
	QPainter painter(&img);
	QFont font;
	font.setFamily("Abyssinica SIL"); //设置字体，宋体
	font.setPixelSize(40); //设置字号32,以像素为单位
	painter.setFont(font);
	int flags = Qt::AlignCenter | Qt::AlignTop;
	painter.setBackground(Qt::SolidPattern);
	painter.drawText(0, 5, 256, 50, flags, content);
	img.save(filename);
}

bool LAScanner::GetScannerHWInformation(char *info, int length)
{
	bool ret;
	if(ScannerGetHWInformation == NULL) return false;
	if(pthread_mutex_lock(&scannerMutex))
	{
		qDebug("读票机繁忙!");
		return false;
	}
	ret = ScannerGetHWInformation(info, length);
	pthread_mutex_unlock(&scannerMutex);
	return ret;
}

bool LAScanner::GetScannerStatus()
{
	bool isOK;
	while(pthread_mutex_lock(&scannerMutex))
	{
		usleep(500000);
		continue;
	}
	if(!ScannerIsReady())
	{
		isOK = false;
                //emit ScannerError(TYPE_STATUS, "读票机未准备好");
	}
	else
	{
		isOK = true;
	}
	pthread_mutex_unlock(&scannerMutex);
	return isOK;
}

void LAScanner::run()
{
	unsigned char ticketinfo[1024] = { 0 };
    QString strResult;
    int ret;
    int errCode = 0;
	char errStr[1024];
	qDebug()<<"scan thread start:"<<QThread::currentThreadId();

	while(pthread_mutex_trylock(&scannerMutex))
	{
		usleep(500000);
		continue;
	}
    if(TRUE != ScannerIsReady())
    {
		fprintf(stderr, "ScannerIsReady error\n");
		pthread_mutex_unlock(&scannerMutex);
		emit ScanFinished(-1, tr("读票机未准备好"));
		return;
    }

    if (TRUE != ScannerStart())
    {
		fprintf(stderr, "启动扫描失败\n");
		pthread_mutex_unlock(&scannerMutex);

		emit ScanFinished(-1, tr("读票机启动扫描失败"));
        return ;
    }
	pthread_mutex_unlock(&scannerMutex);
    fprintf(stderr, "\n扫描中......\n");
	emit ScanFinished(-1, tr("扫描中......"));
	bStop = false;
	while(!bStop)
    {
		while(!bStop)
		{
			if(pthread_mutex_trylock(&scannerMutex))
			{
				usleep(500000);
				continue;
			}

			if (ScannerIsComplete())
			{
				qDebug()<<"1-"<<QTime::currentTime().toString();
				memset(ticketinfo, 0, sizeof(ticketinfo));
				ret = ScannerGetTicketInfo(ticketinfo, sizeof(ticketinfo));
				//ret 可以判断是否返回内容，返回非0为内容长度。
				errCode = ScannerGetLastErrorCode();
				ScannerGetLastErrorStr(errStr, sizeof(errStr));
				qDebug()<<"get ticket error, errCode = "<<errCode<<"errStr:"<<errStr<<" ret = "<<ret;
				qDebug()<<"2-"<<QTime::currentTime().toString();
				if(errCode != 0)
				{
					strResult.clear();
					switch(errCode)
					{
						case 1011:
						{
							strResult = tr("\n读票机彩票（投注单）数据读取错误");
							break;
						}
						case 1012:
						{
							strResult = tr("\n读票机卡纸");
							break;
						}
						case 1013:
						{
							strResult = tr("\n运送状态失败");
							break;
						}
						case 1014:
						{
							strResult = tr("\n标记已打印");
							break;
						}
						default:
						{
							strResult = "";
							break;
						}
					}
					ScannerRollBack();
					emit ScanFinished(errCode, "error Code: "+QString::number(errCode)+"\nerror string:"+QString(errStr)+strResult);
					pthread_mutex_unlock(&scannerMutex);
					continue;
				}
				break;
			}
			else
			{/**/

				pthread_mutex_unlock(&scannerMutex);
				usleep(500000);
				continue;
			}
		}
		//扫描完成，处理数据
		/*
		int timeout = 5 * 1000; //超时时间设置（ms）
		QTimer t;
		QEventLoop q;
		t.setSingleShot(true);
		connect(&t, SIGNAL(timeout()), &q, SLOT(quit()));  //异步调用超时退出
		connect(this, SIGNAL(BrandPrintDataReturn()), &q, SLOT(quit()));  //异步调用完成退出
		pDialog->BrandPrintData();   //调用异步函数，判断是否需要打印标记
		t.start(timeout);
		q.exec();
		*/
		if(bOnlyScanning && bPrint)//打印标识
		{
			qDebug("BrandPrintDataReturn:x:%d y:%d index:%d printFile:%s", nXpos, nYpos, nIndex, printFile.toLocal8Bit().constData());
			if(printFile.length() >0)
			{
				ReaderPrnImage(nXpos, nYpos, nIndex, printFile.toLocal8Bit().constData());
			}
			else
			{
				BrandPrintImage(NULL, nIndex, nXpos, nYpos);
			}
		}
#if 0
		int width = 0, height = 0, bufsize = 0;
		ScannerGetOriginImageSize(&width, &height, &bufsize);
		qDebug("OriginImageSize:width=%d,height=%d,bufsize=%d", width, height, bufsize);
		char *buffer = (char*)malloc(bufsize+1);
		if(buffer)
		{
			if(0 < (ret = ScannerGetOriginImage(buffer, bufsize+1)))
			{
				qDebug("Image size: %d, read %d", bufsize, ret);
				if(ret == bufsize)
				{
					QFile file("scan.bmp");
					file.open(QIODevice::WriteOnly);
					file.write(buffer, ret);
					file.close();
				}
				else
				{
					emit ScannerError(TYPE_INFO, "读票机读取图片错误");
				}
			}
			if(buffer) free(buffer);
			buffer = NULL;
		}
#endif
		/**
		 * 如果只是扫描票,则退票,
		 * 否则,由ScanFinished槽退票,
		 * 为了实现识别兑奖数据
		 */
		if(bOnlyScanning)
		{
			ScannerRollBack();//退票
		}

		strResult.clear();
		//write(1, ticketinfo, ret);
		qDebug()<<"3-"<<QTime::currentTime().toString();
		ParseTickerinfo((char*)ticketinfo, &strResult);
		qDebug()<<"4-"<<QTime::currentTime().toString();
		emit ScanFinished(0, QString(G2U(strResult.toStdString().c_str())));
		pthread_mutex_unlock(&scannerMutex);
		//qDebug()<<"---------\n"<<strResult<<"\n------------";
    }
    qDebug("scan thread quit!");
}
