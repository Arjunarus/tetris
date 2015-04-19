#include <QDebug>
#include <QKeyEvent>
#include "mainwindow.h"
#include "ui_mainwindow.h"

unsigned int rand(const int &_max) {
    return qrand() % _max;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    cur_figure(NULL),
    m_score(0),
    m_level(1),
    m_speed(600)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(1);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimerTick()));
    ui->graphicsView->setScene(&m_scene);
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::StrongFocus);
    connect(ui->startBtn, SIGNAL(clicked()), this, SLOT(startGame()));
    connect(ui->restartBtn, SIGNAL(clicked()), this, SLOT(startGame()));
    ui->stackedWidget->setCurrentIndex(START_WIDGET_INDEX);
    ui->lcdLevel->setPalette(Qt::green);
    ui->lcdScore->setPalette(Qt::green);
}

MainWindow::~MainWindow()
{
    if (cur_figure != NULL)
        delete cur_figure;
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{

    Figure newFig = *cur_figure;
    switch(event->key()){
    case Qt::Key_Up:{
        newFig.rotate();
        break;
    }
    case Qt::Key_Down: {
        newFig.setPosition(cur_figure->getPosition().x(), cur_figure->getPosition().y() + 1);
        break;
    }
    case Qt::Key_Left: {
        newFig.setPosition(cur_figure->getPosition().x() - 1, cur_figure->getPosition().y());
        break;
    }
    case Qt::Key_Right: {
        newFig.setPosition(cur_figure->getPosition().x() + 1, cur_figure->getPosition().y());
        break;
    }
    default: {
        QMainWindow::keyPressEvent(event);
    }
    }

    if (m_cap.hasPlace(newFig)) {
        delete cur_figure;
        cur_figure = new Figure(newFig);
    }
    draw();
}

void MainWindow::draw()
{
    m_scene.clear();
    m_cap.drawOnScene(m_scene);
    cur_figure->drawOnScene(m_scene);
    ui->lcdScore->display(m_score);
    ui->lcdLevel->display(m_level);
}

void MainWindow::onTimerTick()
{
    Figure newFig = *cur_figure;
    newFig.setPosition(newFig.getPosition().x(), cur_figure->getPosition().y() + 1);
    if (m_cap.hasPlace(newFig)) {
        delete cur_figure;
        cur_figure = new Figure(newFig);
    } else {
        m_cap.putFigure(*cur_figure);
        m_score += m_cap.clearedLines() * 100 + m_cap.clearedLines() > 0 ? 100 : 0;
        if (m_score > NEXTLEVELSCORE) {
            m_score = 0;
            if (m_level < MAX_LEVEL)
                m_level++;
            else
                endGame();
            init();
        }
        delete cur_figure;
        cur_figure = new Figure(m_cap);
        if (m_cap.filled())
            gameOver();
    }
    draw();
}

void MainWindow::startGame()
{
    m_level = 1;
    init();
    ui->stackedWidget->setCurrentIndex(GAME_WIDGET_INDEX);
    m_timer.start(m_speed);
}

void MainWindow::init()
{
    m_cap.clear();
    switch (m_level) {
    default:
    case 1:
        m_cap.setSize(10, 20);
        m_speed = 600;
        break;
    case 2:
        m_cap.setSize(5, 40);
        m_speed = 500;
        break;
    case 3:
        m_cap.setSize(20, 40);
        m_speed = 400;
        break;
    case 4:
        m_cap.setSize(40, 50);
        m_speed = 300;
        break;
    case 5:
        m_cap.setSize(40, 30);
        m_speed = 250;
        break;
    case 6:
        m_cap.setSize(40, 20);
        m_speed = 200;
        break;
    }
    if (cur_figure != NULL)
        delete cur_figure;
    cur_figure = new Figure(m_cap);
}

void MainWindow::gameOver()
{
    m_timer.stop();
    ui->stackedWidget->setCurrentIndex(GAMEOVER_WIDGET_INDEX);
}

void MainWindow::endGame()
{
    m_timer.stop();
    ui->stackedWidget->setCurrentIndex(WIN_WIDGET_INDEX);
}

// =============================================================================

TetrisCap::TetrisCap() :
    m_height(20),
    m_width(10),
    m_content(m_width, m_height)
{}

void TetrisCap::setSize(int wx, int wy)
{
    m_height = wy;
    m_width = wx;
    m_content.setSize(wx, wy);
}

void TetrisCap::putFigure(const Figure &_figure)
{
    m_clearedLines = 0;
    Matrix figMatrix = _figure.getContent();
    for (int i = 0; i < figMatrix.getWidth(); i++)
        for (int j = 0; j < figMatrix.getHeight(); j++)
            if (figMatrix.getValue(i, j) > 0)
                m_content.setValue(_figure.getX() + i, _figure.getY() + j, figMatrix.getValue(i, j));

    int i = 0;
    while (i < m_content.getHeight()) {
        bool filled = true;
        for (int j = 0; j < m_content.getWidth(); j++)
            if (m_content.getValue(j, i) == 0) {
                filled = false;
                break;
            }
        if (filled) {
            m_content.delRow(i);
            m_clearedLines++;
        }
        else
            i++;
    }
}

bool TetrisCap::hasPlace(const Figure &_figure)
{
    Matrix fContent(_figure.getContent());

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if ((fContent.getValue(j, i) == 1)) {
                if (j + _figure.getX() < 0 ||
                        i + _figure.getY() < 0 ||
                        j + _figure.getX() >= m_content.getWidth() ||
                        i + _figure.getY() >= m_content.getHeight() ||
                        (m_content.getValue(j + _figure.getX(), i + _figure.getY()) == 1))
                    return false;
            }

    return true;
}

void TetrisCap::drawOnScene(QGraphicsScene &_scene)
{
    _scene.addItem(new QGraphicsRectItem(-1, -1, BRICK * m_width + 2, BRICK * m_height + 2));

    for (int i = 0; i < m_content.getHeight(); ++i)
        for (int j = 0; j < m_content.getWidth(); j++) {
            if (m_content.getValue(j, i) == 1) {
                QGraphicsRectItem *rect = new QGraphicsRectItem(j * BRICK, i * BRICK, BRICK, BRICK);
                rect->setPen(QPen(QBrush(QColor(Qt::lightGray)), 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
                rect->setBrush(QBrush(QColor(Qt::darkGray)));
                _scene.addItem(rect);
            }
        }
}

void TetrisCap::clear()
{
    m_content.clear();
}

bool TetrisCap::filled()
{
    bool res = false;
    for (int i = 0; i < m_width; ++i)
        if (m_content.getValue(i, 0) != 0) {
            res = true;
            break;
        }
    return res;
}

// =============================================================================

Figure::Figure(const TetrisCap &cap)
{
    shape = rand(7);
    position = QPoint(cap.getWidth() / 2 - 2, 0);// TODO: use WIDTH of cap
    angle = rand(4);
    m_color = QColor(rand(256), rand(256), rand(256));
}

Matrix Figure::getContent() const
{
    Matrix res(4, 4);
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 4; ++j)
            res.setValue(QPoint(i+1, j), FigureMatrix[static_cast<int>(shape)][i][j]);

    if (angle == 0) return res;

    Matrix oldres(res);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            switch (angle) {
            case 0: ;
            case 1: {
                res.setValue(i, j, oldres.getValue(3-j,i));
                break;
            }
            case 2: {
                res.setValue(i, j, oldres.getValue(3-i, 3-j));
                break;
            }
            case 3: {
                res.setValue(i, j, oldres.getValue(j, 3-i));
                break;
            }
            }
        }
    return res;
}

QPoint Figure::getPosition() const
{
    return position;
}

void Figure::setPosition(const QPoint &_value)
{
    position = _value;
}

void Figure::setPosition(const int x, const int y)
{
    setPosition(QPoint(x, y));
}

unsigned int Figure::getAngle() const
{
    return angle;
}

void Figure::setAngle(int _value)
{
    angle = _value % 4;
}

void Figure::rotate()
{
    angle += 1;
    angle %= 4;
}

void Figure::drawOnScene(QGraphicsScene &_scene)
{
    Matrix content(getContent());
    for (int i = 0; i < content.getHeight(); ++i)
        for (int j = 0; j < content.getWidth(); j++) {
            if (content.getValue(j, i) == 1) {
                QGraphicsRectItem *rect = new QGraphicsRectItem((j + getX()) * BRICK, (i + getY()) * BRICK, BRICK, BRICK);
                rect->setBrush(QBrush(m_color));
                _scene.addItem(rect);
            }
        }
}

// =============================================================================

Matrix::Matrix(const int _w, const int _h)
{
    setSize(_w, _h);
}

void Matrix::setSize(const int _w, const int _h)
{
    if (_w > 0 && _h > 0) {
        width = _w;
        height = _h;
        if (data.size() != _h)
            data.resize(_h);

        if (data[0].size() != _w)
            for (int i = 0; i < data.size(); ++i) {
                data[i].resize(_w);
            }
    }
}

void Matrix::setValue(const QPoint _pos, const int &_value)
{
    if (width > _pos.x() && height > _pos.y())
        data[_pos.y()][_pos.x()] = _value;
}

void Matrix::setValue(const int x, const int y, const int &_value)
{
    setValue(QPoint(x, y), _value);
}

int Matrix::getValue(const QPoint _pos) const
{
    if (width > _pos.x() && height > _pos.y())
        return data[_pos.y()][_pos.x()];
    return 0;
}

int Matrix::getValue(const int x, const int y) const
{
    return getValue(QPoint(x, y));
}

void Matrix::setData(const QVector<QVector<int> > &value)
{
    data = value;
}

void Matrix::delRow(const int _row)
{
    for (int i = _row; i > 0; --i)
        data[i] = data[i-1];
}

void Matrix::clear()
{
    data.clear();
    setSize(width, height);
}
