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
	void onEmotionBtnClicked(bool);//表情按钮点击后
private slots:
	void onTalkWindowItemClicked(QListWidgetItem*item);//左侧列表
	void onEmotionItemClicked(int emotionNum);//表情被选中
private:
	Ui::TalkWindowShellClass ui;
	QMap<QListWidgetItem*, QWidget*>m_talkWindowItemMap;//打开的聊天窗口
	EmotionWindow* m_emotionWindow;//表情窗口*/
};
