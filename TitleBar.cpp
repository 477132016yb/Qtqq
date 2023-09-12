#include "TitleBar.h"
#define BUTTON_HEIGHT 27 //按钮高度
#define BUTTON_WIDTH  27 //按钮宽度
#define TITLE_HEIGHT  27 //标题栏高度
TitleBar::TitleBar(QWidget* parent):QWidget(parent)
,m_isPressed(false)
,m_buttonType(MIN_MAX_BUTTON)
{
	initControl();
	initConnections();
	loadStyleSheet("Title");
}

TitleBar::~TitleBar()
{
}

void TitleBar::initControl() {
	m_pIcon = new QLabel(this);
	m_pTitleContent = new QLabel(this);

	m_pButtonMax = new QPushButton(this);
	m_pButtonMin = new QPushButton(this);
	m_pButtonRestore = new QPushButton(this);
	m_pButtonClose= new QPushButton(this);

	//设置按钮固定大小
	m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));

	//设置对象名字
	m_pTitleContent->setObjectName("TitleContent");
	m_pButtonMax->setObjectName("ButtonMax");
	m_pButtonMin->setObjectName("ButtonMin");
	m_pButtonRestore->setObjectName("ButtonRestore");
	m_pButtonClose->setObjectName("ButtonClose");

	//设置布局
	QHBoxLayout* mylayout = new QHBoxLayout(this);
	mylayout->addWidget(m_pIcon);
	mylayout->addWidget(m_pTitleContent);

	mylayout->addWidget(m_pButtonMin);
	mylayout->addWidget(m_pButtonRestore);
	mylayout->addWidget(m_pButtonMax);
	mylayout->addWidget(m_pButtonClose);

	mylayout->setContentsMargins(5, 0, 0, 0);//设置布局的间隙
	mylayout->setSpacing(0);//设置布局部件间的间隙

	m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);//水平方向扩展，垂直方向固定
	setFixedHeight(TITLE_HEIGHT);
	setWindowFlags(Qt::FramelessWindowHint);//无边框
}

void TitleBar::initConnections() {
	connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));
	connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));
	connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked()));
	connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
}

void TitleBar::setTitleIcon(const QString&filePath) {
	QPixmap titleIcon(filePath);
	m_pIcon->setFixedSize(titleIcon.size());
	m_pIcon->setPixmap(titleIcon);
}

void TitleBar::setTitleContent(const QString& titleContent) {
	m_pTitleContent->setText(titleContent);
	m_titleContent = titleContent;
}

void TitleBar::setTitleWidth(int width) {
	setFixedWidth(width);
}

void TitleBar::setButtonType(ButtonType buttonType) {
	m_buttonType = buttonType;

	switch (buttonType) {
	case MIN_BUTTON:{	
		m_pButtonRestore->setVisible(false);
		m_pButtonMax->setVisible(false);
	}break;
	case MIN_MAX_BUTTON: {
		m_pButtonRestore->setVisible(false);
	}break;
	case ONLY_CLOSE_BUTTON: {
		m_pButtonRestore->setVisible(false);
		m_pButtonMax->setVisible(false);
		m_pButtonMin->setVisible(false);
	}break;
	default:
	break;
	}
}

void TitleBar::saveRestoreInfo(const QPoint& point, const QSize& size) {
	m_restorePos = point;
	m_restoreSize = size;
}

void TitleBar::getRestoreInfo(QPoint& point, QSize& size) {
	point = m_restorePos;
	size = m_restoreSize;
}


void TitleBar::paintEvent(QPaintEvent* event) {
	//设计背景色
	QPainter painter(this);
	QPainterPath pathBack;
	pathBack.setFillRule(Qt::WindingFill);//设置填充规则
	pathBack.addRoundedRect(QRect(0, 0, width(), height()), 3, 3);//添加圆角矩形到绘图路径
	painter.setRenderHint(QPainter::SmoothPixmapTransform,true);

	//当窗口最大化或者还原之后，窗口长度改变，标题栏做出相应改变
	if (width() != parentWidget()->width()) {
		setFixedWidth(parentWidget()->width());
	}

	QWidget::paintEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent*event){
	//只有存在最大化最小化按钮时才有效
	if (m_buttonType == MIN_MAX_BUTTON) {
		if (m_pButtonMax->isVisible()) {
			onButtonMaxClicked();
		}
		else {
			onButtonRestoreClicked();
		}
	}
	return QWidget::mouseDoubleClickEvent(event);
}

void TitleBar::mousePressEvent(QMouseEvent* event) {
	if (m_buttonType == MIN_MAX_BUTTON) {
		//在窗口最大化时禁止拖动
		if (m_pButtonMax->isVisible()) {
			m_isPressed = true;
			m_starMovePos = event->globalPos();//返回事件发生时鼠标所在的位置
		}
	}
	else {
		m_isPressed = true;
		m_starMovePos = event->globalPos();
	}
	return QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent* event) {
	if (m_isPressed) {
		QPoint movePoint = event->globalPos() - m_starMovePos; 
		QPoint widgetPos = parentWidget()->pos();
		m_starMovePos = event->globalPos();
		parentWidget()->move(widgetPos.x() + movePoint.x(), widgetPos.y()+movePoint.y());
	}
	return QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent* event) {
	m_isPressed = false;
	return QWidget::mouseReleaseEvent(event);
}

void TitleBar::loadStyleSheet(const QString &sheetName) {
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen()) {
		QString styleSheet = this->styleSheet();
		styleSheet += QLatin1String(file.readAll());
		setStyleSheet(styleSheet);
	}
	file.close();
}

void TitleBar::onButtonMinClicked() {
	emit signalButtonMinClicked();
}

void TitleBar::onButtonMaxClicked() {
	m_pButtonMax->setVisible(false);
	m_pButtonRestore->setVisible(true);
	emit signalButtonMaxClicked();
}

void TitleBar::onButtonRestoreClicked() {
	m_pButtonRestore->setVisible(false);
	m_pButtonMax->setVisible(true);
	emit signalButtonRestoreClicked();
}

void TitleBar::onButtonCloseClicked() {
	emit signalButtonCloseClicked();
}