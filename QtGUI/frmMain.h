#pragma once

#include <QtWidgets/QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include "ui_frmMain.h"
#include "BingPicker.h"

class frmMain : public QMainWindow {
	Q_OBJECT

public:
	frmMain(QWidget *parent = Q_NULLPTR);

private:
	Ui::frmMainClass ui;
	BingPicker bingpicker;

private slots:
	void already_exist();
	void download_complete();
	void network_error(QNetworkReply::NetworkError eid);
	void progress(qint64, qint64);

public slots:
	void setWallpaper();
	void onDownloadClick();
};
