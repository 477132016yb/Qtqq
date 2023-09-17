#pragma once

#include <QWidget>
#include "ui_ReceiveFile.h"
#include"BasicWindow.h"

class ReceiveFile : public BasicWindow
{
	Q_OBJECT

public:
	ReceiveFile(QWidget *parent = nullptr);
	~ReceiveFile();
	void setMsg(QString& msgLabel);
private slots:
	void on_okBtn_clicked();
	void on_cancelBtn_clicked();
signals :
	void refuseFile();
private:
	Ui::ReceiveFileClass ui;
};
