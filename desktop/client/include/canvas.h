#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPainter>
#include "comservice.h"
#include <QMediaPlayer>
#include <QAudioOutput>

class Canvas : public QWidget
{
private:
    QPainter painter;
    QFont icon_font{"Material Icons"};
    QFont text_font{"Arial"};

    QMediaPlayer clickPlayer;
    QAudioOutput clickAudio;

    bool lastBlinkOn = false;

    COMService *service;

    bool blink_on{false};

    void paintEvent(QPaintEvent *event) override;
    void drawTemperature(void);
    void drawBattery(void);
    void drawSpeedometerCenterCirle(void);
    void drawSpeedometerArc(void);
    /**
     * @brief Draws the longest lines that represents km for: 0, 20, 40, 60 .. 240
     *
     */
    void drawSpeedometerLongLines(void);
    /**
     * @brief Draws the medium lines that represents km for: 10, 30, 50, 70 .. 230
     *
     */
    void drawSpeedometerMediumLines(void);
    /**
     * @brief Draws the small lines that represents km for: 5, 15, 25, 35, .. 235
     *
     */
    void drawSpeedometerSmallLines(void);
    void drawSpeedometerSpeedLabels(void);
    void drawSpeedometerNeedle(void);
    void drawSpeedomterIcon(void);
    void drawSpeedometerConnectionErrorIcon(void);

    void drawTurnSignals(void);

public:
    void toggle_blink(void);
    Canvas(COMService *_service);
};

#endif