#include "UserLogin.h"

UserLogin::UserLogin(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("UserLogin");
	initControl();
}

UserLogin::~UserLogin()
{}

void UserLogin::onLoginBtnClicked()
{
	if (!veryfyAccountCode()) {
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("您输入的账号或密码有误，请重新输入！"));
		ui.editPassword->setText("");
		ui.editUserAccount->setText("");
		return;
	}
	close();
	CCMainWindow *mainwindow = new CCMainWindow(nullptr,m_gLoginEmployeeID);
	mainwindow->show();
}

void UserLogin::onButtonCloseClicked()
{
	QApplication::quit();
}

void UserLogin::initControl()
{
	QLabel* headlabel = new QLabel(this);
	headlabel->setFixedSize(68, 68);
	QPixmap pix(":/Resources/MainWindow/head_mask.png");
	headlabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/app/logo.ico"), pix, headlabel->size()));
	headlabel->move(width() / 2 - 34, ui.titleWidget->height()-34);
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);  
	if(!connectMySql()){
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("连接数据库失败"));
		close();
	}
}

bool UserLogin::connectMySql()
{
	return  DBconn::getInstance()->getConnection("localhost", "root", "123456", "qtqq");
}

bool UserLogin::veryfyAccountCode()
{
	QString strAccountInput = ui.editUserAccount->text();
	QString strCodeInput = ui.editPassword->text();

	string strSqlCode="SELECT code FROM tab_accounts WHERE employeeID ="+ strAccountInput.toStdString();

	DBconn::getInstance()->myQuery("ALTER TABLE tab_department CONVERT TO CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci;");
	MYSQL_RES* res = DBconn::getInstance()->myQuery(strSqlCode);
	MYSQL_ROW row;
	if (res&& mysql_num_rows(res)) {
		row = mysql_fetch_row(res);
		QString strCode = QString(row[0]);
		if (strCode == strCodeInput) {
			m_gLoginEmployeeID = strAccountInput;
			return true;
		}
		else {
			return false;
		}
	}
	//账号登录
	string temp = "'" + strAccountInput.toStdString() + "'";
	string strSqlAccount = "SELECT code,employeeID FROM tab_accounts WHERE account =" + temp;
	res = DBconn::getInstance()->myQuery(strSqlAccount);
	if (res && mysql_num_rows(res)) {
		row = mysql_fetch_row(res);
		QString strCode = QString(row[0]);
		if (strCode == strCodeInput) {
			m_gLoginEmployeeID = QString(row[1]);
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}
