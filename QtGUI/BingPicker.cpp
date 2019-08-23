#include "BingPicker.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

QString BingPicker::url2name(QString url) {
	auto split = [](const QString& x) -> QString {
		QString r;
		r.reserve(x.size());
		r.push_back(x[0]);
		bool ign = x.begin()->isUpper();

		for (auto i = x.begin() + 1; i != x.end(); i++) {

			if (!ign && i->isUpper()) r.push_back(' ');
			else if (!i->isUpper()) ign = false;

			r.push_back(*i);
		}
		return r;
	};

	QRegExp name_reg("id=.*([a-zA-Z]+)_");
	name_reg.setMinimal(true);
	name_reg.indexIn(url);

	return split(name_reg.cap(1));
}

BingPicker::~BingPicker() {
	if (img_dld) {
		delete img_dld;
		img_dld = nullptr;
	}
}

std::tuple<QString, QString> BingPicker::BingAPI(const int idx) {
	assert(idx < 7);
	constexpr int n = 1;
	QString args = "https://www.bing.com/HPImageArchive.aspx?"
		"format=%1"								//格式, js
		"&idx=%2"								//应该是几天前的图片
		"&n=%3"									//几张?
		"&nc=%4"								//时间戳
		"&pid=hp";
	QUrl request(args.arg("js").arg(idx).arg(n).arg(QDateTime::currentMSecsSinceEpoch()));
	QByteArray byte = HttpGetter::HttpGet(request);
	if (byte.isNull()) {
		throw QNetworkReply::NetworkError::UnknownContentError;
	}

	QJsonObject json = QJsonDocument::fromJson(byte).object();
	QJsonObject images = json.value("images").toArray().begin()->toObject();
	return {
		BING + images.value("url").toString(), 
		images.value("copyright").toString()
	};
}

void BingPicker::download(const int idx) {
	QString url;
	try{
		using std::get;
		auto t = BingAPI(idx);
		url = get<0>(t); copyright = get<1>(t);
	}
	catch (QNetworkReply::NetworkError eid) {
		emit network_error(eid);
		return;
	}

	filename = url2name(url);
	QDir user_picPath = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0];
	user_picPath.mkpath("BingPicker");
	bool cd_res = user_picPath.cd("BingPicker");
	assert(cd_res);

	filepath = user_picPath.filePath(filename + ".jpg");
	if (QFile::exists(filepath)) {
		emit already_exist();
		return;
	}

	img_dld = new HttpGetter(url);
	connect(img_dld, &HttpGetter::download_progress,
	[this](qint64 c, qint64 t) {
		emit download_progress(c, t);
	});
	connect(img_dld, &HttpGetter::download_complete,
	[this](QByteArray bytes) {
		this->bytes = bytes;
		QFile file(filepath);
		file.open(QIODevice::WriteOnly);
		file.write(bytes);
		file.close();
		emit download_complete();
	});
}