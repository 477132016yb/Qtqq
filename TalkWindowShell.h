#pragma once
#include<QWidget>
#include"ui_TalkWindowShell.h"
#include"BasicWindow.h"
#include<QMap>
#include<QListWidget>
#include"DBconn.h"
#include<QTcpSocket>
class TalkWindow;
class TalkWindowItem;
class QListWidgetItem;
class EmotionWindow;

const int gtcpPort = 6666;

class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = nullptr);
	~TalkWindowShell();
public:
	void addTalkWindow(TalkWindow* talkWindow,TalkWindowItem*talkWindowItem, const QString& uid);
	void setCurrentWidget(QWidget* widget);
	const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap() const;
private:
	void initControl();
	void initTcpSocket();//初始化TCP
	void getEmployeesID(QStringList&emplyeesList);//获取所有员工QQ号
	bool createJSFile(QStringList&emplyeesList);
public slots:
	void onEmotionBtnClicked(bool);//表情按钮点击后
	void updateSendTcpMsg(QString& strData, int& msgType, QString sFile = "");//客户端发送tcp数据(数据，类型，文件)
private slots:
	void onTalkWindowItemClicked(QListWidgetItem*item);//左侧列表
	void onEmotionItemClicked(int emotionNum);//表情被选中
private:
	Ui::TalkWindowShellClass ui;
	QMap<QListWidgetItem*, QWidget*>m_talkWindowItemMap;//打开的聊天窗口
	EmotionWindow* m_emotionWindow;//表情窗口
private:
	QTcpSocket* m_tcpClientSocket;
};
