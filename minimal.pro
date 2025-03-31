# Started with the minimal example: https://code.qt.io/cgit/qt/qtwebengine.git/tree/examples/webengine/minimal?h=5.15
TEMPLATE = app

QT += webengine

# Needed to add this line (at least on Ubuntu 20.04.6) to fix building the minimal example
# See: https://bugreports.qt.io/browse/QTBUG-80964
CONFIG += use_gold_linker

SOURCES += main.cpp

RESOURCES += qml.qrc
