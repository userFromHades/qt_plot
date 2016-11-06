DESCRIPTION
==========
This is simple Qt Widget classes to plot series of points. As it uses OpenGl 2.0 and vertex buffer object for drawing It is quick enough. I used this tool for few years in my debug applications that interact with hardware and realtime data sources.

BUILDING AND DEPENDENCIES
=========================
There is no anything special. It requires only qmake and depend on Qt 5 and standard library of c++11. You can do :
```
qmake
```
to build all sources in repository.
In order to use widget and classes you need just add line in your qt project file like that one:
```
include (qt_plot_path/src/qt_plot.pri)
```
Also you can add all files manually.

USAGE
=====
It's quite simple. Directory src consist of three classes. PlotCurve represent a curve that you want to draw. You can add point to it. But PlotCurve can't draw herself without PlotWidget. PlotWidget is a class that can only draw yourself and curves it consist. User can't interact with it. It's like a picture. PlotBehavior implements minimal interaction for user interface. There are examples that illustrate how to use it.

LICENSE
=======
Except one file It's distributed under MIT license (see LICENSE file).
/res/settings.png designed by Gregor Cresnar from Flaticon.
