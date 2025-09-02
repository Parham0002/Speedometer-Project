#ifndef WINDOW_H
#define WINDOW_H

#include "canvas.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QTimer>

class Window : public QDialog
{
    QVBoxLayout mainLayout{this};
    Canvas canvas;
    QTimer blink_timer;
    QTimer draw_timer;

public:
    explicit Window(COMService *_service);
    void showEvent(QShowEvent *event) override;
};

#endif // WINDOW_H
