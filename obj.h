#ifndef OBJ_H
#define OBJ_H
#include <QPoint>
#include <QPolygon>
#include <QVector>

class Obj
{
public:
    Obj(QPolygonF polygon, int dur);
    int point_numb();
    int get_transp();
    QVector<QPointF> get_points();
    QPolygonF get_pol();
private:
    QPolygonF pol;
    int transp;
};

#endif // OBJ_H
