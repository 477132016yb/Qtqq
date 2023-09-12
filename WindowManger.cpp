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
			talkWindow->setWindowName(QStringLiteral("yb�Ƽ�"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("��˾Ⱥ"));
			break;
		case PERSONELGROUP:
			talkWindow->setWindowName(QStringLiteral("��Һ�"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("����Ⱥ"));
			break;
		case DEVELOPMENTGROUP:
			talkWindow->setWindowName(QStringLiteral("ֻ�����ֱ�����ԣ�һ�������찤���һ����û����"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("�з�Ⱥ"));
			break;
		case MARKETGROUP:
			talkWindow->setWindowName(QStringLiteral("���칤����Ŭ��������Ŭ���ҹ���"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("�г�Ⱥ"));
			break;
		case PTOP:
			talkWindow->setWindowName(QStringLiteral("����Ϊ�ң���Ϊ����"));
			talkWindowItem->setMsgLabelContent(people);
			break;
		default:
			break;
		}
		m_talkWindowShell->addTalkWindow(talkWindow, talkWindowItem, groupType);
	}
	else {
		//��������б���Ϊѡ��
		QListWidgetItem* item = m_talkWindowShell->getTalkWindowItemMap().key(widget);
		item->setSelected(true);
		//���õ�ǰ�Ҳ����촰��
		m_talkWindowShell->setCurrentWidget(widget);
	}
	m_talkWindowShell->show();
	m_talkWindowShell->activateWindow();
}