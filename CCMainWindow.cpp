#include "CCMainWindow.h"

CCMainWindow::CCMainWindow(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::Tool);
	loadStyleSheet("CCMainWindow");
	inintControl();
	initTimer();
}

CCMainWindow::~CCMainWindow()
{}

void CCMainWindow::initTimer()
{
	QTimer* timer = new QTimer(this);
	timer->setInterval(1000);
	connect(timer, &QTimer::timeout, [this] {
		static int level = 0;
		if (level++ >= 99) {
			level = 0;
		}
		setLevelPixmap(level);
	});
	timer->start();
}

void CCMainWindow::inintControl()
{
	//树获取焦点时不绘制边框
	ui.treeWidget->setStyle(new CustomProxyStyle);
	setHeadPixmap(":/Resources/MainWindow/girl.png");
	setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");

	QHBoxLayout* appUpLayout = new QHBoxLayout;
	appUpLayout->setContentsMargins(0, 0, 0, 0);
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_7.png", "app_7"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_2.png", "app_2"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_3.png", "app_3"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_4.png", "app_4"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_5.png", "app_5"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_6.png", "app_6"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/skin.png", "app_skin"));
	appUpLayout->addStretch();
	appUpLayout->setSpacing(1);
	ui.appWidget->setLayout(appUpLayout);

	ui.buttom_Layout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_10.png", "app_10"));
	ui.buttom_Layout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_8.png", "app_8"));
	ui.buttom_Layout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_11.png", "app_11"));
	ui.buttom_Layout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_9.png", "app_9"));
	ui.buttom_Layout_up->addStretch();

	initContactTree();

	//个性签名
	ui.lineEdit->installEventFilter(this);
	//好友搜索
	ui.searchLineEdit->installEventFilter(this);

	connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowHide(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), this, SLOT(onShowClose(bool)));
	connect(NotifyManager::getInstance(), &NotifyManager::signalSkinChanged, [this]() {
		updateSearchStyle();
	});
	SysTray* sysTray = new SysTray(this);
}

void CCMainWindow::setUserName(const QString& userName)
{
	ui.nameLabel->adjustSize();
	//文本过长则进行省略
	QString temp=ui.nameLabel->fontMetrics().elidedText(userName, Qt::ElideRight, ui.nameLabel->width());
	ui.nameLabel->setText(temp);
}

void CCMainWindow::setLevelPixmap(int level)
{
	QPixmap levelPixmap(ui.levelBtn->size());
	levelPixmap.fill(Qt::transparent);

	QPainter painter(&levelPixmap);
	painter.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));

	int unitNum = level % 10;
	int tenNum = level / 10;

	painter.drawPixmap(10, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"),tenNum*6,0,6,7);
	painter.drawPixmap(16, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"), unitNum * 6, 0, 6, 7);

	ui.levelBtn->setIcon(levelPixmap);
	ui.levelBtn->setIconSize(ui.levelBtn->size());
}

void CCMainWindow::setHeadPixmap(const QString& headPath)
{
	QPixmap pix;
	pix.load(":/Resources/MainWindow/head_mask.png");
	ui.headLabel->setPixmap(getRoundImage(QPixmap(headPath), pix, ui.headLabel->size()));
}

void CCMainWindow::setStatusMenuIcon(const QString& statusPath)
{
	QPixmap statusPixmap(ui.statusBtn->size());
	statusPixmap.fill(Qt::transparent);

	QPainter painter(&statusPixmap);
	painter.drawPixmap(4, 4, QPixmap(statusPath));
	
	ui.statusBtn->setIcon(statusPixmap);
	ui.statusBtn->setIconSize(ui.statusBtn->size());
}

QWidget* CCMainWindow::addOtherAppExtension(const QString& appPath, const QString& appName)
{
	QPushButton* btn = new QPushButton;
	btn->setFixedSize(20, 20);

	QPixmap pixmap(btn->size());
	pixmap.fill(Qt::transparent);

	QPainter painter(&pixmap);
	QPixmap appPixmap(appPath);
	painter.drawPixmap((btn->width() - appPixmap.width()) / 2, (btn->height() - appPixmap.height()) / 2, appPixmap);
	btn->setIcon(pixmap);
	btn->setIconSize(btn->size());
	btn->setObjectName(appName);
	btn->setProperty("hasborder", true);
	connect(btn, &QPushButton::clicked, this, &CCMainWindow::onAppIconClicked);
	return btn;
}

void CCMainWindow::initContactTree()
{
	//展开与收缩时的信号
	connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
	connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
	connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	//根节点
	QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem;
	pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	pRootGroupItem->setData(0, Qt::UserRole, 0);

	RootContactItem* pItemName = new RootContactItem(true, ui.treeWidget);
	QString strGroupName = QString::fromLocal8Bit("yb科技");
	pItemName->setText(strGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootGroupItem);
	ui.treeWidget->setItemWidget(pRootGroupItem, 0, pItemName);

	QStringList sCompDeps;//公司部门
	sCompDeps << QString::fromLocal8Bit("公司群");
	sCompDeps << QString::fromLocal8Bit("人事部");
	sCompDeps << QString::fromLocal8Bit("研发部");
	sCompDeps << QString::fromLocal8Bit("市场部");
	for (auto&a:sCompDeps) {
		addCompanyDeps(pRootGroupItem, a);
	}
}

void CCMainWindow::resizeEvent(QResizeEvent* event)
{
	setUserName(QString::fromLocal8Bit("你好世界，世界你好"));
	BasicWindow::resizeEvent(event);
}

bool CCMainWindow::eventFilter(QObject* obj, QEvent* event)
{
	if (ui.searchLineEdit == obj) {
		if (event->type() == QEvent::FocusIn) {
			ui.seachWidget->setStyleSheet(QString("QWidget#seachWidget{background-color:rgb(255,255,255);border-bottom:1px solid rgba(%1,%2,%3,100)}\
														QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/main_search_deldown.png)}\
														QPushButton#searchBtn:hover{border-image:url(:/Resources/MainWindow/search/main_search_delhighlight.png)}\
														QPushButton#searchBtn:pressed{border-image:url(:/Resources/MainWindow/search/main_search_delhighdown.png)}").
														arg(m_colorBackGround.red()).
														arg(m_colorBackGround.green()).
														arg(m_colorBackGround.blue()));
		}
	}
	else if (event->type() == QEvent::FocusOut) {
		updateSearchStyle();
	}
	return false;
}

void CCMainWindow::mousePressEvent(QMouseEvent* event)
{
	if (qApp->widgetAt(event->pos())!=ui.searchLineEdit&&ui.searchLineEdit->hasFocus()) {
		ui.searchLineEdit->clearFocus();
	}
	else if (qApp->widgetAt(event->pos()) != ui.lineEdit && ui.lineEdit->hasFocus()) {
		ui.lineEdit->clearFocus();
	}
	BasicWindow::mousePressEvent(event);
}

void CCMainWindow::updateSearchStyle()
{
	ui.seachWidget->setStyleSheet(QString("QWidget#seachWidget{background-color:rgba(%1,%2,%3,50);border-bottom:1px solid rgba(%1,%2,%3,30)}\
										   QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/search_icon.png)}").
										   arg(m_colorBackGround.red()).
										   arg(m_colorBackGround.green()).
	                                       arg(m_colorBackGround.blue()));
}

void CCMainWindow::addCompanyDeps(QTreeWidgetItem* pRootGroupItem, const QString& sDeps)
{
	QTreeWidgetItem* pChild = new QTreeWidgetItem;

	QPixmap pix;
	pix.load(":/Resources/MainWindow/head_mask.png");
	//添加子节点
	pChild->setData(0, Qt::UserRole, 1);//子项数据设为1
	pChild->setData(0, Qt::UserRole+1, QString::number((int)pChild));//唯一标识号
	ContactItem* pContactItem = new ContactItem(ui.treeWidget);
	pContactItem->setHeadPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/girl.png"),pix,pContactItem->getHeadLabelSize()));
	pContactItem->setUserName(sDeps);
	pRootGroupItem->addChild(pChild);
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	m_groupMap.insert(pChild, sDeps);
}

void CCMainWindow::onItemClicked(QTreeWidgetItem* item, int col)
{
	bool isChild = item->data(0, Qt::UserRole).toBool();
	if (!isChild) {
		item->setExpanded(!item->isExpanded());//未展开则展开子项
	}
}

void CCMainWindow::onItemExpanded(QTreeWidgetItem* item)
{
	bool isChild = item->data(0, Qt::UserRole).toBool();
	if (!isChild) {
		RootContactItem* pRootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));
		if (pRootItem) {
			pRootItem->setExpanded(true);
		}
	}
}

void CCMainWindow::onItemCollapsed(QTreeWidgetItem* item)
{
	bool isChild = item->data(0, Qt::UserRole).toBool();
	if (!isChild) {
		RootContactItem* pRootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));
		if (pRootItem) {
			pRootItem->setExpanded(false);
		}
	}
}

void CCMainWindow::onItemDoubleClicked(QTreeWidgetItem* item, int col)
{
	bool isChild = item->data(0, Qt::UserRole).toBool();
	if (isChild) {
		QString strGroup = m_groupMap.value(item);

		if (strGroup == QString::fromLocal8Bit("公司群")) {
			WindowManger::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), COMPANY);
		}
		else if (strGroup == QString::fromLocal8Bit("人事部")) {
			WindowManger::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), PERSONELGROUP);
		}
		else if (strGroup == QString::fromLocal8Bit("市场部")) {
			WindowManger::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), MARKETGROUP);
		}
		else if (strGroup == QString::fromLocal8Bit("研发部")) {
			WindowManger::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), DEVELOPMENTGROUP);
		}
	}
}

void CCMainWindow::onAppIconClicked()
{
	//判断信号发生者
	if (sender()->objectName() == "app_skin") {
		SkinWindow* skinWindow = new SkinWindow;
		skinWindow->show();
	}
}
