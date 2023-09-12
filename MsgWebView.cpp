#include "MsgWebView.h"

MsgWebView::MsgWebView(QWidget* parent)
	: QWebEngineView(parent)
{
}

MsgWebView::~MsgWebView()
{}

void MsgWebView::appendMsg(const QString& html)
{
}

QList<QStringList> MsgWebView::parseHtml(const QString& html)
{
	return QList<QStringList>();
}

QList<QStringList> MsgWebView::parseDocNode(const QDomNode& node)
{
	return QList<QStringList>();
}

MsgHtmlObject::MsgHtmlObject(QObject* parent)
{
	initHtmlTmpl();
}

void MsgHtmlObject::initHtmlTmpl()
{
	m_msgLHtmlTmpl = getMsgTempHtml("msgleftTmpl");
	m_msgRHtmlTmpl = getMsgTempHtml("msgrightTmpl");
}

QString MsgHtmlObject::getMsgTempHtml(const QString& code)
{
	QFile file(":/Resources/MainWindow/MsgHtml/" + code + ".html");
	file.open(QFile::ReadOnly);
	QString strData;
	if (file.isOpen()) {
		strData = QLatin1Literal(file.readAll());
	}
	else {
		QMessageBox::information(nullptr, "Tips", "Failed to init html!");
		return QString("");
	}
	file.close();
	return strData;
}

bool MsgWebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
	//仅接受qrc:/*.html
	if (url.scheme() == QString("qrc")) {//判断url类型
		return true;
	}
	return false;
}

