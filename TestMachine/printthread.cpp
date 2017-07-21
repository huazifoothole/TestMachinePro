#include "printthread.h"
#include <QDebug>


PrintThread::PrintThread(unsigned int interval, QObject *parent) :
	intValue(interval),
	QThread(parent)
{
	bStop = false;
      //  connect( &Timer, SIGNAL(timeout()), this, SLOT(testFunction()) );
      //  laprinter = new  LAPrinter();
       //  Timer.start(10000);


}

void PrintThread::SetInterval(unsigned int interval)
{
	intValue = interval;
}

void PrintThread::PrintStop()
{
	bStop = true;
}

void PrintThread::run()
{


     //   emit PrintTicket();

}


void PrintThread::testFunction()
{
   // char errStr[1024];
  //  qDebug("it is Timer");
   // laprinter->PrinterGetLastErrorStr(errStr,1024);
 //   emit PrintErrStr();

}
