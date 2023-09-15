#pragma once

#include <QWebEngineView>
#include<QDomNode>
//QWebEngineView������ʾ��ҳ
class MsgHtmlObject :public QObject {
	Q_OBJECT
	Q_PROPERTY(QString msgLHtmlTmpl MEMBER m_msgLHtmlTmpl NOTIFY signalMsgHtml)	//��������Ϣ
	Q_PROPERTY(QString msgRHtmlTmpl MEMBER m_msgRHtmlTmpl NOTIFY signalMsgHtml)	//��������Ϣ
public:
	MsgHtmlObject(QObject* parent,QString msgLPicPath="");//����Ϣ���˵�ͷ��·��
signals:
	void signalMsgHtml(const QString& html);
private:
	void initHtmlTmpl();//��ʼ��������ҳ
	QString getMsgTempHtml(const QString& code);
private:
	QString m_msgLPicPath;
	QString m_msgLHtmlTmpl;
	QString m_msgRHtmlTmpl;
};
class MsgWebPage :public QWebEnginePage {
	Q_OBJECT
public:
	MsgWebPage(QObject* parent = nullptr) :QWebEnginePage(parent) {}
protected:
	bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame);
};
class MsgWebView : public QWebEngineView
{
	Q_OBJECT

public:
	MsgWebView(QWidget* parent=nullptr);
	~MsgWebView();
	void appendMsg(const QString& html,const QString obj="0");
private:
	QList<QStringList>parseHtml(const QString& html);//����html
	//Qt������DOM���ԣ����ԡ�˵�����ı���������ʹ��QDomNode��ʾ
	QList<QStringList>parseDocNode(const QDomNode& node);
signals:
	void signalSendMsg(QString& strData, int& msgType, QString sFile = "");
private:
	MsgHtmlObject* m_msgHtmlObj;
	QWebChannel* m_channel;
};
