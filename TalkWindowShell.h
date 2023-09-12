#pragma once
#include<QWidget>
#include"ui_TalkWindowShell.h"
#include"BasicWindow.h"
#include<QMap>
#include<QListWidget>
class TalkWindow;
class TalkWindowItem;
class QListWidgetItem;
class EmotionWindow;
class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = nullptr);
	~TalkWindowShell();
public:
	void addTalkWindow(TalkWindow* talkWindow,TalkWindowItem*talkWindowItem, GroupType);
	void setCurrentWidget(QWidget* widget);
	const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap() const;
private:
	void initControl();
public slots:
	void onEmotionBtnClicked(bool);//���鰴ť�����
private slots:
	void onTalkWindowItemClicked(QListWidgetItem*item);//����б�
	void onEmotionItemClicked(int emotionNum);//���鱻ѡ��
private:
	Ui::TalkWindowShellClass ui;
	QMap<QListWidgetItem*, QWidget*>m_talkWindowItemMap;//�򿪵����촰��
	EmotionWindow* m_emotionWindow;//���鴰��*/
};
