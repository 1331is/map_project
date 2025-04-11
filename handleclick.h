#ifndef HANDLECLICK_H
#define HANDLECLICK_H

#include <QGraphicsScene>
#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
class handleClick : public QGraphicsScene {
    Q_OBJECT
public:
    explicit handleClick(QObject* parent = nullptr);

signals:
    void sceneClicked(const QPointF& pos); //сигнал

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};

#endif // HANDLECLICK_H
