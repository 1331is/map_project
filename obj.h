#ifndef OBJ_H
#define OBJ_H
#include <QPoint>
#include <QVector>

class Obj
{
public:
    Obj(QVector<QPoint> pointarr, int dur);
    int point_numb();
    int get_transp();
    QVector<QPoint> get_points();

private:
    QVector<QPoint> points;
    int transp;
};

#endif // OBJ_H
