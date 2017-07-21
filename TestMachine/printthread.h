#ifndef PRINTTHREAD_H
#define PRINTTHREAD_H

#include"./engine/laprinter.h"
#include <QThread>
#include<QTimer>

class PrintThread : public QThread
{
	Q_OBJECT
public:
	explicit PrintThread(unsigned int interval = 5, QObject *parent = 0);
	void SetInterval(unsigned int interval);
	void PrintStop();

        QTimer Timer;
        LAPrinter *laprinter;
protected:
	void run();

private:
	unsigned int intValue;
	bool bStop;
	
signals:
        void PrintTicket();
        void PrintErrStr();
	
public slots:
       void testFunction();
	
};

#endif // PRINTTHREAD_H
