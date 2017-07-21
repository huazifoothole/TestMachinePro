#include "socketthread.h"

SocketThread::SocketThread()
{
    tcpClient = new QTcpSocket(this);
    tcpClient_printer = new QTcpSocket(this);
    tcpClient_scanner = new QTcpSocket(this);
    tcpClient_bcr = new QTcpSocket(this);
    tcpClient_monitor = new QTcpSocket(this);
    tcpClient_reboot = new QTcpSocket(this);
    tcpClient_ups = new QTcpSocket(this);
    hwinfoquery = new LAHW();
    connectFlag = false;


    while(hwinfoquery!= NULL)
    {
        if(!hwinfoquery->LoadDriverSympol(SO_PATH"/HWILatechHWInfo.so"))
          {
              qDebug("load HWILatechHWInfo.so false");
             break;
          }
        qDebug("load HWILatechHWInfo.so ok");
        break;
     }


    ipCfg();
    readIP();
    connectServer(serverIP);
    connect(tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    connect(tcpClient_printer,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError2(QAbstractSocket::SocketError)));



    /*QString localIPAddress = " "; //获取ip地址
    bool netRet =false;
    QList<QHostAddress> listAddress=QNetworkInterface::allAddresses();
     for(int j = 0; j < listAddress.size(); j++){
     if(!listAddress.at(j).isNull()
        && listAddress.at(j).protocol() ==  QAbstractSocket::IPv4Protocol
        && listAddress.at(j) != QHostAddress::LocalHost)
        {
            localIPAddress = listAddress.at(j).toString();
            qDebug()<<"**localIPAddress**"<<localIPAddress;
            netRet =true;
        }
     }
         localIP= localIPAddress;
     if(!netRet)
         localIP = "192.168.0.101";*/


}

SocketThread::~SocketThread()
{
    tcpClient->close();
    tcpClient_printer->close();
    tcpClient_scanner->close();
    tcpClient_bcr->close();
    tcpClient_monitor->close();
}
void SocketThread::readIP()
{
    QSettings setting(QString("./networkIP.ini"), QSettings::IniFormat);
    setting.beginGroup("IP");
    serverIP=("");
    serverIP = setting.value(QString("serverIP")).toString();
    qDebug()<<"serverIP = "<<serverIP;
    setting.endGroup();
    return;
}

void SocketThread::connectServer(QString server_ip)
{
     serverIP = server_ip;
     tcpClient->connectToHost(serverIP,9991);
     qDebug("1st connectServer State1:%d\n",tcpClient->state());   // State: 2; 2代表ConnectingState，正确的应该为3（ConnectedState）
     const int Timeout=2*1000;
     if(!tcpClient->waitForConnected(Timeout))
    {

        //emit unconnectSer();
        connectFlag = false;
        qDebug("connect to 9991 timeout\n");
        usleep(50000);
        return;
    }
     /*判断ip地址是否>100*/
     if(!judgeIP()) {
         connectFlag = false;
         tcpClient->close();
         return;
     }
     connectFlag =true;
     qDebug("1sted connect State2:%d\n",tcpClient->state());
     sendMesg1();
     connect(tcpClient,SIGNAL(readyRead()),this,SLOT(readConfMesg()));
     return;
 }

void SocketThread::readConfMesg()
{

    qDebug("it is readMesg\n");
    qDebug("recive size is %d\n",tcpClient->bytesAvailable());

    info.resize(64);
    info =  tcpClient->readAll();
    DevConf  devconf;
    qDebug("devconf1 size is %d\n",sizeof(devconf));
    memset(&devconf , 0, sizeof(devconf));
    memcpy(&devconf,info,sizeof(devconf));
    tcpClient->close();
    emit getConfInfo(info);

}

void SocketThread::sendMesg1()
{
    char mbinfo[4096]={0};
    QString tmpstr = mbinfo;
    QString biosVer,mainUuid;
    if(hwinfoquery->hwGetMainboardInfo(mbinfo, sizeof(mbinfo)))//获取bios 主板信息
    {

            qDebug("****GetMainboardInfo ok******\n");
              QString tmpstr = mbinfo;
              QStringList tmpList = tmpstr.split('\n');
              qDebug()<<mbinfo;

              for(int i=0; i<tmpList.size(); i++)
              {
                  //qDebug()<<tmpList.at(i);
                  QStringList list2;
                  list2 = tmpList.at(i).split('=');
                                  if(list2.size() == 2 && list2.at(0) == "BIOSVersion")
                                  {
                                      // showText += tr("BIOS版本 = ") + list2.at(1) + "\n";
                                      biosVer=list2.at(1);
                                      qDebug("BIOS版本 =%s",biosVer.toLatin1().data());
                                  }
                                  if(list2.size() == 2 && list2.at(0) == "UUID")
                                  {
                                          //showText += tr("主板产品UUID = ") + list2.at(1) + "\n\n";
                                      mainUuid=list2.at(1);
                                      qDebug("主板产品UUID = %s",mainUuid.toLatin1().data());
                                  }
              }
          }
   else {
           qDebug("****GetMainboardInfo false******\n");
           //return;
            }

    send64info buff;
    memset(&buff,0,sizeof(buff));
    buff.cmdType = 1;
    strcpy(buff.biosStr,mainUuid.toLatin1().data());
    qDebug("buffsize is %d\n ",sizeof(buff));
    tcpClient->write((char*)&buff,sizeof(buff));
    return;
}


void SocketThread::displayError(QAbstractSocket::SocketError)
{
    qDebug()<<"tcpClient9991 error"<<tcpClient->errorString();

}
void SocketThread::displayError2(QAbstractSocket::SocketError)
{
    qDebug()<<"tcpClient_printe errorr"<<tcpClient_printer->errorString();
}


void SocketThread::sendInfo(SendInfo info)
{
      if(connectFlag == false) return;
     qDebug("*******it sendinfo******\n");
    if(info.dev_type == PRINTER)
    {
        tcpClient_printer->connectToHost(serverIP,9992);
        const int Timeout=2000;
        if(!tcpClient_printer->waitForConnected(Timeout))
       {
           qDebug("tcpClient_printer connect to service 9992  timeout\n");
           return;
       }
       qDebug("tcpClient_printer connect 9992  State:%d\n",tcpClient_printer->state());
       tcpClient_printer->write((char*)&info,sizeof(info));
       tcpClient_printer->waitForBytesWritten(30000);
       tcpClient_printer->close();
    }

    if(info.dev_type == READER)
       {
           tcpClient_scanner->connectToHost(serverIP,9992);
            const int Timeout3=2000;
            if(!tcpClient_scanner->waitForConnected(Timeout3))
           {
               qDebug("tcpClient_scanner  connect to service 9992  timeout\n");
               return;
           }
            qDebug(" tcpClient_scanner connect 9992  State:%d\n",tcpClient_scanner->state());
             tcpClient_scanner->write((char*)&info,sizeof(info));
             tcpClient_scanner->waitForBytesWritten(30000);
             tcpClient_scanner->close();
       }


    if(info.dev_type == MONITOR)
       {
           tcpClient_monitor->connectToHost(serverIP,9992);
           const int Timeout2=2000;
           if(!tcpClient_monitor->waitForConnected(Timeout2))
          {
              qDebug("tcpClient_monitor  connect to service 9992  timeout\n");
              return;
          }
           qDebug(" tcpClient_monitor connect 9992  State:%d\n",tcpClient_monitor->state());
           tcpClient_monitor->write((char*)&info,sizeof(info));
           tcpClient_monitor->waitForBytesWritten(30000);
           tcpClient_monitor->close();
       }


    if(info.dev_type == SCREEN)
    {
        tcpClient_bcr->connectToHost(serverIP,9992);
        const int Timeout4=2000;
        if(!tcpClient_bcr->waitForConnected(Timeout4))
       {
           qDebug("tcpClient_bcr  connect to service 9992  timeout\n");
           return;
       }
        qDebug(" tcpClient_bcr connect 9992  State:%d\n",tcpClient_bcr->state());
        tcpClient_bcr->write((char*)&info,sizeof(info));
        tcpClient_bcr->waitForBytesWritten(30000);
        tcpClient_bcr->close();
    }
    if(info.dev_type == UPS)
    {
        tcpClient_ups->connectToHost(serverIP,9992);
        const int Timeout4=2000;
        if(! tcpClient_ups->waitForConnected(Timeout4))
       {
           qDebug("tcpClient_bcr  connect to service 9992  timeout\n");
           return;
       }
        qDebug(" tcpClient_bcr connect 9992  State:%d\n", tcpClient_ups->state());
         tcpClient_ups->write((char*)&info,sizeof(info));
         tcpClient_ups->waitForBytesWritten(30000);
         tcpClient_ups->close();

    }
}

bool SocketThread::rebootDev()
{
    tcpClient->connectToHost(serverIP,9993);
    qDebug()<<serverIP;
    const int Timeout=2000;
    if(!tcpClient->waitForConnected(Timeout))
   {
       qDebug("tcpClient(send reboot time)  connect to service 9992  time4out\n");
       return false;
   }
     char Str[10] ;
     memset(Str,0,sizeof(Str));
     strcpy(Str,"reboot");
     tcpClient->write(Str);
     return true;
}

bool SocketThread::judgeIP()
{

            QString localIPAddress = " "; //获取ip地址
            QList<QHostAddress> listAddress=QNetworkInterface::allAddresses();
             for(int j = 0; j < listAddress.size(); j++){
             if(!listAddress.at(j).isNull()
                && listAddress.at(j).protocol() ==  QAbstractSocket::IPv4Protocol
                && listAddress.at(j) != QHostAddress::LocalHost)
                {
                    localIPAddress = listAddress.at(j).toString();
                    qDebug()<<"**localIPAddress**"<<localIPAddress;
                }
             }
             localIP= localIPAddress;
             QStringList tmpList =  localIPAddress.split('.');
             qDebug()<<tmpList.at(3).toInt();
             int tmpip = tmpList.at(3).toInt();
             if(tmpip<=100)
             return false;
             else if(tmpip>100&&tmpip<=200)
             return true;

}

bool SocketThread::local()
{
    if(!connectFlag)
      return false;
    else
      return true;
}

void SocketThread::ipCfg()
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
    localIP=("");
    localIP = setting.value(QString("localIP")).toString();
    qDebug()<<"localIP = "<<localIP;
    setting.endGroup();

    QProcess netProcess;
    QString ifconfigcmd= "ifconfig "+thisNet.name()+" "+localIP;
    qDebug()<<"ifconfigcmf"<<ifconfigcmd;
    QByteArray s = ifconfigcmd.toLatin1();
    int ret1=system(s.data());
    int ret2 = system("ifconfig");
    qDebug()<<"ret11=="<<ret1<<"ret22=="<<ret2;
  //  netProcess.start(ifconfigcmd);
  //  sleep(1);
    netProcess.close();


}



