#ifndef POSITIONS_H
#define POSITIONS_H

#include "setting.h"

namespace Positions
{
    inline constexpr int PADDING_RIGHT_SIDE{20};
    inline constexpr int PADDING_SPEEDOMETER_TOP{160};
    inline constexpr int RIGHT_SIDE_ICON_WIDTH{55};

    inline constexpr int TEMP_WIDTH{RIGHT_SIDE_ICON_WIDTH};
    inline constexpr int TEMP_ICON_HEIGHT{60};
    inline constexpr int TEMP_TEXT_HEIGHT{20};
    inline constexpr int TEMP_X = settings::Config::CLIENT_WINDOW_WIDTH - TEMP_WIDTH - PADDING_RIGHT_SIDE;
    inline constexpr int TEMP_Y = settings::Config::CLIENT_WINDOW_HEIGHT - TEMP_ICON_HEIGHT - TEMP_TEXT_HEIGHT - PADDING_RIGHT_SIDE;

    inline constexpr int BATTERY_WIDTH{RIGHT_SIDE_ICON_WIDTH};
    inline constexpr int BATTERY_ICON_HEIGHT{110};
    inline constexpr int BATTERY_TEXT_HEIGHT{20};
    inline constexpr int BATTERY_X{TEMP_X};
    inline constexpr int BATTERY_Y{TEMP_Y - PADDING_RIGHT_SIDE - BATTERY_TEXT_HEIGHT - BATTERY_ICON_HEIGHT};

    inline constexpr int SPEEDOMETER_CENTER_X{(settings::Config::CLIENT_WINDOW_WIDTH - RIGHT_SIDE_ICON_WIDTH - PADDING_RIGHT_SIDE) / 2};
    inline constexpr int SPEEDOMETER_CENTER_Y{PADDING_SPEEDOMETER_TOP + ((settings::Config::CLIENT_WINDOW_HEIGHT - PADDING_SPEEDOMETER_TOP) / 2)};
    inline constexpr int SPEEDOMETER_CENTER_CIRCLE_RADIUS{20};

    inline constexpr int SPEEDOMETER_ARC_DEGREES_TO_DRAW{250};
    inline constexpr int SPEEDOMETER_ARC_START_ANGLE{215};
    inline constexpr int SPEEDOMETER_ARC_THICKNESS{10};
    inline constexpr int SPEEDOMETER_ARC_RADIUS{300};

    inline constexpr int SPEEDOMETER_LINES_DISTANCE_FROM_ARC{10};
    inline constexpr int SPEEDOMETER_LINES_START_RADIUS{Positions::SPEEDOMETER_ARC_RADIUS - (Positions::SPEEDOMETER_ARC_THICKNESS / 2) - Positions::SPEEDOMETER_LINES_DISTANCE_FROM_ARC};
    inline constexpr int SPEEDOMETER_LINES_ANGLE_OFFSET{5};

    inline constexpr int SPEEDOMETER_LONG_LINES_TICK_LENGTH{20};
    inline constexpr int SPEEDOMETER_LONG_LINES_THICHNESS{4};
    inline constexpr int SPEEDOMETER_LONG_LINES_END_RADIUS{Positions::SPEEDOMETER_LINES_START_RADIUS - Positions::SPEEDOMETER_LONG_LINES_TICK_LENGTH};

    inline constexpr int SPEEDOMETER_MEDIUM_LINES_THICKNESS{3};
    inline constexpr int SPEEDOMETER_MEDIUM_LINES_TICK_LENGTH{12};
    inline constexpr int SPEEDOMETER_MEDIUM_LINES_END_RADIUS{Positions::SPEEDOMETER_LINES_START_RADIUS - Positions::SPEEDOMETER_MEDIUM_LINES_TICK_LENGTH};

    inline constexpr int SPEEDOMETER_SMALL_LINES_THICKNESS{2};
    inline constexpr int SPEEDOMETER_SMALL_LINES_TICK_LENGTH{6};
    inline constexpr int SPEEDOMETER_SMALL_LINES_END_RADIUS{Positions::SPEEDOMETER_LINES_START_RADIUS - Positions::SPEEDOMETER_SMALL_LINES_TICK_LENGTH};

    inline constexpr int SPEEDOMETER_LABEL_TEXT_SIZE{18};
    inline constexpr int SPEEDOMETER_LABEL_OFFSET_FROM_ARC{20};
    inline constexpr int SPEEDOMETER_LABEL_TEXT_WIDTH{40};
    inline constexpr int SPEEDOMETER_LABEL_TEXT_HEIGHT{20};
    inline constexpr int SPEEDOMETER_LABEL_INNER_PADDING_X{30};
    inline constexpr int SPEEDOMETER_LABEL_INNER_PADDING_Y{30};

    inline constexpr int SPEEDOMETER_NEEDLE_THICKNESS{6};
    inline constexpr int SPEEDOMTER_NEEDLE_OFFSET_FROM_ARC{70};

    inline constexpr int SPEEDOMETER_ICON_WIDTH{200};
    inline constexpr int SPEEDOMETER_ICON_HIGHT{120};
    inline constexpr int SPEEDOMETER_ICON_SIIZE{60};
    inline constexpr int SPEEDOMETER_ICON_TEXT_SIIZE{20};

    inline constexpr int TURN_SIGNAL_ICON_SIZE{40};
}

#endif