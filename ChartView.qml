import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtCharts 2.2
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import "QMLs"

Item {
    //save names here
    property variant allSeriesName
    property alias editBar_a: editBar
    Connections {
        target: reciever
        onAdjustAxis: {
            graphs.minRngX = (Math.floor((minRng.x - (maxRng.x-minRng.x)*0.02)*10))/10
            graphs.maxRngX = (Math.ceil((maxRng.x + (maxRng.x-minRng.x)*0.02)*10))/10
            graphs.minRngY = minRng.y - (maxRng.y-minRng.y)*0.1
            graphs.maxRngY = maxRng.y
            axisX.min = graphs.minRngX
            axisX.max = graphs.maxRngX
            axisY.min = 0
            axisY.max = graphs.maxRngY*1.1

        }
        onSendDebugInfo: {
            showPopupTips(qsTr(data), time)
        }
    }

    ColumnLayout {
        spacing: 10*app.dp
        anchors.top: menuBar.bottom
        anchors.fill: parent
        anchors.margins: 10*app.dp
        anchors.topMargin: menuBar.height+10*app.dp
        ColumnLayout {
            id: colForSnap
            spacing:0
            Rectangle {
                color: "transparent"
                Layout.fillHeight: true
                Layout.fillWidth: true
                ChartView {
                    id: graphs
                    visible: true
                    anchors.fill: parent
                    antialiasing: true
                    legend.visible: false
                    property int numSeries : 0 //current number of graphs
                    property real minRngX: 0.0
                    property real maxRngX: 0.0
                    property real minRngY: 0.0
                    property real maxRngY: 0.0
                    ValueAxis {
                        id: axisX
                        objectName: "axisX"
                        titleText: qsTr("Wavelength(um)")
                        min: 410
                        max: 500
                        tickCount: 13
                        minorTickCount: 3
                        labelFormat: "%.1f"
                    }
                    ValueAxis {
                        id: axisY
                        objectName: "axisY"
//                        titleText: app.yAxisName
                        min: 0
                        max:2
                        tickCount: 5
                        minorTickCount: 4
                    }
                    MouseArea {
                        anchors.fill: parent
                        property int lastX: 0
                        property int lastY: 0
                        onPressed: {
                            lastX = mouse.x
                            lastY = mouse.y
                        }
                        onReleased: {
        //                    view.interactive : true
                        }
                        onPositionChanged: {
                            if (lastX !== mouse.x) {
                                graphs.scrollRight(lastX - mouse.x)
                                lastX = mouse.x
                            }
                            if (lastY !== mouse.y) {
                                graphs.scrollDown(lastY - mouse.y)
                                lastY = mouse.y
                            }
                        }
                    }
                }
                ListView {
                    z:1
                    id: textData
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.margins: 100*app.dp
                    height: app.fontPixelSize * 1.5 * count
                    width: parent.width - 20*app.dp
                    model: availableData
                    delegate: Text {
                        text: data_
                        color: palette.darkPrimary
                        opacity: 0.7
                        font.pixelSize: app.fontPixelSize
                    }
                    ListModel {
                        id: availableData
                        ListElement { data_: qsTr("testElement0")}
                        ListElement { data_: qsTr("testElement1")}
                        ListElement { data_: qsTr("testElement2")}
                        ListElement { data_: qsTr("testElement3")}
                    }
                }
            }
//            CustomLegend {
//                id: customLegend
//                visible: true
//                Layout.fillWidth: true
//                Layout.preferredHeight: app.menuBarHeight
////                onEntered: chartViewSelector.highlightSeries(seriesName);
////                onExited: chartViewSelector.highlightSeries("");
////                onSelected: chartViewSelector.selectSeries(seriesName);
//            }
        }
        ChartViewEditBar {
            id: editBar
            Layout.alignment: Qt.AlignHCenter |Qt.AlignBottom

        }

    }
    Timer {
        id: timer1
    }
    Timer {
        id: timer2
    }
    function delay(delayTime, cb) {
        timer1.interval = delayTime;
        timer1.repeat = false;
        timer1.triggered.connect(cb);
        timer1.start();
    }
    function createAxis(min, max) {
        // The following creates a ValueAxis object that can be then
        //set as a x or y axis for a series
        return Qt.createQmlObject("import QtQuick 2.7;
                                   import QtCharts 2.7;
                                   ValueAxis { min: "
                                  + min + "; max: " + max + " }", graphs);
    }
    function showPopupTips(text, dTime) {
        tipsWithPath.showedText = qsTr(text)
        tipsWithPath.open()
        delay(dTime !== undefined ? dTime : 300, tipsWithPath.close)
    }
}

