#include "MergeBinTool.h"
#include <QFileDialog>

MergeBinTool::MergeBinTool(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	initTableFileInfo();

	connect(ui.actOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
}

MergeBinTool::~MergeBinTool()
{
	map<int, FileInfo*>::iterator it;
	for (it = mapFileInfo.begin(); it != mapFileInfo.end(); ++it)
	{
		if (it->second)
		{
			delete it->second;
			it->second = nullptr;
		}
	}
}

void MergeBinTool::initTableFileInfo()
{
    QStringList strHeader;
    strHeader << QStringLiteral("�ļ�ͷ") << QStringLiteral("�ļ�����") << QStringLiteral("������Ϣ") << QStringLiteral("ɾ��");
	int nColCouunt = strHeader.size();
	ui.tableWidgetFileInfo->setColumnCount(nColCouunt);            //��������
	ui.tableWidgetFileInfo->verticalHeader()->hide();              //��Ĭ���������
	ui.tableWidgetFileInfo->horizontalHeader()->hide();
	//ui.tableWidgetFileInfo->setHorizontalHeaderLabels(strHeader);  //��ӱ�ͷ
	//ui.tableWidgetFileInfo->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);    //��ͷ��Ϣ��ʾ���п���
	//ui.tableWidgetFileInfo->horizontalHeader()->setStretchLastSection(true);           //�������һ������Ӧ
	ui.tableWidgetFileInfo->verticalHeader()->setDefaultSectionSize(20);          //����һ��Ĭ�ϸ߶�
	ui.tableWidgetFileInfo->setSelectionBehavior(QAbstractItemView::SelectRows);       //ѡȡһ����
	ui.tableWidgetFileInfo->setSelectionMode(QAbstractItemView::SingleSelection);	    //����ѡ��
	ui.tableWidgetFileInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);	    //���ɱ༭
	//ui.tableWidgetFileInfo->setAlternatingRowColors(true);                             //���б�ɫ
	//ui.tableWidgetFileInfo->setStyleSheet("QHeaderView::section{background-color:rgb(227, 247, 255);border:1px solid black;}");
	ui.tableWidgetFileInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.tableWidgetFileInfo->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
	//ui.tableWidgetFileInfo->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	//ui.tableWidgetFileInfo->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

	ui.tableWidgetFileInfo->setColumnWidth(0, 200);
	ui.tableWidgetFileInfo->setColumnWidth(1, 200);
	ui.tableWidgetFileInfo->setColumnWidth(2, 150);
	ui.tableWidgetFileInfo->setColumnWidth(3, 40);
}

bool MergeBinTool::load(const QString& fileName)
{
	if (!QFile::exists(fileName)) {
		return false;
	}

	// open file
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly)) {
		return false;
	}

	FileInfo* pFileInfo = new FileInfo(file.fileName().toStdString().c_str(), file.size());
	if (pFileInfo == nullptr)
	{
		return false;
	}

	QFileInfo qFileInfo(fileName);

	int nCurRow = ui.tableWidgetFileInfo->rowCount();

	mapFileInfo.insert(make_pair(nCurRow, pFileInfo));

	QLineEdit* pLineEdit = new QLineEdit(ui.tableWidgetFileInfo);
	pLineEdit->setFixedHeight(20);
	pLineEdit->setFixedWidth(200);

	ui.tableWidgetFileInfo->setCellWidget(nCurRow, 0, pLineEdit);
	ui.tableWidgetFileInfo->setItem(nCurRow, 1, new QTableWidgetItem(qFileInfo.fileName()));

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
