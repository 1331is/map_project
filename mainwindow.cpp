#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "handleclick.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new handleClick(this);
    ui->map_box->setScene(scene);
    scene->setSceneRect(0, 0, 1090, 670); //эти строчки чтобы сцена не растягивался и координатв точек нормальными были
    ui->map_box->setFixedSize(1090, 670);
    ui->map_box->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->map_box->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(scene, &handleClick::sceneClicked,
            this, &MainWindow::handleSceneClick); //коннект
    design_set();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::parcer()
{

    QFile file(":/test.xml");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Ошибка открытия файла!";
        return;
    }

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        qDebug() << "Ошибка парсинга XML!";
        file.close();
        return;
    }

    QDomElement root = doc.documentElement();
    QDomNodeList objects = root.elementsByTagName("object"); // Список всех <object>

    for (int i = 0; i < objects.size(); ++i) {
        QVector<QPoint> arrPointObj;
        QDomElement obj = objects.at(i).toElement();
        QString durability = obj.firstChildElement("durability").text();

        // Чтение точек
        QDomElement points = obj.firstChildElement("points");
        QDomNodeList pointList = points.elementsByTagName("point"); // список всех поинтов
        for (int j = 0; j < pointList.size(); ++j) {
            QDomElement point = pointList.at(j).toElement();
            int x = point.attribute("x").toInt();
            int y = point.attribute("y").toInt();
            QPoint pointCord(x,y);
            arrPointObj.append(pointCord);
            qDebug() << "Точка:" << x << "," << y;
        }
        draw(arrPointObj);

    }
    file.close();
}

void MainWindow::draw(QVector<QPoint> ap)
{
    QPolygon pol;
    if (ap.size() < 3) return;
    for (int i = 0; i < ap.size(); i++) {
        pol << ap[i];
    }
    scene->addPolygon(pol, QPen(Qt::green));
}

void MainWindow::design_set(){
    //настройки дизайна
    ui->mapEdit_box->setStyleSheet(
        "QGroupBox {"
        "   background-color: #9BAC68;"  // Цвет фона
        "   border: 2px solid #818F54;"     // Рамка
        "   border-radius: 15px;"          // Закругление углов
        "}"
        "QGroupBox::title {"
        "   padding: 0 5px;"             // Отступы вокруг текста заголовка
        "   top: 5px"
        "   left: 1px"
        "   font-size: 15px;"
        "}"
        );
    ui->addObj_but->setStyleSheet(
        "QPushButton {"
        "   background-color: #818F54;"  // Цвет фона
        "   border-radius: 15px;"          // Закругление углов
        "}"
        "QPushButton:hover {"
        "   background-color: #627049;  "
        "}"
        );
    ui->delObj_but->setStyleSheet(
        "QPushButton {"
        "   background-color: #818F54;"  // Цвет фона
        "   border-radius: 15px;"          // Закругление углов
        "}"
        "QPushButton:hover {"
        "   background-color: #627049;  "
        "}"
        );
    ui->imp_but->setStyleSheet(
        "QPushButton {"
        "   background-color: #818F54;"  // Цвет фона
        "   border-radius: 15px;"          // Закругление углов
        "}"
        "QPushButton:hover {"
        "   background-color: #627049;  "
        "}"
        );
    //кнопка маршрута
    ui->createPath_but->setStyleSheet(
        "QPushButton {"
        "   background-color: #F5B8DA;"  // Цвет фона
        "   border-radius: 15px;"          // Закругление углов
        "}"
        "QPushButton:hover {"
        "   background-color: #D47F9D;  "
        "}"
        );
    //блок сохранения
    ui->save_box->setStyleSheet(
        "QGroupBox {"
        "   background-color: #F6D868;"  // Цвет фона
        "   border: 2px solid #D9BB42;"     // Рамка
        "   border-radius: 15px;"          // Закругление углов
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   padding: 0 5px;"             // Отступы вокруг текста заголовка
        "   top: 10px"
        "   left: 5px"
        "}"
        );
    ui->saveMap_but->setStyleSheet(
        "QPushButton {"
        "   background-color: #D9BB42;"  // Цвет фона
        "   border-radius: 15px;"          // Закругление углов
        "}"
        "QPushButton:hover {"
        "   background-color: #D6A83F;  "
        "}"
        );
    ui->savePath_but->setStyleSheet(
        "QPushButton {"
        "   background-color: #D9BB42;"  // Цвет фона
        "   border-radius: 15px;"          // Закругление углов
        "}"
        "QPushButton:hover {"
        "   background-color: #D6A83F;  "
        "}"
        );

    ui->map_box->setStyleSheet(
        "QGraphicsView {"
        "   background-color: #FFFFFF;"  // Цвет фона
        "   border: 2px solid #EDE7D9;"
        "   border-radius: 15px;"          // Закругление углов
        "}"
        );
}

void MainWindow::handleSceneClick(const QPointF& pos) {
    QPixmap icon(":/heart.png"); //картинка из папочки ресурсы ебать
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

void MainWindow::on_createPath_but_clicked()
{
    if (arrpoint.size() >= 2) {
        QPoint p1 = arrpoint[arrpoint.size() - 2];
        QPoint p2 = arrpoint[arrpoint.size() - 1];
        scene->addLine(p1.x(), p1.y(), p2.x(), p2.y(), QPen(Qt::red, 2)); //две последнии точки соединяет
    }
}


void MainWindow::on_imp_but_clicked()
{
    parcer();
}

