
#include <QSettings>
#include <QTime>
#include "hwinfoquery.h"

HWInfoQuery::HWInfoQuery():
    hwInfoQueryLib_(NULL)
 {
    memset(functionlist_, 0, sizeof(functionlist_));
    /*QSettings setting(QString("./conf/config.ini"), QSettings::IniFormat);
    setting.beginGroup("Driver");
    QString soFileName("");
    soFileName = setting.value(QString("hwinfo")).toString();
    setting.endGroup();*/
    QString soFileName = "/home/oneu/ccpos/dist/driver/la";
    HWInfoQueryLoadLib(soFileName);
}

HWInfoQuery::~HWInfoQuery()
{
    if (NULL != hwInfoQueryLib_)
        delete hwInfoQueryLib_;
    hwInfoQueryLib_ = NULL;
}


//加载硬件信息驱动
bool HWInfoQuery::HWInfoQueryLoadLib(const QString &soFileName)
{
    bool ok = true;
    if(hwInfoQueryLib_ != NULL)
    {
        ok = false;
    }
    hwInfoQueryLib_ = new QLibrary(soFileName);
    bool ret = hwInfoQueryLib_->load();
    if (!ret)
    {
        qDebug() << "load library failed!";
    }

    const char namelist[function_count][64] =
    {
        "HWInfoSetDirectory", //new add
        "GetMainboardInfo",
        "GetCPUInfo",
        "GetMemoryInfo",
        "GetDiskInfo",
        "GetSoundCardInfo",
        "GetNICInfo",
        "GetGraphicCardInfo",
        "GetNICCount",
        "GetGraphicCardCount",
        "GetSWVersion",
        "GetLastErrorCode",
        "GetLastErrorStr"
    };
    for (int i = 0; i < function_count; ++i)
    {
        functionlist_[i] = hwInfoQueryLib_->resolve(namelist[i]);// dlsym(libHandle,namelist[i]);
        if (functionlist_[i] == NULL)
        {
            ok = false;
            qDebug() << namelist[i] << " is NULL";
        }
        else
        {
            qDebug() << namelist[i] << " resolve success !";
        }
    }
    if (ok)
    {
        qDebug() << "HWInfoQuery Load Library & resolve success \n";
    }

    return ok;
}

bool HWInfoQuery::HWInfoSetDirectory(const char* input_dir, const char* output_dir)
{
    bool ok = false;
    if (functionlist_[0] != NULL)
    {
        QTime temTimer;
        temTimer.start();
        ok = ((bool(*)(const char*, const char*)) functionlist_[0])(input_dir, output_dir);
        apiTime_ = temTimer.elapsed();
    }
    else
    {
        //                  LOG4CXX_DEBUG(logger_, _T("error when calling HWInfoSetDirectory."));
    }
    return ok;
}

bool HWInfoQuery::GetMainboardInfo (char* const info, unsigned int const length)
{
    bool ok = false;
    if (functionlist_[1] != NULL)
    {
        QTime temTimer;
        temTimer.start();
        ok = ((bool(*)(char* const, unsigned int const)) functionlist_[1])(info, length);
        apiTime_ = temTimer.elapsed();
    }
    else
    {
        //                  LOG4CXX_DEBUG(logger_, _T("error when calling GetMainboardInfo."));
    }
    return ok;
}

bool HWInfoQuery::GetCPUInfo (char* const info, unsigned int const length)
{
    bool ok = false;
    if (functionlist_[2] != NULL)
    {
        QTime temTimer;
        temTimer.start();
        ok = ((bool(*)(char* const, unsigned int const)) functionlist_[2])(info, length);
        apiTime_ = temTimer.elapsed();
    }
    else
    {
        //                  LOG4CXX_DEBUG(logger_, _T("error when calling GetCPUInfo."));
    }
    return ok;
}

bool HWInfoQuery::GetMemoryInfo(char* const info , unsigned int const length )
{
    bool ok = false;
    if (functionlist_[3] != NULL)
    {
        QTime temTimer;
        temTimer.start();
        ok = ((bool(*)(char* const, unsigned int const)) functionlist_[3])(info, length);
        apiTime_ = temTimer.elapsed();
    }
    else
    {
        //                  LOG4CXX_DEBUG(logger_, _T("error when calling GetMemoryInfo."));
    }
    return ok;
}

bool HWInfoQuery::GetDiskInfo (char* const info , unsigned int const length)
{
    bool ok = false;
    if (functionlist_[4] != NULL)
    {
        QTime temTimer;
        temTimer.start();
        ok = ((bool(*)(char* const, unsigned int const)) functionlist_[4])(info, length);
        apiTime_ = temTimer.elapsed();
    }
    else
    {
        //                  LOG4CXX_DEBUG(logger_, _T("error when calling GetDiskInfo."));
    }
    return ok;
}

bool HWInfoQuery::GetSoundCardInfo (char* const info , unsigned int const length)
{
    bool ok = false;
    if (functionlist_[5] != NULL)
    {
        QTime temTimer;
        temTimer.start();
        ok = ((bool(*)(char* const, unsigned int const)) functionlist_[5])(info, length);
        apiTime_ = temTimer.elapsed();
    }
    else
    {
        //                  LOG4CXX_DEBUG(logger_, _T("error when calling GetSoundCardInfo."));
    }
    return ok;
}

bool HWInfoQuery::GetNICInfo (unsigned int number, char* const info , unsigned int const length)
{
    bool ok = false;
    if (functionlist_[6] != NULL)
    {
        QTime temTimer;
        temTimer.start();
        ok = ((bool(*)(unsigned int, char* const, unsigned int const)) functionlist_[6])(number, info, length);
        apiTime_ = temTimer.elapsed();
    }
    else
    {
        //                  LOG4CXX_DEBUG(logger_, _T("error when calling GetNICInfo."));
    }
    return ok;
}

bool HWInfoQuery::GetGraphicCardInfo(unsigned int number, char* const info , unsigned int const length)
{
    bool ok = false;
    if (functionlist_[7] != NULL)
    {
        QTime temTimer;
        temTimer.start();
        ok = ((bool(*)(unsigned int, char* const, unsigned int const)) functionlist_[7])(number, info, length);
        apiTime_ = temTimer.elapsed();
    }
    else
    {
        //                  LOG4CXX_DEBUG(logger_, _T("error when calling GetGraphicCardInfo."));
    }
    return ok;
}

int HWInfoQuery::GetNICCount()
{
    int code = -1;
    if (functionlist_[8] != NULL)
    {
        QTime temTimer;
        temTimer.start();
        code = ((int(*)(void)) functionlist_[8])();
        apiTime_ = temTimer.elapsed();
    }
    else
    {
        //                LOG4CXX_DEBUG(logger_, _T("error when calling GetNICCount."));
    }
    return code;
}

int HWInfoQuery::GetGraphicCardCount()
{
    int code = -1;
    if (functionlist_[9] != NULL)
    {
        QTime temTimer;
        temTimer.start();
        code = ((int(*)(void)) functionlist_[9])();
        apiTime_ = temTimer.elapsed();
    }
    else
    {
        //                LOG4CXX_DEBUG(logger_, _T("error when calling GetGraphicCardCount."));
    }
    return code;
}

bool HWInfoQuery::GetSWVersion(char* const swVersion , unsigned int const length)
{
    bool ok = false;
    if (functionlist_[10] != NULL)
    {
        QTime temTimer;
        temTimer.start();
        ok = ((bool(*)(char* const, unsigned int const)) functionlist_[10])(swVersion, length);
        apiTime_ = temTimer.elapsed();
    }
    else
    {
        //                  LOG4CXX_DEBUG(logger_, _T("error when calling GetSWVersion."));
    }
    return ok;
}

int HWInfoQuery::GetLastErrorCode()
{
    int code = -1;
    if (functionlist_[11] != NULL)
    {
        QTime temTimer;
        temTimer.start();
        code = ((int(*)(void)) functionlist_[11])();
        apiTime_ = temTimer.elapsed();
    }
    else
    {
        //                LOG4CXX_DEBUG(logger_, _T("error when calling GetLastErrorCode."));
    }
    return code;
}

void HWInfoQuery::GetLastErrorStr(char* const errStr, unsigned int const length)
{
    if (functionlist_[12] != NULL)
    {
        QTime temTimer;
        temTimer.start();
        ((void(*)(char* const, unsigned int const)) functionlist_[12])(errStr, length);
        apiTime_ = temTimer.elapsed();
    }
    else
    {
        //              LOG4CXX_DEBUG(logger_, _T("error when calling GetLastErrorStr."));
    }
}

int HWInfoQuery::getPreTime()
{
    return apiTime_;
}
