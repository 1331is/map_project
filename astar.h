#ifndef ASTAR_H
#define ASTAR_H
#include <QGraphicsScene>
#include <QSet>
#include <QVector>
#include <cmath>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

struct Node {
    QPoint pos;
    qreal cost;         // Стоимость достижения узла
    qreal heuristic;    // Эвристическая оценка
    Node* parent = nullptr;

    // Для приоритетной очереди
    bool operator<(const Node& other) const;
};

// Функция расчета стоимости перемещения
qreal getMovementCost(const QPoint& from, const QPoint& to, const QGraphicsScene* scene);

// Функция A* для поиска пути
QVector<QPoint> pathSearch(const QPoint& start, const QPoint& end, const QGraphicsScene* scene);

//оптимизация маршрута
QVector<QPoint> pathOptimis(QVector<QPoint> path, const QGraphicsScene* scene);

#endif // ASTAR_H
