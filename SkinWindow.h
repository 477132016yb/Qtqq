#pragma once

#include <QWidget>
#include "ui_SkinWindow.h"
#include"BasicWindow.h"
#include"QClickLabel.h"

class SkinWindow : public BasicWindow
{
	Q_OBJECT

public:
	SkinWindow(QWidget *parent = nullptr);
	~SkinWindow();
public:
	void initControl();
public slots:
	void onShowClose();
private:
	Ui::SkinWindowClass ui;
};
