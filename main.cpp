#include "retek2.h"
#include "zdownloader.h"
//#include <QtWidgets/QApplication>
//#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    //zDownloader d;
	retek2 w;

	w.init();
	w.show();

	return a.exec();
}
