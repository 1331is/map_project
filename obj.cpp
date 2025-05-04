#include "obj.h"

Obj::Obj(QVector<QPoint> pointarr, int dur) {
    points = pointarr;
    transp = dur;
}

int Obj::point_numb() {
    return points.size();
}

QVector<QPoint> Obj::get_points() {
    return points;
}

int Obj::get_transp() {
    return transp;
}
