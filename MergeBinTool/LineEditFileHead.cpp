#include "LineEditFileHead.h"
#include <QRegExpValidator>

LineEditFileHead::LineEditFileHead(QWidget *parent)
	: QLineEdit(parent)
{
	QRegExp regx("^[0-9a-fA-F ]+$");
	this->setValidator(new QRegExpValidator(regx, this));
	this->setPlaceholderText(QStringLiteral("输入16进制数据，以字节为单位"));
	this->setFocus();
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(slotLineEditFileHead(const QString&)));
}

LineEditFileHead::~LineEditFileHead()
{}

void LineEditFileHead::slotLineEditFileHead(const QString& strFileHead)
{
	QString strTemp = strFileHead;
	strTemp.replace(" ", "");
	QString qsFileHead;

	int i = 0;
	for (; i < strTemp.length() / 2; ++i)
	{
		qsFileHead = qsFileHead + strTemp.mid(i * 2, 2) + " ";
	}
	if (i * 2 < strTemp.length())
	{
		qsFileHead = qsFileHead + strTemp.mid(i * 2);
	}
	this->setText(qsFileHead);
}
