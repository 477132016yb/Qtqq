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
	void initGroupTalK();//初始化群聊
	void initPtoPTalk();//初始化单聊
	void addPeopInfo(QTreeWidgetItem* pRootGroupItem,int employeeID);
private:
	Ui::TalkWindowClass ui;
	QString m_talkId;
	QMap<QTreeWidgetItem*, QString>m_groupPeopleMap;//所有分组联系人
	bool m_isGroupTalk;
	friend class TalkWindowShell;
};
