
#include "mainWindows.h"
#include "qcustomplot.h"
#include <QtWidgets/QApplication>
#include <Python.h>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	mainWindows w;
	w.show();

	//�Ȼ��py�ļ����ڵ�·��������Ҫ����·��
	QString path = QDir::currentPath();
	path = path.replace("\\", "/");
	std::string str;
	str = path.toStdString();

	//std::vector<int> paras(128, 0);
	//for (int i = 0; i < paras.size(); ++i) {
	//	paras[i] = i;
	//}

	ifstream inFile;
	vector<double> paras;
	inFile.open("\oct_data.txt");
	double numin;
	if (inFile) {
		qDebug() << 1;
		for (size_t i = 0; i < 1024; ++i) {
			inFile >> numin; 
			paras.push_back(numin);
		}
	}
	inFile.close();

	InvokeFuncbyBoost(str, "testt", "filterAndPeck", paras);


	return a.exec();
}




//Py_Initialize();
//if (!Py_IsInitialized())
//{
//	PyErr_Print();	//��ӡ����
//	return -1;
//}
//
////�Ȼ��py�ļ����ڵ�·��������Ҫ����·��
//QString path = QDir::currentPath();
//path = path.replace("\\", "/");
//qDebug() << path << endl;
//path = "sys.path.append('" + path + "')";
//qDebug() << path << endl;
//QByteArray temp = path.toLocal8Bit();
////������python����·��
//PyRun_SimpleString("import sys");
//PyRun_SimpleString(temp.data());
///**************************/
//PyRun_SimpleString("sys.path.append('E:\python-install\Lib\site-packages')");
///**************************/
//PyObject* pModule = PyImport_ImportModule("testt");
//if (!pModule) {
//	printf("Cant open python file!\n");
//	return -1;
//}
////����hello�ű�
////PyObject_CallFunction(pModule, NULL);
////ͨ��ģ����add����
//PyObject* pFunc = PyObject_GetAttrString(pModule, "add");
//if (pFunc)
//{
//	//���������б�
//	PyObject* args = PyTuple_New(2);
//	PyObject* arg1 = PyLong_FromLong(11111);
//	PyObject* arg2 = PyLong_FromLong(22222);
//	PyTuple_SetItem(args, 0, arg1);
//	PyTuple_SetItem(args, 1, arg2);
//	PyObject *result = PyObject_CallObject(pFunc, args);
//	if (result)
//	{
//		long a = PyLong_AsLong(result);
//		qDebug() << "a:" << a << endl;
//	}
//}
////�������ͷ�python
//Py_Finalize();