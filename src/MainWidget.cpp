// Qt5 and system includes required by the project
#include <QWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QDesktopWidget>
#include <cstdlib>
#include <xmp.h>
// Project files
#include "mainwidget.h"
#include "file.h"
#include "tinyformat.h"
// include our helicopter.ui file.
#include "ui_helicopter.h"

MainWidget::MainWidget(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->statusbar->showMessage(_("Found %d valid tracker files\n", musicfiles.size()).c_str(), 10000);
	ui->tableWidget->setRowCount(musicfiles.size());
	ui->tableWidget->setColumnCount(3);
	QStringList m_columnLabels;
	m_columnLabels << "Song" << "Tracker" << "Filename";
	ui->tableWidget->setHorizontalHeaderLabels(m_columnLabels);
	ui->tableWidget->verticalHeader()->setVisible(false);
	ui->tableWidget->setShowGrid(false);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

	// Insert all the songs.
	int iter = 0;
	for (auto it : musicfiles)
	{
		// get song info
		struct xmp_test_info info;
		xmp_test_module(const_cast<char*>(it.c_str()), &info);
		std::string filename = basename(it);

		// Now set it up in the table.
		ui->tableWidget->setItem(iter, 0, new QTableWidgetItem(info.name));
		ui->tableWidget->setItem(iter, 1, new QTableWidgetItem(info.type));
		ui->tableWidget->setItem(iter, 2, new QTableWidgetItem(filename.c_str()));
		iter++;
	}
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
	int row = ui->tableWidget->currentRow();
	auto item = ui->tableWidget->currentItem();
	std::string file;
	if (row <= musicfiles.size())
	{
		file = musicfiles[row];
		printf("Item in musicfiles: %s\n", file.c_str());
		printf("Item in column: %s\n", ui->tableWidget->item(row, 2)->text().toStdString().c_str());
	}
	printf("Item: %s\n", item->text().toStdString().c_str());
	printf("Woooo! You pushed the play button! :D\n");
	//QMessageBox::information(nullptr, "Button Pushed", "You pushed the play button!");
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

void MainWidget::on_tableWidget_cellDoubleClicked(int row, int column)
{
	printf("Cell %d %d selected.\n", row, column);
}
