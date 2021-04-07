#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainWindows.h"
#include "qcustomplot.h"
#include <string>
#include <QTimer>
#include <QDebug>


class mainWindows : public QMainWindow
{
    Q_OBJECT

public:
	explicit mainWindows(QWidget *parent = Q_NULLPTR);
	void Show_Plot(QCustomPlot *customPlot);
public slots:
	void Time_Update(void);

private:
    Ui::mainWindowsClass ui;
	QCustomPlot *customPlot;
	double num[10]; // ������
	int cnt;
};



bool InvokeFuncbyBoost(std::string path, std::string module, std::string func, std::vector<double> paras);
int init_numpy();//��ʼ�� numpy ִ�л�������Ҫ�ǵ������python2.7��void�������ͣ�python3.0������int��������
