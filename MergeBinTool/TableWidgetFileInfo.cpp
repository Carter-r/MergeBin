#include "TableWidgetFileInfo.h"
#include <QHeaderview>

TableWidgetFileInfo::TableWidgetFileInfo(QWidget *parent)
	: QTableWidget(parent)
{
	QStringList strHeader;
	strHeader << QStringLiteral("�ļ�ͷ") << QStringLiteral("�ļ�����") << QStringLiteral("������Ϣ(B)") << QStringLiteral("ɾ��");
	int nColCouunt = strHeader.size();
	this->setColumnCount(nColCouunt);            //��������
	this->setHorizontalHeaderLabels(strHeader);  //��ӱ�ͷ
	this->verticalHeader()->hide();              //��Ĭ���������
	//this->horizontalHeader()->hide();
	this->verticalHeader()->setDefaultSectionSize(10);          //����һ��Ĭ�ϸ߶�
	this->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	this->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
	this->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
	this->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
	this->setColumnWidth(0, 200);
	this->setColumnWidth(1, 200);
	this->setColumnWidth(2, 80);
	this->setColumnWidth(3, 40);
	this->horizontalHeader()->setHighlightSections(false);
	this->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:#E0E0E0;border:none;}");
	this->horizontalHeader()->setSectionsClickable(false);

	this->setSelectionMode(QAbstractItemView::SingleSelection);
	this->setEditTriggers(QAbstractItemView::NoEditTriggers);	    //���ɱ༭

	this->setAcceptDrops(true);

	// ��ק����
	this->setDragDropMode(QAbstractItemView::InternalMove);  // �ڲ��ƶ�
	this->setDropIndicatorShown(true);  // drop λ����ʾ
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
}

TableWidgetFileInfo::~TableWidgetFileInfo()
{}

void TableWidgetFileInfo::dropEvent(QDropEvent* event)
{
	// ԭ�к���Ŀ���кŵ�ȷ��
	int row_src, row_dst;
	row_src = this->currentRow();                        // ԭ�к� �ɼ�if
	QTableWidgetItem* item = this->itemAt(event->pos()); // ��ȡ����item
	if (item != nullptr)
	{                                        // �ж��Ƿ�Ϊ��
		row_dst = item->row();                                 // ��Ϊ�� ��ȡ���к�
		emit sigMoveRow(row_src, row_dst);
		// ��֤������µ�λ�� ������ק��һ��������ƶ�����λ��(���ǲ������� �Ƴ�ԭ�е����±仯)
		row_src = (row_src > row_dst ? row_src + 1 : row_src); // ���src��dst���·�(�кŴ�)����������dst��Ӱ��src���к�
		row_dst = (row_src < row_dst ? row_dst + 1 : row_dst); // ���src��dst���Ϸ�(�к�С)�������Ƴ�src��Ӱ��dst���к�
		this->insertRow(row_dst);                              // ����һ��
	}
	else
	{                          // ���û��item ˵���϶�����������
		row_dst = this->rowCount();// ��ȡ������
		emit sigMoveRow(row_src, row_dst - 1);
		this->insertRow(row_dst);  // ���������һ��
	}

	// ִ���ƶ�
	this->setCellWidget(row_dst, 0, this->cellWidget(row_src, 0));
	this->setItem(row_dst, 1, this->takeItem(row_src, 1));
	this->setItem(row_dst, 2, this->takeItem(row_src, 2));
	this->setCellWidget(row_dst, 3, this->cellWidget(row_src, 3));
	this->selectRow(row_dst);

	// ɾ��ԭ��
	this->removeRow(row_src);
}
