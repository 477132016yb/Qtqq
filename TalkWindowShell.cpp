#pragma once
#include"TalkWindowShell.h"
#include"EmotionWindow.h"
#include"TalkWindow.h"
#include"TalkWindowItem.h"
#include"ReceiveFile.h"

QString gfileName;//�ļ�����
QString gfileData;//�ļ�����

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
			QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("����js�ļ�����ʧ��"));
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

	//�ж�Ⱥ�Ļ��ǵ���
	string strSql = "SELECT picture FROM tab_department WHERE departmentID = " + uid.toStdString();
	MYSQL_RES* res = DBconn::getInstance()->myQuery(strSql);
	MYSQL_ROW row;
	QString imgPath;
	if (mysql_num_rows(res) == 0) {//����
		strSql = "SELECT picture FROM tab_employees WHERE employeeID = " + uid.toStdString();
		res = DBconn::getInstance()->myQuery(strSql);
	}
	row = mysql_fetch_row(res);
	imgPath = QString(row[0]);
	QImage img;
	img.load(imgPath);
	talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));//����ͷ��
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
	setWindowTitle(QString::fromLocal8Bit("���촰��"));

	m_emotionWindow = new EmotionWindow;
	m_emotionWindow->hide();//���ر��鴰��

	QList<int>leftWidgetSize;
	leftWidgetSize << 154 << width() - 154;
	ui.splitter->setSizes(leftWidgetSize);//���������óߴ�

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
	//��ȡtxt�ļ�����
	QFile fileRead("Resources/MainWindow/MsgHtml/msgtmpl.txt");
	QString strFile;
	if (fileRead.open(QIODevice::ReadOnly)) {
		strFile = fileRead.readAll();
		fileRead.close();
	}
	else {
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("��ȡmsgtmpl.txtʧ��"));
		fileRead.close();
		return false;
	}
	//�滻��extrnal0��appendHtml0�����Լ�������Ϣ��
	QFile fileWrite("Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		//���¿�ֵ
		QString strSourceInitNull = "var external = null;";
		//���³�ʼ��
		QString strSourceInit = "external = channel.objects.external;";
		//����newWebChannel
		QString strSourceNew = "new QWebChannel(qt.webChannelTransport,\
			function(channel) {\
			external0 = channel.objects.external0;\
			external = channel.objects.external;\
		}\
		); ";
		//����׷��recvhtml
		QString strSourceRecvHtml;
		QFile fileRecvHtml("Resources/MainWindow/MsgHtml/recvHtml.txt");
		if (fileRecvHtml.open(QIODevice::ReadOnly)) {
			strSourceRecvHtml = fileRecvHtml.readAll();
			fileRecvHtml.close();
		}
		else {
			QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("��ȡrecvHtml.txtʧ��"));
			fileRecvHtml.close();
			return false;
		}

		//�����滻��Ľű�
		QString strReplaceInitNull;
		QString strReplaceInit;
		QString strReplaceNew;
		QString strReplaceRecvHtml;

		for (int i = 0; i < emplyeesList.length(); i++) {
			//�༭�滻��Ŀ�ֵ
			QString strInitNull = strSourceInitNull;
			strInitNull.replace("external", QString("external_%1").arg(emplyeesList[i]));
			strReplaceInitNull += strInitNull;
			strReplaceInitNull += "\n";

			//�༭�滻��ĳ�ʼֵ
			QString strInit = strSourceInit;
			strInit.replace("external", QString("external_%1").arg(emplyeesList[i]));
			strReplaceInit += strInit;
			strReplaceInit += "\n";

			//�༭�滻���newWebChannel
			QString strNew = strSourceNew;
			strNew.replace("external", QString("external_%1").arg(emplyeesList[i]));
			strReplaceNew += strNew;
			strReplaceNew += "\n";

			//�༭�滻���recvhtml
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
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("дmsgtempl.jsʧ��"));
		fileWrite.close();
		return false;
	}
	return false;
}

void TalkWindowShell::processPendingData()
{
	/*
	���ݱ��ʽ
	�ı����ݰ���Ⱥ�ı�־+������ϢԱ��qq��+������ϢԱ��QQ�ţ�Ⱥ�ţ�+��Ϣ���ͣ�1��+���ݳ���+����
	�������ݰ���Ⱥ�ı�־+������ϢԱ��qq��+������ϢԱ��QQ�ţ�Ⱥ�ţ�+��Ϣ���ͣ�0��+�������+images+��������
	�ļ����ݰ���Ⱥ�ı�־+������ϢԱ��qq��+������ϢԱ��QQ�ţ�Ⱥ�ţ�+��Ϣ���ͣ�2��+�ļ��ֽ���+bytes+�ļ���+data_begin+�ļ�����

	Ⱥ�ı�־ռһλ 0���� 1Ⱥ��
	��Ϣ����ռһλ 0���� 1�ı� 2�ļ�
	QQ��ռ5λ��Ⱥ��ռ4λ�����ݳ���ռ5λ����������ռ3λ

	Ⱥ���ı���Ϣ�� 1100012001100005Hello ��ʾQQ10001��Ⱥ2001�����ı�Hello
	����ͼƬ��Ϣ�� 0100011000201images060 ��ʾQQ10001��QQ10002����1������60.png
	Ⱥ���ļ���Ϣ�� 1100052000210bytestext.txtdata_beginhelloworld ��ʾQQ10005��Ⱥ2000�����ļ���Ϣ���ļ���text.txt���ļ�����10������helloworld
	*/

	//�˿�����δ���������
	while (m_udpReceiver->hasPendingDatagrams()) {
		const static int groupFlgWidth = 1;//Ⱥ�ı��¿��
		const static int groupWidth = 4;//ȺQQ�ſ��
		const static int employeeWidth = 5;//Ա��QQ�ſ��
		const static int msgTypeWidth = 1;//��Ϣ���Ϳ��
		const static int msgLengthWidth = 5;//�ı���Ϣ���ȵĿ��
		const static int pictureWidth = 3;//����ͼƬ�Ŀ��

		//��ȡudp����
		QByteArray btData;
		btData.resize(m_udpReceiver->pendingDatagramSize());
		m_udpReceiver->readDatagram(btData.data(),btData.size());

		QString strData = btData.data();
		QString strWindowID;//���촰��id��Ⱥ������Ⱥ�ţ���������Ա��QQ��
		QString strSenderEmployeeID, strRecevieEmployeeID;//���ͼ����ն˵�QQ��
		QString strMsg;//����

		int msgLen;//���ݳ���
		int msgType;//��������

		strSenderEmployeeID = strData.mid(groupFlgWidth, employeeWidth);

		//�Լ�������Ϣ��������
		if (strSenderEmployeeID == gLoginID) {
			return;
		}

		if (btData[0] == '1') {//Ⱥ��
			//ȺQQ��
			strWindowID = strData.mid(groupFlgWidth + employeeWidth, groupWidth);

			QChar cmsgType = btData[groupFlgWidth + employeeWidth + groupWidth];
			if ( cmsgType == '1') {//�ı���Ϣ
				msgType = 1;
				msgLen = strData.mid(groupFlgWidth + employeeWidth + groupWidth + msgTypeWidth, msgLengthWidth).toInt();
				strMsg = strData.mid(groupFlgWidth + employeeWidth + groupWidth + msgTypeWidth + msgLengthWidth, msgLen);
			}
			else if (cmsgType == '0') {//������Ϣ
				msgType = 0;
				int posImages = strData.indexOf("images");
				int imagesWidth = QString("images").length();
				strMsg = strData.mid(posImages + imagesWidth);
			}
			else if (cmsgType == '2') {//�ļ���Ϣ
				msgType = 2;
				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int posData_begin = strData.indexOf("data_begin");

				//�ļ�����
				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - posBytes - bytesWidth);
				gfileName = fileName;

				//�ļ�����
				int posData = posData_begin + QString("data_begin").length();
				strMsg = strData.mid(posData);
				gfileData = strMsg;

				//����employeeID��ȡ����������
				QString sender;
				int employeeID = strSenderEmployeeID.toInt();

				QString sql = QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1").arg(employeeID);
				MYSQL_RES* res = DBconn::getInstance()->myQuery(sql.toStdString());
				MYSQL_ROW row = mysql_fetch_row(res);
				sender = QString(row[0]);

				//�����ļ���������
				ReceiveFile* receiveFile = new ReceiveFile(this);
				connect(receiveFile, &ReceiveFile::refuseFile, [this]() {
					return;
				});
				QString msgLabel = QString::fromLocal8Bit("�յ�����%1���͵��ļ����Ƿ���գ�").arg(sender);
				receiveFile->setMsg(msgLabel);
				receiveFile->show();
			}
		}
		else {//����
			//������QQ��
			strRecevieEmployeeID = strData.mid(groupFlgWidth + employeeWidth, employeeWidth);
			strWindowID = strSenderEmployeeID;

			//���Ƿ����ҵ���Ϣ��������
			if (strRecevieEmployeeID != gLoginID) {
				return;
			}

			QChar cmsgType = btData[groupFlgWidth + employeeWidth + employeeWidth];
			if (cmsgType == '1') {//�ı���Ϣ
				msgType = 1;
				msgLen = strData.mid(groupFlgWidth + employeeWidth + employeeWidth + msgTypeWidth, msgLengthWidth).toInt();
				strMsg = strData.mid(groupFlgWidth + employeeWidth + employeeWidth + msgTypeWidth + msgLengthWidth, msgLen);
			}
			else if (cmsgType == '0') {//������Ϣ
				msgType = 0;
				int posImages = strData.indexOf("images");
				int imagesWidth = QString("images").length();
				strMsg = strData.mid(posImages+imagesWidth);
			}
			else if (cmsgType == '2') {//�ļ���Ϣ
				msgType = 2;
				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int posData_begin = strData.indexOf("data_begin");

				//�ļ�����
				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - posBytes - bytesWidth);
				gfileName = fileName;

				//�ļ�����
				int posData = posData_begin + QString("data_begin").length();
				strMsg = strData.mid(posData);
				gfileData = strMsg;

				//����employeeID��ȡ����������
				QString sender;
				int employeeID = strSenderEmployeeID.toInt();

				QString sql = QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1").arg(employeeID);
				MYSQL_RES* res = DBconn::getInstance()->myQuery(sql.toStdString());
				MYSQL_ROW row = mysql_fetch_row(res);
				sender = QString(row[0]);

				//�����ļ���������
				ReceiveFile* receiveFile = new ReceiveFile(this);
				connect(receiveFile, &ReceiveFile::refuseFile, [this]() {
					return;
				});
				QString msgLabel = QString::fromLocal8Bit("�յ�����%1���͵��ļ����Ƿ���գ�").arg(sender);
				receiveFile->setMsg(msgLabel);
				receiveFile->show();
			}
		}

		//�����촰������Ϊ�����
		QWidget* widget = WindowManger::getInstance()->findWindowName(strWindowID);
		if (widget) {//���촰���Ѵ�
			this->setCurrentWidget(widget);

			//ͬ������������촰��
			QListWidgetItem* item = m_talkWindowItemMap.key(widget);
			item->setSelected(true);
		}
		else {
			return;
		}
		int sendID = strSenderEmployeeID.toInt();
		//�ļ���Ϣ��������
		if (msgType != 2) {
			handleReceivedMsg(sendID, msgType, strMsg);
		}
	}
}

void TalkWindowShell::handleReceivedMsg(int senderEmployeeID, int msgType, QString strMsg)
{
	QMsgTextEdit msgTextEdit;
	msgTextEdit.setText(strMsg);

	if (msgType == 1) {//�ı���Ϣ
		msgTextEdit.document()->toHtml();
	}
	else if (msgType == 0) {//������Ϣ
		const int emotionWidth = 3;
		int emotionNum = strMsg.length() / emotionWidth;
		for (int i = 0; i < emotionNum; i++) {
			msgTextEdit.addEmotionUrl(strMsg.mid(i * emotionWidth, emotionWidth).toInt());
		}
	}

	QString html = msgTextEdit.document()->toHtml();

	//�ı�html���û���������������
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
	//0���� 1�ı� 2�ļ�

	//��ȡ��ǰ����촰��
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	QString talkId = curTalkWindow->getTalkId();

	QString strGroupFlag;
	QString strSend;
	if (talkId.length() == 4) {//Ⱥ�ų���
		strGroupFlag = "1";
	}
	else {
		strGroupFlag = "0";
	}

	int nstrDataLength = strData.length();
	int dataLength = QString::number(nstrDataLength).length();
	QString strdataLength;
	if (msgType == 1) {//�����ı���Ϣ
		//�ı���Ϣ�ĳ���Լ��Ϊ5
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
			QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("����������ݳ���"));
		}

		//�ı����ݰ���ʽ��Ⱥ�ı�־+����ϢԱ��qq��+����ϢԱ��qq�ţ�Ⱥ�ţ�+������Ϣ����+���ݳ���+����
		strSend = strGroupFlag + gLoginID + talkId + "1" + strdataLength + strData;
	}
	else if (msgType == 0) {//���ͱ�����Ϣ
		//�������ݰ���ʽ��Ⱥ�ı�־+����ϢԱ��qq��+����ϢԱ��qq�ţ�Ⱥ�ţ�+������Ϣ����+�������+images+����
		strSend = strGroupFlag + gLoginID + talkId + "0" + strData;
	}
	else if (msgType == 2) {//�����ļ���Ϣ
		//�������ݰ���ʽ��Ⱥ�ı�־+����ϢԱ��qq��+����ϢԱ��qq�ţ�Ⱥ�ţ�+������Ϣ����+�ļ��ĳ���+"bytes"+�ļ�����+"data_begin"+�ļ�����
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
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));//����ǰ�ؼ������λ��ת��Ϊ��Ļ�ľ���λ��

	emotionPoint.setX(emotionPoint.x() + 170);
	emotionPoint.setY(emotionPoint.y() + 220);
	m_emotionWindow->move(emotionPoint);
}
