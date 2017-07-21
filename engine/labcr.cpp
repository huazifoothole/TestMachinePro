#include "labcr.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <dlfcn.h>
#include <dirent.h>

LABCR::LABCR(QObject *parent) : QThread(parent)
{
	dp_ = NULL;
	//bStop = false;
	if(0 == pthread_mutex_init(&HscannerMutex, NULL))
	{
		bmutexIsOK = true;
	}
	else
	{
		bmutexIsOK = false;
	}
           bHscannerInit = false;
           testStartFlag = false;
}

LABCR::~LABCR()
{
	if(bmutexIsOK) pthread_mutex_destroy(&HscannerMutex);
	if(dp_ != NULL) dlclose(dp_);
}

bool LABCR::LoadDriverSympol(const char *const file)
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
    dp_ = dlopen(file,RTLD_LAZY);/**/
    if( (error=dlerror()) != NULL)
    {
		printf("openso is %s\n",error);
		return false;
    }

    HscannerGetSWVersion = (GetSWVersion)dlsym(dp_, "BCRGetSWVersion");
    if((error=dlerror()) != NULL)
    {
		printf("load HscannerGetSWVersion is %s\n",error);
		return false;
    }

    HscannerUserLEDOn = (UserLEDOn)dlsym(dp_, "BCRUserLEDOn");
    if((error=dlerror()) != NULL)
    {
		printf("load HscannerUserLEDOn is %s\n",error);
		return false;
    }

    HscannerUserLEDOff = (UserLEDOff)dlsym(dp_, "BCRUserLEDOff");
    if((error=dlerror()) != NULL)
    {
		printf("load HscannerUserLEDOff is %s\n",error);
		return false;
    }

    HscannerGetLastErrorCode = (GetLastErrorCode)dlsym(dp_, "BCRGetLastErrorCode");
    if((error=dlerror()) != NULL)
    {
		printf("load HscannerGetLastErrorCode is %s\n",error);
		return false;
    }

    HscannerGetLastErrorStr = (GetLastErrorStr)dlsym(dp_, "BCRGetLastErrorStr");
    if((error=dlerror()) != NULL)
    {
		printf("load HscannerGetLastErrorStr is %s\n",error);
		return false;
    }

    HscannerEnable = (Enable)dlsym(dp_, "BCREnable");
    if((error=dlerror()) != NULL)
    {
		printf("load HscannerEnable is %s\n",error);
		return false;
    }

    HscannerDisable = (Disable)dlsym(dp_, "BCRDisable");
    if((error=dlerror()) != NULL)
    {
		printf("load HscannerDisable is %s\n",error);
		return false;
    }

    BarcodeScannerHScannerIsReady = (HScannerIsReady)dlsym(dp_,"BCRIsReady");
    if((error=dlerror()) != NULL)
    {
		printf("load BarcodeScannerHScannerIsReady is %s\n",error);
		return false;
    }

    BarcodeScannerScanIsComplete = (ScanIsComplete)dlsym(dp_,"BCRScanIsComplete");
    if((error=dlerror()) != NULL)
    {
		printf("load BarcodeScannerScanIsComplete is %s\n",error);
		return false;
    }

    BarcodeScannerGetDataLength = (int (*)(unsigned int *const))dlsym(dp_,"BCRGetDataLength");
    if((error=dlerror()) != NULL)
    {
		printf("load BarcodeScannerGetDataLength is %s\n",error);
		return false;
    }

    BarcodeScannerGetTicketInfo = (HscannerGetTicketInfo)dlsym(dp_,"BCRGetTicketInfo");
    if((error=dlerror()) != NULL)
    {
		printf("load BarcodeScannerGetTicketInfo is %s\n",error);
		return false;
    }

    BarcodeScannerSetScanMode = (int (*)(unsigned int const))dlsym(dp_,"BCRSetScanMode");
    if((error=dlerror()) != NULL)
    {
		printf("load BarcodeScannerSetScanMode is %s\n",error);
		return false;
    }

    BarcodeScannerGetScanMode = (int (*)(unsigned int *const))dlsym(dp_,"BCRGetScanMode");
    if((error=dlerror()) != NULL)
    {
		printf("load BarcodeScannerGetScanMode is %s\n",error);
		return false;
    }

    BarcodeScannerStartScan = (int (*)())dlsym(dp_,"BCRStartScan");
    if((error=dlerror()) != NULL)
    {
		printf("load BarcodeScannerStartScan is %s\n",error);
		return false;
    }

    BarcodeScannerStopScan = (int (*)())dlsym(dp_,"BCRStopScan");
    if((error=dlerror()) != NULL)
    {
		printf("load BarcodeScannerStopScan is %s\n",error);
		return false;
    }

    BarcodeScannerGetImageSize = (GetImageSize)dlsym(dp_,"BCRGetImageSize");
    if((error=dlerror()) != NULL)
    {
		printf("load BarcodeScannerGetImageSize is %s\n",error);
		return false;
    }

    BarcodeScannerGetImage = (int (*)(char* const, int const))dlsym(dp_,"BCRGetImage");
    if((error=dlerror()) != NULL)
    {
		printf("load BarcodeScannerGetImage is %s\n",error);
		return false;
    }

    BarcodeScannerGetHWInformation = (GetHWInformation)dlsym(dp_,"BCRGetHWInformation");
    if((error=dlerror()) != NULL)
    {
		printf("load BarcodeScannerGetHWInformation is %s\n",error);
		return false;
    }

    BarcodeScannerInit = (BCRInit)dlsym(dp_,"BCRInit");
    if((error=dlerror()) != NULL)
    {
		printf("load BarcodeScannerInit is %s\n",error);
		return false;
    }

    BarcodeScannerClose = (void (*)())dlsym(dp_,"BCRClose");
    if((error=dlerror()) != NULL)
    {
		printf("load BarcodeScannerClose is %s\n",error);
		return false;
    }

    HscannerAimOn = (AimOn)dlsym(dp_, "BCRAimOn");
    if((error=dlerror()) != NULL)
    {
        printf("load AimOn is %s\n", error);
        return false;
    }

    HscannerAimOff = (AimOff)dlsym(dp_, "BCRAimOff");
    if((error=dlerror()) != NULL)
    {
        printf("load AimOff is %s\n", error);
        return false;
    }

    HscannerEnableCode = (EnableCode)dlsym(dp_, "BCREnableCode");
    if((error=dlerror()) != NULL)
    {
        printf("load EnableCode is %s\n", error);
        return false;
    }

	BCRClearBuffer = (ClearBuffer)dlsym(dp_, "BCRClearBuffer");
	if((error=dlerror()) != NULL)
	{
		printf("load BCRClearBuffer is %s\n", error);
		return false;
	}

	Beep = (BCRBeep)dlsym(dp_, "BCRBeep");
	if((error=dlerror()) != NULL)
	{
		printf("load BCRBeep is %s\n", error);
		return false;
	}

	return true;
}

bool LABCR::Init(const char *const input_dir, const char *const output_dir)
{
	int initCount = 0;
	int ret;
	if(!bmutexIsOK) return false;

	while(!bHscannerInit)
	{
                if(initCount >= 2)//初始化2次不成功，则视为设备不存在
		{
			return false;
		}
		while(pthread_mutex_trylock(&HscannerMutex))
		{
			usleep(500000);
		}
		if(BarcodeScannerInit)
		{
			ret = BarcodeScannerInit(NULL, input_dir, output_dir);
			if(NO_ERROR != ret)
			{
				qDebug()<<"BarcodeScannerInit error:"<<ret;
				if(HscannerGetLastErrorCode)
				{
					ret = HscannerGetLastErrorCode();
					if(ret == NO_HSCANNER)
					{
						pthread_mutex_unlock(&HscannerMutex);
						return false;
					}
					qDebug()<<"HscannerGetLastErrorCode:"<<ret;
                                        return false;
				}
				else
				{
					pthread_mutex_unlock(&HscannerMutex);
					return false;
				}
			}
			else
			{
				qDebug("BarcodeScannerInit ok.\n");
				this->bHscannerInit = true;
			}
		}
		else
		{
			pthread_mutex_unlock(&HscannerMutex);
			return false;
		}
		pthread_mutex_unlock(&HscannerMutex);
		initCount++;
                usleep(50000);
	}
	return true;
}

void LABCR::SetStop(bool stop)
{
	bStop = stop;
}

bool LABCR::GetInitStatus()
{
	return bHscannerInit;
}

bool LABCR::GetBCRHWInformation(char *info, int length)
{
	if(BarcodeScannerGetHWInformation == NULL) return false;
	return BarcodeScannerGetHWInformation(info, length);
}

bool LABCR::GetBCRStatus()
{
	bool isOK;
       while(pthread_mutex_trylock(&HscannerMutex))
	{
		usleep(200000);
                continue;
        }
	if(BarcodeScannerHScannerIsReady)
	{
            bool ret=BarcodeScannerHScannerIsReady();
            qDebug("ret= %d",ret);
                if(!ret)
                {
                        isOK = false;
			qDebug("BarcodeScannerHScannerIsReady error");
                        emit BCRError(TYPE_STATUS, tr("扫描枪未准备好"));
		}
		else
		{
			isOK = true;
		}
	}
        /*else
	{
		qDebug("BarcodeScannerHScannerIsReady null");
		pthread_mutex_unlock(&HscannerMutex);
		emit BCRError(TYPE_API_NULL, tr("API BarcodeScannerHScannerIsReady is null"));
		return false;
        }*/
        pthread_mutex_unlock(&HscannerMutex);
	return isOK;
}

void LABCR::run()
{
        qDebug()<<"barcode scan thread start:"<<QThread::currentThreadId();
        //int ret;
        while(pthread_mutex_trylock(&HscannerMutex))
        {
                usleep(200000);
        }
        if(BarcodeScannerHScannerIsReady)
        {
                if(!BarcodeScannerHScannerIsReady())
            {
                        qDebug("BarcodeScannerHScannerIsReady error");
                        pthread_mutex_unlock(&HscannerMutex);
                        qDebug()<<"1***";
                        emit ScanFinished(TYPE_INFO, tr("扫描枪未准备好"));
                        return;
            }
        }
        else
        {
                qDebug("BarcodeScannerHScannerIsReady null");
                pthread_mutex_unlock(&HscannerMutex);
                emit ScanFinished(-1, tr("扫描枪未准备好"));
                qDebug()<<"2***";
                return;
        }

        if(HscannerEnableCode)
        {
                if(!HscannerEnableCode(7))
            {
                        qDebug("HscannerEnableCode error");
                        pthread_mutex_unlock(&HscannerMutex);
                        emit ScanFinished(TYPE_INFO, tr("扫描枪未准备好"));
                        qDebug()<<"3***";
                        return;
            }
        }
        else
        {
                qDebug("BarcodeScannerSetScanMode null");
                pthread_mutex_unlock(&HscannerMutex);
                emit ScanFinished(-1, tr("扫描枪未准备好"));
                qDebug()<<"4***";
                return;
        }

        if(BarcodeScannerSetScanMode)
        {
                if(TRUE != BarcodeScannerSetScanMode(2))
            {
                        qDebug("BarcodeScannerSetScanMode error");
                        pthread_mutex_unlock(&HscannerMutex);
                        emit ScanFinished(TYPE_INFO, tr("扫描枪未准备好"));
                        qDebug()<<"5***";
                        return;
            }
        }
        else
        {
                qDebug("BarcodeScannerSetScanMode null");
                pthread_mutex_unlock(&HscannerMutex);
                emit ScanFinished(-1, tr("扫描枪未准备好"));
                qDebug()<<"6***";
                return;
        }
        //亮灯
        unsigned int on = 2;
        HscannerUserLEDOn(&on);
        HscannerAimOn();

        if(BarcodeScannerStartScan)
        {
                if(!BarcodeScannerStartScan())
            {
                        qDebug()<<"BarcodeScannerStartScan failed!";
                        pthread_mutex_unlock(&HscannerMutex);
                        return;
            }
        }
        else
        {
                qDebug()<<"BarcodeScannerStartScan null!";
                pthread_mutex_unlock(&HscannerMutex);
                return;
        }
        pthread_mutex_unlock(&HscannerMutex);

        emit ScanFinished(1, tr("扫描中..."));
        bStop = false;
         int i=1;
    do
        {
                if(pthread_mutex_trylock(&HscannerMutex))
                {
                        usleep(500000);
                        continue;
                }

                if(BarcodeScannerScanIsComplete())
        {

            unsigned int length;
                        if(BarcodeScannerGetDataLength(&length))
            {
                                if(length <= 0)
                                {
                                        pthread_mutex_unlock(&HscannerMutex);
                                        continue;
                                }

                                unsigned char *buffer;
                unsigned int len = 0;
                buffer = (unsigned char*)malloc(length+2);
                                if(buffer == NULL)
                                {
                                        pthread_mutex_unlock(&HscannerMutex);
                                        continue;
                                }
                buffer[length+1] = '\0';
                                len = BarcodeScannerGetTicketInfo(buffer, length+1);
                                BCRClearBuffer();
                if(len == 0)
                {
                                qDebug("GetTicketInfo error\n");
                                        if(buffer)
                                        {
                                                free(buffer);
                                                buffer = NULL;
                                        }
                                        pthread_mutex_unlock(&HscannerMutex);
                    continue;
                }
                                 Beep(0x0);
                                if(buffer[0] == 152){
                                        emit ScanFinished(0, QString(G2U((char*)buffer+7)));
                                        QString temOutResult = "";
                                        temOutResult.sprintf("%s", (char*)buffer + 11);
                                      qDebug()<<"OutResult+7=="<<temOutResult;
                                }
                                else{
                                       QString OutResult0 = "";
                                       QString OutResult = "";
                                    if(buffer[7] > 1){
                                        OutResult0.sprintf("%s", (char*)buffer + 11);
                                        OutResult = OutResult0 + OutResult.sprintf("%s", (char*)buffer + 11+buffer[9]+2);//双条码，右移第一条条码所占数据位buffer[9]+2
                                         qDebug()<<"OutResult+25=="<<OutResult;
                                        emit ScanFinished(0, OutResult);
                                    }else{
                                        OutResult.sprintf("%s", (char*)buffer + 11);
                                        emit ScanFinished(0, OutResult);
                                    }

//                                      emit ScanFinished(0, QString(G2U((char*)buffer+11)));
                                }


                if(buffer[0] == 152)
                                        qDebug("GetTicketInfo: %s\n", buffer+7);
                                else
                                        qDebug("GetTicketInfo: %s\n", buffer+11);

                                if(buffer)
                                {
                                        free(buffer);
                                        buffer = NULL;
                                }
                        }//if(BarcodeScannerGetDataLength(&length))
                        else
                        {
                                char info[4096]={0};
                                if(NULL != HscannerGetLastErrorStr)
                                {
                                        HscannerGetLastErrorStr(info, sizeof(info));
                                        emit ScanFinished(HscannerGetLastErrorCode(), QString(info));
                                        qDebug()<<"6***";
                                        //qDebug()<<"BarcodeScannerScanIsComplete error string: "<<info;
                                }
                                //printf("ScannerIsComplete error=%d\n", ret);
                                pthread_mutex_unlock(&HscannerMutex);
                                usleep(300000);
                                continue;
                        }//if(BarcodeScannerGetDataLength(&length))... else
                }//if(BarcodeScannerScanIsComplete())
                else
                {
                        usleep(500000);
                        if(testStartFlag)
                        {
                            int time =bcrTime/(0.5);//换算成次数    
                            qDebug()<<"time"<<time;
                            i++;
                            qDebug()<<"i ="<<i;
                            if(i==time)
                            {

                            if(HscannerGetLastErrorCode() != 0)
                            {
                                if(HscannerEnable())
                                {
                                    emit ScanFinished(2,"bcr status is ok");
                                    i=1;
                                }
                                else
                                {
                                char info[1024]={0};
                               HscannerGetLastErrorStr(info, sizeof(info));
                                 qDebug()<<"bcr error info"<<info;
                                 emit ScanFinished(-1,QString(info));
                                i=1;
                                }
                            }
                            else
                            {
                                 qDebug()<<"bcr ok****";
                                emit ScanFinished(2,"bcr status is ok");
                                 i=1;
                            }
                            }
                        }
                }
                pthread_mutex_unlock(&HscannerMutex);
    }while(!bStop);

        if(!pthread_mutex_trylock(&HscannerMutex))
        {
                HscannerUserLEDOff();
                HscannerAimOff();
                BarcodeScannerStopScan();
                pthread_mutex_unlock(&HscannerMutex);
        }
        pthread_mutex_unlock(&HscannerMutex);

    qDebug("barcode scan thread stop");
}

void LABCR::getBcrTestTime(unsigned int Time)
{
    qDebug()<<"bcrTime="<<Time;
    testStartFlag = true;
    bcrTime =Time;
}
