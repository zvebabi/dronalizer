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
    RowLayout{
        anchors.fill: parent
        anchors.margins: statusBar.height + 10*app.dp
        anchors.topMargin: menuBar.height + 10*app.dp
        Column {
            id: deviceSetter
            spacing: 5*app.dp
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.minimumWidth: deviceSetter.itemsWidth * 1.1
            Layout.fillHeight: true
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
            ComboBox {
                id: baudRateComboList
                objectName: "comboList"
                model: availableBaudRate
                width: deviceSetter.itemsWidth
                currentIndex: 3
                ListModel{
                    id: availableBaudRate
                        ListElement{ text: "1200"   }
                        ListElement{ text: "2400"   }
                        ListElement{ text: "4800"   }
                        ListElement{ text: "9600"   }
                        ListElement{ text: "19200"  }
                        ListElement{ text: "38400"  }
                        ListElement{ text: "57600"  }
                        ListElement{ text: "115200" }
                }
            }
            Button {
                id: connectBTN
                contentItem: ButtonLabel {text: qsTr("Connect")}
                width: deviceSetter.itemsWidth
                onClicked: {
                    reciever.initDevice(portsComboList.currentText
                                      , baudRateComboList.currentText
                                      , currentDeviceSetting.propertyName
                                      , currentDeviceSetting.values);
                }
            }
            Button {
                id: sendCoeffsBTN
                contentItem: ButtonLabel {text: qsTr("Send settings")}
                width: deviceSetter.itemsWidth
                onClicked: {
                    reciever.updateProperties(currentDeviceSetting.propertyName
                                            , currentDeviceSetting.values)
                }
            }
            Label {
                text: qsTr("Save images and data to: ")
                font.family: "DejaVu Sans Mono"
                font.pixelSize: app.fontPixelSize
                visible: false
            }
            TextField {
                id: filePathText
                visible: false
                width: deviceSetter.itemsWidth
                text:reciever.getDataPath()
                font.family: "DejaVu Sans Mono"
                font.pixelSize: app.fontPixelSize
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
        }
        Rectangle {
            id: currentDeviceSetting
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            color: "transparent"
//            Layout.preferredHeight: deviceSetter.height
            Layout.minimumHeight: 350*app.dp
            Layout.fillHeight: true
            Layout.minimumWidth: 400 *app.dp
            property var values: ["1", "1.0", "1.0", "1.0", "1.0"]
            property var propertyName:["set_m","cal_a","cal_b","cal_c","cal_n"]
            ListModel{
                id: propertiesModel
                ListElement { name: qsTr("set_m") }
                ListElement { name: qsTr("cal_a") }
                ListElement { name: qsTr("cal_b") }
                ListElement { name: qsTr("cal_c") }
                ListElement { name: qsTr("cal_n") }
            }
            ListView {
                id: propertiesView
                width: 180; height: 200
                anchors.left: parent.left
                anchors.top: parent.top
                model: propertiesModel
                delegate: RowLayout {
                    spacing: 5*app.dp
                    Text {
                        Layout.preferredWidth: 70* app.dp
                        Layout.alignment: Qt.AlignBottom
                        Layout.bottomMargin: 23 * app.dp
                        text: name
                        font.pixelSize: app.fontPixelSize}
                    TextField {
                        Layout.alignment: Qt.AlignBottom
                        text: currentDeviceSetting.values[index]
                        inputMask: index == 0 ? "dd;_" : "9.9dddd;_"
                        placeholderText: placeHolderText
                        font.pixelSize: app.fontPixelSize
                        onEditingFinished: {
                            currentDeviceSetting.values[index] = text;
                            currentDeviceSetting.propertyName[index] = name;
                            //TODO: check correctnes of input
                        }
                    }
                }
            }

    //information
            ColumnLayout{
                anchors.bottom: parent.bottom
                anchors.topMargin: 10*app.dp
                Label {
                    id: pathLbl_h
                    font.pixelSize: app.fontPixelSize
                    text: qsTr("Measurement data storage directory: ")
                }
                Label {
                    id: pathLbl
                    font.pixelSize: app.fontPixelSize
                    text: filePathText.text
                }
                Label {
                    id: pathLbl_f
                    font.pixelSize: app.fontPixelSize
                    text: qsTr("Click \"Save data to…\" to choose another location")
                }
            }
        }
    }
}
