// Qt5 and system includes required by the project
#include <QWidget>
#include <QMainWindow>
// Project files
#include "mainwidget.h"
#include "file.h"
// include our helicopter.ui file.
#include "ui_helicopter.h"

MainWidget::MainWidget(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWidget::~MainWidget()
{
	delete ui;
}

void MainWidget::on_pushButton_play_clicked()
{
	printf("Woooo! You pushed the play button! :D\n");
}
