#include "TalkWindow.h"

TalkWindow::TalkWindow(QWidget* parent, const QString uid)
	: QWidget(parent)
	,m_talkId(uid)
{
	ui.setupUi(this);
	WindowManger::getInstance()->addWindowName(m_talkId,this);
	setAttribute(Qt::WA_DeleteOnClose);
	initGroupTalkStatus();
	initControl();
}

TalkWindow::~TalkWindow()
{
	WindowManger::getInstance()->deleteWindowName(m_talkId);
}

void TalkWindow::addEmotionImage(int emotionNum)
{
	ui.textEdit->setFocus();
	ui.textEdit->addEmotionUrl(emotionNum);
}

void TalkWindow::setWindowName(const QString& name)
{
	ui.nameLabel->setText(name);
}

QString TalkWindow::getTalkId()
{
	return m_talkId;
}

void TalkWindow::onSendBtnClicked(bool)
{
	if (ui.textEdit->toPlainText().isEmpty()) {
		QToolTip::showText(this->mapToGlobal(QPoint(630, 660)), QString::fromLocal8Bit("发送的信息不能为空！"),
			this, QRect(0, 0, 120, 100), 2000);
		return;
	}
	QString html = ui.textEdit->document()->toHtml();
	//文本html如果没有字体则添加字体 msgFont.txt
	if (!html.contains(".png") && !html.contains("</span>")) {
		QString fontHtml;
		QString text = ui.textEdit->toPlainText();
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly)) {
			fontHtml = file.readAll();
			fontHtml.replace("%1", text);
			file.close();
		}
		else {
			QMessageBox::information(this, "Tips", "file not exist!");
			return;
		}
		if (!html.contains(fontHtml)) {
			html.replace(text, fontHtml);
		}
	}

	ui.textEdit->clear();
	ui.textEdit->deletAllEmotionImage();

	ui.msgWidget->appendMsg(html);//收信息窗口添加信息
}

void TalkWindow::onItemDoubleClicked(QTreeWidgetItem* item, int colnum)
{
	bool isChild = item->data(0, Qt::UserRole).toBool();
	if (isChild) {
		WindowManger::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());
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

	if (m_isGroupTalk) {
		initGroupTalK();
	}
	else {
		initPtoPTalk();
	}
}


void TalkWindow::initGroupTalkStatus()
{
	//判断群聊还是单聊
	string strSql = "SELECT picture FROM tab_department WHERE departmentID = " + m_talkId.toStdString();
	MYSQL_RES* res = DBconn::getInstance()->myQuery(strSql);
	if (mysql_num_rows(res) == 0) {//单聊
		m_isGroupTalk = false;
	}
	else {
		m_isGroupTalk = true;
	}
}

void TalkWindow::initGroupTalK()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

	//设置data，用于区分根项子项
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);

	QString strGroupName;
	string strSql = "SELECT department_name FROM tab_department WHERE departmentID = " + m_talkId.toStdString();
	MYSQL_RES* res = DBconn::getInstance()->myQuery(strSql);
	MYSQL_ROW row = mysql_fetch_row(res);
	strGroupName = QString(row[0]);

	if (m_talkId.toInt() == 2000) {//公司群
		strSql = "SELECT employeeID FROM tab_employees WHERE status = 1";
	}
	else {
		strSql = "SELECT employeeID FROM tab_employees WHERE status = 1 AND departmentID = " + m_talkId.toStdString();
	}
	res = DBconn::getInstance()->myQuery(strSql);
	int num = mysql_num_rows(res);
	vector<string>v;
	while ((row = mysql_fetch_row(res)) != NULL) {
		v.push_back(row[0]);
	}

	QString qsGroupName = strGroupName + QString::fromLocal8Bit("%1/%2").arg(0).arg(num);
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	//展开
	pRootItem->setExpanded(true);
	for (int i = 0; i < num; i++) {
		addPeopInfo(pRootItem, stoi(v[i]));
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

void TalkWindow::addPeopInfo(QTreeWidgetItem* pRootGroupItem,int employeeID)
{
	QTreeWidgetItem* pChild = new QTreeWidgetItem();

	//添加子节点
	pChild->setData(0, Qt::UserRole, 1);
	pChild->setData(0, Qt::UserRole + 1, employeeID);
	ContactItem* pContactItem = new ContactItem(ui.treeWidget);

	//获取名字、签名、头像
	QString strName, strSign, strPicture;
	string strSql = "SELECT employee_name,employee_sign,picture FROM tab_employees WHERE employeeID = " + to_string(employeeID);
	MYSQL_RES* res = DBconn::getInstance()->myQuery(strSql);
	MYSQL_ROW row = mysql_fetch_row(res);
	strName = QString(row[0]);
	strSign = QString(row[1]);
	strPicture = QString(row[2]);

	QPixmap mask(":/Resources/MainWindow/head_mask.png");
	QPixmap image(strPicture);
	pContactItem->setHeadPixmap(CommonUtils::getRoundImage(image, mask, pContactItem->getHeadLabelSize()));
	pContactItem->setUserName(strName);
	pContactItem->setSignName(strSign);

	pRootGroupItem->addChild(pChild);
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	QString str = pContactItem->getUserName();
	m_groupPeopleMap.insert(pChild, str);
}