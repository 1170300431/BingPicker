#pragma once
#include "HttpGetter.h"
#include <QFile>
#include <QDir>
#include <QStandardPaths>

class BingPicker: public QObject {
	Q_OBJECT
public:
	void download() {
		parseURL();
	}

	~BingPicker();

	QString Filename() const { return filename; }
	QString FilePath() const { return filepath; }

signals:
	void already_exist();
	void download_progress(qint64, qint64);
	void download_complete();
	void network_error(QNetworkReply::NetworkError);

private:
	QString filename, filepath;
	const QString BING = "https://cn.bing.com";
	HttpGetter* img_dld = nullptr;

	void parseURL();
	

	static std::tuple<QString, QString> regurl(QString httpD);
};

