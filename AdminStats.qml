import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: headerRect
    width: parent.width
    height: 70
    color: "#2c3e50" // رنگ پس‌زمینه پنل مدیریت

    // سیگنال کلیک روی زنگوله
    signal bellClicked()

    // این متغیرها به صورت لحظه‌ای از C++ مقدار می‌گیرند و UI را بدون رفرش آپدیت می‌کنند
    property int onlineUsers: 0
    property int totalUsers: 0
    property int notificationCount: 0

    // -------------------------------------------------------------
    // بخش سمت راست: عنوان پنل و آمار کاربران (راست به چپ)
    // -------------------------------------------------------------
    Row {
        anchors.right: parent.right
        anchors.rightMargin: 20
        height: parent.height
        layoutDirection: Qt.RightToLeft
        spacing: 25

        Text {
            text: "پنل مدیریت سیستم"
            color: "#ffffff"
            font.pixelSize: 18
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }

        Rectangle {
            width: 1; height: 25
            color: "#455a64"
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: "کاربران آنلاین: " + headerRect.onlineUsers
            color: "#2ecc71" // رنگ سبز برای کاربران آنلاین
            font.pixelSize: 14
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: "کل کاربران: " + headerRect.totalUsers
            color: "#3498db" // رنگ آبی برای کل کاربران
            font.pixelSize: 14
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    // -------------------------------------------------------------
    // بخش سمت چپ: زنگوله اعلان‌ها و شمارنده قرمز رنگ پیام‌ها
    // -------------------------------------------------------------
    Rectangle {
        id: bellContainer
        width: 45; height: 45
        color: "transparent"
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: parent.verticalCenter

        Image {
            id: bellImage
            source: "qrc:/images/icons8-bell-48.png" // آدرس آیکون زنگوله شما
            width: 32; height: 32
            anchors.centerIn: parent
        }

        // دایره قرمز تعداد اعلان‌ها (اگر تعداد 0 باشد خودکار غیب می‌شود)
        Rectangle {
            id: badge
            width: 18; height: 18
            color: "#e74c3c" // قرمز هشداری
            radius: 9
            visible: headerRect.notificationCount > 0
            anchors.top: bellImage.top
            anchors.right: bellImage.right
            anchors.topMargin: -2
            anchors.rightMargin: -2

            Text {
                text: headerRect.notificationCount
                color: "#ffffff"
                font.pixelSize: 11
                font.bold: true
                anchors.centerIn: parent
            }
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: headerRect.bellClicked() // شلیک سیگنال به سمت C++
        }
    }
}