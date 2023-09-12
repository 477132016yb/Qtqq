#pragma once

#include <QWidget>
#include "ui_EmotionWindow.h"
#include"CommonUtils.h"
#include"EmotionLabelItem.h"
#include<QStyleOption>

const int emotionColum = 14;
const int emotionRow = 12;

class EmotionWindow : public QWidget
{
	Q_OBJECT

public:
	EmotionWindow(QWidget *parent = nullptr);
	~EmotionWindow();
private:
	void initControl();
private slots:
	void addEmotion(int emotionNum);
signals:
	void signalEmotionWindowHide();
	void signalEmotionItemClicked(int emotionNum);
private:
	void paintEvent(QPaintEvent* event)override;
private:
	Ui::EmotionWindowClass ui;
};
