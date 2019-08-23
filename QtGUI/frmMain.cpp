#include "frmMain.h"
#include <QStandardPaths>
#include <QtWidgets>

frmMain::frmMain(QWidget *parent)
	: QMainWindow(parent), ini(QSettings::IniFormat, QSettings::UserScope,
		QApplication::organizationName(), QApplication::applicationName()),
	storage(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0].append("/BingPicker"))
{
	ui.setupUi(this);
	connect(ui.btnLastDay, &QPushButton::clicked, [this]() {idx++; shapeDownloadButton(); });
	connect(ui.btnNextDay, &QPushButton::clicked, [this]() {idx--; shapeDownloadButton(); });
	connect(&bingpicker, &BingPicker::download_complete, this, &frmMain::download_complete);
	connect(&bingpicker, &BingPicker::already_exist, this, &frmMain::already_exist);
	connect(&bingpicker, &BingPicker::download_progress, this, &frmMain::progress);
	connect(&bingpicker, &BingPicker::network_error, this, [this](QNetworkReply::NetworkError) {
		ui.statusBar->showMessage("Sorry, but no image crawled. Check your network. ");
	});
	shapeDownloadButton(); 
}

void frmMain::shapeDownloadButton()
{
	QVariant picName = ini.value(StringJulianDay(idx));
	QString path = storage.filePath(picName.toString().append(".jpg"));
	if (picName.isNull() || !QFile(path).exists()) {
		ui.btnDownload->setIcon(QIcon());
		ui.btnDownload->setText("Download");
		ui.btnDownload->setFlat(false);
		ui.actionSet_As_Wallpaper->setEnabled(false);
		ui.actionDelete->setEnabled(false);
	}
	else {
		showImage(QIcon(path));
	}
	ui.progressBar->setValue(0);
	showDate();
}

void frmMain::changeEvent(QEvent* event)
{
	if (event->type() != QEvent::WindowStateChange) return;
	if (this->windowState() == Qt::WindowMaximized)
	{
		ui.btnDownload->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	}
	else {
		ui.btnDownload->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
	}
}

void frmMain::setWallpaper()
{
	QString path = storage.filePath(ini.value(StringJulianDay(idx)).toString());
	QSettings wallPaper("HKEY_CURRENT_USER\\Control Panel\\Desktop", QSettings::NativeFormat);
	wallPaper.setValue("Wallpaper", path);

	auto byte = path.constData();
	if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, const_cast<QChar*>(byte), SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE))
		ui.statusBar->showMessage("set wallpaper failed...", 5000);
}

void frmMain::onDownloadClick() {
	if (ui.btnDownload->isFlat()) return;
	bingpicker.download(idx);
}

void frmMain::already_exist() {
	showImage(QIcon(bingpicker.FilePath()));
	ui.progressBar->setMaximum(1);
	ui.statusBar->showMessage(bingpicker.CopyRight());
	ini.setValue(StringJulianDay(idx), bingpicker.Filename());
}

void frmMain::download_complete() {
	QPixmap pixmap;
	pixmap.loadFromData(bingpicker.raw_data());
	showImage(pixmap);
	ui.statusBar->showMessage(bingpicker.CopyRight());
	ini.setValue(StringJulianDay(idx), bingpicker.Filename());
}

void frmMain::showImage(QIcon img) {
	assert(img.isNull() == false);
	ui.btnDownload->setText("");
	ui.btnDownload->setFlat(true);
	ui.btnDownload->setIcon(img);
	ui.actionSet_As_Wallpaper->setEnabled(true);
	ui.actionDelete->setEnabled(true);
}

void frmMain::showDate() {
	if (idx >= 6) {
		idx = 6;
		ui.btnLastDay->setEnabled(false);
		ui.btnNextDay->setEnabled(true);
	}
	else if (idx <= 0) {
		idx = 0;
		ui.btnLastDay->setEnabled(true);
		ui.btnNextDay->setEnabled(false);
	}
	else {
		ui.btnLastDay->setEnabled(true);
		ui.btnNextDay->setEnabled(true);
	}
	QDate day = QDate::currentDate().addDays(-idx);
	ui.statusBar->showMessage(day.toString(), 5000);
}

void frmMain::progress(qint64 cur, qint64 total) {
	ui.progressBar->setMaximum(total);
	ui.progressBar->setValue(cur);
}

void frmMain::deleteFile()
{
	QString picName = ini.value(StringJulianDay(idx)).toString();
	if (!deleteFile(picName)) {
		ui.statusBar->showMessage("delete failed...");
		return;
	}
	shapeDownloadButton();
}

bool frmMain::deleteFile(const QString& picName)
{
	QFile file(storage.filePath(picName + ".jpg"));
	if (!file.remove()) return false;
	ini.remove(StringJulianDay(idx));
	return true;
}

void frmMain::showInExplorer() {
	QDesktopServices::openUrl(QUrl::fromLocalFile(storage.path()));
}

void frmMain::bulkDeletion()
{
	constexpr int expire = 30;
	QStringList list = ini.childKeys();
	qint64 ddl = QDate::currentDate().addDays(-expire).toJulianDay();			//fuck ddl
	for (const QString& key : list) {
		if (key.toLongLong() < ddl) {
			deleteFile(ini.value(key).toString());
		}
	}
}