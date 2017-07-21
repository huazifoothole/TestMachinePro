#include "testmachine.h"
#include <QApplication>
#include <QTextCodec>
#include <QDir>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

	//QDir::setCurrent(LATECH_PATH"/APP");

	TestMachine w;
	w.setWindowFlags(Qt::FramelessWindowHint);
	w.setGeometry(0, 0, 1024, 768);
	w.show();
	
	return a.exec();
}
