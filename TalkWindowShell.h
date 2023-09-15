#pragma once
#include<QWidget>
#include"ui_TalkWindowShell.h"
#include"BasicWindow.h"
#include<QMap>
#include<QListWidget>
#include"DBconn.h"
#include<QTcpSocket>
#include<QudpSocket>
class TalkWindow;
class TalkWindowItem;
class QListWidgetItem;
class EmotionWindow;

const int gtcpPort = 6666;
const int udpPort = 8888;

extern QString gLoginID;

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
	void initUdpSocket();//��ʼ��udp
	void getEmployeesID(QStringList&emplyeesList);//��ȡ����Ա��QQ��
	bool createJSFile(QStringList&emplyeesList);
	void processPendingData();//����㲥�յ�������
public slots:
	void onEmotionBtnClicked(bool);//���鰴ť�����
	void updateSendTcpMsg(QString& strData, int& msgType, QString fileName = "");//�ͻ��˷���tcp����(���ݣ����ͣ��ļ�)
private slots:
	void onTalkWindowItemClicked(QListWidgetItem*item);//����б�
	void onEmotionItemClicked(int emotionNum);//���鱻ѡ��
private:
	Ui::TalkWindowShellClass ui;
	QMap<QListWidgetItem*, QWidget*>m_talkWindowItemMap;//�򿪵����촰��
	EmotionWindow* m_emotionWindow;//���鴰��
private:
	QTcpSocket* m_tcpClientSocket;//tcp�ͻ���
	QUdpSocket* m_udpReceiver;//udp���ն�
};
