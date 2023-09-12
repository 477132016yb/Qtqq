#pragma once
#include"TitleBar.h"
#include"CommonUtils.h"
#include"NotifyManager.h"
#include<QDialog>
#include<QStyleOption>
#include<QApplication>
#include<QDesktopWidget>

class BasicWindow  : public QDialog
{
	Q_OBJECT

public:
	BasicWindow(QWidget *parent=nullptr);
	virtual ~BasicWindow();
public:
	//������ʽ��
	void loadStyleSheet(const QString& sheetName);

	//��ȡԲͷ��
	QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize = QSize(0, 0));
private:
	void initBackGroundColor();//��ʼ������
protected:
	void paintEvent(QPaintEvent* event);//��ͼ�¼�
	void mousePressEvent(QMouseEvent* event);//��갴ѹ�¼�
	void mouseMoveEvent(QMouseEvent* event);//����ƶ��¼�
	void mouseReleaseEvent(QMouseEvent* event);//����ɿ��¼�
protected:
	void initTitleBar(ButtonType buttonType = MIN_BUTTON);
	void setTitleBarTitle(const QString &title, const QString icon = "");
private slots:
	void onShowClose(bool);
	void onShowMin(bool);
	void onShowHide(bool);
	void onShowNormal(bool);
	void onShowQuit(bool);
	void onSignalSkinChanged(const QColor& color);

	void onButtonMinClicked();
	void onButtonRestoreClicked();
	void onButtonMaxClicked();
	void onButtonCloseClicked();
	
protected:
	QPoint m_mousePoint;//���λ��
	bool m_mousePressed;//����Ƿ���
	QColor m_colorBackGround;//����ɫ
	QString m_styleName;//��ʽ�ļ�����
	TitleBar* m_titleBar;//������
};
