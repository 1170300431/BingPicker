#pragma once

#include <QtWidgets/QMainWindow>
#include <QDir>
#include <QEvent>
#include <QNetworkReply>
#include <QSettings>
#include "ui_frmMain.h"
#include "BingPicker.h"

class frmMain : public QMainWindow {
	Q_OBJECT

public:
	frmMain(QWidget *parent = Q_NULLPTR);

private:
	int idx = 0;
	Ui::frmMainClass ui;
	QDir storage;
	QSettings ini;
	BingPicker bingpicker;
	void showDate();
	void showImage(QIcon);
	void shapeDownloadButton();

	static QString StringJulianDay(int idx) {
		//inline
		const static QString BASE("%1");
		return BASE.arg(QDate::currentDate().addDays(-idx).toJulianDay());
	}

	bool deleteFile(const QString& picName);

protected:
	void changeEvent(QEvent*);

private slots:
	void already_exist();
	void download_complete();
	void progress(qint64, qint64);
	void deleteFile();
	void setWallpaper();
	void onDownloadClick();
	void showInExplorer();
	void bulkDeletion();
};
