QT += quick widgets bluetooth

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        src/ble/cooler.cpp \
        src/ble/scanner.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/ble/cooler.h \
    src/ble/scanner.h

INCLUDEPATH += \
    src/ble

DISTFILES +=

#win32 {
#    RC_FILE += win/app_icon.rc
#}

#ios {
#    ios_icon.files = $$files($$PWD/ios/Assets.xcassets/AppIcon.appiconset/Icon-App*.png)
#    app_launch_images.files = $$files($$PWD/ios/Assets.xcassets/LaunchImage.launchimage/Default*.png)
#    QMAKE_BUNDLE_DATA += ios_icon app_launch_images
#    QMAKE_INFO_PLIST = ios/Project-Info.plist
#    OTHER_FILES += $$QMAKE_INFO_PLIST
#}

#macx {
#    ICON = macx/app_icon.icns
#}

#contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
#    ANDROID_PACKAGE_SOURCE_DIR = \
#        $$PWD/android
#}
