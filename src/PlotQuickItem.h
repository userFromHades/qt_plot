#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QOpenGLShaderProgram>
#include <QQuickPaintedItem>

#include "PlotCurve.h"


class PlotQuickItem : public QQuickPaintedItem, protected QOpenGLFunctions
{
	Q_OBJECT

	Q_PROPERTY (QColor       foneColor READ getFoneColor       WRITE setFoneColor)
	Q_PROPERTY (QColor     bgFoneColor READ getBgFoneColor     WRITE setBgFoneColor)
	Q_PROPERTY (QColor     selectColor READ getSelectColor     WRITE setSelectColor)
	Q_PROPERTY (QColor foregroundColor READ getForegroundColor WRITE setForegroundColor)
	Q_PROPERTY (QSizeF   marginForText READ getMarginForText   WRITE setMarginForText)
	Q_PROPERTY (bool  isDrawSelectRect READ isDrawSelectRect   WRITE setEnableSelectRect)
	Q_PROPERTY (QRectF        viewRect READ      getViewRect   WRITE setViewRect)
	Q_PROPERTY (QRectF      selectRect READ    getSelectRect   WRITE setSelectRect)

public:

	PlotQuickItem & operator=(const PlotQuickItem&) = delete;
	PlotQuickItem(const PlotQuickItem&) = delete;

	PlotQuickItem(QQuickItem *parrent = 0);
	virtual ~PlotQuickItem();

    //curves
    void                           addCurve (PlotCurve* curve);
    void                     clearAllCurves ();
    void                       delAllCurves ();
    std::vector<PlotCurve*>     getCurves ();
    //
    void                       shiftEndTime (float time);

	Q_INVOKABLE
    QPointF  screenToSpace (const QPoint& p) const;

	//get
	bool                   isDrawSelectRect () const {return drawSelectRect;}
	QRectF                      getViewRect () const {return viewRect;}
	QRectF                    getSelectRect () const {return selectRect;}
	QColor                     getFoneColor () const {return foneColor;}
	QColor                   getBgFoneColor () const {return bgFoneColor;}
	QColor                   getSelectColor () const {return selectColor;}
	QColor               getForegroundColor () const {return foregroundColor;}
	QSizeF                 getMarginForText () const {return QSizeF (marginForTextW,marginForTextH);}
	//set
	void                setEnableSelectRect (const bool    &_ = true) { drawSelectRect =_; update();} //
	void                        setViewRect (const QRectF  &_)        { viewRect       =_; update();} //
	void                      setSelectRect (const QRectF  &_)        { selectRect     =_; update();} //
	void                       setFoneColor (const QColor  &_)        { foneColor      =_; update();} //
	void                     setBgFoneColor (const QColor  &_)        { bgFoneColor    =_; update();} //
	void                     setSelectColor (const QColor  &_)        { selectColor    =_; update();} //
	void                 setForegroundColor (const QColor  &_)        { foregroundColor = _; update();}
	void                   setMarginForText (const QSizeF  &_)        { marginForTextW    =_.width();
		                                                                marginForTextH    =_.height(); update();} //

private:

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

    GLuint                          posAttr;
    GLuint                         colorUnf;
    GLuint                        matrixUnf;

	QOpenGLShaderProgram           *program;
    int                               frame;

    QOpenGLVertexArrayObject            vao;
    QOpenGLBuffer                       vbo;

    float marginForTextH = 0.0;
    float marginForTextW = 0.0;

	void initializeOpenGL();

	void                 drawTextWithCenter (QPainter &p, QString &str, int x, int y);
	void                           drawMesh ();
	void                      drawTextMarcs (QPainter &p);

	void                              paint (QPainter *painter) Q_DECL_OVERRIDE;

};

#endif // PLOTWIDGET_H
