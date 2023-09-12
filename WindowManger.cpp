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

void WindowManger::addNewTalkWindow(const QString& uid, GroupType groupType, const QString& people)
{
	if (m_talkWindowShell==nullptr) {
		m_talkWindowShell = new TalkWindowShell;
		connect(m_talkWindowShell, &TalkWindowShell::destroyed, [this](QObject* obj) {
			m_talkWindowShell = nullptr;
		});
	}
	QWidget* widget = findWindowName(uid);
	if (!widget) {
		TalkWindow* talkWindow = new TalkWindow(m_talkWindowShell, uid, groupType);
		TalkWindowItem* talkWindowItem = new TalkWindowItem(talkWindow);
		switch (groupType)
		{
		case COMPANY:
			talkWindow->setWindowName(QStringLiteral("yb科技"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("公司群"));
			break;
		case PERSONELGROUP:
			talkWindow->setWindowName(QStringLiteral("大家好"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("人事群"));
			break;
		case DEVELOPMENTGROUP:
			talkWindow->setWindowName(QStringLiteral("只有两种编程语言：一种是天天挨骂，另一种是没人用"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("研发群"));
			break;
		case MARKETGROUP:
			talkWindow->setWindowName(QStringLiteral("今天工作不努力，明天努力找工作"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("市场群"));
			break;
		case PTOP:
			talkWindow->setWindowName(QStringLiteral("人人为我，我为人人"));
			talkWindowItem->setMsgLabelContent(people);
			break;
		default:
			break;
		}
		m_talkWindowShell->addTalkWindow(talkWindow, talkWindowItem, groupType);
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