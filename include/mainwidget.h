#pragma once

#include <QWidget>
#include <QMainWindow>

namespace Ui { class MainWindow; };

class MainWidget : public QMainWindow
{
Q_OBJECT

public:
	explicit MainWidget(QWidget *parent = 0);
	virtual ~MainWidget();

private slots:
	// Qt slots
	void on_pushButton_play_clicked();

private:
	Ui::MainWindow *ui;
};
