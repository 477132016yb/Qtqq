#include "TalkWindow.h"

TalkWindow::TalkWindow(QWidget* parent, const QString uid, GroupType groupType)
	: QWidget(parent)
	,m_talkId(uid)
	,m_groupType(groupType)
{
	ui.setupUi(this);
	WindowManger::getInstance()->addWindowName(m_talkId,this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();
}

TalkWindow::~TalkWindow()
{
	WindowManger::getInstance()->deleteWindowName(m_talkId);
}

void TalkWindow::addEmotionImage(int emotionNum)
{
}

void TalkWindow::setWindowName(const QString& name)
{

}

void TalkWindow::onSendBtnClicked(bool)
{
}

void TalkWindow::onItemDoubleClicked(QTreeWidgetItem* item, int colnum)
{
	bool isChild = item->data(0, Qt::UserRole).toBool();
	if (isChild) {
		QString strName = m_groupPeopleMap.value(item);
		WindowManger::getInstance()->addNewTalkWindow(item->data(0,Qt::UserRole+1).toString(), PTOP, strName);
	}
}

void TalkWindow::initControl()
{
	QList<int>rightWidgetSize;
	rightWidgetSize << 600 << 138;
	ui.bodySplitter->setSizes(rightWidgetSize);

	ui.textEdit->setFontPointSize(10);
	ui.textEdit->setFocus();

	connect(ui.sysmin, SIGNAL(clicked(bool)), parent(), SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.closeBtn, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));

	connect(ui.faceBtn, SIGNAL(clicked(bool)), parent(), SLOT(onEmotionBtnClicked(bool)));
	connect(ui.sendBtn, SIGNAL(clicked(bool)), this, SLOT(onSendBtnClicked(bool)));

	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	switch (m_groupType)
	{
	case COMPANY:
		initCompanyTalk(QString::fromLocal8Bit("��˾Ⱥ"),50);
		break;
	case PERSONELGROUP:
		initCompanyTalk(QString::fromLocal8Bit("���²�"), 32);
		break;
	case DEVELOPMENTGROUP:
		initCompanyTalk(QString::fromLocal8Bit("�з���"), 10);
		break;
	case MARKETGROUP:
		initCompanyTalk(QString::fromLocal8Bit("�г���"), 15);
		break;
	case PTOP:
		initPtoPTalk();
		break;
	default:
		break;
	}
}

void TalkWindow::initCompanyTalk(const QString &name, int num)
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

	//����data���������ָ�������
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);

	QString qsGroupName = name+QString::fromLocal8Bit("%1/%2").arg(0).arg(num);
	pItemName->setText(qsGroupName);

	//�������ڵ�
	ui.treeWidget->addTopLevelItem(pRootItem);
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	//չ��
	pRootItem->setExpanded(true);
	for (int i = 0; i < num; i++) {
		addPeopInfo(pRootItem,i);
	}
}

void TalkWindow::initPtoPTalk()
{
	QPixmap pixSkin(":/Resources/MainWindow/skin.png");

	ui.widget->setFixedSize(pixSkin.size());

	QLabel* skinLabel = new QLabel(ui.widget);
	skinLabel->setPixmap(pixSkin);
	skinLabel->setFixedSize(ui.widget->size());
}

void TalkWindow::addPeopInfo(QTreeWidgetItem* pRootGroupItem,int i)
{
	QTreeWidgetItem* pChild = new QTreeWidgetItem();
	QPixmap mask(":/Resources/MainWindow/head_mask.png");
	QPixmap image(":/Resources/MainWindow/girl.png");

	//�����ӽڵ�
	pChild->setData(0, Qt::UserRole, 1);
	pChild->setData(0, Qt::UserRole + 1, QString::number((int)pChild));
	ContactItem* pContactItem = new ContactItem(ui.treeWidget);

	pContactItem->setHeadPixmap(CommonUtils::getRoundImage(image, mask, pContactItem->getHeadLabelSize()));
	pContactItem->setUserName(QString::fromLocal8Bit("Ԭ��%1��").arg(i));
	//pContactItem->setSignName(QString::fromLocal8Bit("%1��һ��������").arg(i));

	pRootGroupItem->addChild(pChild);
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	QString str = pContactItem->getUserName();
	m_groupPeopleMap.insert(pChild, str);
}