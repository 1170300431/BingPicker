#pragma once
#include "HttpGetter.h"
#include <QFile>
#include <QDir>
#include <QStandardPaths>

class BingPicker: public QObject {
	Q_OBJECT
public:
	void download(const int daysAgo);

	~BingPicker();

	QString Filename() const { return filename; }
	QString FilePath() const { return filepath; }
	QString CopyRight() const { return copyright; }
	const QByteArray raw_data() const { return bytes; }

signals:
	void already_exist();
	void download_progress(qint64, qint64);
	void download_complete();
	void network_error(QNetworkReply::NetworkError);

private:
	QString filename, filepath, copyright;
	QByteArray bytes;

	const QString BING = "https://cn.bing.com";
	HttpGetter* img_dld = nullptr;

	static QString url2name(QString httpD);
	std::tuple<QString, QString> BingAPI(const int daysAgo);
};

