#pragma once

#include <QWebEngineView>
#include<QDomNode>
#include<QFile>
#include<QMessageBox>
#include<QJsonObject>
#include<QJsonDocument>
#include<QWebChannel>
//QWebEngineView用于显示网页
class MsgHtmlObject :public QObject {
	Q_OBJECT
	Q_PROPERTY(QString msgLHtmlTmpl MEMBER m_msgLHtmlTmpl NOTIFY signalMsgHtml)	//发来的信息
	Q_PROPERTY(QString msgRHtmlTmpl MEMBER m_msgRHtmlTmpl NOTIFY signalMsgHtml)	//发出的信息
public:
	MsgHtmlObject(QObject* parent);
signals:
	void signalMsgHtml(const QString& html);
private:
	void initHtmlTmpl();//初始化聊天网页
	QString getMsgTempHtml(const QString& code);
private:
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
	MsgWebView(QWidget* parent);
	~MsgWebView();
	void appendMsg(const QString& html);
private:
	QList<QStringList>parseHtml(const QString& html);//解析html
	//Qt中所有DOM属性（属性、说明、文本）都可以使用QDomNode表示
	QList<QStringList>parseDocNode(const QDomNode& node);
private:
	MsgHtmlObject* m_msgHtmlObj;
};
