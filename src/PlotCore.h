#ifndef PLOTCORE_H
#define PLOTCORE_H

#include <QOpenGLShaderProgram>
#include <QObject>
#include <QPainter>

#include "PlotCurve.h"

#define PLOT_PROPERTIES \
    Q_PROPERTY (QColor       foneColor READ getFoneColor       WRITE setFoneColor) \
    Q_PROPERTY (QColor     bgFoneColor READ getBgFoneColor     WRITE setBgFoneColor) \
    Q_PROPERTY (QColor     selectColor READ getSelectColor     WRITE setSelectColor) \
    Q_PROPERTY (QColor foregroundColor READ getForegroundColor WRITE setForegroundColor) \
    Q_PROPERTY (QColor       meshColor READ getMeshColor       WRITE setMeshColor) \
    Q_PROPERTY (QSizeF   marginForText READ getMarginForText   WRITE setMarginForText) \
    Q_PROPERTY (bool  isDrawSelectRect READ isDrawSelectRect   WRITE setEnableSelectRect) \
    Q_PROPERTY (QRectF        viewRect READ      getViewRect   WRITE setViewRect) \
    Q_PROPERTY (QRectF      selectRect READ    getSelectRect   WRITE setSelectRect)\
    void onUpdate() override {update();} \



class PlotCore : protected QOpenGLFunctions
{

public:

    PlotCore & operator=(const PlotCore&) = delete;
    PlotCore(const PlotCore&) = delete;

    PlotCore();
    virtual ~PlotCore();

    //curves
    void                           addCurve (PlotCurve* curve);
    void                     clearAllCurves ();
    void                       delAllCurves ();
    void                        deleteCurve (PlotCurve* curve);
    std::vector<PlotCurve*>       getCurves ();
    //
    void                       shiftEndTime (float time);

    //Q_INVOKABLE
    QPointF  screenToSpace (const QPoint& p) const;

    //get
    bool                   isDrawSelectRect () const {return drawSelectRect;}
    QRectF                      getViewRect () const {return viewRect;}
    QRectF                    getSelectRect () const {return selectRect;}
    QColor                     getFoneColor () const {return foneColor;}
    QColor                   getBgFoneColor () const {return bgFoneColor;}
    QColor                   getSelectColor () const {return selectColor;}
    QColor               getForegroundColor () const {return foregroundColor;}
    QColor                     getMeshColor () const {return meshColor;}
    QSizeF                 getMarginForText () const {return QSizeF (marginForTextW,marginForTextH);}
    //set
    void                setEnableSelectRect (const bool    &_ = true) { drawSelectRect =_; onUpdate();} //
    void                        setViewRect (const QRectF  &_)        { viewRect       =_; onUpdate();} //
    void                      setSelectRect (const QRectF  &_)        { selectRect     =_; onUpdate();} //
    void                       setFoneColor (const QColor  &_)        { foneColor      =_; onUpdate();} //
    void                     setBgFoneColor (const QColor  &_)        { bgFoneColor    =_; onUpdate();} //
    void                     setSelectColor (const QColor  &_)        { selectColor    =_; onUpdate();} //
    void                 setForegroundColor (const QColor  &_)        { foregroundColor = _; onUpdate();}
    void                       setMeshColor (const QColor  &_)        { meshColor       = _; onUpdate();}
    void                   setMarginForText (const QSizeF  &_)        { marginForTextW    =_.width();
                                                                        marginForTextH    =_.height(); onUpdate();} //


    virtual void onUpdate() = 0;
protected:

    bool                               init = false;

    std::vector<PlotCurve*>          curves;

    bool                           isChange;
    bool                     drawSelectRect;
    QRectF                         viewRect;
    QRectF                       selectRect;

    QColor                        foneColor;
    QColor                      bgFoneColor;
    QColor                      selectColor;
    QColor                  foregroundColor;
    QColor                        meshColor;

    GLuint                          posAttr;
    GLuint                         colorUnf;
    GLuint                        matrixUnf;

    QOpenGLShaderProgram           *program;
    int                               frame;

    QOpenGLVertexArrayObject           *vao;
    QOpenGLBuffer                      *vbo;

    float marginForTextH = 0.0;
    float marginForTextW = 0.0;

    void initializeOpenGL();
    void resize (int _width, int _height) {plotWidth = _width; plotHeight = _height;}

    void                 drawTextWithCenter (QPainter &p, QString &str, int x, int y);
    void                           drawMesh ();
    void                      drawTextMarcs (QPainter &p);

    void                              paint (QPainter *painter) ;

    int  plotWidth;
    int  plotHeight;

};

#endif // PLOTWIDGET_H
