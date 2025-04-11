#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "handleclick.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new handleClick(this);
    ui->graphicsView->setScene(scene);
    scene->setSceneRect(0, 0, 800, 600); //эти строчки чтобы сцена не растягивался и координатв точек нормальными были
    ui->graphicsView->setFixedSize(800, 600);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(scene, &handleClick::sceneClicked,
            this, &MainWindow::handleSceneClick); //коннект ебать
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_pressed()
{

}

void MainWindow::handleSceneClick(const QPointF& pos) {
    QPixmap icon(":/finish(1).png"); //картинка из папочки ресурсы ебать
    if (icon.isNull()) { //у вас винда не ебу сработает или нет а так хоть узнаем
        qDebug() << "Картинки нет";
        return;
    }
    icon = icon.scaled(32, 32, Qt::KeepAspectRatio); //размер картинки балансить
    QGraphicsPixmapItem* marker = scene->addPixmap(icon);
    marker->setPos(pos.x() - icon.width()/2, pos.y() - icon.height());
    arrpoint.append(pos.toPoint()); // записывает точки при клике

    qDebug() << "Точки:" << arrpoint;
}

void MainWindow::on_pushButton_clicked()
{
    if (arrpoint.size() >= 2) {
        QPoint p1 = arrpoint[arrpoint.size() - 2];
        QPoint p2 = arrpoint[arrpoint.size() - 1];
        scene->addLine(p1.x(), p1.y(), p2.x(), p2.y(), QPen(Qt::red, 2)); //две последнии точки соединяет
    }
}

