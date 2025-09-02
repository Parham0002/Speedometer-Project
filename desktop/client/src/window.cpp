#include "window.h"
#include "canvas.h"
#include "setting.h"
#include <QGuiApplication>
#include <QScreen>

Window::Window(COMService *_service) : canvas{_service}
{
    setFixedSize(settings::Config::CLIENT_WINDOW_WIDTH, settings::Config::CLIENT_WINDOW_HEIGHT);
    setStyleSheet("background-color: rgb(61, 36, 53);");
    setLayout(&mainLayout);
    setWindowTitle("Client");
    mainLayout.addWidget(&canvas);
    connect(&draw_timer, SIGNAL(timeout()), &canvas, SLOT(update()));
    connect(&blink_timer, &QTimer::timeout, &canvas, &Canvas::toggle_blink);
    draw_timer.start(settings::DRAW_INTERVAL);
    blink_timer.start(settings::BLINK_INTERVAL);
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
}

void Window::showEvent(QShowEvent *)
{
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = ((screenGeometry.height() - height()) / 2);
    move(x, y);
}