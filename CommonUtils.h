#pragma once
#include<QPixmap>
#include<QProxyStyle>
#include<QPainter>
#include<QFile>
#include<QWidget>
#include<QApplication>
#include<QSettings>
enum GroupType {
	COMPANY = 0,//��˾Ⱥ
	PERSONELGROUP,//���²�
	DEVELOPMENTGROUP,//�з���
	MARKETGROUP,//�г���
	PTOP//ͬ��֮������
};
//�ı�ϵͳĬ�ϵĲ������
class CustomProxyStyle :public QProxyStyle {
public:
	CustomProxyStyle();
	CustomProxyStyle(QObject*parent) {
		setParent(parent);
	}
	virtual void drawPrimititive(PrimitiveElement element, const QStyleOption* option, 
		QPainter* painter, const QWidget* widget = 0) const {
		if (PE_FrameFocusRect == element) {
			//ȥ��window�в���Ĭ�ϵı߿�����߿򣬲�����ý���ʱֱ�ӷ��أ������л���
			return;
		}
		else {
			QProxyStyle::drawPrimitive(element, option, painter, widget);
		}
	}
};

class CommonUtils
{
public:
	CommonUtils();

	static QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize masksize = QSize(0, 0));
	static void loadStyleSheet(QWidget* widget, const QString& sheetName);
	static void setDefaultSkinColor(const QColor& color);
	static QColor getDefaultSkinColor();
};

