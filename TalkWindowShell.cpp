#pragma once
#include"TalkWindowShell.h"
#include"EmotionWindow.h"
#include"TalkWindow.h"
#include"TalkWindowItem.h"
#include"ReceiveFile.h"

QString gfileName;//文件名称
QString gfileData;//文件内容

TalkWindowShell::TalkWindowShell(QWidget* parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();
	initTcpSocket();
	initUdpSocket();

	QFile file("/Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (!file.size()) {
		QStringList emplyeesList;
		getEmployeesID(emplyeesList);
		if (!createJSFile(emplyeesList)) {
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("更新js文件数据失败"));
			file.close();
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
	setWindowTitle(QString::fromLocal8Bit("聊天窗口"));

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

void TalkWindowShell::initUdpSocket()
{
	m_udpReceiver = new QUdpSocket(this);
	for (quint16 port = udpPort; port < udpPort + 20; port++) {
		if (m_udpReceiver->bind(QHostAddress::AnyIPv4,port)) {
			connect(m_udpReceiver, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
			break;
		}
	}
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
		fileRead.close();
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
			fileRecvHtml.close();
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
		fileWrite.close();
		return false;
	}
	return false;
}

void TalkWindowShell::processPendingData()
{
	/*
	数据表格式
	文本数据包：群聊标志+发送信息员工qq号+接收信息员工QQ号（群号）+信息类型（1）+数据长度+数据
	表情数据包：群聊标志+发送信息员工qq号+接收信息员工QQ号（群号）+信息类型（0）+表情个数+images+表情名称
	文件数据包：群聊标志+发送信息员工qq号+接收信息员工QQ号（群号）+信息类型（2）+文件字节数+bytes+文件名+data_begin+文件数据

	群聊标志占一位 0单聊 1群聊
	信息类型占一位 0表情 1文本 2文件
	QQ号占5位，群号占4位，数据长度占5位，表情名称占3位

	群聊文本信息如 1100012001100005Hello 表示QQ10001向群2001发送文本Hello
	单聊图片信息如 0100011000201images060 表示QQ10001向QQ10002发送1个表情60.png
	群聊文件信息如 1100052000210bytestext.txtdata_beginhelloworld 表示QQ10005向群2000发送文件信息，文件是text.txt，文件长度10，内容helloworld
	*/

	//端口中有未处理的数据
	while (m_udpReceiver->hasPendingDatagrams()) {
		const static int groupFlgWidth = 1;//群聊标致宽度
		const static int groupWidth = 4;//群QQ号宽度
		const static int employeeWidth = 5;//员工QQ号宽度
		const static int msgTypeWidth = 1;//信息类型宽度
		const static int msgLengthWidth = 5;//文本信息长度的宽度
		const static int pictureWidth = 3;//表情图片的宽度

		//读取udp数据
		QByteArray btData;
		btData.resize(m_udpReceiver->pendingDatagramSize());
		m_udpReceiver->readDatagram(btData.data(),btData.size());

		QString strData = btData.data();
		QString strWindowID;//聊天窗口id，群聊则是群号，单聊则是员工QQ号
		QString strSenderEmployeeID, strRecevieEmployeeID;//发送及接收端的QQ号
		QString strMsg;//数据

		int msgLen;//数据长度
		int msgType;//数据类型

		strSenderEmployeeID = strData.mid(groupFlgWidth, employeeWidth);

		//自己发的信息不做处理
		if (strSenderEmployeeID == gLoginID) {
			return;
		}

		if (btData[0] == '1') {//群聊
			//群QQ号
			strWindowID = strData.mid(groupFlgWidth + employeeWidth, groupWidth);

			QChar cmsgType = btData[groupFlgWidth + employeeWidth + groupWidth];
			if ( cmsgType == '1') {//文本信息
				msgType = 1;
				msgLen = strData.mid(groupFlgWidth + employeeWidth + groupWidth + msgTypeWidth, msgLengthWidth).toInt();
				strMsg = strData.mid(groupFlgWidth + employeeWidth + groupWidth + msgTypeWidth + msgLengthWidth, msgLen);
			}
			else if (cmsgType == '0') {//表情信息
				msgType = 0;
				int posImages = strData.indexOf("images");
				int imagesWidth = QString("images").length();
				strMsg = strData.mid(posImages + imagesWidth);
			}
			else if (cmsgType == '2') {//文件信息
				msgType = 2;
				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int posData_begin = strData.indexOf("data_begin");

				//文件名称
				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - posBytes - bytesWidth);
				gfileName = fileName;

				//文件内容
				int posData = posData_begin + QString("data_begin").length();
				strMsg = strData.mid(posData);
				gfileData = strMsg;

				//根据employeeID获取发送者姓名
				QString sender;
				int employeeID = strSenderEmployeeID.toInt();

				QString sql = QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1").arg(employeeID);
				MYSQL_RES* res = DBconn::getInstance()->myQuery(sql.toStdString());
				MYSQL_ROW row = mysql_fetch_row(res);
				sender = QString(row[0]);

				//接收文件后续操作
				ReceiveFile* receiveFile = new ReceiveFile(this);
				connect(receiveFile, &ReceiveFile::refuseFile, [this]() {
					return;
				});
				QString msgLabel = QString::fromLocal8Bit("收到来自%1发送的文件，是否接收？").arg(sender);
				receiveFile->setMsg(msgLabel);
				receiveFile->show();
			}
		}
		else {//单聊
			//接收者QQ号
			strRecevieEmployeeID = strData.mid(groupFlgWidth + employeeWidth, employeeWidth);
			strWindowID = strSenderEmployeeID;

			//不是发给我的信息不做处理
			if (strRecevieEmployeeID != gLoginID) {
				return;
			}

			QChar cmsgType = btData[groupFlgWidth + employeeWidth + employeeWidth];
			if (cmsgType == '1') {//文本信息
				msgType = 1;
				msgLen = strData.mid(groupFlgWidth + employeeWidth + employeeWidth + msgTypeWidth, msgLengthWidth).toInt();
				strMsg = strData.mid(groupFlgWidth + employeeWidth + employeeWidth + msgTypeWidth + msgLengthWidth, msgLen);
			}
			else if (cmsgType == '0') {//表情信息
				msgType = 0;
				int posImages = strData.indexOf("images");
				int imagesWidth = QString("images").length();
				strMsg = strData.mid(posImages+imagesWidth);
			}
			else if (cmsgType == '2') {//文件信息
				msgType = 2;
				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int posData_begin = strData.indexOf("data_begin");

				//文件名称
				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - posBytes - bytesWidth);
				gfileName = fileName;

				//文件内容
				int posData = posData_begin + QString("data_begin").length();
				strMsg = strData.mid(posData);
				gfileData = strMsg;

				//根据employeeID获取发送者姓名
				QString sender;
				int employeeID = strSenderEmployeeID.toInt();

				QString sql = QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1").arg(employeeID);
				MYSQL_RES* res = DBconn::getInstance()->myQuery(sql.toStdString());
				MYSQL_ROW row = mysql_fetch_row(res);
				sender = QString(row[0]);

				//接收文件后续操作
				ReceiveFile* receiveFile = new ReceiveFile(this);
				connect(receiveFile, &ReceiveFile::refuseFile, [this]() {
					return;
				});
				QString msgLabel = QString::fromLocal8Bit("收到来自%1发送的文件，是否接收？").arg(sender);
				receiveFile->setMsg(msgLabel);
				receiveFile->show();
			}
		}

		//将聊天窗口设置为活动窗口
		QWidget* widget = WindowManger::getInstance()->findWindowName(strWindowID);
		if (widget) {//聊天窗口已打开
			this->setCurrentWidget(widget);

			//同步激活左侧聊天窗口
			QListWidgetItem* item = m_talkWindowItemMap.key(widget);
			item->setSelected(true);
		}
		else {
			return;
		}
		int sendID = strSenderEmployeeID.toInt();
		//文件信息另做处理
		if (msgType != 2) {
			handleReceivedMsg(sendID, msgType, strMsg);
		}
	}
}

void TalkWindowShell::handleReceivedMsg(int senderEmployeeID, int msgType, QString strMsg)
{
	QMsgTextEdit msgTextEdit;
	msgTextEdit.setText(strMsg);

	if (msgType == 1) {//文本信息
		msgTextEdit.document()->toHtml();
	}
	else if (msgType == 0) {//表情信息
		const int emotionWidth = 3;
		int emotionNum = strMsg.length() / emotionWidth;
		for (int i = 0; i < emotionNum; i++) {
			msgTextEdit.addEmotionUrl(strMsg.mid(i * emotionWidth, emotionWidth).toInt());
		}
	}

	QString html = msgTextEdit.document()->toHtml();

	//文本html如果没有字体则添加字体
	if (!html.contains(".png") && !html.contains("</span>")) {
		QString fontHtml;
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly)) {
			fontHtml = file.readAll();
			fontHtml.replace("%1", strMsg);
			file.close();
		}
		else {
			QMessageBox::information(this, "Tips", "file not exist!");
			return;
		}
		if (!html.contains(fontHtml)) {
			html.replace(strMsg, fontHtml);
		}
	}

	TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	string s = to_string(senderEmployeeID);
	const QString str = QString::fromStdString(s);
	talkWindow->ui.msgWidget->appendMsg(html,str); 
}

void TalkWindowShell::updateSendTcpMsg(QString& strData, int msgType, QString fileName)
{
	//0表情 1文本 2文件

	//获取当前活动聊天窗口
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	QString talkId = curTalkWindow->getTalkId();

	QString strGroupFlag;
	QString strSend;
	if (talkId.length() == 4) {//群号长度
		strGroupFlag = "1";
	}
	else {
		strGroupFlag = "0";
	}

	int nstrDataLength = strData.length();
	int dataLength = QString::number(nstrDataLength).length();
	QString strdataLength;
	if (msgType == 1) {//发送文本信息
		//文本信息的长度约定为5
		if (dataLength == 1) {
			strdataLength = "0000" + QString::number(nstrDataLength);
		}
		else if (dataLength == 2) {
			strdataLength = "000" + QString::number(nstrDataLength);
		}
		else if (dataLength == 3) {
			strdataLength = "00" + QString::number(nstrDataLength);
		}
		else if (dataLength == 4) {
			strdataLength = "0" + QString::number(nstrDataLength);
		}
		else if (dataLength == 5) {
			strdataLength = QString::number(nstrDataLength);
		}
		else {
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("不合理的数据长度"));
		}

		//文本数据包格式：群聊标志+发信息员工qq号+收信息员工qq号（群号）+数据信息类型+数据长度+数据
		strSend = strGroupFlag + gLoginID + talkId + "1" + strdataLength + strData;
	}
	else if (msgType == 0) {//发送表情信息
		//表情数据包格式：群聊标志+发信息员工qq号+收信息员工qq号（群号）+数据信息类型+表情个数+images+数据
		strSend = strGroupFlag + gLoginID + talkId + "0" + strData;
	}
	else if (msgType == 2) {//发送文件信息
		//表情数据包格式：群聊标志+发信息员工qq号+收信息员工qq号（群号）+数据信息类型+文件的长度+"bytes"+文件名称+"data_begin"+文件内容
		QString strLength = QString::number(strData.toUtf8().length());
		strSend = strGroupFlag + gLoginID + talkId + "2" + strLength + "bytes" + fileName + "data_begin" + strData;
	}

	QByteArray dataBt;
	dataBt.resize(strSend.length());
	dataBt = strSend.toUtf8();
	m_tcpClientSocket->write(dataBt);
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
