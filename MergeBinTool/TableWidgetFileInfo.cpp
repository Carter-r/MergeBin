#include "TableWidgetFileInfo.h"
#include <QHeaderview>

TableWidgetFileInfo::TableWidgetFileInfo(QWidget *parent)
	: QTableWidget(parent)
{
	QStringList strHeader;
	strHeader << QStringLiteral("文件头") << QStringLiteral("文件数据") << QStringLiteral("长度信息(B)") << QStringLiteral("删除");
	int nColCouunt = strHeader.size();
	this->setColumnCount(nColCouunt);            //设置列数
	this->setHorizontalHeaderLabels(strHeader);  //添加表头
	this->verticalHeader()->hide();              //将默认序号隐藏
	//this->horizontalHeader()->hide();
	this->verticalHeader()->setDefaultSectionSize(10);          //设置一行默认高度
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
	this->setEditTriggers(QAbstractItemView::NoEditTriggers);	    //不可编辑

	this->setAcceptDrops(true);

	// 拖拽设置
	this->setDragDropMode(QAbstractItemView::InternalMove);  // 内部移动
	this->setDropIndicatorShown(true);  // drop 位置提示
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
}

TableWidgetFileInfo::~TableWidgetFileInfo()
{}

void TableWidgetFileInfo::dropEvent(QDropEvent* event)
{
	// 原行号与目标行号的确定
	int row_src, row_dst;
	row_src = this->currentRow();                        // 原行号 可加if
	QTableWidgetItem* item = this->itemAt(event->pos()); // 获取落点的item
	if (item != nullptr)
	{                                        // 判断是否为空
		row_dst = item->row();                                 // 不为空 获取其行号
		emit sigMoveRow(row_src, row_dst);
		// 保证鼠标落下的位置 就是拖拽的一行最后所移动到的位置(考虑插入新行 移除原行的上下变化)
		row_src = (row_src > row_dst ? row_src + 1 : row_src); // 如果src在dst的下方(行号大)，后续插入dst会影响src的行号
		row_dst = (row_src < row_dst ? row_dst + 1 : row_dst); // 如果src在dst的上方(行号小)，后续移除src会影响dst的行号
		this->insertRow(row_dst);                              // 插入一行
	}
	else
	{                          // 落点没有item 说明拖动到了最下面
		row_dst = this->rowCount();// 获取行总数
		emit sigMoveRow(row_src, row_dst - 1);
		this->insertRow(row_dst);  // 在最后新增一行
	}

	// 执行移动
	this->setCellWidget(row_dst, 0, this->cellWidget(row_src, 0));
	this->setItem(row_dst, 1, this->takeItem(row_src, 1));
	this->setItem(row_dst, 2, this->takeItem(row_src, 2));
	this->setCellWidget(row_dst, 3, this->cellWidget(row_src, 3));
	this->selectRow(row_dst);

	// 删除原行
	this->removeRow(row_src);
}
