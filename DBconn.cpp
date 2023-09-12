#include "DBconn.h"

DBconn::DBconn()
{
	closed = false;
}

DBconn::~DBconn()
{
	if (!closed)
	{
		try
		{
			mysql_close(&mysql);
		}
		catch (exception&e)
		{
			qDebug() << e.what() << endl;
		}
	}
}

bool DBconn::getConnection(string host, string userName, string passWord, string dbName)
{
	mysql_init(&mysql);// ��ʼ��һ��MYSQL ���ӵ�ʵ������
//����MySql
	if (!mysql_real_connect(&mysql, host.c_str(), userName.c_str(), passWord.c_str(), dbName.c_str(), 3306, 0, 0))
	{
		qDebug() << "Error connecting to database:" << mysql_error(&this->mysql) << endl;
		return false;
	}
	//����win7��win10��������������룬�޸����ݿ���뷽ʽ�������
	//mysql_query(&mysql, "SET NAMES GB2312");//�޸�mysql���뷽ʽ
	//mysql_query(&mysql, "SET NAMES UTF8");
	return true;
}

bool DBconn::select(string op)
{
	if (mysql_query(&mysql, op.c_str()))
	{
		qDebug() << "Query failed :" << mysql_error(&this->mysql) << endl;
		return false;
	}
	MYSQL_ROW row; //char** ��ά���飬���һ������¼
	MYSQL_RES* res = mysql_store_result(&mysql);//���ؽ����
	if (!res)
	{
		qDebug() << "Couldn't get result from" << mysql_error(&this->mysql) << endl;
		return false;
	}
	int count1 = mysql_num_rows(res);//��ȡ��¼����(����Ϊ���������,��select����)
	int count2 = mysql_num_fields(res);//�����ֶθ���(�������)
	int k = 1;
	qDebug() << "����" << count1 << "����¼" << endl;
	QVector<string>str_field;               //����һ���ַ�������洢�ֶ���Ϣ  
	for (int i = 0; i < count2; i++)            //����֪�ֶ�����������»�ȡ�ֶ���  
	{
		str_field.push_back(mysql_fetch_field(res)->name);
		//qDebug() << "\t" << str_field[i];
	}
	qDebug() << endl;
	while ((row = mysql_fetch_row(res)) != NULL) {//��ȡ�����(res)�е�һ��,����ָ��ָ����һ��
		qDebug() << "��" << k << "�У�";
		for (int i = 0; i < count2; ++i)
		{
			//qDebug() << string(row[i]) << "\t";    //����ֶ�ֵ
		}
		qDebug() << endl;
		++k;
	}
	return true;
}

bool DBconn::Query(string op)
{
	int count = 0;
	if (mysql_query(&mysql, op.c_str()))
	{
		qDebug() << "Query failed :" << mysql_error(&this->mysql) << endl;
		return false;
	}
	count = mysql_affected_rows(&mysql);//����Ϊmysql�ṹ��(���ز���Ӱ������)
	qDebug() << "���£�" << count << " ����¼" << endl;
	return true;
}

void DBconn::close()
{
	mysql_close(&mysql);
	closed = true;
}

MYSQL_RES* DBconn::myQuery(string op)
{
	if (mysql_query(&mysql, op.c_str()))
	{
		qDebug() << "Query failed :" << mysql_error(&this->mysql) << endl;
		return NULL;
	}
	return mysql_store_result(&mysql);
}
