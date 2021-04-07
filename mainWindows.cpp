
#define BOOST_PYTHON_STATIC_LIB
#include <boost/python.hpp>
#include <python.h>
#include "mainWindows.h"
#include <vector>
#include <QTimer>
#include <QTime>
#include <iostream>
#include <vector>
#include <windows.h>
#include <numpy/arrayobject.h> //包含 numpy 中的头文件arrayobject.h

using namespace std;


mainWindows::mainWindows(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	QTimer *timer = new QTimer(this);
	timer->start(1000);
	connect(timer, SIGNAL(timeout()), this, SLOT(Time_Update()));
}


void mainWindows::Time_Update(void)
{
	QTime t;
	t = QTime::currentTime();
	qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
	cnt = qrand() % 50;//返回0-50
	//qDebug() << cnt;
	Show_Plot(ui.widget);
}

void mainWindows::Show_Plot(QCustomPlot *customPlot)
{
	QVector<double> key(10);
	QVector<double> value(10);
	QVector<double> value2(10);


	for (int i = 0; i < 9; i++)
	{
		num[i] = num[i + 1];
	}
	num[9] = cnt;
	for (int i = 0; i < 10; i++)
	{
		key[i] = i;
		value[i] = num[i];
		value2[i] = num[i] + 5;
	}
	customPlot->addGraph();
	customPlot->graph(0)->setPen(QPen(Qt::red));
	customPlot->graph(0)->setData(key, value);

	customPlot->addGraph();
	customPlot->graph(1)->setPen(QPen(Qt::black));
	customPlot->graph(1)->setData(key, value2);

	customPlot->xAxis->setLabel("Time(s)");
	customPlot->yAxis->setLabel("uA");

	customPlot->xAxis->setRange(0, 10);
	customPlot->yAxis->setRange(0, 100);
	customPlot->replot();

}



int init_numpy() {//初始化 numpy 执行环境，主要是导入包，python2.7用void返回类型，python3.0以上用int返回类型
	import_array();//必须函数，否则无法执行PyArray_SimpleNewFromData
}


vector<double> filtData(1024, 0);
vector<double> peakPos;
vector<double> peakValue;

bool InvokeFuncbyBoost(std::string path, std::string module, std::string func, std::vector<double> paras)
{
	Py_Initialize();
	init_numpy();
	if (!Py_IsInitialized())
	{
		return false;
	}

	try
	{
		//先获得py文件所在的路径，必须要绝对路径
		//QString path = QDir::currentPath();
		//path = path.replace("\\", "/");
		npy_intp Dims[1] = { paras.size() }; //给定维度信息
		double *arr = paras.data();

		//生成包含这个多维数组的PyObject对象，使用PyArray_SimpleNewFromData函数，
		//第一个参数1表示维度，第二个为维度数组Dims,第三个参数指出数组的类型，第四个参数为数组
		PyObject  *PyArray = PyArray_SimpleNewFromData(1, Dims, NPY_DOUBLE, arr);
		PyObject  *ArgArray = PyTuple_New(1);
		PyTuple_SetItem(ArgArray, 0, PyArray); //同样定义大小与Python函数参数个数一致的PyTuple对象


		PyRun_SimpleString("import sys\n");
		std::string importDir = "sys.path.append('" + path + "')\n";
		PyRun_SimpleString(importDir.c_str());
		//PyRun_SimpleString("sys.path.append('E:\python-install\Lib\site-packages')");

		PyObject* moduleName = PyUnicode_FromString(module.c_str());
		PyObject* pModule = PyImport_Import(moduleName);
		if (!pModule){
			return false;
		}
		PyObject* pFunc = PyObject_GetAttrString(pModule, func.c_str());
		PyObject* pResult = PyObject_CallObject(pFunc, ArgArray);

		PyArrayObject *ListItem;
		//if (PyList_Check(pResult)) {
		if (pResult) {
			int SizeOfList = PyList_Size(pResult);

			//第一个返回值是 滤波后的数据
			size_t Index_i = 0;
			ListItem = (PyArrayObject *)PyList_GetItem(pResult, Index_i);//读取List中的PyArrayObject对象，这里需要进行强制转换。
			//从Python中的PyArrayObject解析出数组数据为c的double类型。
			//double *resDataArr = (double *)PyArray_DATA(ListItem);
			int Rows = ListItem->dimensions[0];
			for (int Index_m = 0; Index_m < Rows; Index_m++) {
					//qDebug() << *(double *)(ListItem->data + Index_m * ListItem->strides[0] ) ;//访问数据，Index_m 和 Index_n 分别是数组元素的坐标，乘上相应维度的步长，即可以访问数组元素
					filtData[Index_m] = *(double *)(ListItem->data + Index_m * ListItem->strides[0]);
			}

			//第二个返回值是 峰值的位置
			Index_i = 1;
			ListItem = (PyArrayObject *)PyList_GetItem(pResult, Index_i);
			Rows = ListItem->dimensions[0];
			qDebug() << Rows;
			for (int Index_m = 0; Index_m < Rows; Index_m++) {
				qDebug() << *(double *)(ListItem->data + Index_m * ListItem->strides[0]);
				peakPos.push_back(*(double *)(ListItem->data + Index_m * ListItem->strides[0]));
				peakValue.push_back(filtData[*(double *)(ListItem->data + Index_m * ListItem->strides[0])]);
				qDebug() << filtData[*(double *)(ListItem->data + Index_m * ListItem->strides[0])];
			}
			

		}
		Py_DECREF(ListItem);
	}
	catch (...)
	{
		PyErr_Print();
		PyErr_Clear();
		Py_Finalize();
		return false;
	}
	Py_Finalize();
	return true;
}


