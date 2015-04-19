#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QPoint>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

namespace Ui {
class MainWindow;
}

#define BRICK   10
#define VOID    0
#define NEXTLEVELSCORE 1000
#define MAX_LEVEL 6

// pages on stacked widget:
#define GAMEOVER_WIDGET_INDEX 2
#define START_WIDGET_INDEX 1
#define GAME_WIDGET_INDEX 0
#define WIN_WIDGET_INDEX 3

#define SHAPE_CNT 6

class Figure;

const int FigureMatrix[7][2][4] = {
    {
        {1,1,1,1},
        {0,0,0,0}
    },
    {
        {0,1,1,0},
        {0,1,1,0}
    },
    {
        {0,0,1,1},
        {0,1,1,0}
    },
    {
        {1,1,1,0},
        {1,0,0,0}
    },
    {
        {1,1,0,0},
        {0,1,1,0}
    },
    {
        {0,1,1,1},
        {0,0,0,1}
    },
    {
        {1,1,1,0},
        {0,1,0,0}
    }
};

class Matrix {
public:
    Matrix(const int _w, const int _h);

    void setSize(const int _w, const int _h);
    void setValue(const QPoint _pos, const int &_value);
    inline void setValue(const int x, const int y, const int &_value);
    int  getValue(const QPoint _pos) const;
    inline int  getValue(const int x, const int y) const;
    int  getWidth()  const { return width; }
    int  getHeight() const { return height; }

    QVector<QVector<int> > getData() const { return data;}
    void setData(const QVector<QVector<int> > &value);
    void delRow(const int _row);
    void clear();

private:
    QVector< QVector<int> > data;
    int width;
    int height;
};

class TetrisCap {
public:
    TetrisCap();
    void setSize(int wx, int wy);
    void putFigure(const Figure &_figure);
    bool hasPlace(const Figure &_figure);
    void drawOnScene(QGraphicsScene &_scene);
    int clearedLines() { return m_clearedLines; }
    int getWidth() const { return m_width; }
    void clear();
    bool filled();

private:
    int m_height;
    int m_width;
    Matrix m_content;
    int m_clearedLines;
};

class Figure {
public:
    Figure(const TetrisCap &cap);
    Figure(const int &_shape, const QPoint &_pos, const unsigned int &_angle) :
        shape(_shape),
        position(_pos),
        angle(_angle)
    {}

    Matrix getContent() const;
    int getShape() const { return shape; }
    QPoint getPosition() const;
    int getX() const { return position.x(); }
    int getY() const { return position.y(); }
    unsigned int getAngle() const;
    void setPosition(const QPoint &_value);
    inline void setPosition(const int x, const int y);
    void setAngle(int _value);
    void rotate();
    void drawOnScene(QGraphicsScene &_scene);
    void setColor(QColor color) { m_color = color; }

private:
    int shape;
    QPoint position;
    unsigned int angle;
    QColor m_color;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
protected:
    virtual void keyPressEvent(QKeyEvent *event);

private:
    Ui::MainWindow *    ui;
    QTimer              m_timer;
    TetrisCap           m_cap;
    Figure         *    cur_figure;
    QGraphicsScene      m_scene;
    int m_score;
    int m_level;
    int m_speed;

    void draw();
    void init();
    void gameOver();
    void endGame();

public slots:
    void onTimerTick();
    void startGame();
};

#endif // MAINWINDOW_H
