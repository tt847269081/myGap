
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
#include <numpy/arrayobject.h> //���� numpy �е�ͷ�ļ�arrayobject.h




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
	cnt = qrand() % 50;//����0-50
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



int init_numpy() {//��ʼ�� numpy ִ�л�������Ҫ�ǵ������python2.7��void�������ͣ�python3.0������int��������
	import_array();//���뺯���������޷�ִ��PyArray_SimpleNewFromData
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
		////�Ȼ��py�ļ����ڵ�·��������Ҫ����·��
		//QString path = QDir::currentPath();
		//path = path.replace("\\", "/");
		npy_intp Dims[1] = { paras.size() }; //����ά����Ϣ
		//int *arr = paras.data();
		int arr[8] = { 12,4,5,6,7,8,9 };

		//���ɰ��������ά�����PyObject����ʹ��PyArray_SimpleNewFromData������
		//��һ������1��ʾά�ȣ��ڶ���Ϊά������Dims,����������ָ����������ͣ����ĸ�����Ϊ����
		PyObject  *PyArray = PyArray_SimpleNewFromData(1, Dims, NPY_DOUBLE, reinterpret_cast<double *>(arr));
		//PyByteArrayObject *PyArray = reinterpret_cast<PyByteArrayObject *>(PyArray_SimpleNewFromData(1, Dims, NPY_DOUBLE, arr));
		//boost::python::object PyArray = (boost::python::object)PyArray_SimpleNewFromData(1, Dims, NPY_DOUBLE, arr);
		PyObject  *ArgArray = PyTuple_New(1);
		PyTuple_SetItem(ArgArray, 0, ArgArray); //ͬ�������С��Python������������һ�µ�PyTuple����


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
				ListItem = (PyArrayObject *)PyList_GetItem(pResult, Index_i);//��ȡList�е�PyArrayObject����������Ҫ����ǿ��ת����
				//��Python�е�PyArrayObject��������������Ϊc��double���͡�
				double *resDataArr = (double *)PyArray_DATA(ListItem);

				int dimNum = PyArray_NDIM(ListItem);//���������ά�������˴���Ϊ1
				qDebug() << dimNum;
				npy_intp *pdim = PyArray_DIMS(ListItem);//���������ά���ϵ�Ԫ�ظ���ֵ


				//�����ǶԷ��ؽ���������ʾ
				for (int i = 0; i < dimNum; ++i) {
					for (int j = 0; j < pdim[0]; ++j)
//						qDebug() << resDataArr[i * pdim[0] + j] << ",";
						qDebug() << resDataArr[j] << ",";
				}
				qDebug() << endl;


				//int Rows = ListItem->dimensions[0];
				//for (int Index_m = 0; Index_m < Rows; Index_m++) {
				//	qDebug() << *(double *)(ListItem->data + Index_m * ListItem->strides[0])<< " ";
				//	//�������ݣ�Index_m �� Index_n �ֱ�������Ԫ�ص����꣬������Ӧά�ȵĲ����������Է�������Ԫ��
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


