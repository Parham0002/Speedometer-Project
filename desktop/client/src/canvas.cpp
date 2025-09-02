#include "canvas.h"
#include "positions.h"

Canvas::Canvas(COMService *_service) : service{_service}
{
    setFixedSize(settings::Config::CLIENT_WINDOW_WIDTH, settings::Config::CLIENT_WINDOW_HEIGHT);

    clickPlayer.setAudioOutput(&clickAudio);
    clickAudio.setVolume(0.7f);

    // set the file once (CMake copies sound.wav into the build dir)
    clickPlayer.setSource(QUrl::fromLocalFile("sound.wav"));
}

void Canvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    drawTemperature();

    drawBattery();

    drawSpeedometerCenterCirle();
    drawSpeedometerArc();
    drawSpeedometerLongLines();
    drawSpeedometerMediumLines();
    drawSpeedometerSmallLines();
    drawSpeedometerSpeedLabels();
    drawSpeedometerNeedle();

    if (blink_on)
    {
        drawTurnSignals();
    }

    if (service->get_status())
    {
        drawSpeedomterIcon();
    }
    else
    {
        drawSpeedometerConnectionErrorIcon();
    }
}

void Canvas::drawTemperature(void)
{
    painter.begin(this);
    QRect icon_rect(Positions::TEMP_X, Positions::TEMP_Y, Positions::TEMP_WIDTH, Positions::TEMP_ICON_HEIGHT);
    QRect text_rect(Positions::TEMP_X, Positions::TEMP_Y + Positions::TEMP_ICON_HEIGHT, Positions::TEMP_WIDTH, Positions::TEMP_TEXT_HEIGHT);

    QColor color;

    int temperature = service->get_temperature();

    if (temperature < 5)
    {
        color = Qt::white;
    }
    else if (temperature < 40)
    {
        color = Qt::blue;
    }
    else
    {
        color = Qt::red;
    }
    icon_font.setPointSize(45);
    painter.setFont(icon_font);
    painter.setPen(color);
    painter.drawText(icon_rect, Qt::AlignCenter, QChar(0xe1ff));

    text_font.setPointSize(13);
    color = "white";
    painter.setPen(color);
    painter.setFont(text_font);
    painter.drawText(text_rect, Qt::AlignCenter, QString("%1 °C").arg(temperature));

    painter.end();
}

void Canvas::drawBattery(void)
{
    painter.begin(this);

    QRect icon_rect(Positions::BATTERY_X, Positions::BATTERY_Y, Positions::BATTERY_WIDTH, Positions::BATTERY_ICON_HEIGHT);
    QRect text_rect(Positions::BATTERY_X, Positions::BATTERY_Y + Positions::BATTERY_ICON_HEIGHT, Positions::BATTERY_WIDTH, Positions::BATTERY_TEXT_HEIGHT);

    icon_font.setPointSize(90);
    painter.setFont(icon_font);
    painter.setPen("white");
    painter.drawText(icon_rect, Qt::AlignCenter, QChar(0xe1a3));

    int battery = service->get_battery();

    QColor fill_color;
    if (battery < 25)
    {
        fill_color = Qt::red;
    }
    else if (battery < 50)
    {
        fill_color = Qt::yellow;
    }
    else
    {
        fill_color = Qt::green;
    }

    // Clip area for fill
    int fill_height = icon_rect.height() * battery / 100;
    QRect fill_rect(icon_rect.left(), icon_rect.bottom() - fill_height, icon_rect.width(), fill_height);

    painter.setClipRect(fill_rect);
    painter.setPen(fill_color);
    painter.drawText(icon_rect, Qt::AlignCenter, QChar(0xe1a3));
    painter.setClipping(false);

    text_font.setPointSize(13);
    painter.setPen("white");
    painter.setFont(text_font);
    painter.drawText(text_rect, Qt::AlignCenter, QString("%1 %").arg(battery));

    painter.end();
}
void Canvas::drawSpeedometerCenterCirle(void)
{
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(QPen(Qt::white, 10));
    painter.setBrush(QColor(139, 0, 0)); // Dark red
    painter.drawEllipse(QPoint(Positions::SPEEDOMETER_CENTER_X, Positions::SPEEDOMETER_CENTER_Y), Positions::SPEEDOMETER_CENTER_CIRCLE_RADIUS, Positions::SPEEDOMETER_CENTER_CIRCLE_RADIUS);

    painter.end();
}

void Canvas::drawSpeedometerArc(void)
{
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect outer_rect(Positions::SPEEDOMETER_CENTER_X - Positions::SPEEDOMETER_ARC_RADIUS, Positions::SPEEDOMETER_CENTER_Y - Positions::SPEEDOMETER_ARC_RADIUS,
                     2 * Positions::SPEEDOMETER_ARC_RADIUS, 2 * Positions::SPEEDOMETER_ARC_RADIUS);

    painter.setPen(QPen(Qt::white, Positions::SPEEDOMETER_ARC_THICKNESS));

    painter.drawArc(outer_rect, Positions::SPEEDOMETER_ARC_START_ANGLE * 16, -(Positions::SPEEDOMETER_ARC_DEGREES_TO_DRAW * 16));

    painter.end();
}

void Canvas::drawSpeedometerLongLines(void)
{
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(QPen(Qt::white, Positions::SPEEDOMETER_LONG_LINES_THICHNESS));

    int speed_value;
    float angle_deg;
    float angle_rad;

    for (int i = 0; i <= 12; ++i)
    {
        speed_value = i * 20;
        angle_deg = Positions::SPEEDOMETER_ARC_START_ANGLE - Positions::SPEEDOMETER_LINES_ANGLE_OFFSET - speed_value;
        angle_rad = qDegreesToRadians(angle_deg);

        QPointF outer(
            Positions::SPEEDOMETER_CENTER_X + Positions::SPEEDOMETER_LINES_START_RADIUS * std::cos(angle_rad),
            Positions::SPEEDOMETER_CENTER_Y - Positions::SPEEDOMETER_LINES_START_RADIUS * std::sin(angle_rad));
        QPointF inner(
            Positions::SPEEDOMETER_CENTER_X + Positions::SPEEDOMETER_LONG_LINES_END_RADIUS * std::cos(angle_rad),
            Positions::SPEEDOMETER_CENTER_Y - Positions::SPEEDOMETER_LONG_LINES_END_RADIUS * std::sin(angle_rad));
        painter.drawLine(inner, outer);
    }

    painter.end();
}

void Canvas::drawSpeedometerMediumLines(void)
{

    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(QPen(Qt::white, Positions::SPEEDOMETER_MEDIUM_LINES_THICKNESS));

    int speed_value;
    float angle_deg;
    float angle_rad;

    for (int i = 0; i < 12; ++i)
    {
        speed_value = (i * 20) + 10;
        angle_deg = Positions::SPEEDOMETER_ARC_START_ANGLE - Positions::SPEEDOMETER_LINES_ANGLE_OFFSET - speed_value; // ändra LONG till en gemensam till alla lines?
        angle_rad = qDegreesToRadians(angle_deg);

        QPointF outer(
            Positions::SPEEDOMETER_CENTER_X + Positions::SPEEDOMETER_LINES_START_RADIUS * std::cos(angle_rad),
            Positions::SPEEDOMETER_CENTER_Y - Positions::SPEEDOMETER_LINES_START_RADIUS * std::sin(angle_rad));
        QPointF inner(
            Positions::SPEEDOMETER_CENTER_X + Positions::SPEEDOMETER_MEDIUM_LINES_END_RADIUS * std::cos(angle_rad),
            Positions::SPEEDOMETER_CENTER_Y - Positions::SPEEDOMETER_MEDIUM_LINES_END_RADIUS * std::sin(angle_rad));
        painter.drawLine(inner, outer);
    }
    painter.end();
}
void Canvas::drawSpeedometerSmallLines(void)
{
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(QPen(Qt::white, Positions::SPEEDOMETER_SMALL_LINES_THICKNESS));

    int speed_value;
    float angle_deg;
    float angle_rad;

    for (int i = 0; i <= 23; ++i)
    {
        speed_value = i * 10 + 5; // 5, 15, 25, ..., 235

        angle_deg = Positions::SPEEDOMETER_ARC_START_ANGLE - Positions::SPEEDOMETER_LINES_ANGLE_OFFSET - speed_value;
        angle_rad = qDegreesToRadians(angle_deg);

        QPointF outer(
            Positions::SPEEDOMETER_CENTER_X + Positions::SPEEDOMETER_LINES_START_RADIUS * std::cos(angle_rad),
            Positions::SPEEDOMETER_CENTER_Y - Positions::SPEEDOMETER_LINES_START_RADIUS * std::sin(angle_rad));
        QPointF inner(
            Positions::SPEEDOMETER_CENTER_X + Positions::SPEEDOMETER_SMALL_LINES_END_RADIUS * std::cos(angle_rad),
            Positions::SPEEDOMETER_CENTER_Y - Positions::SPEEDOMETER_SMALL_LINES_END_RADIUS * std::sin(angle_rad));
        painter.drawLine(inner, outer);
    }

    painter.end();
}

void Canvas::drawSpeedometerSpeedLabels(void)
{
    painter.begin(this);

    text_font.setPointSize(Positions::SPEEDOMETER_LABEL_TEXT_SIZE);
    painter.setFont(text_font);
    painter.setPen(QPen(Qt::white));

    const int label_radius = Positions::SPEEDOMETER_ARC_RADIUS - Positions::SPEEDOMETER_ARC_THICKNESS - Positions::SPEEDOMETER_LABEL_OFFSET_FROM_ARC;
    const int lable_text_offset_x = Positions::SPEEDOMETER_LABEL_TEXT_WIDTH / 2;
    const int lable_text_offset_y = Positions::SPEEDOMETER_LABEL_TEXT_HEIGHT / 2;

    for (int i = 0; i <= 12; ++i)
    {
        int speed_value = i * 20;
        float angle_deg = Positions::SPEEDOMETER_ARC_START_ANGLE - Positions::SPEEDOMETER_LINES_ANGLE_OFFSET - speed_value;
        float angle_rad = qDegreesToRadians(angle_deg);

        QPointF label_point(
            Positions::SPEEDOMETER_CENTER_X + (label_radius - Positions::SPEEDOMETER_LABEL_INNER_PADDING_X) * std::cos(angle_rad) - lable_text_offset_x,
            Positions::SPEEDOMETER_CENTER_Y - (label_radius - Positions::SPEEDOMETER_LABEL_INNER_PADDING_Y) * std::sin(angle_rad) - lable_text_offset_y);

        painter.drawText(QRectF(label_point, QSizeF(Positions::SPEEDOMETER_LABEL_TEXT_WIDTH, Positions::SPEEDOMETER_LABEL_TEXT_HEIGHT)),
                         Qt::AlignCenter, QString::number(speed_value));
    }

    painter.end();
}

void Canvas::drawSpeedometerNeedle(void)
{
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(QPen(QColor(139, 0, 0), Positions::SPEEDOMETER_NEEDLE_THICKNESS));

    const int needle_length = Positions::SPEEDOMETER_ARC_RADIUS - (Positions::SPEEDOMETER_ARC_THICKNESS / 2) - Positions::SPEEDOMTER_NEEDLE_OFFSET_FROM_ARC;

    float angle_deg = Positions::SPEEDOMETER_ARC_START_ANGLE - Positions::SPEEDOMETER_LINES_ANGLE_OFFSET - service->get_speed();
    float angle_rad = qDegreesToRadians(angle_deg);

    QPointF outer(
        Positions::SPEEDOMETER_CENTER_X + needle_length * std::cos(angle_rad),
        Positions::SPEEDOMETER_CENTER_Y - needle_length * std::sin(angle_rad));
    QPointF inner(
        Positions::SPEEDOMETER_CENTER_X,
        Positions::SPEEDOMETER_CENTER_Y);
    painter.drawLine(inner, outer);

    painter.end();
}

void Canvas::drawSpeedomterIcon(void)
{
    painter.begin(this);

    icon_font.setPointSize(Positions::SPEEDOMETER_ICON_SIIZE);

    painter.setFont(icon_font);
    painter.setPen(Qt::white);

    const int icon_x{Positions::SPEEDOMETER_CENTER_X - (Positions::SPEEDOMETER_ICON_WIDTH / 2)};
    const int icon_y{Positions::SPEEDOMETER_CENTER_Y + (Positions::SPEEDOMETER_ARC_RADIUS / 2) - (Positions::SPEEDOMETER_ICON_HIGHT / 2) - Positions::SPEEDOMETER_LABEL_OFFSET_FROM_ARC - Positions::SPEEDOMETER_LABEL_TEXT_HEIGHT};

    QRect icon_rec = QRect(icon_x, icon_y, Positions::SPEEDOMETER_ICON_WIDTH, Positions::SPEEDOMETER_ICON_HIGHT / 2);
    painter.drawText(icon_rec, Qt::AlignCenter, QChar(0xe9e4));

    text_font.setPointSize(Positions::SPEEDOMETER_ICON_TEXT_SIIZE);
    painter.setFont(text_font);
    QRect text_rec = QRect(icon_x, icon_y + (Positions::SPEEDOMETER_ICON_HIGHT / 2), Positions::SPEEDOMETER_ICON_WIDTH, Positions::SPEEDOMETER_ICON_HIGHT / 2);
    painter.drawText(text_rec, Qt::AlignCenter, QString("%1 km/h").arg(service->get_speed()));

    painter.end();
}

void Canvas::drawSpeedometerConnectionErrorIcon(void)
{
    painter.begin(this);

    icon_font.setPointSize(Positions::SPEEDOMETER_ICON_SIIZE);

    painter.setFont(icon_font);
    painter.setPen(Qt::red);

    const int icon_x{Positions::SPEEDOMETER_CENTER_X - (Positions::SPEEDOMETER_ICON_WIDTH / 2)};
    const int icon_y{Positions::SPEEDOMETER_CENTER_Y + (Positions::SPEEDOMETER_ARC_RADIUS / 2) - (Positions::SPEEDOMETER_ICON_HIGHT / 2) - Positions::SPEEDOMETER_LABEL_OFFSET_FROM_ARC - Positions::SPEEDOMETER_LABEL_TEXT_HEIGHT};

    QRect icon_rec = QRect(icon_x, icon_y, Positions::SPEEDOMETER_ICON_WIDTH, Positions::SPEEDOMETER_ICON_HIGHT / 2);
    painter.drawText(icon_rec, Qt::AlignCenter, QChar(0xe628));

    text_font.setPointSize(Positions::SPEEDOMETER_ICON_TEXT_SIIZE);
    painter.setFont(text_font);
    QRect text_rec = QRect(icon_x, icon_y + (Positions::SPEEDOMETER_ICON_HIGHT / 2), Positions::SPEEDOMETER_ICON_WIDTH, Positions::SPEEDOMETER_ICON_HIGHT / 2);
    painter.drawText(text_rec, Qt::AlignCenter, QString("Connection Error"));

    painter.end();
}

void Canvas::drawTurnSignals(void)
{
    painter.begin(this);

    icon_font.setPointSize(Positions::TURN_SIGNAL_ICON_SIZE);
    painter.setFont(icon_font);
    painter.setPen(Qt::green);

    bool lightActive = false;

    if (service->get_left_light())
    {
        QRect left_rect(40, 30, 80, 80);
        painter.drawText(left_rect, Qt::AlignCenter, QChar(0xe5c4)); // arrow_left
        lightActive = true;
    }
    if (service->get_right_light())
    {
        QRect right_rect(590, 30, 80, 80);
        painter.drawText(right_rect, Qt::AlignCenter, QChar(0xe5c8)); // arrow_right
        lightActive = true;
    }

    painter.end();

    if (lightActive)
    {
        if (clickPlayer.mediaStatus() == QMediaPlayer::MediaStatus::EndOfMedia ||
            clickPlayer.playbackState() != QMediaPlayer::PlaybackState::PlayingState)
        {
            clickPlayer.setSource(QUrl());
            clickPlayer.setSource(QUrl::fromLocalFile("sound.wav"));
            clickPlayer.play();
        }
    }
    else
    {
        clickPlayer.stop();
    }
}

void Canvas::toggle_blink(void)
{
    // Fast GUI timer setup
    // static constexpr int BLINK_PERIOD_MS = 350; // how often the arrow toggles
    // static constexpr int BLINK_TICKS = BLINK_PERIOD_MS / settings::INTERVAL;

    // static int blinkTick = 0; // divider counter for blink

    // Called every 50 ms. Only toggle the blink every BLINK_TICKS.
    // blinkTick++;
    // if (blinkTick >= BLINK_TICKS)
    // {
    //     blinkTick = 0;
    blink_on = !blink_on; // toggle every ~310 ms
    //}

    update(); // keep UI smooth at 50 ms regardless
}