#include "BingPicker.h"

void BingPicker::parseURL() try {
	emit download_progress(0, 0);
	QByteArray httpd = HttpGetter::HttpGet(BING);
	auto [path, name] = regurl(httpd);
	filename = name;
	QDir user_picPath = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0];
	user_picPath.mkpath("BingPicker");
	bool cd_res = user_picPath.cd("BingPicker");
	assert(cd_res);

	filepath = user_picPath.filePath(name + ".jpg");
	if (QFile::exists(filepath)) {
		emit already_exist();
		return;
	}

	img_dld = new HttpGetter(BING + path);
	connect(img_dld, &HttpGetter::download_progress, 
	[this](qint64 c, qint64 t) {
		emit download_progress(c, t);
	});
	connect(img_dld, &HttpGetter::download_complete, 
	[this](QByteArray bytes) {
		QFile file(filepath);
		file.open(QIODevice::WriteOnly);
		file.write(bytes);
		file.close();
		emit download_complete();
	});
}
catch (QNetworkReply::NetworkError eid) {
	emit network_error(eid);
}

std::tuple<QString, QString> BingPicker::regurl(QString httpD) {
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

	QRegExp url_reg("href=\"(.*1920x1080.jpg.*)\"");
	url_reg.setMinimal(true);
	if (url_reg.indexIn(httpD) == -1) {
		throw QNetworkReply::NetworkError::UnknownContentError;
	}

	QRegExp name_reg("id=.*([a-zA-Z]+)_");
	name_reg.setMinimal(true);
	name_reg.indexIn(url_reg.cap(1));

	return { url_reg.cap(1), split(name_reg.cap(1)) };
}

BingPicker::~BingPicker() {
	if (img_dld) {
		delete img_dld;
		img_dld = nullptr;
	}
}