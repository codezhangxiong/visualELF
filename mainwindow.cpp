#include <QFileDialog>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "elfmap.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    parser_icon = false;
    QString showdemo;
    showdemo += "RVA        0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F  Value   \n";
    showdemo += "00000000  CC CC CC CC CC CC CC CC  CC CC CC CC CC CC CC CC  ........\n";
    showdemo += "00000010  CC CC CC CC CC CC CC CC  CC CC CC CC CC CC CC CC  ........\n";
    showdemo += "00000020  CC CC CC CC CC CC CC CC  CC CC CC CC CC CC CC CC  ........\n";
    showdemo += "00000030  CC CC CC CC CC CC CC CC  CC CC CC CC CC CC CC CC  ........\n";
    ui->plainTextEdit->setPlainText(showdemo);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionopen_triggered()
{
    QString filePathAndName = QFileDialog::getOpenFileName(this,"选择文件");
    QString filename = filePathAndName.section("/",-1,-1);

    if(!elfdata.parser(filePathAndName.toStdString().c_str()))
    {
        ui->plainTextEdit->setPlainText("文件解析失败！");
        return;
    };

    QTreeWidgetItem * itemroot = new QTreeWidgetItem(QTreeWidgetItem::Type);
    itemroot->setText(0, filename);
    ui->treeWidget->addTopLevelItem(itemroot);
    QTreeWidgetItem * itemehdr = new QTreeWidgetItem(QTreeWidgetItem::Type);
    itemehdr->setText(0, "ELF文件头");
    itemroot->addChild(itemehdr);
    QTreeWidgetItem * itemphdr = new QTreeWidgetItem(QTreeWidgetItem::Type);
    itemphdr->setText(0, "程序段表");
    itemroot->addChild(itemphdr);
    int num = 0;
    for(auto a:elfdata.vshdr)
    {
        char name[64] = {0};
        sprintf(name,"[%02d]%s",num,elfdata.shname + a->sh_name);
        QTreeWidgetItem * item = new QTreeWidgetItem(QTreeWidgetItem::Type);
        item->setText(0, name);
        itemroot->addChild(item);
        num++;
    }
    QTreeWidgetItem * itemshdr = new QTreeWidgetItem(QTreeWidgetItem::Type);
    itemshdr->setText(0, "节表");
    itemroot->addChild(itemshdr);

    QString s = QString::fromStdString(elfdata.get_hex("all"));
    ui->plainTextEdit->setPlainText(s);
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QString s("This sentence should not be displayed\n");
    if(parser_icon)
    {
        if(item->text(column) == elfdata.elffilename.c_str())
        {
            s = QString::fromStdString(elfdata.get_hex("all"));
        }
        else if(item->text(column) == "ELF文件头")
        {
            s = QString::fromStdString(elfdata.get_content("EFL header"));
        }
        else if(item->text(column) == "程序段表")
        {
            s = QString::fromStdString(elfdata.get_content("program header"));
        }
        else if(item->text(column) == "节表")
        {
            s = QString::fromStdString(elfdata.get_content("section header"));
        }
        else
        {
            s = QString::fromStdString(elfdata.get_content(item->text(column).toStdString().c_str()));
        }
    }
    else
    {
        if(item->text(column) == elfdata.elffilename.c_str())
        {
            s = QString::fromStdString(elfdata.get_hex("all"));
        }
        else if(item->text(column) == "ELF文件头")
        {
            s = QString::fromStdString(elfdata.get_hex("EFL header"));
        }
        else if(item->text(column) == "程序段表")
        {
            s = QString::fromStdString(elfdata.get_hex("program header"));
        }
        else if(item->text(column) == "节表")
        {
            s = QString::fromStdString(elfdata.get_hex("section header"));
        }
        else
        {
            s = QString::fromStdString(elfdata.get_hex(item->text(column).toStdString().c_str()));
        }
    }
    ui->plainTextEdit->setPlainText(s);
}

void MainWindow::on_action_triggered(bool checked)
{
    parser_icon = checked;
    if(true == parser_icon)
    {
        ui->plainTextEdit->setPlainText("当前为解析模式");
    }
    else
    {
        ui->plainTextEdit->setPlainText("当前为二进制模式");
    }
}
