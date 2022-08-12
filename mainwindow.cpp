#include <QFileDialog>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ELFMap.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_isParserIcon = false;
    QString showDemo;
    showDemo += "RVA        0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F  Value   \n";
    showDemo += "00000000  CC CC CC CC CC CC CC CC  CC CC CC CC CC CC CC CC  ........\n";
    showDemo += "00000010  CC CC CC CC CC CC CC CC  CC CC CC CC CC CC CC CC  ........\n";
    showDemo += "00000020  CC CC CC CC CC CC CC CC  CC CC CC CC CC CC CC CC  ........\n";
    showDemo += "00000030  CC CC CC CC CC CC CC CC  CC CC CC CC CC CC CC CC  ........\n";
    ui->plainTextEdit->setPlainText(showDemo);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionopen_triggered()
{
    QString filePathAndName = QFileDialog::getOpenFileName(this,"选择文件");

    if(!elfMap.Parser(filePathAndName.toStdString().c_str()))
    {
        ui->plainTextEdit->setPlainText("文件解析失败！");
        return;
    };

    QTreeWidgetItem * itemRoot = new QTreeWidgetItem(QTreeWidgetItem::Type);
    itemRoot->setText(0, elfMap.m_fileName.c_str());
    ui->treeWidget->addTopLevelItem(itemRoot);

    QTreeWidgetItem * itemEhdr = new QTreeWidgetItem(QTreeWidgetItem::Type);
    itemEhdr->setText(0, "ELF文件头");
    itemRoot->addChild(itemEhdr);

    QTreeWidgetItem * itemPhdr = new QTreeWidgetItem(QTreeWidgetItem::Type);
    itemPhdr->setText(0, "程序段表");
    itemRoot->addChild(itemPhdr);

    int num = 0;
    for(auto it : elfMap.m_vshdr)
    {
        char name[64] = {0};
        sprintf(name,"[%02d]%s",num,elfMap.m_shNameSection + it -> sh_name);
        QTreeWidgetItem * item = new QTreeWidgetItem(QTreeWidgetItem::Type);
        item->setText(0, name);
        itemRoot->addChild(item);
        num++;
    }

    QTreeWidgetItem * itemShdr = new QTreeWidgetItem(QTreeWidgetItem::Type);
    itemShdr->setText(0, "节表");
    itemRoot->addChild(itemShdr);

    QString s = QString::fromStdString(elfMap.GetHex("all"));
    ui->plainTextEdit->setPlainText(s);
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QString showStr("This sentence should not be displayed\n");

    if(m_isParserIcon)
    {
        if(item->text(column) == elfMap.m_fileName.c_str())
        {
            showStr = QString::fromStdString(elfMap.GetHex("all"));
        }
        else if(item->text(column) == "ELF文件头")
        {
            showStr = QString::fromStdString(elfMap.GetContentELFHead());
        }
        else if(item->text(column) == "程序段表")
        {
            showStr = QString::fromStdString(elfMap.GetContentSectionHead());
        }
        else if(item->text(column) == "节表")
        {
            showStr = QString::fromStdString(elfMap.GetContentProgramHead());
        }
        else
        {
            showStr = QString::fromStdString(elfMap.GetContent(item->text(column).toStdString().c_str()));
        }
    }
    else
    {
        if(item->text(column) == elfMap.m_fileName.c_str())
        {
            showStr = QString::fromStdString(elfMap.GetHex("all"));
        }
        else if(item->text(column) == "ELF文件头")
        {
            showStr = QString::fromStdString(elfMap.GetHex("EFL header"));
        }
        else if(item->text(column) == "程序段表")
        {
            showStr = QString::fromStdString(elfMap.GetHex("program header"));
        }
        else if(item->text(column) == "节表")
        {
            showStr = QString::fromStdString(elfMap.GetHex("section header"));
        }
        else
        {
            showStr = QString::fromStdString(elfMap.GetHex(item->text(column).toStdString().c_str()));
        }
    }
    ui->plainTextEdit->setPlainText(showStr);
}

void MainWindow::on_action_triggered(bool checked)
{
    m_isParserIcon = checked;
    if(true == m_isParserIcon)
    {
        ui->plainTextEdit->setPlainText("当前为解析模式");
    }
    else
    {
        ui->plainTextEdit->setPlainText("当前为二进制模式");
    }
}
