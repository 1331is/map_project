#include "astar.h"
#include <queue>

// Функция расчета стоимости перемещения
qreal getMovementCost(const QPoint& from, const QPoint& to, const QGraphicsScene* scene) {
    //Проверка пересечения с полигонами
    qreal totalCost = 1.0;

    QList<QGraphicsPolygonItem*> polygons;
    foreach(QGraphicsItem* item, scene->items()) {
        if (QGraphicsPolygonItem* polygon = dynamic_cast<QGraphicsPolygonItem*>(item)) {
            polygons.append(polygon);
        }
    }
    for (int j=0;j<polygons.size();j++) {
        if(polygons[j]->contains(to)) {
            qreal obstruction = polygons[j]->brush().color().alpha();
            obstruction /=255;
            if (obstruction >= 1.0)
                return -1; // Полная непроходимость
            totalCost += obstruction;
        }
    }
    return totalCost;
}

// Функция A* для поиска пути
QVector<QPoint> pathSearch(const QPoint& start, const QPoint& end, const QGraphicsScene* scene) {
    QHash<QPoint, Node*> allNodes;
    QHash<qreal, Node*> sortNodes;
    std::priority_queue<qreal, std::vector<qreal>, std::greater<qreal>> sortOpenSet;
    QSet<QPoint> closedSet;

    // 2. Создание начального узла
    Node* startNode = new Node{start, 0, 0, nullptr};
    startNode->heuristic = QLineF(start, end).length();
    allNodes[start] = startNode;
    sortNodes[startNode->heuristic+startNode->cost] = startNode;
    sortOpenSet.push(startNode->heuristic+startNode->cost);

    // 3. Направления движения (8-соседняя сетка)
    const QList<QPoint> directions = {
        QPoint(-1, 0), QPoint(1, 0), QPoint(0, -1), QPoint(0, 1),
        QPoint(-1, -1), QPoint(-1, 1), QPoint(1, -1), QPoint(1, 1)
    };

    // 4. Основной цикл алгоритма
    while (!sortOpenSet.empty()) {
        //Node* current = openSet.top();
        Node* current = sortNodes.value(sortOpenSet.top());
        sortOpenSet.pop();

        // Цель достигнута
        if (current->pos == end) {
            QVector<QPoint> path;
            while (current) {
                path.prepend(current->pos);
                current = current->parent;
            }

            // Очистка памяти
            qDeleteAll(allNodes);
            return path;
        }

        closedSet.insert(current->pos);

        // 5. Обработка соседей
        for (const QPoint& dir : directions) {
            QPoint neighborPos = current->pos + dir;


            // Проверка границ сцены
            if (neighborPos.x() < 0 || neighborPos.x() >= scene->width() ||
                neighborPos.y() < 0 || neighborPos.y() >= scene->height()) {
                continue;
            }

            // Проверка на препятствия
            qreal moveCost = getMovementCost(current->pos, neighborPos, scene);
            if (moveCost == -1) continue; // Непроходимый узел

            // Расчет стоимости
            qreal newCost = current->cost + moveCost * (dir.x() && dir.y() ? 1.4 : 1.0);

            // Создание или обновление узла
            Node* neighbor = allNodes.value(neighborPos, nullptr);
            if (!neighbor) {
                neighbor = new Node{neighborPos,
                                    std::numeric_limits<qreal>::max(),
                                    QLineF(neighborPos, end).length(),
                                    nullptr};
                allNodes[neighborPos] = neighbor;
            }

            if (newCost < neighbor->cost) {
                neighbor->cost = newCost;
                neighbor->parent = current;

                if (!closedSet.contains(neighborPos)) {
                    sortOpenSet.push(neighbor->cost+neighbor->heuristic);
                    sortNodes[neighbor->cost+neighbor->heuristic] = neighbor;
                }
            }
        }
    }

    // Путь не найден
    qDeleteAll(allNodes);
    return QVector<QPoint>();
}

//удаление лишних точек с одной прямой
QVector<QPoint> pointExcessDel(QVector<QPoint> path) {
    QVector<QPoint> optimizedPath;
    optimizedPath.append(path.first());
    for (int i = 0; i<path.size()-2;i++){
        if (path[i]!=optimizedPath.last())
            continue;
        for (int j=i+1;j<path.size()-1;j++){
            QPoint dir (path[j].x()-path[i].x(), path[j].y()-path[i].y());
            if (!((abs(dir.x()) == abs(dir.y())) || (dir.x() == 0) || (dir.y() == 0))) {
                optimizedPath.append(path[j-1]);
                break;
            }
        }
    }
    optimizedPath.append(path.last());
    return optimizedPath;
}

//удаление лишних точек из 8-направленного пути
QVector<QPoint> dirPointDel(QVector<QPoint> path, QList<QGraphicsPolygonItem*> polygons) {

    //определяем принадлежность точек к полигонам
    QHash<QPoint, int> allPoints;
    for (int i=0;i<path.size();i++) {
        for (int j = 0; j < polygons.size(); ++j) {
            if (polygons[j]->contains(path[i]))
                allPoints[path[i]]=polygons[j]->brush().color().alpha();
        }
        if (!allPoints.contains(path[i]))
            allPoints[path[i]]=0;
    }

    QVector<QPoint> optimizedPath;
    optimizedPath.append(path.first());
    for (int i=0;i<path.size()-2;i++) {
        if (path[i]!=optimizedPath.last())
            continue;
        for (int k=i+1;k<path.size()-1;k++) {
            for (int j = 0; j < polygons.size(); ++j) {
                //рисуем линию от точки до точки и проверяем пересекает ли она сторону полигона
                QPolygonF polygon = polygons[j]->mapToScene(polygons[j]->polygon());
                QPainterPath polygonPath;
                polygonPath.addPolygon(polygon);

                QPainterPath linePath;
                linePath.moveTo(path[i]);
                linePath.lineTo(path[k]);
                QPainterPathStroker stroker;
                stroker.setWidth(1);
                QPainterPath strokedLine = stroker.createStroke(linePath);


                // Проверяем пересечение
                if (strokedLine.intersects(polygonPath)) {
                    //если обе точки не принадлежат полигону
                    if (allPoints[path[i]]!=polygons[j]->brush().color().alpha()&&allPoints[path[k]]!=polygons[j]->brush().color().alpha()) {
                        if (k-1!=i) {
                            optimizedPath.append(path[k-1]);
                            i = k-2;
                            break;
                        }
                        else {
                            optimizedPath.append(path[k]);
                            i = k-1;
                            break;
                        }
                    }
                }
            }
        }
    }
    optimizedPath.append(path.last());
    return optimizedPath;
}

//оптимизация маршрута
QVector<QPoint> pathOptimis(QVector<QPoint> path, const QGraphicsScene* scene) {
    qDebug() << "Пришло точек " << path.size();
    //удаление лишних точек с одной прямой
    path = pointExcessDel(path);
    qDebug() << "Стало точек " << path.size();

    //удаление лишних точек из 8-направленного пути
    QList<QGraphicsPolygonItem*> polygons;
    foreach(QGraphicsItem* item, scene->items()) {
        if (QGraphicsPolygonItem* polygon = dynamic_cast<QGraphicsPolygonItem*>(item)) {
            polygons.append(polygon);
        }
    }
    path = dirPointDel(path, polygons);
    qDebug() << "Ушло точек " << path.size();

    return path;
}

