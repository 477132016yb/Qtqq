#include"WindowManger.h"
#include"TalkWindow.h"
#include"TalkWindowItem.h"
Q_GLOBAL_STATIC(WindowManger,theInstance)
WindowManger::WindowManger()
	:QObject(nullptr)
	,m_talkWindowShell(nullptr)
{}

WindowManger::~WindowManger()
{}

QWidget* WindowManger::findWindowName(const QString& qWindowName)
{
	if (m_windowMap.contains(qWindowName)) {
		return m_windowMap.value(qWindowName);
	}
	return nullptr;
}

void WindowManger::deleteWindowName(const QString& qWindowName)
{
	m_windowMap.remove(qWindowName);
}

void WindowManger::addWindowName(const QString& qWindowName, QWidget* aWidget)
{
	if (!m_windowMap.contains(qWindowName)) {
		m_windowMap.insert(qWindowName, aWidget);
	}
}

WindowManger* WindowManger::getInstance()
{
	return theInstance;
}

void WindowManger::addNewTalkWindow(const QString& uid)
{
	if (m_talkWindowShell == nullptr) {
		m_talkWindowShell = new TalkWindowShell;
		connect(m_talkWindowShell, &TalkWindowShell::destroyed, [this](QObject* obj) {
			m_talkWindowShell = nullptr;
			});
	}
	QWidget* widget = findWindowName(uid);
	if (!widget) {
		TalkWindow* talkWindow = new TalkWindow(m_talkWindowShell, uid);
		TalkWindowItem* talkWindowItem = new TalkWindowItem(talkWindow);
		//判断群聊还是单聊
		string strSql = "SELECT department_name,sign FROM tab_department WHERE departmentID = "+uid.toStdString();
		MYSQL_RES* res = DBconn::getInstance()->myQuery(strSql);
		MYSQL_ROW row;
		QString strWindowName, strMsgLabel;
		if (mysql_num_rows(res)==0) {//单聊
			strSql = "SELECT employee_name,employee_sign FROM tab_employees WHERE employeeID = " + uid.toStdString();
			res = DBconn::getInstance()->myQuery(strSql);
		}
		row = mysql_fetch_row(res);
		strWindowName = QString(row[1]);
		strMsgLabel = QString(row[0]);
		talkWindow->setWindowName(strWindowName);
		talkWindowItem->setMsgLabelContent(strMsgLabel);
		m_talkWindowShell->addTalkWindow(talkWindow, talkWindowItem, uid);
	}
	else {
		//左侧聊天列表设为选中
		QListWidgetItem* item = m_talkWindowShell->getTalkWindowItemMap().key(widget);
		item->setSelected(true);
		//设置当前右侧聊天窗口
		m_talkWindowShell->setCurrentWidget(widget);
	}
	m_talkWindowShell->show();
	m_talkWindowShell->activateWindow();
}