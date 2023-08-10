#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MergeBinTool.h"
#include "FileInfo.h"

using namespace std;

class MergeBinTool : public QMainWindow
{
    Q_OBJECT

public:
    MergeBinTool(QWidget *parent = nullptr);
    ~MergeBinTool();

private:
    void initTableFileInfo();
	bool load(const QString& fileName);

private slots:
	void fileOpen();

private:
    Ui::MergeBinToolClass *ui;

    map<int, FileInfo*> mapFileInfo;
};
