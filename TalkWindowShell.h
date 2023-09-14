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
	void initTcpSocket();//��ʼ��TCP
	void getEmployeesID(QStringList&emplyeesList);//��ȡ����Ա��QQ��
	bool createJSFile(QStringList&emplyeesList);
public slots:
	void onEmotionBtnClicked(bool);//���鰴ť�����
	void updateSendTcpMsg(QString& strData, int& msgType, QString sFile = "");//�ͻ��˷���tcp����(���ݣ����ͣ��ļ�)
private slots:
	void onTalkWindowItemClicked(QListWidgetItem*item);//����б�
	void onEmotionItemClicked(int emotionNum);//���鱻ѡ��
private:
	Ui::TalkWindowShellClass ui;
	QMap<QListWidgetItem*, QWidget*>m_talkWindowItemMap;//�򿪵����촰��
	EmotionWindow* m_emotionWindow;//���鴰��
private:
	QTcpSocket* m_tcpClientSocket;
};
