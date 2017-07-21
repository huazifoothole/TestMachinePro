#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QDebug>
#include<QtNetwork>
#include <QTcpSocket>
//#include "./engine/hwinfoquery.h"
#include "TypeDef.h"
#include "engine/lahw.h"


class SocketThread : public QThread
{
    Q_OBJECT

public:
   // SocketThread();
    SocketThread();
    ~SocketThread();
public:
   // SocketThread *socketthread_A;
  //  SocketThread *socketthread_B;
    SendInfo sendinfo;
    LAHW *hwinfoquery;
    //virtual void run();
    void connectServer(QString);
    void sendMesg1();
    void sendInfo(SendInfo);
 //   void sendInfo9991(send64info);
    void readIP();
    bool judgeIP();
    bool rebootDev();
    bool local();
     QByteArray info;
     QString serverIP;
     QString localIP;
     bool connectFlag;
     void ipCfg();



private:
    QTcpSocket *tcpClient,*tcpClient_printer,*tcpClient_scanner,*tcpClient_bcr,*tcpClient_monitor,*tcpClient_reboot,*tcpClient_ups;
  //  bool connectServerFlag1;

private  slots:
    void  readConfMesg();
    void  displayError(QAbstractSocket::SocketError);
    void displayError2(QAbstractSocket::SocketError);

signals:
     void getConfInfo(QByteArray);
     void unconnectSer();
    // void getMainboardInfo9991();


};
#endif // HSCANNERTHREAD_H
