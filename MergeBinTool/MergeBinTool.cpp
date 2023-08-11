#include "MergeBinTool.h"
#include <QFileDialog>
#include <QMimeData>

MergeBinTool::MergeBinTool(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	this->setAcceptDrops(true);

	initTableFileInfo();

	ui.lineEditOutPath->setFocus();

	connect(ui.actOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
	connect(ui.pushButtonMerge, SIGNAL(clicked()), this, SLOT(mergeBinFile()));
}

MergeBinTool::~MergeBinTool()
{
	for (int i = 0; i < vecFileInfo.size(); ++i)
	{
		if (vecFileInfo[i] != nullptr)
		{
			delete vecFileInfo[i];
			vecFileInfo[i] = nullptr;
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
		QString fileName = urls.at(0).toLocalFile();

		load(fileName);
	}
}

void MergeBinTool::initTableFileInfo()
{
    QStringList strHeader;
    strHeader << QStringLiteral("文件头") << QStringLiteral("文件数据") << QStringLiteral("长度信息(B)") << QStringLiteral("删除");
	int nColCouunt = strHeader.size();
	//ui.tableWidgetFileInfo->setShowGrid(false);
	ui.tableWidgetFileInfo->setColumnCount(nColCouunt);            //设置列数
	ui.tableWidgetFileInfo->verticalHeader()->hide();              //将默认序号隐藏
	//ui.tableWidgetFileInfo->horizontalHeader()->hide();
	ui.tableWidgetFileInfo->setHorizontalHeaderLabels(strHeader);  //添加表头
	//ui.tableWidgetFileInfo->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);    //表头信息显示居中靠左
	//ui.tableWidgetFileInfo->horizontalHeader()->setStretchLastSection(true);           //设置最后一列自适应
	ui.tableWidgetFileInfo->verticalHeader()->setDefaultSectionSize(10);          //设置一行默认高度
	ui.tableWidgetFileInfo->setSelectionBehavior(QAbstractItemView::SelectRows);       //选取一整行
	ui.tableWidgetFileInfo->setSelectionMode(QAbstractItemView::NoSelection);
	ui.tableWidgetFileInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);	    //不可编辑
	//ui.tableWidgetFileInfo->setAlternatingRowColors(true);                             //隔行变色
	//ui.tableWidgetFileInfo->setStyleSheet("QHeaderView::section{background-color:rgb(227, 247, 255);border:1px solid black;}");
	ui.tableWidgetFileInfo->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui.tableWidgetFileInfo->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
	ui.tableWidgetFileInfo->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
	ui.tableWidgetFileInfo->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
	ui.tableWidgetFileInfo->setColumnWidth(0, 200);
	ui.tableWidgetFileInfo->setColumnWidth(1, 200);
	ui.tableWidgetFileInfo->setColumnWidth(2, 80);
	ui.tableWidgetFileInfo->setColumnWidth(3, 40);
	ui.tableWidgetFileInfo->horizontalHeader()->setHighlightSections(false);
	//ui.tableWidgetFileInfo->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	//ui.tableWidgetFileInfo->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	//ui.tableWidgetFileInfo->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	ui.tableWidgetFileInfo->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:#E0E0E0;border:none;}");
	ui.tableWidgetFileInfo->horizontalHeader()->setSectionsClickable(false);

	ui.tableWidgetFileInfo->setDragDropMode(QAbstractItemView::InternalMove);
	ui.tableWidgetFileInfo->setAcceptDrops(true);

	//ui.tableWidgetFileInfo->setColumnWidth(0, 197);
	//ui.tableWidgetFileInfo->setColumnWidth(1, 119);
	//ui.tableWidgetFileInfo->setColumnWidth(2, 125);
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
	vecFileInfo.push_back(pFileInfo);

	QFileInfo qFileInfo(fileName);

	// file head
	QLineEdit* pLineEdit = new QLineEdit(ui.tableWidgetFileInfo);
	QRegExp regx("^[0-9a-fA-F]+$");
	pLineEdit->setValidator(new QRegExpValidator(regx, pLineEdit));
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
	if (ui.lineEditOutPath->text().isEmpty() || ui.lineEditMergeFile->text().isEmpty()
		|| ui.tableWidgetFileInfo->rowCount() == 0 || vecFileInfo.size() == 0)
	{
		return;
	}

	QDir dirOutPath(ui.lineEditOutPath->text());
	if (!dirOutPath.exists())
	{
		return;
	}

	QString strOutFile = ui.lineEditOutPath->text();
	if (strOutFile.at(strOutFile.size() - 1) == '\\')
		strOutFile = strOutFile + ui.lineEditMergeFile->text();
	else
		strOutFile = strOutFile + "\\" + ui.lineEditMergeFile->text();
	if (checkFileExist(strOutFile))
	{
		return;
	}

	for (int i = 0; i < vecFileInfo.size(); ++i)
	{
		if (!checkFileExist(vecFileInfo[i]->getFileName().c_str()))
		{
			return;
		}
	}

	QFile outputFile(strOutFile);
	if (!outputFile.open(QIODevice::WriteOnly))
	{
		return;
	}

	QDataStream outputDataStream(&outputFile);
	for (int i = 0; i < vecFileInfo.size(); ++i)
	{
		QLineEdit* pLineEdFileHead = qobject_cast<QLineEdit*>(ui.tableWidgetFileInfo->cellWidget(i, 0));
		std::string strFileHead = pLineEdFileHead->text().toStdString();
		outputDataStream.writeRawData(strFileHead.c_str(), strFileHead.length());

		QFile inputFile(vecFileInfo[i]->getFileName().c_str());

		if (!inputFile.open(QIODevice::ReadOnly))
		{
			outputFile.close();
			return;
		}

		//QDataStream inputDataStream(&inputFile);
		QByteArray byteArray = inputFile.readAll();
		outputDataStream.writeRawData(byteArray.data(), byteArray.size());
		inputFile.close();
	}

	outputFile.close();
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

	if (vecFileInfo[row] != nullptr)
	{
		delete vecFileInfo[row];
		vecFileInfo[row] = nullptr;
	}
	vecFileInfo.erase(vecFileInfo.begin() + row);

	ui.tableWidgetFileInfo->removeRow(row);
}
