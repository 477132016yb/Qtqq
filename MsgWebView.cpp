#include"MsgWebView.h"
#include<QFile>
#include<QMessageBox>
#include<QJsonObject>
#include<QJsonDocument>
#include<QWebChannel>
#include"TalkWindowShell.h"
#include"WindowManger.h"

extern QString gstrLoginHeadPath;

MsgWebView::MsgWebView(QWidget* parent)
	: QWebEngineView(parent)
	,m_channel(new QWebChannel(this))
{
	MsgWebPage* page = new MsgWebPage(this);
	setPage(page);

	m_msgHtmlObj = new MsgHtmlObject(this);
	m_channel->registerObject("external0", m_msgHtmlObj);

	TalkWindowShell* talkWindowShell = WindowManger::getInstance()->getTalkWindowShell();
	connect(this, &MsgWebView::signalSendMsg, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);

	//当前正在构建的聊天窗口（QQ号）
	QString strTalkId = WindowManger::getInstance()->getCreatingTalkId();

	QString strEmployeeID, strPicturePath;
	QString strExternal;
	bool isGroup = false;

	//获取公司群id
	QString sql = QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(QString::fromLocal8Bit("公司群"));
	MYSQL_RES* res = DBconn::getInstance()->myQuery(sql.toStdString());
	MYSQL_ROW row = mysql_fetch_row(res);
	QString strCompanyID = QString(row[0]);

	if (strTalkId == strCompanyID) {//公司群聊
		isGroup = true;
		sql = QString("SELECT employeeID,picture FROM tab_employees WHERE status = 1");
	}
	else {
		if (strTalkId.length() == 4) {//其他群聊
			isGroup = true;
			sql = QString("SELECT employeeID,picture FROM tab_employees WHERE status = 1 AND departmentID = %1").arg(strTalkId);
		}
		else {//单独聊天
			sql = QString("SELECT picture FROM tab_employees WHERE status = 1 AND employeeID = %1").arg(strTalkId);
			res = DBconn::getInstance()->myQuery(sql.toStdString());
			row = mysql_fetch_row(res);
			strPicturePath = QString(row[0]);

			strExternal = "external_" + strTalkId;
			MsgHtmlObject* msgHtml = new MsgHtmlObject(this, strPicturePath);
			m_channel->registerObject(strExternal, msgHtml);
		}
	}

	if (isGroup) {
		res = DBconn::getInstance()->myQuery(sql.toStdString());
		int count = mysql_num_rows(res);
		for (int i = 0; i < count; i++) {
			row = mysql_fetch_row(res);
			strEmployeeID = QString(row[0]);
			strPicturePath = QString(row[1]);

			strExternal = "external_" + strEmployeeID;

			MsgHtmlObject* msgHtml = new MsgHtmlObject(this, strPicturePath);
			m_channel->registerObject(strExternal, msgHtml);
		}
	}

	this->page()->setWebChannel(m_channel);
	//初始化收信息页面
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));
}

MsgWebView::~MsgWebView()
{}

void MsgWebView::appendMsg(const QString& html,const QString obj)
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
		QString js = QString("recvHtml_%1(%2)").arg(obj).arg(Msg);
		this->page()->runJavaScript(js);
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

MsgHtmlObject::MsgHtmlObject(QObject* parent, QString msgLPicPath):QObject(parent)
,m_msgLPicPath(msgLPicPath)
{
	initHtmlTmpl();
}

void MsgHtmlObject::initHtmlTmpl()
{
	m_msgLHtmlTmpl = getMsgTempHtml("msgleftTmpl");
	m_msgLHtmlTmpl.replace("%1", m_msgLPicPath);

	m_msgRHtmlTmpl = getMsgTempHtml("msgrightTmpl");
	m_msgRHtmlTmpl.replace("%1", gstrLoginHeadPath);
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

