#include "PlotWidget.h"

#include "glerrorlog.cpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static qreal breakupInterval(qreal length)
{
    qreal step = 1.0;
    int i = 50;
    while (i) {
        i--;
        int num = length / step;

        if (num <= 1)
            step /= 10.0;
        else if (num < 5)
            step /= 2.0;
        else if (num > 19)
            step *= 10.0;
        else if (num > 12)
            step *= 2.0;
        else
            break;
    }
    return step;
}

static qreal beginInterval(qreal begin, qreal step)
{
    if (begin >= 0.0)
        return step * (int(begin / step) + 1);
    return step * (int(begin / step));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

PlotWidget::PlotWidget(QWidget *parent):
    QOpenGLWidget(parent),
    program(this),
    frame(0)
{
    foneColor = QColor(255, 255, 255); //nc
    bgFoneColor = QColor(155, 155, 155);
    selectColor = QColor(180,180,180,180);
    drawSelectRect = false;

    viewRect.setRect(-1.0,-1.0,2.0,2.0);
}

PlotWidget::~PlotWidget()
{
    vao.destroy();
    program.removeAllShaders();
}

void PlotWidget::addCurve(PlotCurve* curve)
{
    Q_CHECK_PTR(curve);
    curves.push_back(curve);
}

void PlotWidget::clearAllCurves()
{
    for (auto& curve : curves) {
        curve->clear();
    }
}

void PlotWidget::delAllCurves()
{
    curves.clear();
}

std::vector<PlotCurve*> PlotWidget::getCurves()
{
    return curves;
}

void PlotWidget::shiftEndTime(float timeEnd)
{
    QRectF rect = viewRect;
    rect.setLeft(timeEnd - rect.width());
    rect.setRight(timeEnd);
    setViewRect(rect);
}

QPointF  PlotWidget::screenToSpace (const QPoint& p) const{

    qreal scaleW =  viewRect.width() / width()  / (1.0 - 2.0 * marginForTextW);
    qreal scaleH =  viewRect.height()/ height() / (1.0 - 2.0 * marginForTextH);

    return QPointF ( viewRect.left()   - viewRect.width()  * marginForTextW + p.x()* scaleW,
                     viewRect.bottom() + viewRect.height() * marginForTextH - p.y() *scaleH );
}


//-----------------------------------------------------------------------------
//                             Drawing code
//-----------------------------------------------------------------------------

static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "uniform highp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";

void PlotWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);

    program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    program.bindAttributeLocation("posAttr", 0);
    program.link();
    posAttr = program.attributeLocation("posAttr");
    colorUnf = program.uniformLocation("col");
    matrixUnf = program.uniformLocation("matrix");


    static GLfloat vertices[] = {

       -1.0,  1.0,
        1.0,  1.0,
        1.0, -1.0,
       -1.0, -1.0,

        //VrLine
        0.0, -1.0,
        0.0,  1.0,

        //HrLine
       -1.0,  0.0,
        1.0,  0.0
    };

    vao.create();
    if (not vao.isCreated())
        QTextStream(stdout) <<"Can't create vao:"<<endl;
    vao.bind();

    vbo.create();
    if (not vbo.isCreated())
        QTextStream(stdout) <<"Can't create vbo:"<<endl;

    vbo.bind();

    vbo.allocate(vertices, 16 * sizeof(GLfloat));
    logGLError();

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    vbo.release();
    vao.release();

}

void PlotWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    marginForTextH = 25.0/height;
    marginForTextW = 70.0/width;
}

void PlotWidget::paintGL()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);
    logGLError();


    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);

    glClearColor(bgFoneColor.redF(),bgFoneColor.greenF(),bgFoneColor.blueF(),1.0);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    logGLError();

    glViewport( width() * marginForTextW,              height() * marginForTextH,
                width() * (1.0 - 2.0* marginForTextW), height() * (1.0- 2.0* marginForTextH) );

    program.bind();
    logGLError();
    {
        QMatrix4x4 matrix;
        matrix.ortho( -1.0 ,  1.0  ,
                      -1.0  , 1.0  , -1.0f, 1.0f);

        program.setUniformValue(matrixUnf, matrix);
        program.setUniformValue(colorUnf, foneColor);
        logGLError();

        vao.bind();
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        logGLError();

        drawMesh();
        vao.release();
        logGLError();
    }

    glLineWidth(2.5);

    logGLError();

    for (auto& curve : curves) {

        if (not curve->isVisible())
            continue;

        QMatrix4x4 matrix;
        matrix.ortho( viewRect.left() ,  viewRect.right()  ,
                      viewRect.top()  , viewRect.bottom()  , -1.0f, 1.0f);
        auto scale = curve->getScale();
        matrix.scale(scale.x(),scale.y());
        program.setUniformValue(matrixUnf, matrix);
        program.setUniformValue(colorUnf, curve->color);
        curve->draw();
    }

    if (drawSelectRect and
        selectRect.width() and
        selectRect.height() )
    {

        float x1 = 2.0 * selectRect.x()                        / this->width()  - 1.0;
        float y1 =-2.0 * selectRect.y()                        / this->height() + 1.0;
        float x2 = 2.0 *(selectRect.x() + selectRect.width() ) / this->width()  - 1.0;
        float y2 =-2.0 *(selectRect.y() + selectRect.height()) / this->height() + 1.0;

        x1 /= 0.92; y1 /= 0.92; x2 /= 0.92; y2 /= 0.92;

        QMatrix4x4 matrix;
        matrix.ortho(  -1.0 , 1.0,
                       -1.0 , 1.0 , -1.0f, 1.0f);
        matrix.translate(x1 + (x2 - x1)/2.0,
                         y1 + (y2 - y1)/2.0);
        matrix.scale((x2 - x1)/2.0,
                     (y2 - y1)/2.0);

        program.setUniformValue(matrixUnf, matrix);
        program.setUniformValue(colorUnf, selectColor);
        logGLError();

        vao.bind();
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        logGLError();
        vao.release();
    }

    program.release();
    logGLError();

    drawTextMarcs();

    ++frame;
}


#include <QPainter>
void PlotWidget::drawTextWithCenter(QString &str, int x, int y)
{

    QPainter p;
    p.begin(this);
    const QFontMetrics fm = p.fontMetrics();
    int width  = fm.width(str);
    int height = fm.height();
    p.drawText(QPoint(x - width/2 ,y + height/2),str);
    p.end();
}

void PlotWidget::drawMesh()
{
    glLineWidth (0.5);

    qreal wstep = breakupInterval(viewRect.width());
    qreal wBegin = beginInterval(viewRect.x(), wstep);
    int cnt = int(viewRect.width() / wstep);

    if (wBegin + cnt * wstep < viewRect.right())
        cnt++;

    for (int i = 0; i < cnt; i++) {
        float x = wBegin + i * wstep;

        QMatrix4x4 matrix;

        matrix.ortho( viewRect.left()   ,  viewRect.right()   ,
                      -1.0   ,      1.0 , -1.0f, 1.0f);
        matrix.translate(x,0.0,0.0);

        program.setUniformValue(matrixUnf, matrix);
        program.setUniformValue(colorUnf, QColor::fromRgbF(0.5, 0.5, 0.5));

        glDrawArrays(GL_LINES, 4, 2);

    }

    qreal hstep = breakupInterval(viewRect.height());
    qreal hBegin = beginInterval(viewRect.y(), hstep);
    cnt = int((viewRect.height()) / hstep) +1 ;

    if (hBegin + cnt * hstep < viewRect.top())
        cnt++;

    for (int i = 0; i < cnt; i++) {
        float y = this->height();
        y =  hBegin + i * hstep;

        QMatrix4x4 matrix;

        matrix.ortho(-1.0 ,      1.0    ,
                      viewRect.top()   ,  viewRect.bottom()    , -1.0f, 1.0f);
        matrix.translate(0.0,y,0.0);

        program.setUniformValue(matrixUnf, matrix);
        program.setUniformValue(colorUnf, QColor::fromRgbF(0.5, 0.5, 0.5));

        glDrawArrays(GL_LINES, 6, 2);
    }
}

void PlotWidget::drawTextMarcs()
{
    QRectF rectMesh(viewRect.x() , viewRect.y() ,
            viewRect.width() , viewRect.height() );

    qreal wstep = breakupInterval(rectMesh.width());
    qreal wBegin = beginInterval(rectMesh.x(), wstep);
    int cnt = int(viewRect.width() / wstep);

    if (wBegin + cnt * wstep < viewRect.right())
        cnt++;

    for (int i = 0; i < cnt; i++) {
        float x = width() * marginForTextW +
                (1.0 - 2.0 * marginForTextW) * (wBegin + (i ) * wstep - viewRect.x()) / (viewRect.width()) * width() ;

        char buff[256];
        sprintf (buff, "%.3f",(wBegin+i*wstep));
        QString text(buff);
        drawTextWithCenter (text,x, height() * marginForTextH/2.0 );
        drawTextWithCenter (text,x,height() * (1.0 - marginForTextH/2.0));
    }

    qreal hstep = breakupInterval(rectMesh.height());
    qreal hBegin = beginInterval(rectMesh.y(), hstep);
    cnt = int((rectMesh.height()) / hstep);

    if (hBegin + cnt * hstep < viewRect.top())
        cnt++;

    for (int i = 0; i < cnt; i++) {
        float y = height() * (1.0 -  marginForTextH) - (1.0 - 2.0 * marginForTextH) * (hBegin + i * hstep - viewRect.y())/ (viewRect.height()) * height() ;

        char buff[256];
        sprintf (buff, "%.3f",(hBegin+i*hstep));
        QString text(buff);
        drawTextWithCenter (text,width() * marginForTextW/2.0,y );
        drawTextWithCenter (text,width() * (1.0 - marginForTextW/2.0),y );
    }
}
