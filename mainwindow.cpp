#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "handleclick.h"
#include <QIntValidator>
#include <QDomDocument>
#include <QFile>
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //настройки сцены
    scene = new handleClick(this);
    ui->map_box->setScene(scene);
    scene->setSceneRect(0, 0, 1090, 670); //чтобы сцена не растягивался и координатв точек нормальными были
    ui->map_box->setFixedSize(1090, 670);
    ui->map_box->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->map_box->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    connect(scene, &handleClick::sceneClicked,
            this, &MainWindow::handleSceneClick); //коннект
    connect(ui->mapWidth_edit, &QLineEdit::returnPressed, this, &MainWindow::on_mapWidth_edit_Return_Pressed);
    connect(ui->mapHeight_edit, &QLineEdit::returnPressed, this, &MainWindow::on_mapHeight_edit_Return_Pressed);

    design_set();
    scale = 1;
    windStat = 0; //статус окна: ожидание действий
}




//вывод координат мыши в строку состояния
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj->isWidgetType() && event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QPointF scenePos = static_cast<QGraphicsView *>(obj)->mapToScene(mouseEvent->pos());
        statusBar()->showMessage(QString("Масштаб: %1px/m  |  Координаты: (%2, %3)").arg(scale).arg(scenePos.x()).arg(scenePos.y()));
        return true;
    }
    return QMainWindow::eventFilter(obj, event); // Передаем остальные события родителю
}

//настройка дизайна
void MainWindow::design_set(){

    //настройки строки состояния
    statusBar()->showMessage("Масштаб: 1px/m  |  Координаты: (0, 0)");
    ui->map_box->setMouseTracking(true); // Включаем отслеживание мыши
    ui->map_box->installEventFilter(this);

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
    ui->mapWidth_edit->setValidator(new QIntValidator(this));
    ui->mapHeight_edit->setValidator(new QIntValidator(this));
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

    switch(windStat){
        //сейчас задается новый объект
    case 1:
        if (arrpoint.size() == 3) {
            ui->ok_but->setVisible(1);
            ui->transp_box->setVisible(1);
            ui->transp_label->setVisible(1);
        }
        break;
        //сейчас выбирается объект для удаления
    case 2:
        if (arrpoint.size() == 1)
            ui->ok_but->setVisible(1);
        break;
        //сейчас задается маршрут
    case 3:
        if (arrpoint.size() == 1) {
            //добавить подпись "начало" рядом с меткой
        }
        if (arrpoint.size() == 2) {
            //добавить подпись "конец" рядом с меткой
            ui->map_box->setEnabled(0);
            ui->ok_but->setVisible(1);
        }
        break;
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

    windStat = 3; //статус окна: добавление маршрута
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
        for (int i = 1; i<=arrpoint.size(); i++) {
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

        bool ok;
        int transp = ui->transp_box->text().toInt(&ok);
        arrobj.append(Obj(arrpoint, transp)); //добавление объекта в массив

        draw(arrobj.last()); //рисование объекта
        arrpoint.clear();
        break;
    }
        //пользователь удалил объект
    case 2: {
        //получаем массив всех полигонов
        QList<QGraphicsPolygonItem*> polygons;
        foreach(QGraphicsItem* item, scene->items()) {
            if (QGraphicsPolygonItem* polygon = dynamic_cast<QGraphicsPolygonItem*>(item)) {
                polygons.append(polygon);
            }
        }
        for (int i=0;i<arrpoint.size();i++){
            for (int j=0;j<polygons.size();j++) {
                if(polygons[j]->contains(arrpoint[i])) {
                    scene->removeItem(polygons[j]); // Удаляем из сцены
                    polygons.removeAt(j);
                }
            }
        }
        //удаление изображений точек
        QPixmap icon(":/heart.png");
        icon = icon.scaled(32, 32, Qt::KeepAspectRatio);
        for (int i = 1; i<=arrpoint.size(); i++) {
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
        arrpoint.clear();
        break;
    }
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



//ЗАКОНЧЕННЫЕ ФУНКЦИИ
//В РЕДАКТИРОВАНИИ НЕ НУЖДАЮТСЯ

//нажатие на кнопку импорт карту
void MainWindow::on_imp_but_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        nullptr,                     // Родительское окно
        "Выберите файл",             // Заголовок окна
        QDir::homePath(),           // Начальная директория (домашняя папка)
        "XML файлы (*.xml)"  // Фильтры файлов
        );

    if (!filePath.isEmpty()) {
        qDebug() << "Выбран файл:" << filePath;
        // обработка файла
        parcer(filePath);
    }

}

//нажатие на кнопку сохранить карту
void MainWindow::on_saveMap_but_clicked() {
    QDomDocument doc;

    //создаем корневой элемент
    QDomElement root = doc.createElement("map");
    doc.appendChild(root);

    //разрешение
    QDomElement resolution = doc.createElement("resolution");
    resolution.setAttribute("x", QString::number(scene->sceneRect().width()));
    resolution.setAttribute("y", QString::number(scene->sceneRect().height()));
    root.appendChild(resolution);

    //масштаб
    QDomElement scaleElement = doc.createElement("scale");
    scaleElement.appendChild(doc.createTextNode(QString::number(scale)));
    root.appendChild(scaleElement);

    //контейнер объектов
    QDomElement objectsElement = doc.createElement("objects");
    root.appendChild(objectsElement);

    for (int i=0;i<arrobj.size();i++){
        //заголовок
        QDomElement objectElement = doc.createElement("object");
        objectElement.setAttribute("id", QString::number(i+1));
        objectsElement.appendChild(objectElement);

        //проходимость
        QDomElement durabilityElement = doc.createElement("durability");
        durabilityElement.appendChild(doc.createTextNode(QString::number(arrobj[i].get_transp())+"%"));
        objectElement.appendChild(durabilityElement);

        //точки
        QDomElement pointsElement = doc.createElement("points");
        objectElement.appendChild(pointsElement);
        for (const QPoint& p : arrobj[i].get_points()) {
            QDomElement pointElement = doc.createElement("point");
            pointElement.setAttribute("x", QString::number(p.x()));
            pointElement.setAttribute("y", QString::number(p.y()));
            pointsElement.appendChild(pointElement);
        }
    }

    // Сохраняем в файл
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить файл"), "", tr("XML файлы (*.xml)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return;
        }
        QTextStream out(&file);
        out << doc.toString(4);
        file.close();
    }
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
    for (int i = 0; i<arrpoint.size(); i++) {
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
    arrpoint.clear();
    windStat = 0; //статус окна: ожидание действий
}

//ввод ширины
void MainWindow::on_mapWidth_edit_Return_Pressed()
{
    QString text = ui->mapWidth_edit->text();
    int x = text.toInt();
    scene->setSceneRect(0, 0, x, scene->sceneRect().height());
    if (x<1090)
        ui->map_box->setFixedSize(x, scene->sceneRect().height());
}

//ввод высоты
void MainWindow::on_mapHeight_edit_Return_Pressed()
{
    QString text = ui->mapHeight_edit->text();
    int y = text.toInt();
    scene->setSceneRect(0, 0, scene->sceneRect().width(), y);
    if (y<670)
        ui->map_box->setFixedSize(scene->sceneRect().width(), y);
}

//отдаление
void MainWindow::on_scaleDown_but_clicked() {
    scale-=0.2;
    ui->map_box->setTransform(QTransform::fromScale(scale, scale));
}

//приближение
void MainWindow::on_scaleUp_but_clicked() {
    scale+=0.2;
    ui->map_box->setTransform(QTransform::fromScale(scale, scale));
}


//деструктор класса
MainWindow::~MainWindow()
{
    delete ui;
}

//изображение объекта на карте
void MainWindow::draw(Obj object)
{
    QPolygon pol;
    if (object.point_numb() < 3) return;
    for (int i = 0; i < object.point_numb(); i++) {
        pol << object.get_points()[i];
    }

    QColor penC(129, 143, 84, 255);
    QPen pen(penC, 3);
    QColor brush(155, 172, 104, object.get_transp()*255/100);
    scene->addPolygon(pol, pen,  QBrush(brush));
}

//парсинг XML файла
void MainWindow::parcer(QString filePath)
{

    QFile file(filePath);
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

    //очищаем сцену
    scene->clear();
    arrobj.clear();

    QDomElement root = doc.documentElement();
    //разрешение карты
    QDomElement resolution = root.firstChildElement("resolution");
    if (!resolution.isNull()) {
        int x = resolution.attribute("x").toInt();
        int y = resolution.attribute("y").toInt();
        scene->setSceneRect(0, 0, x, y);
        if (x<1090)
            ui->map_box->setFixedSize(x, scene->sceneRect().height());
        if (y<670)
            ui->map_box->setFixedSize(scene->sceneRect().width(), y);
        qDebug() << "Разрешение: " << x << 'x' << y;
    } else {
        qDebug() << "Элемент resolution не найден";
    }

    //масштаб карты
    //х пикселей на метр
    QDomElement scaleElement = root.firstChildElement("scale");
    if (!scaleElement.isNull()) {
        int scaleValue = scaleElement.text().toInt();
        ui->map_box->scale(scaleValue, scaleValue);
        qDebug() << "Масштаб: " << scaleValue;
    } else {
        qDebug() << "Элемент scale не найден";
    }


    //работа с объектами
    QDomNodeList objects = root.elementsByTagName("object"); // Список всех <object>
    for (int i = 0; i < objects.size(); ++i) {
        QVector<QPoint> arrPointObj;
        QDomElement obj = objects.at(i).toElement();

        //значение непроходимости
        QString durability = obj.firstChildElement("durability").text();
        durability.remove('%');
        bool ok;
        int transp = durability.toInt(&ok);

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

        //добавление объекта в массив объектов
        arrobj.append(Obj(arrPointObj, transp));

        //рисование объекта
        draw(arrobj.last());

    }
    file.close();
}
