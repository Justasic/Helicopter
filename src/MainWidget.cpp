// Qt5 and system includes required by the project
#include <QWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <cstdlib>
// Project files
#include "mainwidget.h"
#include "file.h"
// include our helicopter.ui file.
#include "ui_helicopter.h"

MainWidget::MainWidget(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->tableWidget->setRowCount(10);
	ui->tableWidget->setColumnCount(4);
	QStringList m_columnLabels;
	m_columnLabels << "This" << "Is" << "An" << "Example";
	ui->tableWidget->setHorizontalHeaderLabels(m_columnLabels);
	ui->tableWidget->verticalHeader()->setVisible(false);
	ui->tableWidget->setShowGrid(false);
}

MainWidget::~MainWidget()
{
	delete ui;
}

static inline int valuecheck(int cur, int increment)
{
	cur += increment;
	cur %= 100;
	return cur;
}

static inline int randint(int min, int max)
{
	srand(time(NULL));
	return (rand() % max) + min;
}

void MainWidget::on_pushButton_play_clicked()
{
	printf("Woooo! You pushed the play button! :D\n");
	QMessageBox::information(nullptr, "Button Pushed", "You pushed the play button!");
}

void MainWidget::on_pushButton_pause_clicked()
{
	printf("Woooo! You pushed the pause button! :D\n");
	QMessageBox::information(nullptr, "Button Pushed", "You pushed the pause button!");
}

void MainWidget::on_pushButton_next_clicked()
{
	printf("Woooo! You pushed the next button! :D %d\n", ui->progressBar->value());
	//QMessageBox::information(nullptr, "Button Pushed", "You pushed the next button!");
	ui->progressBar->setValue(valuecheck(ui->progressBar->value(), randint(1, 10)));
}

void MainWidget::on_pushButton_previous_clicked()
{
	printf("Woooo! You pushed the previous button! :D\n");
	QMessageBox::information(nullptr, "Button Pushed", "You pushed the previous button!");
}
