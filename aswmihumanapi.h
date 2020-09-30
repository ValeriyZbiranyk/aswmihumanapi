/******************************************************************************
     * File: aswmihumanapi.h
     * Description: AsWMI human api for asus tuf gaming laptop.
     * Created: 21 sep 2020
     * Copyright: (C) 2020 Valeriy Zbiranyk
     * Authors: programing - Valeriy Zbiranyk
     *          reverse engineering - Valeriy Zbiranyk, Mykola Vorobiov
     * Email: culikov.valeriy@gmail.com, flingseal@gmail.com

******************************************************************************/

#ifndef ASWMIHUMANAPI_H
#define ASWMIHUMANAPI_H


#include <string>

//Laptop TUF Gaming fx705dt deviceId 0x100056

namespace TUFLaptopRGBBacklightControl
{

    enum EffectType
    {
        STATIC = 0x00,
        BREATHING = 0x01,
        COLOR_CYCLE = 0x02,
        STROBING = 0x0a
    };

    enum Tempo
    {
        NONE = 0x00,
        FAST = 0xf5,
        MEDIUM = 0xeb,
        SLOW = 0xe1
    };

    enum BacklightPowerSettings
    {
        BOOT_UP = 0x3,
        AWAKE = 0xc,
        SLEEP = 0x30,
        SHUT_DOWN = 0xc0,
    };


    /**
     * Set laptop keyboard rgb backlight effect type, tempo and color.
     * @param effectType - type of effect, can be 'static', 'breathing',
     *        'color cycle', 'strobbing'.
     * @param tempo - tempo of effect, can be 'fast', 'medium', 'slow', 'none'.
     *        Tempo 'none' ignored by 'breathing' and 'color cycle' effects,
     *        and seted 'slow' by default. For effects 'static' and 'strobing'
     *        tempo seted 'none' in any cases.
     * @param colorHex - heximal color value for laptop keyboard backlight.
     *        For example:
     *        0xff0000 (red color)
     *        0x00ff00 (green color)
     *        0x0000ff (blue color)
     * @param HTMLColor - HTML string color value for laptop keyboard backlight.
     *        For example:
     *        '#ff0000' (red color)
     *        '#00ff00' (green color)
     *        '#0000ff' (blue color)
     * @param brightnessRedDiode, brightnessGreenDiode, brightnessBueDiode - maximum
     *        brightness of diode, range from 0 to 255, ignored by
     *        'color cycle' effect. By setting different brightness for each diode,
     *        you can set keyboard backlight color.
     *        Example:
     *        0xff, 0x00, 0xff (fuchsia color)
     * @return 0 if successful, -1 indicates invalid argument(s) or
     *         absence of 'ACPIWMI.dll' in folder with your executable.
     */

    int setLaptopRGBBacklight(EffectType effectType,
                              Tempo tempo,
                              unsigned int colorHex);

    int setLaptopRGBBacklight(EffectType effectType,
                              Tempo tempo,
                              unsigned __int8 brightnessRedDiode,
                              unsigned __int8 brightnessGreenDiode,
                              unsigned __int8 brightnessBueDiode);

    int setLaptopRGBBacklight(EffectType effectType,
                              Tempo tempo,
                              std::string HTMLColor);


    /**
     * Set laptop keyboard rgb backlight power settings.
     * @param backlightPowerSettings - sum of bakclight power settings.
     *        Example:
     *        BOOT_UP + AWAKE (backlight on when device boot up and awake)
     *        SLEEP + AWAKE + SHUT_DOWN (backlight on when device sleep, awake and shut down)
     * @param onBootUp - set backlight on or off when device boot up.
     * @param onAwake - set backlight on or off when device awake.
     * @param onSleep - set backlight on or off when device sleep.
     * @param onShutDown - set backlight on or off when device shut down.
     * @return 0 if successful, -1 indicates invalid argument(s) or
     *         absence of 'ACPIWMI.dll' in folder with your executable.
     */

    int setLaptopRGBBacklightPowerSettings(unsigned __int8 backlightPowerSettings);

    int setLaptopRGBBacklightPowerSettings(bool onBootUp,
                                           bool onAwake,
                                           bool onSleep,
                                           bool onShutDown);


    /**
     * Set laptop keyboard rgb backlight effect type, tempo, color and power settings.
     * @param same as in setLaptopRGBBacklight and setLaptopRGBBacklightPowerSettings functions.
     * @return 0 if successful, -1 indicates invalid argument(s) or
     *         absence of 'ACPIWMI.dll' in folder with your executable.
     */

    int setLaptopRGBBacklightSettings(EffectType effectType,
                                      Tempo tempo,
                                      unsigned int colorHex,
                                      unsigned __int8 backlightPowerSettings);

    int setLaptopRGBBacklightSettings(EffectType effectType,
                                      Tempo tempo,
                                      std::string HTMLColor,
                                      unsigned __int8 backlightPowerSettings);

    int setLaptopRGBBacklightSettings(EffectType effectType,
                                      Tempo tempo,
                                      unsigned __int8 brightnessRedDiode,
                                      unsigned __int8 brightnessGreenDiode,
                                      unsigned __int8 brightnessBueDiode,
                                      unsigned __int8 backlightPowerSettings);

    int setLaptopRGBBacklightSettings(EffectType effectType,
                                      Tempo tempo,
                                      unsigned __int8 brightnessRedDiode,
                                      unsigned __int8 brightnessGreenDiode,
                                      unsigned __int8 brightnessBueDiode,
                                      bool onBootUp,
                                      bool onAwake,
                                      bool onSleep,
                                      bool onShutDown);

    int setLaptopRGBBacklightSettings(EffectType effectType,
                                      Tempo tempo,
                                      unsigned int colorHex,
                                      bool onBootUp,
                                      bool onAwake,
                                      bool onSleep,
                                      bool onShutDown);

    int setLaptopRGBBacklightSettings(EffectType effectType,
                                      Tempo tempo,
                                      std::string HTMLColor,
                                      bool onBootUp,
                                      bool onAwake,
                                      bool onSleep,
                                      bool onShutDown);
}

#endif // ASWMIHUMANAPI_H
