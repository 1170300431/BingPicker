#pragma once
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class HttpGetter: public QObject {
	Q_OBJECT
public:

	explicit HttpGetter(QUrl Url, QObject* parent = 0);

	static QByteArray HttpGet(QUrl url);

	virtual ~HttpGetter() {}

signals:
	void download_complete(QByteArray);
	void download_progress(qint64, qint64);
	void network_error(QNetworkReply::NetworkError);

private:
	QNetworkAccessManager webCtrl;

};

