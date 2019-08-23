#include "frmMain.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName("JamzumSum");
	a.setApplicationName("BingPicker");
	frmMain w;
	w.show();
	return a.exec();
}
