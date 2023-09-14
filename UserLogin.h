#pragma once

#include "BasicWindow.h"
#include "ui_UserLogin.h"
#include"CCMainWindow.h"
#include<QMessageBox>
#include"DBconn.h"
class UserLogin : public BasicWindow
{
	Q_OBJECT

public:
	UserLogin(QWidget *parent = nullptr);
	~UserLogin();
private slots:
	void onLoginBtnClicked();
	void onButtonCloseClicked();
private:
	void initControl();
	bool connectMySql();
	bool veryfyAccountCode();
	QString m_gLoginEmployeeID;
private:
	Ui::UserLoginClass ui;
};
