
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

	//先获得py文件所在的路径，必须要绝对路径
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
//	PyErr_Print();	//打印错误
//	return -1;
//}
//
////先获得py文件所在的路径，必须要绝对路径
//QString path = QDir::currentPath();
//path = path.replace("\\", "/");
//qDebug() << path << endl;
//path = "sys.path.append('" + path + "')";
//qDebug() << path << endl;
//QByteArray temp = path.toLocal8Bit();
////再设置python运行路径
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
////调用hello脚本
////PyObject_CallFunction(pModule, NULL);
////通过模块获得add函数
//PyObject* pFunc = PyObject_GetAttrString(pModule, "add");
//if (pFunc)
//{
//	//创建参数列表
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
////结束，释放python
//Py_Finalize();