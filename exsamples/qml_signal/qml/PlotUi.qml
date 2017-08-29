import QtQuick 2.5

import qtplot 0.1


PlotWidget {
    id: plot

    marginForText.width: 0.98
    marginForText.height: 0.98


    MouseArea {
        anchors.fill: parent

        property var doMove : false
        property var doScale : false
        property var doSelect : false
        property var mouseLastPos: [0.0,0.0]

        acceptedButtons: Qt.AllButtons

        onPressed:  {

            var mousePnt =  plot.screenToSpace(Qt.point(mouseX, mouseY))

            doMove = false;
            doScale = false;
            doSelect = false;
            mouseLastPos = [mouseX, mouseY]

            switch (pressedButtons){
            case Qt.LeftButton: {
                doSelect = true
                plot.selectRect =  Qt.rect(mousePnt.x, mousePnt.x, 0, 0)
                plot.isDrawSelectRect = true
            }
            break;
            case Qt.RightButton: {
                doScale = true;
                plot.isDrawSelectRect = false
            }
            break;
            case Qt.MiddleButton: {
                doMove = true;
                plot.isDrawSelectRect = false
            }
            break;
            }
            mouse.accepted = true

        }

        onReleased: {

            if (doSelect) {

                var p1 = plot.screenToSpace (Qt.point(mouseLastPos[0], mouseLastPos[1]))
                var p2 = plot.screenToSpace (Qt.point(mouseX,          mouseY))

                var x1 =  p1.x < p2.x ? p1.x: p2.x
                var x2 =  p1.x < p2.x ? p2.x: p1.x

                var y1 =  p1.y < p2.y ? p1.y: p2.y
                var y2 =  p1.y < p2.y ? p2.y: p1.y


                var p = Math.abs(mouseX - mouseLastPos[0]) * Math.abs(mouseY - mouseLastPos[1])
                if ( p > 40 )//minimal area in pixels
                {
                    plot.viewRect= Qt.rect( x1,       y1,
                                            x2 - x1,  y2-y1 )
                }
                plot.isDrawSelectRect = false
            }

            mouseLastPos = [mouseX, mouseY]


            doMove = false;
            doScale = false;
            doSelect = false;

            mouse.accepted = true
        }

        onPositionChanged: {

            if (doSelect ){

                var p1 = plot.screenToSpace (Qt.point(mouseLastPos[0], mouseLastPos[1]))
                var p2 = plot.screenToSpace (Qt.point(mouseX,          mouseY))

                var x1 =  p1.x < p2.x ? p1.x: p2.x
                var x2 =  p1.x < p2.x ? p2.x: p1.x

                var y1 =  p1.y < p2.y ? p1.y: p2.y
                var y2 =  p1.y < p2.y ? p2.y: p1.y

                plot.selectRect= Qt.rect( x1,       y1,
                                          x2 - x1,  y2-y1 )

                plot.isDrawSelectRect = true

            }
            else if (doScale)
            {
                plot.isDrawSelectRect = false;
                var scaleH = Math.pow (2.0, (mouseY - mouseLastPos[1])/250.0);
                var scaleW = Math.pow (2.0, (mouseLastPos[0] -  mouseX)/250.0);
                mouseLastPos = [mouseX, mouseY]

                var w = plot.viewRect.width*scaleW;
                var h = plot.viewRect.height*scaleH;

                plot.viewRect = Qt.rect( viewRect.x + (plot.viewRect.width-w)/2,
                                         viewRect.y + (plot.viewRect.height-h)/2,
                                         w,h)
            }
            else if (doMove){

                var p1 = plot.screenToSpace(Qt.point(mouseLastPos[0], mouseLastPos[1]))
                var p2 = plot.screenToSpace(Qt.point(mouseX,          mouseY))

                plot.viewRect = Qt.rect( plot.viewRect.x + p1.x - p2.x,
                                         plot.viewRect.y + p1.y - p2.y,
                                         plot.viewRect.width,
                                         plot.viewRect.height );

                mouseLastPos = [mouseX, mouseY]
            }
            mouse.accepted = true

        }
    }
}


