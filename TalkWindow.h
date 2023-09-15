#pragma once
#include <QWidget>
#include<QToolTip>
#include "ui_TalkWindow.h"
#include"CommonUtils.h"
#include"RootContactItem.h"
#include"ContactItem.h"
#include"WindowManger.h"
#include<QFile>
#include<QMessageBox>
class TalkWindow : public QWidget
{
	Q_OBJECT

public:
	TalkWindow(QWidget *parent,const QString uid);
	~TalkWindow();
public:
	void addEmotionImage(int emotionNum);
	void setWindowName(const QString& name);
	QString getTalkId();
private slots:
	void onSendBtnClicked(bool);
	void onItemDoubleClicked(QTreeWidgetItem*item, int colnum);
private:
	void initControl();
	void initGroupTalkStatus();
	void initGroupTalK();//��ʼ��Ⱥ��
	void initPtoPTalk();//��ʼ������
	void addPeopInfo(QTreeWidgetItem* pRootGroupItem,int employeeID);
private:
	Ui::TalkWindowClass ui;
	QString m_talkId;
	QMap<QTreeWidgetItem*, QString>m_groupPeopleMap;//���з�����ϵ��
	bool m_isGroupTalk;
	friend class TalkWindowShell;
};
