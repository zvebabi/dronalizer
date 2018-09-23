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
            axisY_Umeas.min = axisYRange_Umeas.x
            axisY_Umeas.max = axisYRange_Umeas.y

            axisY_Uref.min = axisYRange_Uref.x
            axisY_Uref.max = axisYRange_Uref.y

            axisY_Upn.min = axisYRange_Upn.x
            axisY_Upn.max = axisYRange_Upn.y

            axisY_C.min = axisYRange_C.x
            axisY_C.max = axisYRange_C.y
        }
        onSendDebugInfo: {
            showPopupTips(qsTr(data), time)
        }
        onSendDataToUI: {
            uMeasMean.text = Math.round(_mean.x*10000)/10000
            uMeasSD.text   = Math.round(_mean.y*10000)/10000
            uRefMean.text  = Math.round(_ref.x *10000)/10000
            uRefSD.text    = Math.round(_ref.y *10000)/10000
            uPnMean.text   = Math.round(_pn.x  *10000)/10000
            uPnSD.text     = Math.round(_pn.y  *10000)/10000
        }
    }
    RowLayout{
        spacing: 10*app.dp
        anchors.top: menuBar.bottom
        anchors.fill: parent
        anchors.margins: 10*app.dp
        anchors.topMargin: menuBar.height+10*app.dp
        ColumnLayout {
            spacing: 10*app.dp
            anchors.top: menuBar.bottom
            anchors.fill: parent
            anchors.margins: 10*app.dp
            anchors.topMargin: menuBar.height+10*app.dp
            ColumnLayout {
                id: colForSnap
                spacing:2
                Rectangle {
                    color: "transparent"
                    Layout.preferredHeight: app.height / 5
                    Layout.preferredWidth: app.width / 3
                    ChartView {
                        id: graph_Umeas
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
                            id: axisX_Umeas
                            visible:false
                            objectName: "axisX_Umeas"
                            titleText: qsTr("Time")
                            min: 410
                            max: 500
                            tickCount: 13
                            minorTickCount: 3
                            labelFormat: "%.1f"
                        }
                        ValueAxis {
                            id: axisY_Umeas
                            objectName: "axisY_Umeas"
    //                        titleText: app.yAxisName
                            min: 0.9
                            max:1.1
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
                }
                Rectangle {
                    color: "transparent"
                    Layout.preferredHeight: app.height / 5
                    Layout.preferredWidth: app.width / 3
                    ChartView {
                        id: graph_Uref
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
                            id: axisX_Uref
                            visible:false
                            objectName: "axisX_Uref"
                            titleText: qsTr("Time")
                            min: 410
                            max: 500
                            tickCount: 13
                            minorTickCount: 3
                            labelFormat: "%.1f"
                        }
                        ValueAxis {
                            id: axisY_Uref
                            objectName: "axisY_Uref"
    //                        titleText: app.yAxisName
                            min: 0.9
                            max:1.1
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
                }
                Rectangle {
                    color: "transparent"
                    Layout.preferredHeight: app.height / 5
                    Layout.preferredWidth: app.width / 3
                    ChartView {
                        id: graph_Upn
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
                            id: axisX_Upn
                            visible: false
                            objectName: "axisX_Upn"
                            titleText: qsTr("Time")
                            min: 410
                            max: 500
                            tickCount: 13
                            minorTickCount: 3
                            labelFormat: "%.1f"
                        }
                        ValueAxis {
                            id: axisY_Upn
                            objectName: "axisY_Upn"
    //                        titleText: app.yAxisName
                            min: 0.9
                            max:1.1
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
                }
                Rectangle {
                    color: "transparent"
                    Layout.preferredHeight: app.height / 5
                    Layout.preferredWidth: app.width / 3
                    ChartView {
                        id: graph_C
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
                            id: axisX_C
                            objectName: "axisX_C"
                            titleText: qsTr("Time")
                            min: 410
                            max: 500
                            tickCount: 13
                            minorTickCount: 3
                            labelFormat: "%.1f"
                        }
                        ValueAxis {
                            id: axisY_C
                            objectName: "axisY_C"
    //                        titleText: app.yAxisName
                            min: 0.025
                            max: 0.035
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
                }
    //            Rectangle {
    //                color: "transparent"
    //                Layout.height: app.height/5
    //                Layout.width: app.width/3
    //            ListView {
    //                    z:1
    //                    id: textData
    //                    anchors.bottom: parent.bottom
    //                    anchors.left: parent.left
    //                    anchors.margins: 100*app.dp
    //                    height: app.fontPixelSize * 1.5 * count
    //                    width: parent.width - 20*app.dp
    //                    model: availableData
    //                    delegate: Text {
    //                        text: data_
    //                        color: palette.darkPrimary
    //                        opacity: 0.7
    //                        font.pixelSize: app.fontPixelSize
    //                    }
    //                    ListModel {
    //                        id: availableData
    ////                        ListElement { data_: qsTr("testElement0")}
    ////                        ListElement { data_: qsTr("testElement1")}
    ////                        ListElement { data_: qsTr("testElement2")}
    ////                        ListElement { data_: qsTr("testElement3")}
    //                    }
    //                }
    //            }
            }
            ChartViewEditBar {
                id: editBar
                visible: false
                Layout.alignment: Qt.AlignHCenter |Qt.AlignBottom

            }

        }
        ColumnLayout {
            RowLayout{
            ColumnLayout{
                RowLayout {
                    Button {
                        id: startMeasureAverage
                        enabled: true
                        contentItem: ButtonLabel {text: qsTr("StartMeasureAverage")}
                        onClicked: {
                            reciever.prepareCommandToSend("duty\r")
                            reciever.setFlyMode(true,sampleSize.text)
                        }
                    }
                    Button {
                        id: writeToFile1
                        enabled: true
                        contentItem: ButtonLabel {text: qsTr("WriteToFile#1")}
                        onClicked: {
                            if(filePathText1_tf.text.length < 2)
                            {
                                showPopupTips(qsTr("Filename#1 too short"), 1500)
                            } else {
                            reciever.writeToFileOne(true
                                    ,currentTemperature_tf.text
                                    ,gasConc_tf.text
                                    ,filePathText1_tf.text)
                            }
                        }
                    }
                }
                TextField {
                    id: sampleSize
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Sample Size")
                    text:qsTr("20")
                    placeholderText: "Sample Size"
                }
                GridLayout {
                    columns: 2
    //                height: 3
                    TextField {
                        id: uMeasMean
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Measured value")
                        placeholderText: "uMeasMean"
                    }
                    TextField {
                        id: uMeasSD
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("StDev")
                        placeholderText: "uMeasSD"
                    }
                    TextField {
                        id: uRefMean
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Reference")
                        placeholderText: "uRefMean"
                    }
                    TextField {
                        id: uRefSD
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("StDev")
                        placeholderText: "uRefSD"
                    }
                    TextField {
                        id: uPnMean
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Pn")
                        placeholderText: "uPnMean"
                    }
                    TextField {
                        id: uPnSD
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("StDev")
                        placeholderText: "uPnSD"
                    }
                }
            }
            ColumnLayout{
                TextField {
                    id: currentTemperature_tf
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Temperature")
                    placeholderText: "Temp"
                }
                TextField {
                    id: gasConc_tf
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Gas conc.")
                    placeholderText: "Conc"
                }

            }
            }
            RowLayout{
                TextField {
                    id: filePathText1_tf
                    visible: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Filename#1")
                    placeholderText: "Filename#1"
//                    text:reciever.getDataPath()
                    font.family: "DejaVu Sans Mono"
                    font.pixelSize: app.fontPixelSize
                    readOnly: false
                    selectByMouse: true
                }
                Button {
                    id: selectPath1
                    contentItem:  ButtonLabel {text:qsTr("Write to File#1")}
                    visible: false
                    FileDialog {
                        id: fileDialog1
                        title: qsTr("Select directory")
                        visible: false
                        folder: "file:///" + reciever.getDataPath()
                        selectExisting: true
                        selectFolder: false
                        selectMultiple: false
                        onAccepted: {
    //                        reciever.selectPath(fileUrl.toString().substring(8) + "/")
    //                        filePathText1_tf.text = reciever.getDataPath()
                        }
                    }
                    onClicked: fileDialog1.open()
                }
            }
            TextField {
                id: filePathText2_tf
                visible: true
//                text:reciever.getDataPath()
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Filenume#2")
                placeholderText: "Filename#2"
                font.family: "DejaVu Sans Mono"
                font.pixelSize: app.fontPixelSize
                readOnly: false
                selectByMouse: true
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: {
                        if(mouse.button === Qt.RightButton) {
                            filePathText2_tf.copy()
                            tipsWithPath.showedText = qsTr("Path has been copied to clipboard")
                            tipsWithPath.open()
                            delay(1500, tipsWithPath.close)
                            filePathText2_tf.deselect()
                        }
                        if(mouse.button === Qt.LeftButton) {
                            filePathText2_tf.selectAll()
                        }
                    }
                }
            }
            Button {
                id: selectPath2
                contentItem:  ButtonLabel {text:qsTr("Select File#2")}
                visible: false
                FileDialog {
                    id: fileDialog2
                    title: qsTr("Select directory")
                    visible: false
                    folder: "file:///" + reciever.getDataPath()
                    selectExisting: true
                    selectFolder: true
                    selectMultiple: false
                    onAccepted: {
//                        reciever.selectPath(fileUrl.toString().substring(8) + "/")
//                        filePathText_tf.text = reciever.getDataPath()
                    }
                }
                onClicked: fileDialog2.open()
            }
            RowLayout{
                TextField {
                    id: sampleSize2
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Sample Size#2")
                    placeholderText: "Sample Size 2"
                }
                TextField {
                    id: serialNumber
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Serial#")
                    placeholderText: "SerialNumber"
                }
                TextField {
                    id: writeDelay
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Write delay")
                    placeholderText: "WriteDelay"
                }
            }
            Switch {
                id: writeToFileSwitcher
                text: qsTr("WriteModeOff")
                onClicked: {
                    //TODO: send a signal to reciever to switch between DEBUG/WORK mode
                    console.log("position: " + position)
                    if (position == 1) {
                        writeToFileSwitcher.text = qsTr("WriteModeOn")
                        reciever.prepareCommandToSend("debug\r")
                        reciever.setFlyMode(false)
                    }
                    else {
                        writeToFileSwitcher.text = qsTr("WriteModeOff")
//                        reciever.prepareCommandToSend("work\r")
//                        reciever.setFlyMode(true)
                    }
                }
            }
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

