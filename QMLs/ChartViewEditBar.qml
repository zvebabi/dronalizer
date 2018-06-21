import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import QtCharts 2.2
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0


Column {
    id: ctrlPane
    spacing: 3
    property int itemWidth: 150*app.dp
    Connections {
        target: reciever
        onMakeSeries: {
            createSeries()
        }
    }
    Rectangle
    {
        id: mainBtnHolder
        width: ctrlPane.itemWidth
        height: 120*app.dp
        anchors.top:  parent.top - app.menuBarHeight
        color: "transparent"
        Grid {
            id : chartEditMenu2
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            columns: 1
            spacing: 20
            ToolButton {
                id: enableFlyMode
                enabled: true
                height: 2.3*48*app.dp
                width: height
                ToolTip.visible: hovered
                    ToolTip.text: qsTr("Enable fly mode")
                Image {
                    id: rAa
                    anchors.centerIn: parent
                    width: parent.width
                    height: width
                    source: "qrc:/images/runAnalizer.png"
                    antialiasing: true
                    smooth: true
                }
                ColorOverlay {
                    anchors.fill: rAa
                    source: rAa
                    color: "#d000ff00"
                }
                onClicked: {
                    createSeries()
                }
            }
            ToolButton {
                id: setZeroLevel
                height: 2.3*48*app.dp
                width: height
                ToolTip.visible: hovered
                    ToolTip.text: qsTr("Set zero lavel")
                Image {
                    id: rAe
                    anchors.centerIn: parent
                    width: parent.width
                    height: width
                    source: "qrc:/images/runCalibration.png"
                    antialiasing: true
                    smooth: true
                }

                ColorOverlay {
                    anchors.fill: rAe
                    source: rAe
                    color: "#80ff0000"
                }
                onClicked: {
                    reciever.doMeasurements(graphs.series(seriesName),true);
                }
            }
        }
    }

    function createSeries() {
        var colorList = [
                    "#F44336", "#673AB7", "#03A9F4", "#4CAF50", "#FFEB3B", "#FF5722",
                    "#E91E63", "#3F51B5", "#00BCD4", "#8BC34A", "#FFC107",
                    "#9C27B0", "#2196F3", "#009688", "#CDDC39", "#FF9800"
                ]
        graphs.numSeries++;
        var seriesName = qsTr(graphs.numSeries + "_"
                            + lineLabel.text)
        graphs.createSeries(ChartView.SeriesTypeLine,
                            seriesName,
                            axisX, axisY);
//        reciever.doMeasurements(graphs.series(seriesName));
        graphs.series(seriesName).color = colorList[
                    Math.random()*100*( graphs.numSeries - 1) % colorList.length ]//"#B71C1C"
//dotted series start
//        var seriesNameDotted = qsTr(seriesName + "_dotted")
//        var series = graphs.createSeries(ChartView.SeriesTypeScatter,
//                            seriesNameDotted,
//                            axisX, axisY);
////        graphs.legend.markers(series)[0].setVisible(false);
//        series.color = graphs.series(seriesName).color;
//        series.markerSize = 7;
//        customLegend.addSeries(seriesName,series.color)
//dotted series end
        reciever.doMeasurements(graphs.series(seriesName), false, graphs.series(seriesNameDotted));
        tableModel.append({
           "name": seriesName,
           "isChecked": true,
           "seriesColor": graphs.series(seriesName).color.toString() })
    }
}
