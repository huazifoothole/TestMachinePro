

#ifndef HWINFOQUERY_H
#define HWINFOQUERY_H

#include <QLibrary>
#include <QString>
#include <QDebug>

class HWInfoQuery
{
public:

    /**
     * @brief 构造函数
     * @return 无
     **/
    HWInfoQuery();

    /**
     * @brief 析构函数
     * @return 无
     **/
    ~HWInfoQuery();

    /**
    * @brief 根据参数加载硬件信息查询驱动
    * @param fileName:驱动记载库文件
    * @return true-成功  false-失败
    **/
    bool HWInfoQueryLoadLib(const QString &fileName);

    /**
    * @brief 设置输入文件路径和输出文件路径
    * @param 入口参数: input_dir   输入文件路径
    *                 output_dir  输出文件路径
    * @return  true--成功   false--失败
    **/
    bool HWInfoSetDirectory(const char*  input_dir, const char*  output_dir);

    /**
    * @brief  主板主要信息查询
    * @param  入口参数   length   info缓冲区的大小（在4096个字节内，包含结束符\0）
    *         出口参数    info	 信息
    *         info需包含如下信息：
    *                    Vendor	        终端厂家
    *                    ProductType	产品型号
    *                    HWVersion	    主板版本
    *                    SN	            整机序列号
    *                    ProductDate	整机生产日期
    *                    BIOSVender	    BIOS厂商
    *                    BIOSVersion	BIOS版本
    *                    UUID	        主板产品UUID
    * @return true-成功  false-失败
    **/
    bool GetMainboardInfo (char* const info, unsigned int const length);

    /**
    * @brief  CPU主要信息查询
    * @param  入口参数  length   info缓冲区的大小（在4096个字节内，包含结束符\0）
    *  	      出口参数	info     信息
    *         info需包含如下信息：
    *                      Vendor	    CPU厂家
    *                      ProductType	硬件型号
    *                      HWVersion	硬件版本
    *                      CPUName	    CPU名称
    *                      Cores	    核心个数
    *                      Frequency	CPU主频，单位MHz
    * @return   true-成功  false-失败
    **/
    bool GetCPUInfo (char* const info, unsigned int const length);

    /**
    * @brief  内存主要信息查询
    * @param  入口参数  length  info缓冲区的大小（在4096个字节内，包含结束符\0）
    *         出口参数   info	信息
    *        info需包含如下信息：
    *                   Vendor	     硬件厂家
    *                   ProductType	 硬件型号
    *                   HWVersion	 硬件版本
    *                   Capacity	 产品容量，单位M
    * @return true-成功  false-失败
    **/
    bool GetMemoryInfo(char* const info , unsigned int const length);

    /**
    * @brief  磁盘主要信息查询
    * @param   入口参数  length  info缓冲区的大小（在4096个字节内，包含结束符\0）
    *          出口参数   info	信息
    *      info需包含如下信息：
    *                Vendor	    硬件厂家
    *                roductType	硬件型号
    *                HWVersion	硬件版本
    *                Capacity	产品容量，单位M
    * @return true-成功  false-失败
    **/
    bool GetDiskInfo (char* const info , unsigned int const length);

    /**
    * @brief  声卡主要信息查询
    * @param  入口参数  length  info缓冲区的大小（在4096个字节内，包含结束符\0）
    *         出口参数   info	信息
    *         info需包含如下信息：
    *                     Vendor	    硬件厂家
    *                     ProductType	硬件型号
    *                     HWVersion	    硬件版本
    * @return true-成功  false-失败
    **/
    bool GetSoundCardInfo (char* const info , unsigned int const length);

    /**
    * @brief  网卡主要信息查询
    * @param  入口参数    number   网卡的序号
    *                    length   info缓冲区的大小（在4096个字节内，包含结束符\0）
    *          出口参数   info	  信息
    *          info需包含如下信息：
    *                     Vendor	    硬件厂家
    *                     ProductType	硬件型号
    *                     HWVersion	    硬件版本
    *                     MacAddress	产品Mac地址
    * @return true-成功  false-失败
    **/
    bool GetNICInfo (unsigned int number, char* const info , unsigned int const length);

    /**
    * @brief  显卡主要信息查询
    * @param  入口参数    number   显卡的序号
    *                    length   info缓冲区的大小（在4096个字节内，包含结束符\0）
    *          出口参数   info	  信息
    *          info需包含如下信息：
    *                    Vendor	        硬件厂家
    *                    ProductType	硬件型号
    *                    HWVersion	    硬件版本
    * @return  true-成功  false-失败
    **/
    bool GetGraphicCardInfo(unsigned int number, char* const info , unsigned int const length);

    /**
    * @brief   获取网卡个数
    * @param
    * @return  网卡个数
    **/
    int GetNICCount();

    /**
    * @brief   获取显卡个数
    * @param
    * @return  显卡个数
    **/
    int GetGraphicCardCount();

    /**
    * @brief   获取软件版本号的接口
    * @param   入口参数   length		标识swVersion缓冲区的大小
    *          出口参数   swVersion 	版本号
    * @return true-成功  false-失败
    **/
    bool GetSWVersion(char* const swVersion , unsigned int const length);

    /**
    * @brief  获取最近一次的错误码
    * @param
    * @return  最近一次的错误码
    **/
    int GetLastErrorCode();

    /**
    * @brief  获取最近一次的错误描述
    * @param  入口参数   length	errStr缓冲区的大小（返回在20个字节内，包含结束符\0）
    *         出口参数   errStr	错误描述，最多100个字符（包含结束符），以\0结尾
    * @return
    **/
    void GetLastErrorStr(char* const errStr, unsigned int const length);

    /**
    * @brief 获取上一个接口调用的运行时间
    * @param
    * @return 上一个接口调用的运行时间
    **/
    int getPreTime();

private:
    int apiTime_;//计算上一个执行接口的运行时间
    QLibrary *hwInfoQueryLib_;
    static const int function_count = 13;  //add one
    void *functionlist_[function_count];
};

#endif // HWINFOQUERY_H
