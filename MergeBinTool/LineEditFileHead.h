#pragma once

#include <QLineEdit>

class LineEditFileHead  : public QLineEdit
{
	Q_OBJECT

public:
	LineEditFileHead(QWidget *parent);
	~LineEditFileHead();

private slots:
	void slotLineEditFileHead(const QString& strFileHead);
};
