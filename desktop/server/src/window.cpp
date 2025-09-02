#include "window.h"
#include "setting.h"
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>

window::window(ComService *_service) : service{_service}
{

    // Align labels and value labels
    speedLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    tempLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    batteryLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    speedValueLabel.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    tempValueLabel.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    batteryValueLabel.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    int maxLabelWidth = qMax(speedLabel.sizeHint().width(),
                             qMax(tempLabel.sizeHint().width(), batteryLabel.sizeHint().width()));

    speedLabel.setFixedWidth(maxLabelWidth);
    tempLabel.setFixedWidth(maxLabelWidth);
    batteryLabel.setFixedWidth(maxLabelWidth);
    lightSignalsLabel.setFixedWidth(maxLabelWidth);

    auto setupSlider = [](QSlider &slider, int min, int max)
    {
        slider.setOrientation(Qt::Horizontal);
        slider.setRange(min, max);
        slider.setValue(min);
        slider.setFixedWidth(600);
    };

    settings::Settings &settings = settings::Settings::getInstance();
    setupSlider(speedSlider, settings["speed"].min, settings["speed"].max);
    setupSlider(tempSlider, settings["temperature"].min, settings["temperature"].max);
    setupSlider(batterySlider, settings["battery"].min, settings["battery"].max);

    service->setTemperature(-60);

    // Speed row
    speedLayout.addWidget(&speedLabel);
    speedLayout.addWidget(&speedSlider);
    speedLayout.addWidget(&speedValueLabel);
    layout.addLayout(&speedLayout);

    // Temperature row
    tempLayout.addWidget(&tempLabel);
    tempLayout.addWidget(&tempSlider);
    tempLayout.addWidget(&tempValueLabel);
    layout.addLayout(&tempLayout);

    // Battery row
    batteryLayout.addWidget(&batteryLabel);
    batteryLayout.addWidget(&batterySlider);
    batteryLayout.addWidget(&batteryValueLabel);
    layout.addLayout(&batteryLayout);

    // Light signals row
    lightSignalsLayout.addWidget(&lightSignalsLabel);
    lightSignalsLayout.addWidget(&leftCheckBox);
    lightSignalsLayout.addWidget(&rightCheckBox);
    lightSignalsLayout.addWidget(&warningCheckBox);
    layout.addLayout(&lightSignalsLayout);

    setLayout(&layout);
    setWindowTitle("Server");
    setFixedSize(settings::Config::SERVER_WINDOW_WIDTH, settings::Config::SERVER_WINDOW_HEIGHT);

    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);

    connect(&speedSlider, &QSlider::valueChanged, this, &window::onSpeedChanged);
    connect(&tempSlider, &QSlider::valueChanged, this, &window::onTemperatureChanged);
    connect(&batterySlider, &QSlider::valueChanged, this, &window::onBatteryChanged);

    connect(&leftCheckBox, &QCheckBox::toggled, this, &window::onLeftChecked);
    connect(&rightCheckBox, &QCheckBox::toggled, this, &window::onRightChecked);
    connect(&warningCheckBox, &QCheckBox::toggled, this, &window::onWarningChecked);
}

void window::showEvent(QShowEvent *)
{
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = ((screenGeometry.height() - height()) / 2) + (settings::Config::CLIENT_WINDOW_HEIGHT / 2) + (settings::Config::SERVER_WINDOW_HEIGHT / 2) + settings::Config::OFFSET;
    move(x, y);
}

window::~window() {}

void window::onSpeedChanged(int val)
{
    service->setSpeed(val);
    speedValueLabel.setText(QString("%1 km/h").arg(val));
}

void window::onTemperatureChanged(int val)
{
    service->setTemperature(val);
    tempValueLabel.setText(QString("%1 °C").arg(val));
}

void window::onBatteryChanged(int val)
{
    service->setBatteryLevel(val);
    batteryValueLabel.setText(QString("%1 %").arg(val));
}

void window::onLeftChecked(bool checked)
{
    if (!warningCheckBox.isChecked())
    {
        service->setLeftLight(checked);
    }

    if (checked)
    {

        rightCheckBox.setEnabled(false);
    }
    else
    {
        rightCheckBox.setEnabled(true);
    }
}

void window::onRightChecked(bool checked)
{
    if (!warningCheckBox.isChecked())
    {
        service->setRightLight(checked);
    }

    if (checked)
    {
        leftCheckBox.setEnabled(false);
    }
    else
    {
        leftCheckBox.setEnabled(true);
    }
}

void window::onWarningChecked(bool checked)
{
    if (checked)
    {
        service->setLeftLight(true);
        service->setRightLight(true);
    }
    else
    {
        if (leftCheckBox.isChecked())
        {
            service->setLeftLight(true);
            rightCheckBox.setEnabled(false);
        }
        else
        {
            service->setLeftLight(false);
            rightCheckBox.setEnabled(true);
        }
        if (rightCheckBox.isChecked())
        {
            service->setRightLight(true);
            leftCheckBox.setEnabled(false);
        }
        else
        {
            service->setRightLight(false);
            leftCheckBox.setEnabled(true);
        }
    }
}
