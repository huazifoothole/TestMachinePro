#ifndef TYPEDEF_H
#define TYPEDEF_H
//#include "Public.h"
#define DEV_NUM (100)
#define COMPONENT_NUM (5)
#define BYTE unsigned char
typedef struct UpsInfoParam{
 BYTE uchStatus;
 float fBatteryVol; //????
 float fACVol;  //????
 float fPower;  //????
 float fTemp;  //??
 int	nTempPrecent;	//?????

}UPSINFOPARAM;

#define VERSION_STRING_LENG				64		//版本信息字符长度
#define DESCTRIPTION_STRING_LENG		128		//版本描述字符长度
typedef struct Sversion
{
        char	szVersion[VERSION_STRING_LENG];
        char	szDateTime[VERSION_STRING_LENG];
        char	szDescription[DESCTRIPTION_STRING_LENG];
}Sversion;

/*设备类型*/
typedef enum
{
    DEV_NO_MEANS = 0,
    UPS = 1,
    READER,
    MONITOR,
    SCREEN,
    PRINTER,
}DevType;
/*设备状态*/
typedef enum
{
    STA_NO_MEANS,
    STA_NORMAL = 1,
    STA_NOT_START,
    STA_ERROR,
    STA_STOPED,
    STA_NOT_TEST
}DevSta;

/*?????*/
typedef struct ups_conf
{
    BYTE ups_enable;
    BYTE ups_port;
    BYTE ups_ex;
    int ups_time;
}UpsConf;
/*阅读器配置项*/
typedef struct reader_conf
{
    BYTE reader_enable;
    BYTE reader_port;
    int reader_time;
}ReaderConf;
/*主板配置项*/
typedef struct monitor_conf
{
    BYTE monitor_enable;
    int monitor_time;
}MonitorConf;


/*顾客凭配置项*/
typedef struct screen_conf
{
    BYTE screen_enable;
    BYTE screen_port;
    int screen_time;
}ScreenConf;
/*打印机配置项*/
typedef struct printer_conf
{
    BYTE printer_enable;
    int printer_time;
}PrinterConf;

/*设备配置信息*/
typedef struct dev_conf
{
    UpsConf ups_conf;
    ReaderConf reader_conf;
    MonitorConf monitor_conf;
    int img_time;
    int reboot_time;
    BYTE sound_enable;
    ScreenConf screen_conf;
    PrinterConf printer_conf;
}DevConf;

/*typedef struct srv_info
{
    int client_no;
    char srv_ip[16];
}SrvInfo;*/
typedef struct ups_read_info
{
    UpsInfoParam info_param;
    char ups_info[32];/*UPS????*/
    char ups_sequence[32];/*ups???*/
}UpsReadInfo;

typedef struct monitor_read_info
{
    int sys_fan_speed;/*主板风扇转速*/
    int cpu_fan_speed;/*CPU风扇转速*/
    int sys_temp;/*主板温度*/
    int cpu_temp;/*CPU温度*/
    char monitor_id[128];/*主板ID*/
    char bios_ver[64];/*BIOS版本*/
    Sversion s_version;/*软件版本*/
}MonitorReadInfo;

typedef struct reader_read_info
{
    char har_ver[64];
    Sversion s_version;
}ReaderReadInfo;
typedef struct count_info
{
        int test_count;/*测试次数*/
        int error_count;/*错误次数*/
        int acc_error;/*连续错误次数*/
}CountInfo;


/*通信接口*/
typedef struct send_info
{
    //int client_no;
    DevType dev_type;
    DevSta dev_sta;
    CountInfo count_info;
    char time_str[32];
    char read_info[512];
    char error_info[128];
}SendInfo;

typedef struct Send64Info{
int cmdType;
char biosStr[20];
char info[40];
}send64info;

/*统计信息*/
typedef struct  dev_statistical
{
    int dev_test_no;
    char main_board_id[20];
    char start_time[32];
    char stop_time[32];
    int ups_test_count;
    int ups_err_count;
    int reader_test_count;
    int reader_err_count;
    int screen_test_count;
    int screen_err_count;
    int printer_test_count;
    int printer_err_count;
    int monitor_test_count;
    int monitor_err_count;
}DevStatistical;
typedef enum
{
    CMD_MIN = 0,
    LOGIN = 1,
    CMD_MAX
}CMD_TYPE;

#define MAIN_BOARD_ID_LEN (20)
#endif // TYPEDEF_H

