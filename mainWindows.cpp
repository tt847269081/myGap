
#define BOOST_PYTHON_STATIC_LIB
#include <boost/python.hpp>

#include <python.h>
#include "mainWindows.h"
#include <QVector>
#include <QTimer>
#include <QTime>
#include <iostream>
#include <vector>
#include <windows.h>
#include <numpy/arrayobject.h> //包含 numpy 中的头文件arrayobject.h




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


bool InvokeFuncbyBoost(std::string path, std::string module, std::string func, std::vector<int> paras)
{
	Py_Initialize();
	init_numpy();
	if (!Py_IsInitialized())
	{
		return false;
	}

	try
	{
		////先获得py文件所在的路径，必须要绝对路径
		//QString path = QDir::currentPath();
		//path = path.replace("\\", "/");
		npy_intp Dims[1] = { paras.size() }; //给定维度信息
		//int *arr = paras.data();
		int arr[8] = { 12,4,5,6,7,8,9 };

		//生成包含这个多维数组的PyObject对象，使用PyArray_SimpleNewFromData函数，
		//第一个参数1表示维度，第二个为维度数组Dims,第三个参数指出数组的类型，第四个参数为数组
		PyObject  *PyArray = PyArray_SimpleNewFromData(1, Dims, NPY_DOUBLE, reinterpret_cast<double *>(arr));
		//PyByteArrayObject *PyArray = reinterpret_cast<PyByteArrayObject *>(PyArray_SimpleNewFromData(1, Dims, NPY_DOUBLE, arr));
		//boost::python::object PyArray = (boost::python::object)PyArray_SimpleNewFromData(1, Dims, NPY_DOUBLE, arr);
		PyObject  *ArgArray = PyTuple_New(1);
		PyTuple_SetItem(ArgArray, 0, ArgArray); //同样定义大小与Python函数参数个数一致的PyTuple对象


		PyRun_SimpleString("import sys\n");
		std::string importDir = "sys.path.append('" + path + "')\n";
		PyRun_SimpleString(importDir.c_str());
		PyRun_SimpleString("sys.path.append('E:\python-install\Lib\site-packages')");

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
			for (int Index_i = 0; Index_i < SizeOfList; Index_i++) {
				ListItem = (PyArrayObject *)PyList_GetItem(pResult, Index_i);//读取List中的PyArrayObject对象，这里需要进行强制转换。
				//从Python中的PyArrayObject解析出数组数据为c的double类型。
				double *resDataArr = (double *)PyArray_DATA(ListItem);

				int dimNum = PyArray_NDIM(ListItem);//返回数组的维度数，此处恒为1
				qDebug() << dimNum;
				npy_intp *pdim = PyArray_DIMS(ListItem);//返回数组各维度上的元素个数值


				//以下是对返回结果的输出显示
				for (int i = 0; i < dimNum; ++i) {
					for (int j = 0; j < pdim[0]; ++j)
//						qDebug() << resDataArr[i * pdim[0] + j] << ",";
						qDebug() << resDataArr[j] << ",";
				}
				qDebug() << endl;


				//int Rows = ListItem->dimensions[0];
				//for (int Index_m = 0; Index_m < Rows; Index_m++) {
				//	qDebug() << *(double *)(ListItem->data + Index_m * ListItem->strides[0])<< " ";
				//	//访问数据，Index_m 和 Index_n 分别是数组元素的坐标，乘上相应维度的步长，即可以访问数组元素
				//}
			}
		}
		Py_DECREF(ListItem);


		//int Index_i = 0, Index_k = 0, Index_m = 0, Index_n = 0;
		//if (PyList_Check(pResult)) {

		//	int SizeOfList = PyList_Size(pResult);

		//	for (Index_i = 0; Index_i < SizeOfList; Index_i++) {

		//		PyArrayObject *ListItem = (PyArrayObject *)PyList_GetItem(pResult, Index_i);

		//		int Rows = ListItem->dimensions[0], columns = ListItem->dimensions[1];
		//		for (Index_m = 0; Index_m < Rows; Index_m++) {

		//			for (Index_n = 0; Index_n < columns; Index_n++) {

		//				qDebug() << *(double *)(ListItem->data + Index_m * ListItem->strides[0] + Index_n * ListItem->strides[1]) << " ";
		//			}
		//			qDebug() << endl;
		//		}

		//		Py_DECREF(ListItem);
		//	}
		//	
		//}

		//qDebug() << "Result = " << boost::python::extract<int32_t>(pyResult) << " by boost::python";

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


