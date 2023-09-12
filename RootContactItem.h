#pragma once

#include <QLabel>
#include<QPropertyAnimation>
#include<QPainter>

class RootContactItem  : public QLabel
{
	Q_OBJECT
	//箭头角度
	Q_PROPERTY(int rotation READ rotation WRITE setRotation)
public:
	RootContactItem(bool hasArrow=true,QWidget *parent=nullptr);
	~RootContactItem();
public:
	void setText(const QString& title);
	void setExpanded(bool expand);
private:
	int rotation();
	void setRotation(int rotation);
protected:
	void paintEvent(QPaintEvent* event);
private:
	QPropertyAnimation* m_animation;
	QString m_titleText;//显示的文本
	int m_rotation;//是否有角度
	bool m_hasArrow;//是否有箭头
};
