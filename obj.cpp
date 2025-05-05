#include "obj.h"

Obj::Obj(QPolygonF polygon, int dur) {
    pol = polygon;
    transp = dur;
}

int Obj::point_numb() {
    QVector<QPointF> points;
    for (const QPointF &point : pol) {
        points.append(point);
    }
    return points.size();
}

QVector<QPointF> Obj::get_points() {
    QVector<QPointF> points;
    for (const QPointF &point : pol) {
        points.append(point);
    }
    return points;
}

QPolygonF Obj::get_pol() {
    return pol;
}

int Obj::get_transp() {
    return transp;
}
