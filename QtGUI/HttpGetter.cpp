#include "HttpGetter.h"
#include <QEventLoop>


HttpGetter::HttpGetter(QUrl Url, QObject* parent) : QObject(parent), webCtrl(this) {
	connect(&webCtrl, &QNetworkAccessManager::finished,
	[this](QNetworkReply* pReply) {
		if (pReply->error() == QNetworkReply::NetworkError::NoError) {
			//emit a signal
			pReply->deleteLater();
			emit download_complete(pReply->readAll());
		}
		else emit network_error(pReply->error());
	});
	auto reply = webCtrl.get(QNetworkRequest(Url));
	connect(reply, &QNetworkReply::downloadProgress,
	[this](qint64 c, qint64 t) {
		emit download_progress(c, t);
	});
}

QByteArray HttpGetter::HttpGet(QUrl url) {
	QNetworkAccessManager webCtrl;
	QEventLoop wait;
	connect(&webCtrl, &QNetworkAccessManager::finished, &wait, &QEventLoop::quit);
	QNetworkReply* reply = webCtrl.get(QNetworkRequest(url));
	wait.exec();

	QByteArray bytes = reply->readAll();
	reply->deleteLater();
	return bytes;
}
