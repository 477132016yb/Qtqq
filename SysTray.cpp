#include "SysTray.h"

SysTray::SysTray(QWidget *parent)
	: m_parent(parent),
	QSystemTrayIcon(parent)
{
	initSystemTray();
	show();
}

SysTray::~SysTray()
{}

void SysTray::onIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger) {
		m_parent->show();
		m_parent->activateWindow();
	}
	else if(reason==QSystemTrayIcon::Context) {
		addSystrayMenu();
	}
}

void SysTray::initSystemTray()
{
	setToolTip(QStringLiteral("QQ-yb"));
	setIcon(QIcon(":/Resources/MainWindow/app/logo.ico"));
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onIconActivated(QSystemTrayIcon::ActivationReason)));
}

void SysTray::addSystrayMenu()
{
	CustomMenu* custMenu = new CustomMenu(m_parent);
	custMenu->addCustomMenu("onShow", ":/Resources/MainWindow/app/logo.ico", QStringLiteral("ÏÔÊ¾"));
	custMenu->addCustomMenu("onQuit", ":/Resources/MainWindow/app/page_close_btn_hover.png", QStringLiteral("ÍË³ö"));
	connect(custMenu->getAction("onShow"),  SIGNAL(triggered(bool)), m_parent, SLOT(onShowNormal(bool)));
	connect(custMenu->getAction("onQuit"), SIGNAL(triggered(bool)), m_parent, SLOT(onShowQuit(bool)));

	custMenu->exec(QCursor::pos());
	delete custMenu;
	custMenu = nullptr;
}
