#include "RootContactItem.h"

RootContactItem::RootContactItem(bool hasArrow,QWidget *parent)
	: QLabel(parent)
	,m_rotation(0)
	,m_hasArrow(hasArrow)
{
	setFixedHeight(32);
	setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

	//初始化动画属性
	m_animation = new QPropertyAnimation(this, "rotation");
	m_animation->setDuration(50);//设置单次动画时长50ms
	m_animation->setEasingCurve(QEasingCurve::InQuad);//缓和类型曲线 t^2从0加速
}

RootContactItem::~RootContactItem()
{}

void RootContactItem::setText(const QString & title)
{
	m_titleText = title;
	update();
}

void RootContactItem::setExpanded(bool expand)
{
	if (expand) {
		m_animation->setEndValue(90);
	}
	else{
		m_animation->setEndValue(0);
	}
	m_animation->start();
}

int RootContactItem::rotation()
{
	return m_rotation;
}

void RootContactItem::setRotation(int rotation)
{
	m_rotation = rotation;
	update();
}

void RootContactItem::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::TextAntialiasing, true);//文本防锯齿

	QFont font;
	font.setPointSize(10);
	painter.setFont(font);
	painter.drawText(24, 0, width() - 24, height(), Qt::AlignLeft | Qt::AlignVCenter, m_titleText);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.save();//保存画家设置

	if (m_hasArrow) {
		QPixmap pix;
		pix.load(":/Resources/MainWindow/arrow.png");

		QPixmap temp(pix.size());
		temp.fill(Qt::transparent);

		QPainter p(&temp);
		p.setRenderHint(QPainter::SmoothPixmapTransform, true);

		p.translate(pix.width() / 2, pix.height() / 2);//坐标系偏移
		p.rotate(m_rotation);//旋转坐标系（顺时针）
		p.drawPixmap(0 - pix.width() / 2, 0 - pix.height() / 2, pix);

		painter.drawPixmap(6, (height() - pix.height()) / 2, temp);
		painter.restore();//恢复画家设置
	}

	QLabel::paintEvent(event);
}
