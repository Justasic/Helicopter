#pragma once

#include <QWidget>

namespace Ui
{
        class MainWindow;
}

class MainWidget : public QWidget
{
Q_OBJECT

public:
	explicit MainWidget(QWidget *parent = 0);
	~MainWidget();

	// Qt slots
	void pushButton_play_clicked();

private:
	Ui::MainWindow *ui;
};
