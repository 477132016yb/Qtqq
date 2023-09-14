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
			QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("����js�ļ�����ʧ��"));
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
	setWindowTitle(QString::fromLocal8Bit("����-���촰��"));

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
		return false;
	}
	return false;
}

void TalkWindowShell::updateSendTcpMsg(QString& strData, int& msgType, QString fileName)
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

	int dataLength = strData.length();
	const int sourceDataLength = dataLength;
	QString strdataLength;
	if (msgType == 1) {//�����ı���Ϣ
		//�ı���Ϣ�ĳ���Լ��Ϊ5
		if (dataLength == 1) {
			strdataLength = "0000" + QString::number(sourceDataLength);
		}
		else if (dataLength == 2) {
			strdataLength = "000" + QString::number(sourceDataLength);
		}
		else if (dataLength == 3) {
			strdataLength = "00" + QString::number(sourceDataLength);
		}
		else if (dataLength == 4) {
			strdataLength = "0" + QString::number(sourceDataLength);
		}
		else if (dataLength == 5) {
			strdataLength = QString::number(sourceDataLength);
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
