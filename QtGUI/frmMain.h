#pragma once

#include <QtWidgets/QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "ui_frmMain.h"

class frmMain : public QMainWindow
{
	Q_OBJECT

public:
	frmMain(QWidget *parent = Q_NULLPTR);
	void download(QUrl url);
	void ParseAndDownload(QByteArray);
	void DownloadAndSave(QByteArray);
	std::function<void(frmMain*, QByteArray)> onGet;
	static std::tuple<std::string, std::string> regurl(std::string httpD) throw(std::runtime_error);

private:
	Ui::frmMainClass ui;
	QString filename;
	QString filePath;

public slots:
	void parseURL();
	void setWallpaper();
	void finishedSlot(QNetworkReply* reply);
};
