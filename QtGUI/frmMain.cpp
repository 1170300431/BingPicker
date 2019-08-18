#include "frmMain.h"
#include <QtWidgets>
#include <Regex>

frmMain::frmMain(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

void frmMain::setWallpaper()
{
	QSettings wallPaper("HKEY_CURRENT_USER\\Control Panel\\Desktop", QSettings::NativeFormat);
	//把注册表的桌面图片路径改为指定路径.
	wallPaper.setValue("Wallpaper", bingpicker.FilePath());

	auto byte = bingpicker.FilePath().constData();
	//调用windows api.
	if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, const_cast<QChar*>(byte), SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE))
		ui.statusBar->showMessage("set wallpaper failed...", 5000);
}

void frmMain::onDownloadClick() {
	connect(&bingpicker, &BingPicker::download_complete, this, &frmMain::download_complete);
	connect(&bingpicker, &BingPicker::already_exist, this, &frmMain::already_exist);
	connect(&bingpicker, &BingPicker::download_progress, this, &frmMain::progress);
	connect(&bingpicker, &BingPicker::network_error, this, &frmMain::network_error);
	bingpicker.download();
}

void frmMain::already_exist() {
	ui.statusBar->showMessage("\"" + bingpicker.Filename() + ".jpg\" already exist. ", 5000);
	ui.progressBar->setMaximum(1);
	ui.actionSet_As_Wallpaper->setEnabled(true);
}

void frmMain::download_complete() {
	ui.statusBar->showMessage("\"" + bingpicker.Filename() + "\" is downloaded. ");
}

void frmMain::progress(qint64 cur, qint64 total) {
	ui.progressBar->setMaximum(total);
	ui.progressBar->setValue(cur);
	ui.actionSet_As_Wallpaper->setEnabled(true);
}

void frmMain::network_error(QNetworkReply::NetworkError eid) {
	ui.statusBar->showMessage("Sorry, but no image crawled. Check your network. ");
}