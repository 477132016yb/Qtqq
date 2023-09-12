#pragma once

#include <QWidget>
#include "ui_MainWindow.h"
#include "BasicWindow.h"
#include <QTimer>
#include"SkinWindow.h"
#include"SysTray.h"
#include<QTreeWidgetItem>
#include"RootContactItem.h"
#include"ContactItem.h"
#include"WindowManger.h"
#include"TalkWindowShell.h"

class CCMainWindow : public BasicWindow
{
	Q_OBJECT

public:
	CCMainWindow(QWidget *parent = nullptr);
	~CCMainWindow();
public:
	void setUserName(const QString& userName);//�����û���
	void setLevelPixmap(int level);//���õȼ�
	void setHeadPixmap(const QString& headPath);//����ͷ��
	void setStatusMenuIcon(const QString& statusPath);//����״̬
	QWidget* addOtherAppExtension(const QString& appPath,const QString& appName);//����Ӧ�ò���
	void initContactTree();
private:
	void initTimer();//��ʼ����ʱ��
	void inintControl();
	void updateSearchStyle();//����������ʽ
	void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, const QString& sDeps);
private:
	void resizeEvent(QResizeEvent* event);
	bool eventFilter(QObject* obj, QEvent* event);
	void mousePressEvent(QMouseEvent* event);
private slots:
	void onItemClicked(QTreeWidgetItem*item, int col);
	void onItemExpanded(QTreeWidgetItem*item);
	void onItemCollapsed(QTreeWidgetItem*item);
	void onItemDoubleClicked(QTreeWidgetItem* item, int col);
	void onAppIconClicked();
private:
	Ui::MainWindowClass ui;
	QMap<QTreeWidgetItem*, QString>m_groupMap;//���з�����
};