#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "handleclick.h"
#include <QIntValidator>
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
    windStat = 0; //статус окна: ожидание действий
}

//деструктор класса
MainWindow::~MainWindow()
{
    delete ui;
}

//парсинг XML файла
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
        draw(arrPointObj, arrPointObj.size(), 255);

    }
    file.close();
}

//изображение фигуры на карте
void MainWindow::draw(QVector<QPoint> ap, int size, int transp)
{
    QPolygon pol;
    if (ap.size() < 3) return;
    for (int i = ap.size()-size; i < ap.size(); i++) {
        pol << ap[i];
    }

    QColor penC(129, 143, 84, 255);
    QPen pen(penC, 3);
    QColor brush(155, 172, 104, transp*255/100);
    scene->addPolygon(pol, pen,  QBrush(brush));
}


//настройка дизайна
void MainWindow::design_set(){

    ui->mapEdit_box->setStyleSheet(
        "QGroupBox {"
        "   background-color: #9BAC68;"  // Цвет фона
        "   border-radius: 15px;"          // Закругление углов
        "}"
        "QGroupBox::title {"
        "   padding: 0 5px;"             // Отступы вокруг текста заголовка
        "   top: 1px;"
        "   left: 39px;"
        "}"
        );
    QFont font = ui->mapEdit_box->font();
    font.setPointSize(18);  // Устанавливаем размер в пунктах
    ui->mapEdit_box->setFont(font);
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
    ui->saveMap_but->setStyleSheet(
        "QPushButton {"
        "   background-color: #818F54;"  // Цвет фона
        "   border-radius: 15px;"          // Закругление углов
        "}"
        "QPushButton:hover {"
        "   background-color: #627049;  "
        "}"
        );

    ui->map_box->setStyleSheet(
        "QGraphicsView {"
        "   background-color: #FFFFFF;"  // Цвет фона
        "   border: 2px solid #EDE7D9;"
        "   border-radius: 15px;"          // Закругление углов
        "}"
        );
    //кнопки ок и отмена
    ui->ok_but->setStyleSheet(
        "QPushButton {"
        "   background-color: #F6D868;"  // Цвет фона
        "   border-radius: 15px;"          // Закругление углов
        "}"
        "QPushButton:hover {"
        "   background-color: #D6A83F;  "
        "}"
        );
    ui->cancel_but->setStyleSheet(
        "QPushButton {"
        "   background-color: #F6D868;"  // Цвет фона
        "   border-radius: 15px;"          // Закругление углов
        "}"
        "QPushButton:hover {"
        "   background-color: #D6A83F;  "
        "}"
        );
    ui->ok_but->setVisible(0);
    ui->cancel_but->setVisible(0);
    ui->transp_box->setVisible(0);
    ui->transp_label->setVisible(0);
    ui->transp_box->setValidator(new QIntValidator(this));
}

//добавление точки на карту
void MainWindow::handleSceneClick(const QPointF& pos) {
    QPixmap icon(":/heart.png"); //картинка из папочки ресурсы
    if (icon.isNull()) {
        qDebug() << "Картинки нет";
        return;
    }
    icon = icon.scaled(32, 32, Qt::KeepAspectRatio); //размер картинки балансить
    QGraphicsPixmapItem* marker = scene->addPixmap(icon);
    marker->setPos(pos.x() - icon.width()/2, pos.y() - icon.height());
    arrpoint.append(pos.toPoint()); // записывает точки при клике

    qDebug() << "Точки:" << arrpoint;

    point_count++;
    //если сейчас задается маршрут
    if (windStat==3) {
        if (point_count == 1) {
            //добавить подпись "начало" рядом с меткой
        }
        if (point_count == 2) {
            //добавить подпись "конец" рядом с меткой
            ui->map_box->setEnabled(0);
            ui->ok_but->setVisible(1);
        }
    }
    if (point_count == 3) {
        ui->ok_but->setVisible(1);
        ui->transp_box->setVisible(1);
        ui->transp_label->setVisible(1);
    }
}

//нажатие на кнопку задать маршрут
void MainWindow::on_createPath_but_clicked()
{
    //включение карты, отключение всех кнопок
    ui->map_box->setEnabled(1);
    ui->delObj_but->setEnabled(0);
    ui->createPath_but->setEnabled(0);
    ui->imp_but->setEnabled(0);
    ui->saveMap_but->setEnabled(0);
    ui->addObj_but->setEnabled(0);
    ui->cancel_but->setVisible(1);
    //удалить прошлый маршрут

    point_count = 0;
    windStat = 3; //статус окна: добавление маршрута
}

//нажатие на кнопку добавить объект
void MainWindow::on_addObj_but_clicked() {
    //включение карты, отключение всех кнопок
    ui->map_box->setEnabled(1);
    ui->delObj_but->setEnabled(0);
    ui->createPath_but->setEnabled(0);
    ui->imp_but->setEnabled(0);
    ui->saveMap_but->setEnabled(0);
    ui->addObj_but->setEnabled(0);
    ui->cancel_but->setVisible(1);
    point_count = 0;
    windStat = 1; //статус окна: добавление объекта
}

//нажатие на кнопку удалить объект
void MainWindow::on_delObj_but_clicked() {
    //включение карты, отключение всех кнопок
    ui->map_box->setEnabled(1);
    ui->delObj_but->setEnabled(0);
    ui->createPath_but->setEnabled(0);
    ui->imp_but->setEnabled(0);
    ui->saveMap_but->setEnabled(0);
    ui->addObj_but->setEnabled(0);
    ui->cancel_but->setVisible(1);
    windStat = 2; //статус окна: удаление объекта
}

//нажатие на кнопку сохранить карту
void MainWindow::on_saveMap_but_clicked() {

}

//нажатие на кнопку ок
void MainWindow::on_ok_but_clicked(){
    //отключение карты, включение всех кнопок
    ui->map_box->setEnabled(0);
    ui->delObj_but->setEnabled(1);
    ui->createPath_but->setEnabled(1);
    ui->imp_but->setEnabled(1);
    ui->saveMap_but->setEnabled(1);
    ui->addObj_but->setEnabled(1);
    ui->cancel_but->setVisible(0);
    ui->ok_but->setVisible(0);
    ui->transp_box->setVisible(0);
    ui->transp_label->setVisible(0);
    switch (windStat) {
        //пользователь добавил объект
    case 1: {
        //удаление изображений точек
        QPixmap icon(":/heart.png");
        icon = icon.scaled(32, 32, Qt::KeepAspectRatio);
        for (int i = 1; i<=point_count; i++) {
            if (!arrpoint.isEmpty()) {
                for (QGraphicsItem* item : scene->items()) {
                    if (item->type() == QGraphicsPixmapItem::Type) {
                        QGraphicsPixmapItem* pixmapItem = static_cast<QGraphicsPixmapItem*>(item);
                        if ((pixmapItem->pos().x() == arrpoint[arrpoint.size()-i].x() - icon.width()/2) && (pixmapItem->pos().y() == arrpoint[arrpoint.size()-i].y() - icon.height())) {
                            delete pixmapItem;
                        }
                    }
                }

            }
        }
        //рисование объекта
        bool ok;
        int transp = ui->transp_box->text().toInt(&ok);
        draw(arrpoint, point_count, transp);
        point_count=0;
        break;
    }
        //пользователь удалил объект
    case 2:
        //удалить объект
        break;
        //пользователь задал маршрут
    case 3: {
        //построить маршрут
        QPoint p1 = arrpoint[arrpoint.size() - 2];
        QPoint p2 = arrpoint[arrpoint.size() - 1];
        scene->addLine(p1.x(), p1.y(), p2.x(), p2.y(), QPen(Qt::red, 2));
        break;
    }
    }
    windStat = 0; //статус окна: ожидание действий
}

//нажатие на кнопку отмены
void MainWindow::on_cancel_but_clicked() {
    //отключение карты, включение всех кнопок
    ui->map_box->setEnabled(0);
    ui->delObj_but->setEnabled(1);
    ui->createPath_but->setEnabled(1);
    ui->imp_but->setEnabled(1);
    ui->saveMap_but->setEnabled(1);
    ui->addObj_but->setEnabled(1);
    ui->cancel_but->setVisible(0);
    ui->ok_but->setVisible(0);
    ui->transp_box->setVisible(0);
    ui->transp_label->setVisible(0);
    //удаление добавленных точек
    QPixmap icon(":/heart.png");
    icon = icon.scaled(32, 32, Qt::KeepAspectRatio);
    for (int i = 0; i<point_count; i++) {
        if (!arrpoint.isEmpty()) {
            for (QGraphicsItem* item : scene->items()) {
                if (item->type() == QGraphicsPixmapItem::Type) {
                    QGraphicsPixmapItem* pixmapItem = static_cast<QGraphicsPixmapItem*>(item);
                    if ((pixmapItem->pos().x() == arrpoint.last().x() - icon.width()/2) && (pixmapItem->pos().y() == arrpoint.last().y() - icon.height())) {
                        delete pixmapItem;
                        arrpoint.removeLast();
                    }
                }
            }

        }
    }
    point_count=0;
    windStat = 0; //статус окна: ожидание действий
}

//нажатие на кнопку импорт карту
void MainWindow::on_imp_but_clicked()
{
    parcer();
}

