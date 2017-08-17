#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

#include "PlotCurve.h"

class PlotWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

    Q_PROPERTY(QColor foneColor   READ getFoneColor   WRITE setFoneColor)
    Q_PROPERTY(QColor bgFoneColor READ getBgFoneColor WRITE setBgFoneColor)
    Q_PROPERTY(QColor selectColor READ getSelectColor WRITE setSelectColor)

public:

    PlotWidget & operator=(const PlotWidget&) = delete;
    PlotWidget(const PlotWidget&) = delete;
    PlotWidget() = default;

    PlotWidget(QWidget *parent = 0);
    virtual ~PlotWidget();

    //curves
    void                           addCurve (PlotCurve* curve);
    void                     clearAllCurves ();
    void                       delAllCurves ();
    std::vector<PlotCurve*>     getCurves ();
    //
    void                       shiftEndTime (float time);
    QPointF  screenToSpace (const QPoint& p) const;
    //get
    bool                   isDrawSelectRect () const {return drawSelectRect;}
    QRectF                      getViewRect () const {return viewRect;}
    QRectF                    getSelectRect () const {return selectRect;}
    QColor                     getFoneColor () const {return foneColor;}
    QColor                   getBgFoneColor () const {return bgFoneColor;}
    QColor                   getSelectColor () const {return selectColor;}
    //set
    void                setEnableSelectRect (const bool    &_ = true) { drawSelectRect =_; update();} //
    void                        setViewRect (const QRectF  &_)        { viewRect       =_; update();} //
    void                      setSelectRect (const QRectF  &_)        { selectRect     =_; update();} //
    void                       setFoneColor (const QColor  &_)        { foneColor      =_; update();} //
    void                     setBgFoneColor (const QColor  &_)        { bgFoneColor    =_; update();} //
    void                     setSelectColor (const QColor  &_)        { selectColor    =_; update();} //


private:

    std::vector<PlotCurve*>          curves;

    bool                           isChange;
    bool                     drawSelectRect;
    QRectF                         viewRect;
    QRectF                       selectRect;

    QColor                        foneColor;
    QColor                      bgFoneColor;
    QColor                      selectColor;

    GLuint                          posAttr;
    GLuint                         colorUnf;
    GLuint                        matrixUnf;

    QOpenGLShaderProgram           *program;
    int                               frame;

    QOpenGLVertexArrayObject           *vao;
    QOpenGLBuffer                      *vbo;

    float marginForTextH = 0.0;
    float marginForTextW = 0.0;

    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

    void                 drawTextWithCenter (QString &str, int x, int y);
    void                           drawMesh ();
    void                      drawTextMarcs ();

};

#endif // PLOTWIDGET_H
