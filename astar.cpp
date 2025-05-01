#include "astar.h"
#include <queue>


bool Node::operator<(const Node& other) const {
    return (cost + heuristic) < (other.cost + other.heuristic);
}

// Функция расчета стоимости перемещения
qreal getMovementCost(const QPoint& from, const QPoint& to, const QGraphicsScene* scene) {
    //Проверка пересечения с полигонами
    qreal totalCost = 0.0;

    QList<QGraphicsPolygonItem*> polygons;
    foreach(QGraphicsItem* item, scene->items()) {
        if (QGraphicsPolygonItem* polygon = dynamic_cast<QGraphicsPolygonItem*>(item)) {
            polygons.append(polygon);
        }
    }
    for (int j=0;j<polygons.size();j++) {
        if(polygons[j]->contains(to)) {
            qreal obstruction = polygons[j]->brush().color().alpha()/255;
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
    std::priority_queue<Node*, std::vector<Node*>, std::greater<Node*>> openSet;
    std::priority_queue<qreal, std::vector<qreal>, std::greater<qreal>> sortOpenSet;
    QSet<QPoint> closedSet;



    // 2. Создание начального узла
    Node* startNode = new Node{start, 0, 0, nullptr};
    startNode->heuristic = QLineF(start, end).length();
    allNodes[start] = startNode;
    sortNodes[startNode->heuristic+startNode->cost] = startNode;
    openSet.push(startNode);
    sortOpenSet.push(startNode->heuristic+startNode->cost);

    // 3. Направления движения (8-соседняя сетка)
    const QList<QPoint> directions = {
        QPoint(-1, 0), QPoint(1, 0), QPoint(0, -1), QPoint(0, 1),
        QPoint(-1, -1), QPoint(-1, 1), QPoint(1, -1), QPoint(1, 1)
    };

    // 4. Основной цикл алгоритма
    while (!openSet.empty()) {
        //Node* current = openSet.top();
        Node* current = sortNodes.value(sortOpenSet.top());
        openSet.pop();
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
                    openSet.push(neighbor);
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

//оптимизация маршрута
QVector<QPoint> pathOptimis(QVector<QPoint> path, const QGraphicsScene* scene) {
    qDebug() << "Пришло точек " << path.size();
    //удаление точек с одной прямой
    QVector<QPoint> optimizedPath;
    optimizedPath.append(path.first());
    int k = 0;
    for (int i = 0; i<path.size()-2;i++){
        if (path[i]!=optimizedPath.last())
            continue;
        for (int j=i+1;j<path.size()-1;j++){
            QPoint dir (path[j].x()-path[i].x(), path[j].y()-path[i].y());
            if (!((abs(dir.x()) == abs(dir.y())) || (dir.x() == 0) || (dir.y() == 0))) {
                optimizedPath.append(path[j]);
                break;
            }
        }
    }
    optimizedPath.append(path.last());
    qDebug() << "Стало точек " << optimizedPath.size();
    path.clear();
    path.append(optimizedPath.first());
    //проверка на пересечения с полигонами
    QList<QGraphicsPolygonItem*> polygons;
    foreach(QGraphicsItem* item, scene->items()) {
        if (QGraphicsPolygonItem* polygon = dynamic_cast<QGraphicsPolygonItem*>(item)) {
            polygons.append(polygon);
        }
    }
    QHash<QPoint, int> allPoints;
    for (int i=0;i<optimizedPath.size();i++) {
        for (int j = 0; j < polygons.size(); ++j) {
            if (polygons[j]->contains(optimizedPath[i]))
                allPoints[optimizedPath[i]]=polygons[j]->brush().color().alpha();
        }
        if (!allPoints.contains(optimizedPath[i]))
            allPoints[optimizedPath[i]]=0;
    }

    for (int i=0;i<optimizedPath.size()-2;i++) {
        for (int k=i+1;k<optimizedPath.size()-1;k++) {
            for (int j = 0; j < polygons.size(); ++j) {
                // Получаем полигон в координатах сцены
                QPolygonF polygon = polygons[j]->mapToScene(polygons[j]->polygon());

                QPainterPath linePath;
                linePath.moveTo(optimizedPath[i]);
                linePath.lineTo(optimizedPath[k]);
                QPainterPathStroker stroker;
                stroker.setWidth(1); // Минимальная толщина для точного пересечения
                QPainterPath strokedLine = stroker.createStroke(linePath);
                QPainterPath polygonPath;
                polygonPath.addPolygon(polygon);

                // Проверяем пересечение
                if (strokedLine.intersects(polygonPath)) {
                    if (allPoints[optimizedPath[i]]!=polygons[j]->brush().color().alpha()) {
                        path.append(optimizedPath[k-1]);
                        i=k-2;
                    }
                }
            }
        }
    }
    path.append(optimizedPath.last());
    qDebug() << "Ушло точек " << path.size();
    return path;
}

