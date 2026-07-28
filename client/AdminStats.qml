import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: headerRect
    width: parent.width
    height: 80 // متناسب با سایزی که خودت قفل کردی

    // این متغیرها از C++ مقدار می‌گیرند
    property int onlineUsers: 0
    property int totalUsers: 0
    property int unreadCount: 0

    // سیگنالی که به C++ شلیک می‌شود
    signal bellClicked()

    // پس‌زمینه نوار بالا
    Rectangle {
        anchors.fill: parent
        color: "#2c3e50"
    }

    // بخش آمار سمت راست
    Row {
        anchors.right: parent.right
        anchors.rightMargin: 20
        height: parent.height
        layoutDirection: Qt.RightToLeft
        spacing: 25

        Text { text: "پنل مدیریت سیستم"; color: "white"; font.pixelSize: 18; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
        Rectangle { width: 1; height: 25; color: "#455a64"; anchors.verticalCenter: parent.verticalCenter }
        Text { text: "کاربران آنلاین: " + headerRect.onlineUsers; color: "#2ecc71"; font.pixelSize: 14; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
        Text { text: "کل کاربران: " + headerRect.totalUsers; color: "#3498db"; font.pixelSize: 14; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
    }

    // بخش زنگوله سمت چپ
    Rectangle {
        id: bellContainer
        width: 45; height: 45
        color: "transparent"
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: parent.verticalCenter

        Image {
            id: bellImage
            source: "qrc:/images/icons8-bell-48.png"
            width: 32; height: 32
            anchors.centerIn: parent
        }

        // دایره قرمز (فقط وقتی پیام نخوانده داریم نشان داده می‌شود)
        Rectangle {
            width: 18; height: 18; radius: 9
            color: "#e74c3c"
            visible: headerRect.unreadCount > 0
            anchors.top: bellImage.top; anchors.right: bellImage.right
            anchors.topMargin: -2; anchors.rightMargin: -2

            Text {
                text: headerRect.unreadCount;
                color: "white"; font.pixelSize: 11; font.bold: true;
                anchors.centerIn: parent
            }
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: headerRect.bellClicked() // اینجا به C++ خبر می‌دهیم
        }
    }
}