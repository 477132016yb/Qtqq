#pragma once

#include <QLabel>
#include<QMouseEvent>
#include<QMovie>

class QClickLabel  : public QLabel
{
	Q_OBJECT

public:
	QClickLabel(QWidget *parent);
	~QClickLabel();
protected:
	void mousePressEvent(QMouseEvent* event);
signals:
	void clicked();
};
