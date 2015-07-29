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
#include "pulseaudio_interface.h"
// include our helicopter.ui file.
#include "ui_helicopter.h"

extern Pulseaudio *pulse;

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

		// Now set it up in the table, make sure there is a title or use the filename instead.
		if (QString(info.name).isEmpty())
			ui->tableWidget->setItem(iter, 0, new QTableWidgetItem(filename.c_str()));
		else
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

void PlayFile(const std::string &filename)
{
	// Initialize the player
	pulse->player->Start();
	pulse->player->LoadFromFile(filename);
	pulse->player->SetVolume(100);
	pulse->player->SetPlaying(true);

	// Connect to pulse.
	pulse->ConnectContext();
}

void MainWidget::on_pushButton_play_clicked()
{
	int row = ui->tableWidget->currentRow();
	printf("on_pushButton_play_clicked: %d\n", row);
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
	PlayFile(file);
	//QMessageBox::information(nullptr, "Button Pushed", "You pushed the play button!");
}

void MainWidget::on_pushButton_pause_clicked()
{
	static bool paused = false;
	paused = !paused;
	printf("Woooo! You pushed the pause button! :D\n");
	if (paused)
	{
		pulse->ConnectContext();
	}
	else
	{
		pulse->player->Stop();
		pulse->DisconnectContext();
	}
	//QMessageBox::information(nullptr, "Button Pushed", "You pushed the pause button!");
}

void MainWidget::on_pushButton_next_clicked()
{
	// Get the next song according to the list
	int row = ui->tableWidget->currentRow();
	printf("on_pushButton_next_clicked: %d\n", row);
	if (row + 1 > (musicfiles.size()-1))
		row = 0; // Wrap around to the beginning of the list.
	else
		row++;

	std::string nextsong = musicfiles[row];
	PlayFile(nextsong);
	//printf("Woooo! You pushed the next button! :D %d\n", ui->progressBar->value());
	printf("Next song is: %s\n", nextsong.c_str());
	ui->tableWidget->selectRow(row);
	//QMessageBox::information(nullptr, "Button Pushed", "You pushed the next button!");
	//ui->progressBar->setValue(valuecheck(ui->progressBar->value(), randint(1, 10)));
}

void MainWidget::on_pushButton_previous_clicked()
{
	// Get the previous song according to the list
	int row = ui->tableWidget->currentRow();
	printf("on_pushButton_previous_clicked: %d\n", row);
	if (row - 1 < 0)
		row = musicfiles.size() - 1; // wrap around to the end of the list.
	else
		row--; // otherwise, decrement to the previous song.
	ui->tableWidget->selectRow(row);
	std::string nextsong = musicfiles[row];

	PlayFile(nextsong);
	//printf("Woooo! You pushed the previous button! :D\n");
	printf("Previous song is: %s\n", nextsong.c_str());
	//QMessageBox::information(nullptr, "Button Pushed", "You pushed the previous button!");
}

void MainWidget::on_tableWidget_cellDoubleClicked(int row, int column)
{
	std::string m = musicfiles[row];
	printf("Cell %d %d selected -> %s\n", row, column, m.c_str());
	PlayFile(m);
}
