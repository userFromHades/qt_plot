#include "PlotCore.h"

#include <QPainter>

#include "glerrorlog.cpp"

#include <QtDebug>

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

PlotCore::PlotCore():
    program(nullptr),
    frame(0),
    vao(nullptr)
{
    foneColor = QColor(255, 255, 255); //nc
    bgFoneColor = QColor(155, 155, 155);
    selectColor = QColor(180,180,180,180);
    foregroundColor = QColor (0,0,0);
    meshColor = QColor (127,127,127);
    drawSelectRect = false;

    viewRect.setRect(-1.0,-1.0,2.0,2.0);

    //Todo setRenderTarget(QQuickPaintedItem::FramebufferObject);
}

PlotCore::~PlotCore()
{
    if(program)
        program->deleteLater();
    if(vao)
        vao->deleteLater();
}

void PlotCore::addCurve(PlotCurve* curve)
{
    Q_CHECK_PTR(curve);
    curves.push_back(curve);
}

void PlotCore::clearAllCurves()
{
    for (auto& curve : curves) {
        curve->clear();
    }
}

void PlotCore::delAllCurves()
{
    curves.clear();
}

void PlotCore::deleteCurve(PlotCurve *curve){
    auto it = std::find(curves.begin(),curves.end(),curve);
    if (it != curves.end())
        curves.erase(it);
}

std::vector<PlotCurve*> PlotCore::getCurves()
{
    return curves;
}

void PlotCore::shiftEndTime(float timeEnd)
{
    QRectF rect = viewRect;
    rect.setLeft(timeEnd - rect.width());
    rect.setRight(timeEnd);
    setViewRect(rect);
}

QPointF  PlotCore::screenToSpace (const QPoint& p) const{

    qreal scaleW =  viewRect.width() / plotWidth  / (1.0 - 2.0 * marginForTextW);
    qreal scaleH =  viewRect.height()/ plotHeight / (1.0 - 2.0 * marginForTextH);

    return QPointF ( viewRect.left()   + (p.x() - plotWidth  * 1.0 * marginForTextW) * scaleW,
                     viewRect.bottom() - (p.y() - plotHeight * 1.0 * marginForTextH) * scaleH );

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

void  PlotCore::paint (QPainter *painter) {

    if (not init)
        initializeOpenGL ();

    painter->save();
    painter->beginNativePainting();

    marginForTextH = 25.0/plotHeight;
    marginForTextW = 70.0/plotWidth;

    //--------------------------

    //const qreal retinaScale = devicePixelRatio();
    const qreal retinaScale = 1.0;
    glViewport(0, 0, plotWidth * retinaScale, plotHeight * retinaScale);
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

    glViewport( plotWidth * marginForTextW,              plotHeight * marginForTextH,
                plotWidth * (1.0 - 2.0* marginForTextW), plotHeight * (1.0- 2.0* marginForTextH) );

    program->bind();
    logGLError();
    {
        QMatrix4x4 matrix;
        matrix.ortho( -1.0 ,  1.0  ,
                      -1.0  , 1.0  , -1.0f, 1.0f);

        program->setUniformValue(matrixUnf, matrix);
        program->setUniformValue(colorUnf, foneColor);
        logGLError();

        vao->bind();
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        logGLError();

        drawMesh();
        vao->release();
        logGLError();
    }

    glLineWidth(2.5);

    logGLError();

    for (auto& curve : curves) {

        if (not curve->isVisible())
            continue;

        QMatrix4x4 matrix;
        matrix.ortho( viewRect.left() ,  viewRect.right()  ,
                      viewRect.bottom(), viewRect.top(), -1.0f, 1.0f);
        auto scale = curve->getScale();
        matrix.scale(scale.x(),scale.y());
        program->setUniformValue(matrixUnf, matrix);
        program->setUniformValue(colorUnf, curve->color);
        curve->draw();
    }

    if (drawSelectRect and
        selectRect.width() and
        selectRect.height() )
    {

        QMatrix4x4 matrix;
        matrix.ortho(  -1.0 , 1.0,
                        1.0 , -1.0 , -1.0f, 1.0f);

        matrix.scale(2.0/viewRect.width(),
                     2.0/viewRect.height());

        matrix.translate(- viewRect.center().x(),
                         - viewRect.center().y());

        matrix.translate(selectRect.center().x(),
                         selectRect.center().y());

        matrix.scale(selectRect.width()/2.0,
                     selectRect.height()/2.0);

        program->setUniformValue(matrixUnf, matrix);
        program->setUniformValue(colorUnf, selectColor);
        logGLError();

        vao->bind();
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        logGLError();
        vao->release();
    }

    program->release();
    logGLError();

    ++frame;

    //----------------------------


    painter->endNativePainting();

    painter->restore();
    drawTextMarcs(*painter);

}


void PlotCore::initializeOpenGL()
{
    initializeOpenGLFunctions();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);

    program = new QOpenGLShaderProgram;

    program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    program->bindAttributeLocation("posAttr", 0);
    program->link();
    posAttr = program->attributeLocation("posAttr");
    colorUnf = program->uniformLocation("col");
    matrixUnf = program->uniformLocation("matrix");


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

    vao = new QOpenGLVertexArrayObject;
    vao->create();
    if (not vao->isCreated())
        QTextStream(stdout) <<"Can't create vao:"<<endl;
    vao->bind();

    vbo = new QOpenGLBuffer;
    vbo->create();
    if (not vbo->isCreated())
        QTextStream(stdout) <<"Can't create vbo:"<<endl;

    vbo->bind();

    vbo->allocate(vertices, 16 * sizeof(GLfloat));
    logGLError();

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    vbo->release();
    vao->release();

    init = true;

}

void PlotCore::drawTextWithCenter( QPainter &p ,QString &str, int x, int y)
{

    //p.begin(this);
    const QFontMetrics fm = p.fontMetrics();
    int width  = fm.width(str);
    int height = fm.height();
    p.drawText(QPoint(x - width/2 ,y + height/2),str);
    //p.end();
}

void PlotCore::drawMesh()
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

        program->setUniformValue(matrixUnf, matrix);
        program->setUniformValue(colorUnf, meshColor);

        glDrawArrays(GL_LINES, 4, 2);

    }

    qreal hstep = breakupInterval(viewRect.height());
    qreal hBegin = beginInterval(viewRect.y(), hstep);
    cnt = int((viewRect.height()) / hstep) +1 ;

    if (hBegin + cnt * hstep < viewRect.top())
        cnt++;

    for (int i = 0; i < cnt; i++) {
        float y = plotHeight;
        y =  hBegin + i * hstep;

        QMatrix4x4 matrix;

        matrix.ortho(-1.0 ,      1.0    ,
                      viewRect.bottom()   ,  viewRect.top()    , -1.0f, 1.0f);
        matrix.translate(0.0,y,0.0);

        program->setUniformValue(matrixUnf, matrix);
        program->setUniformValue(colorUnf, meshColor);

        glDrawArrays(GL_LINES, 6, 2);
    }
}

void PlotCore::drawTextMarcs( QPainter &p )
{
    auto priorPen = p.pen();
    p.setPen(foregroundColor);


    QRectF rectMesh(viewRect.x() , viewRect.y() ,
            viewRect.width() , viewRect.height() );

    qreal wstep = breakupInterval(rectMesh.width());
    qreal wBegin = beginInterval(rectMesh.x(), wstep);
    int cnt = int(viewRect.width() / wstep);

    if (wBegin + cnt * wstep < viewRect.right())
        cnt++;

    for (int i = 0; i < cnt; i++) {
        float x = plotWidth * marginForTextW +
                (1.0 - 2.0 * marginForTextW) * (wBegin + (i ) * wstep - viewRect.x()) / (viewRect.width()) * plotWidth ;

        char buff[256];
        sprintf (buff, "%.3f",(wBegin+i*wstep));
        QString text(buff);
        drawTextWithCenter (p, text,x, plotHeight * marginForTextH/2.0 );
        drawTextWithCenter (p, text,x,plotHeight * (1.0 - marginForTextH/2.0));
    }

    qreal hstep = breakupInterval(rectMesh.height());
    qreal hBegin = beginInterval(rectMesh.y(), hstep);
    cnt = int((rectMesh.height()) / hstep);

    if (hBegin + cnt * hstep < viewRect.top())
        cnt++;

    for (int i = 0; i < cnt; i++) {
        float y = plotHeight * (1.0 -  marginForTextH) - (1.0 - 2.0 * marginForTextH) * (hBegin + i * hstep - viewRect.y())/ (viewRect.height()) * plotHeight ;

        char buff[256];
        sprintf (buff, "%.3f",(hBegin+i*hstep));
        QString text(buff);
        drawTextWithCenter (p, text,plotWidth * marginForTextW/2.0,y );
        drawTextWithCenter (p,text,plotWidth * (1.0 - marginForTextW/2.0),y );
    }

    p.setPen(priorPen);
}
