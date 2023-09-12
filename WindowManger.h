#pragma once

#include <QObject>
#include"TalkWindowShell.h"
class WindowManger  : public QObject
{
	Q_OBJECT

public:
	WindowManger();
	~WindowManger();
public:
	QWidget* findWindowName(const QString& qWindowName);
	void deleteWindowName(const QString& qWindowName);
	void addWindowName(const QString& qWindowName, QWidget* aWidget);

	static WindowManger* getInstance();
	void addNewTalkWindow(const QString& uid, GroupType groupType = COMPANY, const QString& people = "");
private:
	TalkWindowShell* m_talkWindowShell;
	QMap<QString, QWidget*>m_windowMap;
};
