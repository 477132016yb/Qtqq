#include "QMsgTextEdit.h"

QMsgTextEdit::QMsgTextEdit(QWidget *parent)
	: QTextEdit(parent)
{}

QMsgTextEdit::~QMsgTextEdit()
{
	deletAllEmotionImage();
}

void QMsgTextEdit::addEmotionUrl(int emotionNum)
{
	const QString& imageName = QString("qrc:/Resources/MainWindow/emotion/%1.png").arg(emotionNum);
	const QString& flagName = QString("%1").arg(imageName);
	insertHtml(QString("<img src='%1' />").arg(flagName));
	if (m_listEmotionUrl.contains(imageName)) {
		return;
	}
	else {
		m_listEmotionUrl.append(imageName);
	}

	QMovie* apngMovie = new QMovie(imageName, "apng", this);
	m_emotionMap.insert(apngMovie, flagName);
	//数据帧改变时发射信号
	connect(apngMovie, SIGNAL(frameChanged(int)), this, SLOT(onEmotionImageFrameChange(int)));
	apngMovie->start();
	updateGeometry();
}

void QMsgTextEdit::deletAllEmotionImage()
{
	for (auto it = m_emotionMap.constBegin(); it != m_emotionMap.constEnd(); it++) {
		delete it.key();
	}
	m_emotionMap.clear();
}

void QMsgTextEdit::onEmotionImageFrameChange(int frame)
{
	QMovie* movie = qobject_cast<QMovie*>(sender());
	document()->addResource(QTextDocument::ImageResource,QUrl(m_emotionMap.value(movie)),movie->currentPixmap());
}
