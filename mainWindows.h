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
	double num[10]; // 缓冲区
	int cnt;
};



bool InvokeFuncbyBoost(std::string path, std::string module, std::string func, std::vector<double> paras);
int init_numpy();//初始化 numpy 执行环境，主要是导入包，python2.7用void返回类型，python3.0以上用int返回类型
