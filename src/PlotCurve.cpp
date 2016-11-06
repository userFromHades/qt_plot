#include "PlotCurve.h"

#include <algorithm>

#include "glerrorlog.cpp"

void PlotCurve::Part::addPoint(const PlotPoint& p)
{
    if (count == 0)
        rect.setRect(p.x, p.y, 0.0f, 0.0f);

    count++;

    qreal x1 = rect.x();
    qreal y1 = rect.y();
    qreal x2 = x1 + rect.width();
    qreal y2 = y1 + rect.height();

    if (x1 > p.x)
        x1 = p.x;
    if (x2 < p.x)
        x2 = p.x;

    if (y1 > p.y)
        y1 = p.y;
    if (y2 < p.y)
        y2 = p.y;

    rect.setRect(x1, y1, x2 - x1, y2 - y1);
}

PlotCurve::PlotCurve(const QString &_name):
name (_name)
{
    parts.push_back(new Part(0, 0));
}

PlotCurve::~PlotCurve()
{
    for (unsigned int i = 0; i < parts.size(); i++) {
        delete parts[i];
    }
    vao.destroy();
}

void PlotCurve::chageRect(const PlotPoint& p)
{
    qreal x1 = rect.x();
    qreal y1 = rect.y();
    qreal x2 = x1 + rect.width();
    qreal y2 = y1 + rect.height();

    if (x1 > p.x)
        x1 = p.x;
    if (x2 < p.x)
        x2 = p.x;

    if (y1 > p.y)
        y1 = p.y;
    if (y2 < p.y)
        y2 = p.y;

    rect.setRect(x1, y1, x2 - x1, y2 - y1);
}

void PlotCurve::addPoint(const QPointF& p)
{
    addPoint (p.x(),p.y());
}

void PlotCurve::addPoint(const float& x, const float& y)
{
    if (points.empty())
        rect.setRect(x, y, 0.0f, 0.0f);

    points.push_back(PlotPoint{x,y});

    Part *part = parts[parts.size() - 1];
    if (part->count == PlotCurve::Part::maxPartSize) {
        part = new Part(part->begin + PlotCurve::Part::maxPartSize, 0);
        parts.push_back(part);
        part->rect.setX( x );
        part->rect.setY( y );
    }
    part->addPoint(PlotPoint{x,y});
    chageRect(PlotPoint{x,y});
}


QRectF PlotCurve::getRect() const
{
    return rect;
}

void PlotCurve::clear()
{
    points.clear();

    for (unsigned int i = 0; i < parts.size(); i++) {
        delete parts[i];
    }
    parts.clear();

    parts.push_back(new Part(0, 0));
    rect = parts[0]->rect;

    poinisInVideo = 0;
    firstPointInVideo = 0;
}

PlotPoint PlotCurve::getPoint (unsigned int num) const{

    Q_ASSERT(num < points.size());
    return points[num];
}
void PlotCurve::setPoint (unsigned int num,PlotPoint &point){

    Q_ASSERT(num<points.size());
    points[num] = point;
}
void PlotCurve::draw (){

    if (points.empty())
        return;

    if (not vao.isCreated()){
        initializeOpenGLFunctions();

        vao.create();
        vao.bind();

        vbo.create();

        vbo.bind();

        vbo.allocate( maxPointsInVideo * 2 * sizeof(GLfloat));
        logGLError();

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        vbo.release();
        vao.release();
        logGLError();

    }


    if (poinisInVideo < points.size() - firstPointInVideo){

        if ( poinisInVideo >= maxPointsInVideo){
            firstPointInVideo += maxPointsInVideo / 10;
            poinisInVideo = 0;
        }

        vao.bind();
        vbo.bind();

        auto ptr = (PlotPoint*)vbo.map(QOpenGLBuffer::WriteOnly);
        size_t totalPoints = std::min(maxPointsInVideo, points.size()-firstPointInVideo);
        size_t size = (totalPoints - poinisInVideo) * sizeof(PlotPoint);
        memcpy(ptr + poinisInVideo, points.data() + firstPointInVideo + poinisInVideo,  size );
        vbo.unmap();
        vbo.release();
        vao.release();

        poinisInVideo = totalPoints;
    }


    vao.bind();
    logGLError();

    glDrawArrays(GL_LINE_STRIP, 0, poinisInVideo);
    logGLError();

    vao.release();
    logGLError();
}
