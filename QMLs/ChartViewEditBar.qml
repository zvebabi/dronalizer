import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import QtCharts 2.2
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0

RowLayout {
    id: ctrlPane
    spacing: 10*app.dp
    property int itemWidth: 250*app.dp
    Connections {
        target: reciever
        onMakeSeries: {
            createSeries()
        }
    }
    Button {
        id: enableFlyMode
        enabled: true
//                height: 2.3*24*app.dp
        width: 250*app.dp
        contentItem: ButtonLabel {text: qsTr("Fly mode")}
//                ToolTip.visible: hovered
//                    ToolTip.text: qsTr("Enable fly mode")
        onClicked: {
            createSeries()
        }
    }
    Button {
        id: setZeroLevel
//                height: 2.3*48*app.dp
        width: 250*app.dp
//                ToolTip.visible: hovered
//                    ToolTip.text: qsTr("Set zero level")
        contentItem: ButtonLabel {text: qsTr("Set zero level")}
        onClicked: {
            reciever.doMeasurements(graphs.series(seriesName),true);
        }
    }
    Switch {
        text: qsTr("Debug mode")
        onClicked: {
            //TODO: send a signal to reciever to switch between DEBUG/WORK mode
            console.log("position: " + position)
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
