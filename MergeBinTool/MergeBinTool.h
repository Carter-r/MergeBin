#pragma once

#include <QtWidgets/QMainWindow>
#include <QDropEvent>
#include "ui_MergeBinTool.h"
#include "FileInfo.h"

using namespace std;

class MergeBinTool : public QMainWindow
{
    Q_OBJECT

public:
    MergeBinTool(QWidget *parent = nullptr);
    ~MergeBinTool();

    void dragEnterEvent(QDragEnterEvent* event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent* event) Q_DECL_OVERRIDE;

private:
    void initTableFileInfo();
	bool load(const QString& fileName);
    bool checkFileExist(const QString& fileName);

private slots:
	void fileOpen();
	void fileDelete();
	void mergeBinFile();

private:
    Ui::MergeBinToolClass ui;

    vector<FileInfo*> vecFileInfo;
};
