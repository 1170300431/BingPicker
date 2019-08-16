#include "frmMain.h"
#include <QtWidgets>
#include <Regex>

using namespace std;
constexpr auto BING = "https://cn.bing.com";

frmMain::frmMain(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

void frmMain::setWallpaper()
{
	QSettings wallPaper("HKEY_CURRENT_USER\\Control Panel\\Desktop", QSettings::NativeFormat);
	//把注册表的桌面图片路径改为指定路径.
	wallPaper.setValue("Wallpaper", filePath);

	QByteArray byte = filePath.toLocal8Bit();
	//调用windows api.
	SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, byte.data(), SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
}

void frmMain::ParseAndDownload(QByteArray bytes) try {
	auto [path, name] = regurl(bytes.toStdString());
	filename = QString::fromStdString(name);
	QDir user_picPath = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0];
	user_picPath.mkpath("BingPicker");
	bool cd_res = user_picPath.cd("BingPicker");
	assert(cd_res);
	filePath = user_picPath.filePath(filename + ".jpg");

	if (QFile::exists(filePath)) {
		ui.progressBar->setMaximum(1);
		ui.statusBar->showMessage("\"" + filename + ".jpg\" already exists. ");
		return;
	}
	download(QUrl(QString::fromStdString(BING + path)));
}
catch (runtime_error ex) {
	ui.statusBar->showMessage(ex.what(), 3);
}

void frmMain::DownloadAndSave(QByteArray bytes)
{
	QFile file(filePath);
	file.open(QIODevice::WriteOnly);
	file.write(bytes);
	file.close();
	ui.statusBar->showMessage("\"" + filename + "\" is downloaded. ");
}

void frmMain::parseURL()
{
	this->onGet = &frmMain::ParseAndDownload;
	QNetworkAccessManager* accessManager = new QNetworkAccessManager(this);
	connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));

	QNetworkRequest request;
	request.setUrl(QUrl(BING));

	//get
	accessManager->get(request);
	ui.progressBar->setMinimum(0);
	ui.progressBar->setMaximum(0);
	
}

void frmMain::finishedSlot(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray bytes = reply->readAll();
		onGet(this, bytes);	
	}
	else
	{
		qDebug() << "finishedSlot errors here";
		qDebug("found error .... code: %d\n", static_cast<int>(reply->error()));
		qDebug(qPrintable(reply->errorString()));
	}
	reply->deleteLater();
}

void frmMain::download(QUrl url) {
	this->onGet = &frmMain::DownloadAndSave;

	QNetworkAccessManager* accessManager = new QNetworkAccessManager(this);
	connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));

	QNetworkRequest request;
	request.setUrl(url);

	//get
	QNetworkReply* reply = accessManager->get(request);
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT([](qint64 cur, qint64 total) {
		ui.progressBar->setMaximum(total);
		ui.progressBar->setValue(cur);
	}));
}

tuple<string, string> frmMain::regurl(string httpD) throw(runtime_error) {
	auto split = [](const string& x) -> string {
		string r;
		r.reserve(x.size());
		r.push_back(x[0]);

		for (auto i = x.begin() + 1; i != x.end(); i++) {
			if (*i >= 'A' && *i <= 'Z') r.push_back(' ');
			r.push_back(*i);
		}
		return r;
	};

	smatch rurl, rname;
	if (!std::regex_search(httpD, rurl, regex("href=\"(.*?1920x1080.jpg.*?)\""))) throw std::runtime_error("Sorry. But no picture is crawled. Ask for your dev.");
	string tmp = rurl[1];
	regex_search(tmp, rname, regex("id=.*?([a-zA-Z]+)_"));
	return make_tuple(tmp, split(rname[1]));
}