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
    void draw(QVector<QPoint> ap, int, int);
private slots:
    void handleSceneClick(const QPointF& pos); // получаем сигнал о том, что мышка была нажата на сцене и обрабатываем
    void design_set();
    //обработчки нажатия на кнопку
    void on_createPath_but_clicked();
    void on_addObj_but_clicked();
    void on_delObj_but_clicked();
    void on_imp_but_clicked();
    void on_saveMap_but_clicked();
    void on_ok_but_clicked();
    void on_cancel_but_clicked();

private:
    Ui::MainWindow *ui;
    handleClick *scene; //сцена
    int point_count; //количество поставленных точек
    int windStat; //текущее действие пользователя
};
#endif // MAINWINDOW_H
