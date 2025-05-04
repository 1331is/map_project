#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QStatusBar>
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QDomDocument>
#include <QFile>
#include "obj.h"
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

private slots:
    void handleSceneClick(const QPointF& pos); // получаем сигнал о том, что мышка была нажата на сцене и обрабатываем
    void on_createPath_but_clicked();
    void on_addObj_but_clicked();
    void on_delObj_but_clicked();
    void on_imp_but_clicked();
    void on_saveMap_but_clicked();
    void on_ok_but_clicked();
    void on_cancel_but_clicked();
    void on_mapWidth_edit_Return_Pressed();
    void on_mapHeight_edit_Return_Pressed();
    void on_scaleDown_but_clicked();
    void on_scaleUp_but_clicked();
    void on_savePath_but_clicked();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override; //обработчик события: курсор на карте

private:
    Ui::MainWindow *ui;
    handleClick *scene; //сцена
    int windStat; //текущее действие пользователя
    float scale;
    QVector<QPoint> arrpoint; //массив точек
    QVector<Obj> arrobj; //массив объектов
    QVector<QPoint> path;
    void parcer(QString);
    QDomDocument pathFileMaker(qreal, qreal, qreal);
    QDomDocument mapFileMaker();
    void draw(Obj object);
    void design_set();

};
#endif // MAINWINDOW_H
