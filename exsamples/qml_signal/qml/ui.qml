import QtQuick 2.5
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0

import QtQuick.Layouts 1.3
import QtQuick.Window 2.1


ApplicationWindow {
    id: root
    visible: true
    width: 1024
    height: 600

    Material.theme: Material.Dark
    Material.accent: "#FF5722"

    title: "qml signal"

    PlotUi{
        id: plot
        objectName: "plot"

        height: parent.height
        width: parent.width
        anchors.centerIn: parent

        bgFoneColor: Material.background
        foneColor: Material.background
        foregroundColor: Material.foreground
        meshColor: "#888888"

    }

}
