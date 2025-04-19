#include "handleclick.h"

handleClick::handleClick(QObject* parent) : QGraphicsScene(parent) {}

void handleClick::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsScene::mousePressEvent(event);
    emit sceneClicked(event->scenePos());
}
