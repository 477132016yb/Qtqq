#pragma once

#include<QTextEdit>
#include<QMovie>
#include<QUrl>

class QMsgTextEdit  : public QTextEdit
{
	Q_OBJECT

public:
	QMsgTextEdit(QWidget *parent);
	~QMsgTextEdit();
private slots:
	void onEmotionImageFrameChange(int frame);//�����ı�ʱ��Ӧ�Ĳۺ���
public:
	void addEmotionUrl(int emotionNum);
	void deletAllEmotionImage();
private:
	QList<QString>m_listEmotionUrl;
	QMap<QMovie*,QString>m_emotionMap;
};