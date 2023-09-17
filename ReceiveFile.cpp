#include "ReceiveFile.h"
#include<QFileDialog>
#include<QMessageBox>

extern QString gfileName;
extern QString gfileData;

ReceiveFile::ReceiveFile(QWidget* parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("ReceiveFile");
	this->move(100, 400);

	connect(m_titleBar, &TitleBar::signalButtonCloseClicked, this, &ReceiveFile::refuseFile);
}

ReceiveFile::~ReceiveFile()
{}

void ReceiveFile::setMsg(QString& msgLabel)
{
	ui.label->setText(msgLabel);
}

void ReceiveFile::on_okBtn_clicked()
{
	this->close();

	//��ȡ��Ҫ������ļ�·��
	QString fileDirPath = QFileDialog::getExistingDirectory(
		nullptr,
		QString::fromLocal8Bit("�ļ�����·��"),
		"/"
	);
	QString filePath = fileDirPath + "/" + gfileName;

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�����ļ�ʧ�ܣ�"));
		file.close();
	}
	else {
		file.write(gfileData.toUtf8());
		file.close();
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�ļ����ճɹ���"));
	}
}

void ReceiveFile::on_cancelBtn_clicked()
{
	emit refuseFile();
	this->close();
}