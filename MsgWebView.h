#pragma once

#include <QWebEngineView>
#include<QDomNode>
//QWebEngineView用于显示网页
class MsgHtmlObject :public QObject {
	Q_OBJECT
	Q_PROPERTY(QString msgLHtmlTmpl MEMBER m_msgLHtmlTmpl NOTIFY signalMsgHtml)	//发来的信息
	Q_PROPERTY(QString msgRHtmlTmpl MEMBER m_msgRHtmlTmpl NOTIFY signalMsgHtml)	//发出的信息
public:
	MsgHtmlObject(QObject* parent,QString msgLPicPath="");//发信息的人的头像路径
signals:
	void signalMsgHtml(const QString& html);
private:
	void initHtmlTmpl();//初始化聊天网页
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
	QList<QStringList>parseHtml(const QString& html);//解析html
	//Qt中所有DOM属性（属性、说明、文本）都可以使用QDomNode表示
	QList<QStringList>parseDocNode(const QDomNode& node);
signals:
	void signalSendMsg(QString& strData, int& msgType, QString sFile = "");
private:
	MsgHtmlObject* m_msgHtmlObj;
	QWebChannel* m_channel;
};
