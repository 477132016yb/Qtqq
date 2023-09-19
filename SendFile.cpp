#include "SendFile.h"
#include"TalkWindow.h"
#include"WindowManger.h"
#include<QFileDialog>
#include<QMessageBox>

SendFile::SendFile(QWidget *parent)
	: BasicWindow(parent)
	,m_filePath("")
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("SendFile");
	this->move(100, 400);

	TalkWindowShell* talkWindowShell = WindowManger::getInstance()->getTalkWindowShell();
	connect(this, &SendFile::sendFileClicked, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);
}

SendFile::~SendFile()
{}

void SendFile::on_openBtn_clicked()
{
	m_filePath = QFileDialog::getOpenFileName(
		this,
		QString::fromLocal8Bit("ѡ���ļ�"),
		"/",
		QString::fromLocal8Bit("���͵��ļ�(*.txt *.doc);;�����ļ�(*.*);;")
	);
	ui.lineEdit->setText(m_filePath);
}

void SendFile::on_sendBtn_clicked()
{
	if (!m_filePath.isEmpty()) {
		QFile file(m_filePath);
		if (file.open(QIODevice::ReadOnly)) {
			QString str = file.readAll();
			int msgType = 2;

			//�ļ�����
			QFileInfo fileinfo(m_filePath);
			QString fileName = fileinfo.fileName();

			emit sendFileClicked(str,msgType,fileName);
			file.close();
		}
		else {
			QMessageBox::information(this,QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("��ȡ�ļ�:%1ʧ�ܣ�").arg(m_filePath));
		}
		this->close();
		return;
	}
}
