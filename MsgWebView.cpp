#include "MsgWebView.h"

MsgWebView::MsgWebView(QWidget* parent)
	: QWebEngineView(parent)
{
	MsgWebPage* page = new MsgWebPage(this);
	setPage(page);

	QWebChannel* channel = new QWebChannel(this);
	m_msgHtmlObj = new MsgHtmlObject(this);
	channel->registerObject("external", m_msgHtmlObj);
	this->page()->setWebChannel(channel);
}

MsgWebView::~MsgWebView()
{}

void MsgWebView::appendMsg(const QString& html)
{
	QJsonObject msgObj;
	QString qsMsg;
	const QList<QStringList>msgList = parseHtml(html);//解析html
	for (int i = 0; i < msgList.size(); i++) {
		if (msgList.at(i).at(0) == "img") {
			QString imagePath = msgList.at(i).at(1);
			QPixmap pixmap;
			if (imagePath.left(3) == "qrc") {
				pixmap.load(imagePath.mid(3));
			}
			else{
				pixmap.load(imagePath);
			}
			//表情图片html格式文本组合
			QString imgPath = QString("<img src=\"%1\" width=\"%2\" height=\"%3\"/>")
									 .arg(imagePath).arg(pixmap.width()).arg(pixmap.height());
			qsMsg += imgPath;
		}
		else if (msgList.at(i).at(0) == "text") {
			qsMsg += msgList.at(i).at(1);
		}
	}

	msgObj.insert("MSG", qsMsg);
	const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);
	this->page()->runJavaScript(QString("appendHtml(%1)").arg(Msg));
}

QList<QStringList> MsgWebView::parseHtml(const QString& html)
{
	QDomDocument doc;
	doc.setContent(html);
	const QDomElement& root = doc.documentElement();//节点元素
	const QDomNode& node = root.firstChildElement("body");
	return parseDocNode(node);
}

QList<QStringList> MsgWebView::parseDocNode(const QDomNode& node)
{
	QList<QStringList>attribute;
	const QDomNodeList& list = node.childNodes();//返回所有子节点
	for (int i = 0; i < list.count(); i++) {
		const QDomNode& node = list.at(i);

		if (node.isElement()) {
			//转换元素
			const QDomElement& element = node.toElement();
			if (element.tagName() == "img") {
				QStringList attributeList;
				attributeList << "img" << element.attribute("src");
				attribute << attributeList;
			}
			if (element.tagName() == "span") {
				QStringList attributeList;
				attributeList << "text" << element.text();
				attribute << attributeList;
			}
			if (node.hasChildNodes()) {
				attribute<<parseDocNode(node);
			}
		}
	}

	return attribute;
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

