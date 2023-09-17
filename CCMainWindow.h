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
#include "DBconn.h"
class CCMainWindow : public BasicWindow
{
	Q_OBJECT

public:
	CCMainWindow(QWidget *parent = nullptr,QString str=" ");
	~CCMainWindow();
public:
	void setUserName(const QString& userName);//设置用户名
	void setLevelPixmap(int level);//设置等级
	void setHeadPixmap(const QString& headPath);//设置头像
	void setStatusMenuIcon(const QString& statusPath);//设置状态
	QWidget* addOtherAppExtension(const QString& appPath,const QString& appName);//添加应用部件
	void initContactTree();
private:
	void initTimer();//初始化定时器
	void inintControl();
	void updateSearchStyle();//更新搜索样式
	void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID);
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
	void onShowQuit(bool);
private:
	Ui::MainWindowClass ui;
	QString gLoginEmployeeID;//登陆者的qq号
	QString m_sign;
	QString m_name;
};
