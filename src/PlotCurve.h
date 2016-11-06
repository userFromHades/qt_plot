#ifndef PLOTCURVE_H
#define PLOTCURVE_H

#include <vector>

#include <QPoint>
#include <QRect>
#include <QColor>
#include <QObject>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>


class PlotCurve;
class PlotWidget;

struct PlotPoint {
    float x;
    float y;
};

typedef  std::vector<PlotPoint> PointList;
typedef  std::pair<PlotCurve*,PointList> CurvePair;


class PlotCurve : protected QOpenGLFunctions
{
public:

    const size_t maxPointsInVideo = 1000000;

    PlotCurve (const QString &_name);
    virtual ~PlotCurve ();

    PlotCurve & operator=(const PlotCurve&) = delete;
    PlotCurve(const PlotCurve&) = delete;
    PlotCurve() = default;

    virtual void       addPoint        (const QPointF&);
    virtual void       addPoint        (const float& x, const float& y);
    virtual PlotPoint  getPoint        (unsigned int) const;
    virtual void       setPoint        (unsigned int num,PlotPoint &point);
    virtual int        getNumPoint     () const { return points.size();}
    virtual QRectF     getRect         () const;
    virtual void       clear           ();
    virtual bool       isEmpty         (){return points.empty();}

    virtual void       setColor        (const QColor &_) {color = _;}
    virtual QColor     getColor        () const { return color;}

    virtual void       setScale        (const QPointF &_) {scale = _;}
    virtual QPointF    getScale        () const { return scale;}

    virtual void       setName         (const QString &_) {name = _;}
    virtual QString    getName         () {return name;}

    virtual void       setVisible      (const bool &_) {visible = _;}
    virtual bool       isVisible       () const { return visible;}

private:
    QString name;

    PointList  points;
    void chageRect (const PlotPoint&);
    void draw ();

    bool visible = true;
    QColor color;
    QPointF scale = QPointF(1.0,1.0);

    struct Part
    {
        static const size_t maxPartSize = 1000;
        QRectF rect;
        uint  begin;
        uint  count;

        Part ( uint _begin,  uint _count){
            begin = _begin;
           count = _count;
        }
        void addPoint (const PlotPoint &p);
    };

    std::vector <Part*> parts;

    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    size_t poinisInVideo = 0;
    size_t firstPointInVideo = 0;

    QRectF rect;

    friend class PlotWidget;

};

#endif //PLOTCURVE_H
