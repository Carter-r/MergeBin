#include "MergeBinTool.h"
#include <QFileDialog>
#include <QMimeData>
#include <QTableWidget>

MergeBinTool::MergeBinTool(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	this->setAcceptDrops(true);
	this->setWindowIcon(QIcon(":/image/resource/fileMerge.png"));

	ui.lineEditOutPath->setFocus();

	connect(ui.actOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
	connect(ui.pushButtonMerge, SIGNAL(clicked()), this, SLOT(mergeBinFile()));
	connect(ui.tableWidgetFileInfo, SIGNAL(sigMoveRow(int, int)), this, SLOT(slotTableMoveRow(int, int)));
}

MergeBinTool::~MergeBinTool()
{
	list<FileInfo*>::iterator it = listFileInfo.begin();
	for (; it != listFileInfo.end(); ++it)
	{
		if ((*it) != nullptr)
		{
			delete (*it);
			(*it) = nullptr;
		}
	}
}

void MergeBinTool::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasUrls())
	{
		event->acceptProposedAction(); //事件数据中存在路径，方向事件
	}
	else
	{
		event->ignore();
	}
}

void MergeBinTool::dropEvent(QDropEvent* event)
{
	const QMimeData* mimeData = event->mimeData();
	if (mimeData->hasUrls())
	{
		QList<QUrl> urls = mimeData->urls();
		foreach(QUrl u, urls)
		{
			QString filepath = u.toLocalFile();
			load(filepath);
		}
	}
}

bool MergeBinTool::load(const QString& fileName)
{
	if (!QFile::exists(fileName))
	{
		return false;
	}

	// open file
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly))
	{
		return false;
	}

	FileInfo* pFileInfo = new FileInfo(file.fileName().toStdString().c_str(), file.size());
	if (pFileInfo == nullptr)
	{
		return false;
	}

	file.close();
	listFileInfo.push_back(pFileInfo);

	QFileInfo qFileInfo(fileName);

	// file head
	QLineEdit* pLineEdit = new QLineEdit(ui.tableWidgetFileInfo);
	QRegExp regx("^[0-9a-fA-F]+$");
	pLineEdit->setValidator(new QRegExpValidator(regx, pLineEdit));
	pLineEdit->setPlaceholderText(QStringLiteral("输入16进制数据，以字节为单位"));
	pLineEdit->setFocus();

	// file name
	QTableWidgetItem* pItemFileName = new QTableWidgetItem(qFileInfo.fileName());
	pItemFileName->setToolTip(fileName);

	// file size
	QString strFileLen = QString::number(qFileInfo.size(), 16).toUpper();
	if (strFileLen.length() % 2 == 0)
	{
		strFileLen = QString("0x") + strFileLen;
	}
	else
	{
		strFileLen = QString("0x0") + strFileLen;
	}
	QTableWidgetItem* pItemFileSize = new QTableWidgetItem(strFileLen);
	pItemFileSize->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

	// delete button
	QPushButton* pButtonDelete = new QPushButton();
	QPixmap pixmDelete(":/image/resource/delete.png");
	QPixmap scaledPixmapDelete = pixmDelete.scaled(QSize(20, 20));
	pButtonDelete->setIcon(scaledPixmapDelete);
	pButtonDelete->setIconSize(scaledPixmapDelete.size());
	connect(pButtonDelete, SIGNAL(clicked()), this, SLOT(fileDelete()));

	// insert table
	int nCurRow = ui.tableWidgetFileInfo->rowCount();
	ui.tableWidgetFileInfo->insertRow(nCurRow);
	ui.tableWidgetFileInfo->setCellWidget(nCurRow, 0, pLineEdit);
	ui.tableWidgetFileInfo->setItem(nCurRow, 1, pItemFileName);
	ui.tableWidgetFileInfo->setItem(nCurRow, 2, pItemFileSize);
	ui.tableWidgetFileInfo->setCellWidget(nCurRow, 3, pButtonDelete);

	return true;
}

void MergeBinTool::mergeBinFile()
{
	// clear warning text
	ui.labelWarningOutput->setText("");
	ui.pushButtonMerge->setEnabled(false);

	{
		// check source file
		if (ui.tableWidgetFileInfo->rowCount() == 0 || listFileInfo.size() == 0)
		{
			ui.labelWarningOutput->setText(QStringLiteral("请添加需要合并的文件"));
			QPalette pe;
			pe.setColor(QPalette::WindowText, Qt::red);
			ui.labelWarningOutput->setPalette(pe);
			goto mergeBinFileEnd;
		}

		// check file head data
		for (int i = 0; i < ui.tableWidgetFileInfo->rowCount(); ++i)
		{
			QLineEdit* pLineEdFileHead = qobject_cast<QLineEdit*>(ui.tableWidgetFileInfo->cellWidget(i, 0));
			QString strFileHead = pLineEdFileHead->text();
			if (strFileHead.size() % 2 != 0)
			{
				std::string strFWarning = "第";
				strFWarning += std::to_string(i + 1);
				strFWarning += "行文件头数据长度为单数";
				ui.labelWarningOutput->setText(QString::fromLocal8Bit(strFWarning.c_str()));
				QPalette pe;
				pe.setColor(QPalette::WindowText, Qt::red);
				ui.labelWarningOutput->setPalette(pe);
				pLineEdFileHead->setFocus();
				goto mergeBinFileEnd;
			}
		}

		// check source file exist
		list<FileInfo*>::iterator it = listFileInfo.begin();
		for (; it != listFileInfo.end(); ++it)
		{
			if (!checkFileExist((*it)->getFileName().c_str()))
			{
				std::string strFile = "文件\"";
				strFile += (*it)->getFileName();
				strFile += "\"不存在";
				ui.labelWarningOutput->setText(QString::fromLocal8Bit(strFile.c_str()));
				QPalette pe;
				pe.setColor(QPalette::WindowText, Qt::red);
				ui.labelWarningOutput->setPalette(pe);
				goto mergeBinFileEnd;
			}
		}

		// check output path
		if (ui.lineEditOutPath->text().isEmpty())
		{
			ui.labelWarningOutput->setText(QStringLiteral("输出路径为空"));
			QPalette pe;
			pe.setColor(QPalette::WindowText, Qt::red);
			ui.labelWarningOutput->setPalette(pe);
			ui.lineEditOutPath->setFocus();
			goto mergeBinFileEnd;
		}

		QDir dirOutPath(ui.lineEditOutPath->text());
		if (!dirOutPath.exists())
		{
			ui.labelWarningOutput->setText(QStringLiteral("输出路径不存在"));
			QPalette pe;
			pe.setColor(QPalette::WindowText, Qt::red);
			ui.labelWarningOutput->setPalette(pe);
			ui.lineEditOutPath->setFocus();
			goto mergeBinFileEnd;
		}

		// check output file
		if (ui.lineEditMergeFile->text().isEmpty())
		{
			ui.labelWarningOutput->setText(QStringLiteral("合并文件名为空"));
			QPalette pe;
			pe.setColor(QPalette::WindowText, Qt::red);
			ui.labelWarningOutput->setPalette(pe);
			ui.lineEditMergeFile->setFocus();
			goto mergeBinFileEnd;
		}

		QString strOutFile = ui.lineEditOutPath->text();
		if (strOutFile.at(strOutFile.size() - 1) == '\\')
			strOutFile = strOutFile + ui.lineEditMergeFile->text();
		else
			strOutFile = strOutFile + "\\" + ui.lineEditMergeFile->text();
		if (checkFileExist(strOutFile))
		{
			ui.labelWarningOutput->setText(QStringLiteral("输出路径下存在同名的合并文件名"));
			QPalette pe;
			pe.setColor(QPalette::WindowText, Qt::red);
			ui.labelWarningOutput->setPalette(pe);
			ui.lineEditMergeFile->setFocus();
			goto mergeBinFileEnd;
		}

		// start to merge
		QFile outputFile(strOutFile);
		if (!outputFile.open(QIODevice::WriteOnly))
		{
			std::string strFile = "打开文件\"";
			strFile += strOutFile.toStdString();
			strFile += "\"失败";
			ui.labelWarningOutput->setText(QString::fromLocal8Bit(strFile.c_str()));
			QPalette pe;
			pe.setColor(QPalette::WindowText, Qt::red);
			ui.labelWarningOutput->setPalette(pe);
			ui.lineEditMergeFile->setFocus();
			goto mergeBinFileEnd;
		}

		QDataStream outputDataStream(&outputFile);
		it = listFileInfo.begin();
		for (int i = 0; it != listFileInfo.end() && i < ui.tableWidgetFileInfo->rowCount(); ++it, ++i)
		{
			QLineEdit* pLineEdFileHead = qobject_cast<QLineEdit*>(ui.tableWidgetFileInfo->cellWidget(i, 0));
			QString strFileHead = pLineEdFileHead->text();
			std::string sFileHead;
			sFileHead.resize(strFileHead.size() / 2);
			for (int j = 0; j < strFileHead.size(); j += 2)
			{
				sFileHead[j / 2] = strFileHead.mid(j, 2).toUInt(nullptr, 16) & 0xFF;
			}
			outputDataStream.writeRawData(sFileHead.c_str(), sFileHead.length());

			QFile inputFile((*it)->getFileName().c_str());

			if (!inputFile.open(QIODevice::ReadOnly))
			{
				std::string strFile = "打开文件\"";
				strFile += (*it)->getFileName();
				strFile += "\"失败";
				ui.labelWarningOutput->setText(QString::fromLocal8Bit(strFile.c_str()));
				QPalette pe;
				pe.setColor(QPalette::WindowText, Qt::red);
				ui.labelWarningOutput->setPalette(pe);

				outputFile.close();
				goto mergeBinFileEnd;
			}

			//QDataStream inputDataStream(&inputFile);
			QByteArray byteArray = inputFile.readAll();
			outputDataStream.writeRawData(byteArray.data(), byteArray.size());
			inputFile.close();
		}

		outputFile.close();

		ui.labelWarningOutput->setText(QStringLiteral("合并文件成功！"));
		QPalette pe;
		pe.setColor(QPalette::WindowText, Qt::black);
		ui.labelWarningOutput->setPalette(pe);
	}

mergeBinFileEnd:
	ui.pushButtonMerge->setEnabled(true);
	return;
}

void MergeBinTool::slotTableMoveRow(int nFrom, int nTo)
{
	if (nFrom < nTo)
	{
		list<FileInfo*>::iterator itFrom = std::next(listFileInfo.begin(), nFrom);
		list<FileInfo*>::iterator itTo = std::next(listFileInfo.begin(), nTo + 1);
		listFileInfo.insert(itTo, itFrom, std::next(itFrom, 1));
		listFileInfo.erase(itFrom);
	}
	else
	{
		list<FileInfo*>::iterator itFrom = std::next(listFileInfo.begin(), nFrom);
		list<FileInfo*>::iterator itTo = std::next(listFileInfo.begin(), nTo);
		listFileInfo.insert(itTo, itFrom, std::next(itFrom, 1));
		listFileInfo.erase(itFrom);
	}
}

bool MergeBinTool::checkFileExist(const QString& fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly))
	{
		return false;
	}
	file.close();
	return true;
}

void MergeBinTool::fileOpen()
{
	QString name = QFileDialog::getOpenFileName(this, tr("Open File..."),
		QString(), tr("*.bin *.BIN"));
	if (!name.isEmpty()) {
		load(name);
	}
}

void MergeBinTool::fileDelete()
{
	QPushButton* pBtonDelelte = qobject_cast<QPushButton*>(sender());
	if (pBtonDelelte == nullptr)
	{
		return;
	}

	int x = pBtonDelelte->frameGeometry().x();
	int y = pBtonDelelte->frameGeometry().y();
	QModelIndex index = ui.tableWidgetFileInfo->indexAt(QPoint(x, y));
	int row = index.row();
	int column = index.column();

	list<FileInfo*>::iterator it = std::next(listFileInfo.begin(), row);
	if ((*it) != nullptr)
	{
		delete (*it);
		(*it) = nullptr;
	}
	listFileInfo.erase(std::next(listFileInfo.begin(), row));

	ui.tableWidgetFileInfo->removeRow(row);
}
