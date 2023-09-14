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

	TalkWindowShell* talkWindowShell = WindowManger::getInstance()->getTalkWindowShell();
	connect(this, &MsgWebView::signalSendMsg, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);
	//初始化收信息页面
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));
}

MsgWebView::~MsgWebView()
{}

void MsgWebView::appendMsg(const QString& html, QString obj)
{
	QJsonObject msgObj;
	QString qsMsg;
	const QList<QStringList>msgList = parseHtml(html);//解析html

	int imageNum = 0;
	int msgType = 1;//信息类型:0是表情 1是文本 2是文件
	bool isImageMsg = false;
	QString strData;//发送的数据


	for (int i = 0; i < msgList.size(); i++) {
		if (msgList.at(i).at(0) == "img") {
			QString imagePath = msgList.at(i).at(1);
			QPixmap pixmap;

			//获取表情名称的位置
			QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";
			int pos = strEmotionPath.size();
			isImageMsg = true;

			//获取表情名称
			QString strEmotionName = imagePath.mid(pos);
			strEmotionName.replace(".png", "");

			//表情名称长度补足三位
			int emotionNameL = strEmotionName.length();
			if (emotionNameL == 1) {
				strData = strData + "00" + strEmotionName;
			}
			else if (emotionNameL == 2) {
				strData = strData + "0" + strEmotionName;
			}
			else if (emotionNameL == 3) {
				strData = strData + strEmotionName;
			}
			msgType = 0;

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
			imageNum++;
		}
		else if (msgList.at(i).at(0) == "text") {
			qsMsg += msgList.at(i).at(1);
			strData = qsMsg;
		}
	}

	msgObj.insert("MSG", qsMsg);
	const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);
	if (obj == "0") {//发信息
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));

		if (isImageMsg) {
			strData = QString::number(imageNum) + "images" + strData;
		}

		emit signalSendMsg(strData, msgType);
	}
	else {//来信
		this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(obj).arg(Msg));
	}
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

