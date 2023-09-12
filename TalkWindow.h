#pragma once
#include <QWidget>
#include "ui_TalkWindow.h"
#include"CommonUtils.h"
#include"RootContactItem.h"
#include"ContactItem.h"
#include"WindowManger.h"
class TalkWindow : public QWidget
{
	Q_OBJECT

public:
	TalkWindow(QWidget *parent,const QString uid, GroupType groupType);
	~TalkWindow();
public:
	void addEmotionImage(int emotionNum);
	void setWindowName(const QString& name);
private slots:
	void onSendBtnClicked(bool);
	void onItemDoubleClicked(QTreeWidgetItem*item, int colnum);
private:
	void initControl();
	void initCompanyTalk(const QString& name,int num);//��ʼ��Ⱥ��
	void initPtoPTalk();//��ʼ������
	void addPeopInfo(QTreeWidgetItem* pRootGroupItem,int i);
private:
	Ui::TalkWindowClass ui;
	QString m_talkId;
	GroupType m_groupType;
	QMap<QTreeWidgetItem*, QString>m_groupPeopleMap;//���з�����ϵ��
};