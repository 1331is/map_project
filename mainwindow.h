#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QGraphicsScene>
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QDomDocument>
#include <QFile>
#include "handleclick.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QVector<QPoint> arrpoint; //здесь хранить точки и потом по ним отрисовывать маршрут
    void parcer();
    void draw(QVector<QPoint> ap);
private slots:
    void handleSceneClick(const QPointF& pos); // получаем сигнал о том, что мышка была нажата на сцене и обрабатываем
    void design_set();
    void on_createPath_but_clicked();

    void on_imp_but_clicked();

private:
    Ui::MainWindow *ui;

    handleClick *scene; //сцена ебать
};
#endif // MAINWINDOW_H
