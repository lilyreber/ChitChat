#ifndef SPRITE_H
#define SPRITE_H

#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QObject>
#include <QtGui>
#include <unordered_map>

static int STEP_SIZE = 5;

enum class directions { UP, DOWN, LEFT, RIGHT };

class sprite : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    sprite(const std::string &name, std::string skin);
    void keyPressEvent(QKeyEvent *event);

    const std::string &name;

    ~sprite();

    QGraphicsSimpleTextItem *name_display = new QGraphicsSimpleTextItem;

    void change_skin(const std::string &skin);

signals:
    void run_send_request(const std::string &message);
};

void change_position(int step_size, sprite *walker, directions dir);

class sprite_for_choice : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    sprite_for_choice(const std::string &skin);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    std::string skin;
    // current_user_sprite;

signals:
    void add_curren_sprite();
    void run_send_skin(const std::string &message);
};

class sprite_of_object : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    sprite_of_object(std::string object);
    std::string type_of_object;
};

#endif  // SPRITE_H
