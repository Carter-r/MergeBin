#pragma once

#include <QTableWidget>
#include <QDropEvent>

class TableWidgetFileInfo  : public QTableWidget
{
	Q_OBJECT

public:
	TableWidgetFileInfo(QWidget *parent);
	~TableWidgetFileInfo();

signals:
	void sigMoveRow(int nFrom, int nTo);

protected:
	void dropEvent(QDropEvent* event) override;
};
