#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QtWidgets/QTreeWidgetItem>
#include "ELFMap.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    ELFMap elfMap;
    bool m_isParserIcon;

private:
    Ui::MainWindow *ui;

private slots:
    void on_actionopen_triggered();
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void on_action_triggered(bool checked);
};

#endif // MAINWINDOW_H
