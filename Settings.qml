import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtCharts 2.2
import QtQuick.Dialogs 1.2
import "QMLs"

Item {
    Connections {
        target: reciever
        onSendPortName: {
            availablePorts.append({"text": port});
            console.log(port)
        }
        onSendDebugInfo: {
            tipsWithPath.showedText = qsTr(data)
            tipsWithPath.open()
            delay(time, tipsWithPath.close)
        }
        onSendAxisName: {
            app.yAxisName = qsTr(data)
        }
        onDisableButton: {
            connectBTN.enabled = false
            listDeviceBTN.enabled = false
            portsComboList.enabled =false
        }
        onActivateRelativeMod: {
            relativeMeasurements.checked = true
            relativeMeasurements.enabled = false
            etalonNameLbl.text = qsTr("Load etalon data file to switch"
                                    + " to Absolute measuring mode")
            app.relativeMode = true
            reciever.setRelativeMode(true)
            selectEtalonPath.visible = true
        }
        onDeActivateRelativeMod: {
            relativeMeasurements.checked = false
            relativeMeasurements.enabled = true
            app.relativeMode = false
            reciever.setRelativeMode(false)
            selectEtalonPath.visible = false
//            selectEtalonPath.background = "green"
        }
        onSendSerialNumber: {
            serNumLbl.text = serNumber;
        }
        onSendEtalonName: {
            etalonNameLbl.text = etalonName;
        }
    }
    Timer {
        id: timer
    }
    function delay(delayTime, cb) {
        timer.interval = delayTime;
        timer.repeat = false;
        timer.triggered.connect(cb);
        timer.start();
    }
    Grid{
        columns: 2
        spacing: 50*app.dp
//        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 100*app.dp
        anchors.top: parent.top
        Column {
            id: deviceSetter
            spacing: 5*app.dp
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.rightMargin: 50*app.dp
            property int itemsWidth: 250*app.dp
            Button {
                id: listDeviceBTN
                contentItem: ButtonLabel {text: qsTr("Select Device")}
                width: deviceSetter.itemsWidth
                onClicked: {//getPorts()
                    availablePorts.clear()
                    reciever.getListOfPort()
                }
//                height: 150*app.dp
            }

            ComboBox {
                id: portsComboList
                objectName: "comboList"
                model: availablePorts
                width: deviceSetter.itemsWidth
                ListModel{
                    id: availablePorts
                }

            }

            Button {
                id: connectBTN
                contentItem: ButtonLabel {text: qsTr("Connect")}
                width: deviceSetter.itemsWidth
                onClicked: {
                    reciever.initDevice(portsComboList.currentText);
//                    app.ctmLegendVisibility = false;
                }
            }
            Label {
                text: qsTr("Save images and data to: ")
                font.family: "DejaVu Sans Mono"
                font.pixelSize: 22*app.dp
                visible: false
            }
            TextField {
                id: filePathText
                visible: false
                width: deviceSetter.itemsWidth
                text:reciever.getDataPath()
                font.family: "DejaVu Sans Mono"
                font.pixelSize: 22*app.dp
                readOnly: true
                selectByMouse: true
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: {
                        if(mouse.button === Qt.RightButton) {
                            filePathText.copy()
                            tipsWithPath.showedText = qsTr("Path has been copied to clipboard")
                            tipsWithPath.open()
                            delay(1500, tipsWithPath.close)
                            filePathText.deselect()
                        }
                        if(mouse.button === Qt.LeftButton) {
                            filePathText.selectAll()
                        }
                    }
                }
            }
            Button {
                id: selectPath
                contentItem:  ButtonLabel {text:qsTr("Save data to…")}
                width: deviceSetter.itemsWidth
                FileDialog {
                    id: fileDialog
                    title: qsTr("Select directory")
                    visible: false
                    folder: "file:///" + reciever.getDataPath()
                    selectExisting: true
                    selectFolder: true
                    selectMultiple: false
                    onAccepted: {
                        reciever.selectPath(fileUrl.toString().substring(8) + "/")
                        filePathText.text = reciever.getDataPath()

                    }
                }
                onClicked: fileDialog.open()
            }

//            CheckBox {
//                id:serviceMode
//                text: qsTr("Save raw data")
//                checked: app.serviceMode
//                onClicked: {
//                    reciever.setServiceMode(checked)
//                    app.serviceMode = checked
//                }
//            }
//            CheckBox {
//                id:cumulativeMeasurements
//                text: qsTr("Cumulative mode")
//                checked: app.cumulativeMode
//                onClicked: {
//                    reciever.setCumulativeMode(checked)
//                    app.cumulativeMode = checked
//                }
//            }
//            CheckBox {
//                id:antialiasingManual
//                text: qsTr("Enable antialiasing")
//                checked: app.aaManual
//                onClicked: {
//                    reciever.enableAAManual(checked)
//                    app.aaManual = checked
//                }
//            }
//            RadioButton {
//                id:name1
//                checked: true
//                text: qsTr("Absorbance")
//                onClicked: {
//                    app.yAxisName = name1.text
//                }
//            }
//            RadioButton {
//                id:name2
//                checked: false
//                text: qsTr("Transmittance")
//                onClicked: {
//                    app.yAxisName = name2.text
//                }
//            }
        }

    }
//    Column {
    Rectangle {
        id: currentDeviceSetting
//            spacing: 5*app.dp
        anchors.bottom: parent.bottom
        anchors.bottomMargin: statusBar.height + 10*app.dp
        anchors.left: parent.left
        anchors.leftMargin: anchors.bottomMargin

        color: palette.darkPrimary
        height: 350*app.dp
        width: 400 *app.dp
        Label {
            text: qsTr("cal_a");
            width: 100*app.dp
            anchors.right: cal_a.left
//            anchors.left: parent.left
            anchors.verticalCenter: cal_a.verticalCenter
        }
        TextField {
            id:cal_a
            anchors.right: parent.right
            anchors.bottom: cal_b.top
            placeholderText: qsTr("Enter cal_a")
        }

        Label {
            text: qsTr("cal_b")
            anchors.right: cal_a.left
            anchors.left: parent.left
            anchors.verticalCenter: cal_b.verticalCenter
        }
        TextField {
            id: cal_b
            anchors.bottom: cal_c.top
            placeholderText: qsTr("Enter cal_b")
        }

        Label { text: qsTr("cal_c")
            anchors.right: cal_a.left
            anchors.left: parent.left
            anchors.verticalCenter: cal_c.verticalCenter
        }
        TextField {
            id: cal_c
            anchors.bottom: cal_n.top
            placeholderText: qsTr("Enter cal_c")
        }

        Label { text: qsTr("cal_n")
            anchors.right: cal_a.left
            anchors.left: parent.left
            anchors.verticalCenter: cal_n.verticalCenter
        }
        TextField {
            id: cal_n
            anchors.bottom: set_m.top
            placeholderText: qsTr("Enter cal_n")
        }

        Label {
            text: qsTr("set_m")
            anchors.right: cal_a.left
            anchors.left: parent.left
            anchors.verticalCenter: set_m.verticalCenter
        }
        TextField {
            id: set_m
            anchors.bottom: pathLbl_h.top
            placeholderText: qsTr("1-20")
        }
//information
        Label {
            id: pathLbl_h
            anchors.bottom: pathLbl.top
            anchors.left: parent.left
            text: qsTr("Measurement data storage directory: ")
        }
        Label {
            id: pathLbl
            anchors.bottom: pathLbl_f.top
            anchors.left: parent.left
            text: filePathText.text
        }
        Label {
            id: pathLbl_f
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            text: qsTr("Click \"Save data to…\" to choose another location") }
    }
}
