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

	//��ǰ���ڹ��������촰�ڣ�QQ�ţ�
	QString strTalkId = WindowManger::getInstance()->getCreatingTalkId();

	QString strEmployeeID, strPicturePath;
	QString strExternal;
	bool isGroup = false;

	//��ȡ��˾Ⱥid
	QString sql = QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(QString::fromLocal8Bit("��˾Ⱥ"));
	MYSQL_RES* res = DBconn::getInstance()->myQuery(sql.toStdString());
	MYSQL_ROW row = mysql_fetch_row(res);
	QString strCompanyID = QString(row[0]);

	if (strTalkId == strCompanyID) {//��˾Ⱥ��
		isGroup = true;
		sql = QString("SELECT employeeID,picture FROM tab_employees WHERE status = 1");
	}
	else {
		if (strTalkId.length() == 4) {//����Ⱥ��
			isGroup = true;
			sql = QString("SELECT employeeID,picture FROM tab_employees WHERE status = 1 AND departmentID = %1").arg(strTalkId);
		}
		else {//��������
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
	//��ʼ������Ϣҳ��
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));
}

MsgWebView::~MsgWebView()
{}

void MsgWebView::appendMsg(const QString& html,const QString obj)
{
	QJsonObject msgObj;
	QString qsMsg;
	const QList<QStringList>msgList = parseHtml(html);//����html

	int imageNum = 0;
	int msgType = 1;//��Ϣ����:0�Ǳ��� 1���ı� 2���ļ�
	bool isImageMsg = false;
	QString strData;//���͵�����


	for (int i = 0; i < msgList.size(); i++) {
		if (msgList.at(i).at(0) == "img") {
			QString imagePath = msgList.at(i).at(1);
			QPixmap pixmap;

			//��ȡ�������Ƶ�λ��
			QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";
			int pos = strEmotionPath.size();
			isImageMsg = true;

			//��ȡ��������
			QString strEmotionName = imagePath.mid(pos);
			strEmotionName.replace(".png", "");

			//�������Ƴ��Ȳ�����λ
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
			//����ͼƬhtml��ʽ�ı����
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
	if (obj == "0") {//����Ϣ
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));

		if (isImageMsg) {
			strData = QString::number(imageNum) + "images" + strData;
		}

		emit signalSendMsg(strData, msgType);
	}
	else {//����
		QString js = QString("recvHtml_%1(%2)").arg(obj).arg(Msg);
		this->page()->runJavaScript(js);
	}
}

QList<QStringList> MsgWebView::parseHtml(const QString& html)
{
	QDomDocument doc;
	doc.setContent(html);
	const QDomElement& root = doc.documentElement();//�ڵ�Ԫ��
	const QDomNode& node = root.firstChildElement("body");
	return parseDocNode(node);
}

QList<QStringList> MsgWebView::parseDocNode(const QDomNode& node)
{
	QList<QStringList>attribute;
	const QDomNodeList& list = node.childNodes();//���������ӽڵ�
	for (int i = 0; i < list.count(); i++) {
		const QDomNode& node = list.at(i);

		if (node.isElement()) {
			//ת��Ԫ��
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
	//������qrc:/*.html
	if (url.scheme() == QString("qrc")) {//�ж�url����
		return true;
	}
	return false;
}

