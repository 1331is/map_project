#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QGraphicsScene>
#include <QDebug>
#include <QGraphicsPixmapItem>
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
private slots:
    void on_pushButton_pressed(); //Просто потестить кнопка
    void handleSceneClick(const QPointF& pos); // получаем сигнал о том, что мышка была нажата на сцене и обрабатываем
    void on_pushButton_clicked(); //Просто потестить кнопка

private:
    Ui::MainWindow *ui;
    handleClick *scene; //сцена ебать
};
#endif // MAINWINDOW_H
