#include "TalkWindowShell.h"
#include"EmotionWindow.h"
#include"TalkWindow.h"
#include"TalkWindowItem.h"

TalkWindowShell::TalkWindowShell(QWidget* parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();
	initTcpSocket();

	QFile file("/Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (!file.size()) {
		QStringList emplyeesList;
		getEmployeesID(emplyeesList);
		if (!createJSFile(emplyeesList)) {
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("更新js文件数据失败"));
		}
	}
}

TalkWindowShell::~TalkWindowShell()
{
	delete m_emotionWindow;
	m_emotionWindow = nullptr;
}

void TalkWindowShell::addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem, const QString& uid)
{
	ui.rightStackedWidget->addWidget(talkWindow);
	connect(m_emotionWindow, SIGNAL(signalEmotionWindowHide()), talkWindow, SLOT(onSetEmotionBtnStatus()));

	QListWidgetItem* aItem = new QListWidgetItem(ui.listWidget);
	m_talkWindowItemMap.insert(aItem, talkWindow);

	aItem->setSelected(true);

	//判断群聊还是单聊
	string strSql = "SELECT picture FROM tab_department WHERE departmentID = " + uid.toStdString();
	MYSQL_RES* res = DBconn::getInstance()->myQuery(strSql);
	MYSQL_ROW row;
	QString imgPath;
	if (mysql_num_rows(res) == 0) {//单聊
		strSql = "SELECT picture FROM tab_employees WHERE employeeID = " + uid.toStdString();
		res = DBconn::getInstance()->myQuery(strSql);
	}
	row = mysql_fetch_row(res);
	imgPath = QString(row[0]);
	QImage img;
	img.load(imgPath);
	talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));//设置头像
	ui.listWidget->addItem(aItem);
	ui.listWidget->setItemWidget(aItem, talkWindowItem);

	onTalkWindowItemClicked(aItem);

	connect(talkWindowItem, &TalkWindowItem::signalCloseClicked, [talkWindowItem,talkWindow,aItem,this]() {
		m_talkWindowItemMap.remove(aItem);
		talkWindow->close();
		ui.listWidget->takeItem(ui.listWidget->row(aItem));
		delete talkWindowItem;

		ui.rightStackedWidget->removeWidget(talkWindow);
		if (ui.rightStackedWidget->count() < 1) {
			close();
		}
	});
}

void TalkWindowShell::setCurrentWidget(QWidget * widget)
{
	ui.rightStackedWidget->setCurrentWidget(widget);
}

const QMap<QListWidgetItem*, QWidget*>& TalkWindowShell::getTalkWindowItemMap() const
{
	return m_talkWindowItemMap;
}

void TalkWindowShell::initControl()
{
	loadStyleSheet("TalkWindow");
	setWindowTitle(QString::fromLocal8Bit("张扬-聊天窗口"));

	m_emotionWindow = new EmotionWindow;
	m_emotionWindow->hide();//隐藏表情窗口

	QList<int>leftWidgetSize;
	leftWidgetSize << 154 << width() - 154;
	ui.splitter->setSizes(leftWidgetSize);//分类器设置尺寸

	ui.listWidget->setStyle(new CustomProxyStyle(this));

	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onTalkWindowItemClicked);
	connect(m_emotionWindow, SIGNAL(signalEmotionItemClicked(int)), this, SLOT(onEmotionItemClicked(int)));
}

void TalkWindowShell::initTcpSocket()
{
	m_tcpClientSocket = new QTcpSocket(this);
	m_tcpClientSocket->connectToHost("127.0.0.1", gtcpPort);
}

void TalkWindowShell::getEmployeesID(QStringList& emplyeesList)
{
	string strSql = "SELECT employeeID FROM tab_employees WHERE status = 1";
	MYSQL_RES* res = DBconn::getInstance()->myQuery(strSql);
	MYSQL_ROW row;
	int employeesNum = mysql_num_rows(res);
	while ((row = mysql_fetch_row(res)) != NULL) {
		emplyeesList.append(QString(row[0]));
	}
}

bool TalkWindowShell::createJSFile(QStringList& emplyeesList)
{
	//读取txt文件数据
	QFile fileRead("Resources/MainWindow/MsgHtml/msgtmpl.txt");
	QString strFile;
	if (fileRead.open(QIODevice::ReadOnly)) {
		strFile = fileRead.readAll();
		fileRead.close();
	}
	else {
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("读取msgtmpl.txt失败"));
		return false;
	}
	//替换（extrnal0，appendHtml0用作自己发送信息）
	QFile fileWrite("Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		//更新空值
		QString strSourceInitNull = "var external = null;";
		//更新初始化
		QString strSourceInit = "external = channel.objects.external;";
		//更新newWebChannel
		QString strSourceNew = "new QWebChannel(qt.webChannelTransport,\
			function(channel) {\
			external0 = channel.objects.external0;\
			external = channel.objects.external;\
		}\
		); ";
		//更新追加recvhtml
		QString strSourceRecvHtml;
		QFile fileRecvHtml("Resources/MainWindow/MsgHtml/recvHtml.txt");
		if (fileRecvHtml.open(QIODevice::ReadOnly)) {
			strSourceRecvHtml = fileRecvHtml.readAll();
			fileRecvHtml.close();
		}
		else {
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("读取recvHtml.txt失败"));
			return false;
		}

		//保存替换后的脚本
		QString strReplaceInitNull;
		QString strReplaceInit;
		QString strReplaceNew;
		QString strReplaceRecvHtml;

		for (int i = 0; i < emplyeesList.length(); i++) {
			//编辑替换后的空值
			QString strInitNull = strSourceInitNull;
			strInitNull.replace("external", QString("external_%1").arg(emplyeesList[i]));
			strReplaceInitNull += strInitNull;
			strReplaceInitNull += "\n";

			//编辑替换后的初始值
			QString strInit = strSourceInit;
			strInit.replace("external", QString("external_%1").arg(emplyeesList[i]));
			strReplaceInit += strInit;
			strReplaceInit += "\n";

			//编辑替换后的newWebChannel
			QString strNew = strSourceNew;
			strNew.replace("external", QString("external_%1").arg(emplyeesList[i]));
			strReplaceNew += strNew;
			strReplaceNew += "\n";

			//编辑替换后的recvhtml
			QString strRecvHtml = strSourceRecvHtml;
			strRecvHtml.replace("external", QString("external_%1").arg(emplyeesList[i]));
			strRecvHtml.replace("recvHtml", QString("recvHtml_%1").arg(emplyeesList[i]));
			strReplaceRecvHtml += strRecvHtml;
			strReplaceRecvHtml += "\n";
		}

		strFile.replace(strSourceInitNull, strReplaceInitNull);
		strFile.replace(strSourceInit, strReplaceInit);
		strFile.replace(strSourceNew, strReplaceNew);
		strFile.replace(strSourceRecvHtml, strReplaceRecvHtml);

		QTextStream stream(&fileWrite);
		stream << strFile;
		fileWrite.close();

		return true;
	}
	else {
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("写msgtempl.js失败"));
		return false;
	}
	return false;
}

void TalkWindowShell::updateSendTcpMsg(QString& strData, int& msgType, QString sFile)
{
}

void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem * item)
{
	QWidget* talkWindowWidget = m_talkWindowItemMap.find(item).value();
	ui.rightStackedWidget->setCurrentWidget(talkWindowWidget);
}

void TalkWindowShell::onEmotionItemClicked(int emotionNum)
{
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	if (curTalkWindow) {
		curTalkWindow->addEmotionImage(emotionNum);
	}
}

void TalkWindowShell::onEmotionBtnClicked(bool)
{
	m_emotionWindow->setVisible(!m_emotionWindow->isVisible());
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));//将当前控件的相对位置转化为屏幕的绝对位置

	emotionPoint.setX(emotionPoint.x() + 170);
	emotionPoint.setY(emotionPoint.y() + 220);
	m_emotionWindow->move(emotionPoint);
}
