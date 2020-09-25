import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Qt.labs.settings 1.0

ApplicationWindow {
    width: 800
    height: 800
    title: "Requester"
    id: window

    FontMetrics {
        id: metrics
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 5
        spacing: 10
        GridLayout {
            Layout.fillWidth: true
            columns: 3
            rowSpacing: 5
            columnSpacing: 5

            /// URL
            Label {
                text: "Url:"
            }

            ComboBox {
                id: baseUrl
                Layout.fillWidth: true
                editable: true
                Layout.columnSpan: 2
                model: ListModel {
                }
                function insertCurrent() {
                     if (find(editText) === -1) {
                         model.append({text: editText})
                         currentIndex = model.rowCount()-1;
                         saveList();
                     }
                }
                Component.onCompleted: {
                    let lst = JSON.parse(settings.value("urls", "[]"))
                    for(let i in lst) {
                        model.append({text: lst[i]})
                    }
                    //currentIndex = lst.length - 1;
                }
                function saveList() {
                    let lst = [];
                    for(let i = 0; i < model.rowCount(); ++i) {
                        lst.push(model.get(i).text);
                    }
                    settings.setValue("urls", JSON.stringify(lst));
                }
            }

            /// Credentials
            Label {
                text: "User name:"
            }
            TextField {
                id: userName
                text: ""
                Layout.fillWidth: true
                Layout.columnSpan: 2
            }
            Label {
                text: "Password:"
            }
            TextField {
                id: password
                text: ""
                Layout.fillWidth: true
                echoMode: TextInput.Password
                Layout.columnSpan: 2
            }

            /// Spacer
            Item {
                Layout.columnSpan: 3
                width: 1 // dummy value != 0
                height: 30
            }

            /// Method
            Label {
                text: "Method"
            }

            ComboBox {
                id: method
                Layout.fillWidth: true
                editable: true
                model: ListModel {
                }
                function insertCurrent() {
                     if (find(editText) === -1) {
                         model.append({text: editText})
                         currentIndex = model.rowCount()-1;
                         saveList();
                     }
                }
                Component.onCompleted: {
                    let lst = JSON.parse(settings.value("methods", "[]"))
                    for(let i in lst) {
                        model.append({text: lst[i]})
                    }
                    //currentIndex = lst.length - 1;
                }
                function saveList() {
                    let lst = [];
                    for(let i = 0; i < model.rowCount(); ++i) {
                        lst.push(model.get(i).text);
                    }
                    settings.setValue("methods", JSON.stringify(lst));
                }
            }

            ComboBox {
                id: type
                model: ["GET", "POST", "DELETE"]
            }

            /// Payload
            Label {
                text: "Payload"
            }

            ScrollView {
                id: view
                Layout.columnSpan: 2
                Layout.fillWidth: true
                implicitHeight: 100

                TextArea {
                    id: payload
                    wrapMode: TextEdit.NoWrap
                    tabStopDistance: metrics.advanceWidth(" ")*4
                }

                background: Rectangle {
                    border.color: view.activeFocus ? "#21be2b" : "#bdbebf"
                }
            }
        }

        Button {
            text: "Send request"
            Layout.alignment: Qt.AlignRight
            onClicked: {
                method.insertCurrent();
                baseUrl.insertCurrent();
                request.run(baseUrl.editText, method.editText, type.displayText, payload.text, userName.text, password.text);
            }
        }

        GroupBox {
            Layout.fillWidth: true
            background: Rectangle {
                color: "#dedede"
                border.color: "#aeaeae"
                radius: 2
            }
            GridLayout {
                columns: 2
                Label {
                    text: "Request URL:"
                }
                Label {
                    id: urlInfo
                    Layout.fillWidth: true
                }
                Label {
                    text: "Request Method:"
                }
                Label {
                    id: methodInfo
                    Layout.fillWidth: true
                }
                Label {
                    text: "Response Status:"
                }
                Label {
                    id: statusInfo
                    Layout.fillWidth: true
                }
                Label {
                    id: errorInfo
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    visible: false
                    color: "red"
                }
            }
        }

        ScrollView {
            id: respView
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.fillHeight: true

            TextArea {
                id: response
                wrapMode: TextEdit.NoWrap
                tabStopDistance: metrics.advanceWidth(" ")*4
            }

            background: Rectangle {
                border.color: view.activeFocus ? "#21be2b" : "#bdbebf"
            }
        }
    }

    Connections {
        target: request
        function onSuccess(url, method, status, text) {
            errorInfo.visible = false;
            info(url, method, status, text);
        }
        function onFailure(url, method, status, text, errText) {
            errorInfo.visible = true;
            errorInfo.text = errText;
            info(url, method, status, text);
        }
    }

    function info(url, type, status, text) {
        urlInfo.text = url;
        methodInfo.text = type;
        statusInfo.text = status;
        response.text = text;
    }

    Settings {
        id: settings
        property alias width: window.width
        property alias height: window.height
        property alias userName: userName.text
        property alias type: type.currentIndex
        property alias method: method.currentIndex
        property alias url: baseUrl.currentIndex
        property alias payload: payload.text
    }
}
