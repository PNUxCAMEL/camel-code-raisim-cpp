//
// Created by jaehoon on 22. 4. 26.
//

#include "simulationMainwindow.h"
#include "ui_simulationMainwindow.h"
#include <iostream>

MainWindow *MainUI;

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    MainUI = this;
    ui->setupUi(this);
    for (int i=0; i<201; ++i)
    {
        data_x[i] = 0;
        data_y1[i] = 0;
        data_y2[i] = 0;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    std::cout<<"'Run' button is clicked"<<std::endl;
    if(button1){button1 = false;}
    else{button1 = true;}
}

void MainWindow::plotWidget1()
{
    QVector<double> x(201);
    QVector<double> y1(201);
    for (int i=0; i<201; ++i)
    {
        x[i] = data_x[i];
        y1[i] = data_y1[i];
        if(y1[i] < data_y1_min){data_y1_min = y1[i];}
        if(y1[i] > data_y1_max){data_y1_max = y1[i];}
    }
    ui->widget->addGraph();
    ui->widget->graph(0)->addData(x, y1);
    // give the axes some labels:
    ui->widget->xAxis->setLabel("time [sec]");
    ui->widget->yAxis->setLabel("position");
    // set axes ranges, so we see all data:
    ui->widget->xAxis->setRange(0.0, x[MainUI->data_idx - 1]+ 0.001);
    ui->widget->yAxis->setRange(data_y1_min - 0.001, data_y1_max + 0.001);
    ui->widget->replot();
}

void MainWindow::plotWidget2()
{
    QVector<double> x(201);
    QVector<double> y2(201);
    for (int i=0; i<201; ++i)
    {
        x[i] = data_x[i];
        y2[i] = data_y2[i];
        if(y2[i] < data_y2_min){data_y2_min = y2[i];}
        if(y2[i] > data_y2_max){data_y2_max = y2[i];}
    }
    ui->widget_2->addGraph();
    ui->widget_2->graph(0)->addData(x, y2);
    // give the axes some labels:
    ui->widget_2->xAxis->setLabel("time [sec]");
    ui->widget_2->yAxis->setLabel("velocity");
    // set axes ranges, so we see all data:
    ui->widget_2->xAxis->setRange(0.0, x[MainUI->data_idx - 1]+0.001);
    ui->widget_2->yAxis->setRange(data_y2_min - 0.001, data_y2_max + 0.001);
    ui->widget_2->replot();
}
