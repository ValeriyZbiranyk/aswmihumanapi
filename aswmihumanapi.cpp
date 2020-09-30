/******************************************************************************
     * File: aswmihumanapi.cpp
     * Description: AsWMI human api for asus tuf gaming laptop.
     * Created: 21 sep 2020
     * Copyright: (C) 2020 Valeriy Zbiranyk
     * Authors: programing - Valeriy Zbiranyk
     *          reverse engineering - Valeriy Zbiranyk, Mykola Vorobiov
     * Email: culikov.valeriy@gmail.com, flingseal@gmail.com

******************************************************************************/


#include <algorithm>
#include <cstdio>
#include <fstream>
#include <regex>
#include <windows.h>

#include "aswmihumanapi.h"

const int redBits = 0xff0000;
const int greenBits = 0x00ff00;
const int blueBits = 0x0000ff;

typedef int (*AsWMI_NB_DeviceControl_2arg)(int, int, int);
typedef int (*AsWMI_NB_DeviceControl)(int, int);

enum DevControlFunckType
{
    ASWMI_NB_DEVCONTROL_2ARG = 0x100056,
    ASWMI_NB_DEVCONTROL = 0x100057
};

std::string getCurrentDirectory()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");

    return std::string(buffer).substr(0, pos);
}

int executeAsWMI(DevControlFunckType devControlFunckType,
                 int RGDiodeAndEType,
                 int BDiodeAndTemp,
                 int backlightPowerSettings)
{
    std::string libraryName = "ACPIWMI.dll";
    std::string libraryPath = getCurrentDirectory() + '\\' + libraryName;
    std::ifstream libraryFile(libraryPath.c_str());
    if(!libraryFile.good())
    {
        fprintf(stderr, "Can`t find file \'%s\'", libraryName.c_str());
        return -1;
    }
    libraryFile.close();

    HMODULE ACPIWMI = LoadLibraryA(libraryName.c_str());
    FARPROC AsWMI_Open = GetProcAddress(ACPIWMI, "AsWMI_Open");
    FARPROC AsWMI_Close = GetProcAddress(ACPIWMI, "AsWMI_Close");
    AsWMI_NB_DeviceControl_2arg AsWMI_DeviceControlBacklightEffectTypeTempoColor = (AsWMI_NB_DeviceControl_2arg)GetProcAddress(ACPIWMI, "AsWMI_NB_DeviceControl_2arg");
    AsWMI_NB_DeviceControl AsWMI_DeviceControlBacklightPowerSettings = (AsWMI_NB_DeviceControl)GetProcAddress(ACPIWMI, "AsWMI_NB_DeviceControl");

    AsWMI_Open();

    switch(devControlFunckType)
    {
        case ASWMI_NB_DEVCONTROL:
            AsWMI_DeviceControlBacklightPowerSettings(ASWMI_NB_DEVCONTROL, backlightPowerSettings);
        break;

        case ASWMI_NB_DEVCONTROL_2ARG:
            AsWMI_DeviceControlBacklightEffectTypeTempoColor(ASWMI_NB_DEVCONTROL_2ARG, RGDiodeAndEType, BDiodeAndTemp);
        break;

        default:
            fprintf(stderr, "Wrong AsWMI_NB_DeviceControl function type!");
            return -1;
        break;
    }

    AsWMI_Close();

    return 0;
}

unsigned __int8 correctTempo(TUFLaptopRGBBacklightControl::EffectType effectType,
                             TUFLaptopRGBBacklightControl::Tempo tempo)
{
    if(effectType == TUFLaptopRGBBacklightControl::STATIC || effectType == TUFLaptopRGBBacklightControl::STROBING)
    {
        tempo = TUFLaptopRGBBacklightControl::NONE;
    }

    if((effectType == TUFLaptopRGBBacklightControl::COLOR_CYCLE || effectType == TUFLaptopRGBBacklightControl::BREATHING)
            && tempo == TUFLaptopRGBBacklightControl::NONE)
    {
        tempo = TUFLaptopRGBBacklightControl::SLOW;
    }
    return tempo;
}

int TUFLaptopRGBBacklightControl::setLaptopRGBBacklight(EffectType effectType,
                                                        Tempo tempo,
                                                        unsigned __int8 brightnessRedDiode,
                                                        unsigned __int8 brightnessGreenDiode,
                                                        unsigned __int8 brightnessBueDiode)
{
    unsigned __int8 correctedTempo = correctTempo(effectType, tempo);

    int RGDiodeAndEType = (0x00010000 * brightnessRedDiode) + (0x01000000 * brightnessGreenDiode) + (0x00000100 * effectType) + 0xb3;

    int BDiodeAndTemp = brightnessBueDiode + (0x0100 * correctedTempo);

    return executeAsWMI(ASWMI_NB_DEVCONTROL_2ARG,
                        RGDiodeAndEType,
                        BDiodeAndTemp, 0);
}

int TUFLaptopRGBBacklightControl::setLaptopRGBBacklight(EffectType effectType,
                                                        Tempo tempo,
                                                        unsigned int colorHex)
{
    unsigned __int8 brightnessRedDiode = (colorHex & redBits) >> 16;
    unsigned __int8 brightnessGreenDiode = (colorHex & greenBits) >> 8;
    unsigned __int8 brightnessBueDiode = colorHex & blueBits;

    return setLaptopRGBBacklight(effectType, tempo, brightnessRedDiode, brightnessGreenDiode, brightnessBueDiode);
}

int TUFLaptopRGBBacklightControl::setLaptopRGBBacklight(EffectType effectType,
                                                        Tempo tempo,
                                                        std::string HTMLColor)
{
    std::regex colorReg("#[a-f0-9A-F]{6}");
    std::smatch parts;
    if(!std::regex_search(HTMLColor, colorReg))
    {
        fprintf(stderr, "Wrong HTML color string format! Use \'#FF00FF\' for example, instead of %s", HTMLColor.c_str());
        return -1;
    }

    HTMLColor.erase(std::remove(HTMLColor.begin(), HTMLColor.end(), '#'), HTMLColor.end());
    unsigned int colorHex = std::stoi(HTMLColor, 0, 16);
    return setLaptopRGBBacklight(effectType, tempo, colorHex);
}

int TUFLaptopRGBBacklightControl::setLaptopRGBBacklightPowerSettings(unsigned __int8 backlightPowerSettings)
{
    int backlightSettings = (0x10000 * backlightPowerSettings) + 0xbd;
    return executeAsWMI(ASWMI_NB_DEVCONTROL, 0, 0, backlightSettings);
}

int TUFLaptopRGBBacklightControl::setLaptopRGBBacklightPowerSettings(bool onBootUp,
                                                                     bool onAwake,
                                                                     bool onSleep,
                                                                     bool onShutDown)
{
    unsigned __int8 backlightPowerSettings = (onBootUp * BOOT_UP) + (onAwake * AWAKE) + (onSleep * SLEEP) + (onShutDown * SHUT_DOWN);
    return setLaptopRGBBacklightPowerSettings(backlightPowerSettings);
}

int TUFLaptopRGBBacklightControl::setLaptopRGBBacklightSettings(EffectType effectType,
                                                                Tempo tempo,
                                                                unsigned int colorHex,
                                                                unsigned char backlightPowerSettings)
{
    int backlightColorSuccess = setLaptopRGBBacklight(effectType, tempo, colorHex);
    int backlightPowerSuccess = setLaptopRGBBacklightPowerSettings(backlightPowerSettings);

    if((backlightColorSuccess == -1) || (backlightPowerSuccess == -1))
    {
        return -1;
    }
    return 0;
}

int TUFLaptopRGBBacklightControl::setLaptopRGBBacklightSettings(EffectType effectType,
                                                                Tempo tempo,
                                                                std::string HTMLColor,
                                                                unsigned char backlightPowerSettings)
{
    int backlightColorSuccess = setLaptopRGBBacklight(effectType, tempo, HTMLColor);
    int backlightPowerSuccess = setLaptopRGBBacklightPowerSettings(backlightPowerSettings);

    if((backlightColorSuccess == -1) || (backlightPowerSuccess == -1))
    {
        return -1;
    }
    return 0;
}

int TUFLaptopRGBBacklightControl::setLaptopRGBBacklightSettings(EffectType effectType,
                                                                Tempo tempo,
                                                                unsigned char brightnessRedDiode,
                                                                unsigned char brightnessGreenDiode,
                                                                unsigned char brightnessBueDiode,
                                                                unsigned char backlightPowerSettings)
{
    int backlightColorSuccess = setLaptopRGBBacklight(effectType, tempo, brightnessRedDiode, brightnessGreenDiode,  brightnessBueDiode);
    int backlightPowerSuccess = setLaptopRGBBacklightPowerSettings(backlightPowerSettings);

    if((backlightColorSuccess == -1) || (backlightPowerSuccess == -1))
    {
        return -1;
    }
    return 0;
}

int TUFLaptopRGBBacklightControl::setLaptopRGBBacklightSettings(EffectType effectType,
                                                                Tempo tempo,
                                                                unsigned int colorHex,
                                                                bool onBootUp,
                                                                bool onAwake,
                                                                bool onSleep,
                                                                bool onShutDown)
{
    int backlightColorSuccess = setLaptopRGBBacklight(effectType, tempo, colorHex);
    int backlightPowerSuccess = setLaptopRGBBacklightPowerSettings(onBootUp, onAwake, onSleep, onShutDown);

    if((backlightColorSuccess == -1) || (backlightPowerSuccess == -1))
    {
        return -1;
    }
    return 0;
}

int TUFLaptopRGBBacklightControl::setLaptopRGBBacklightSettings(EffectType effectType,
                                                                Tempo tempo,
                                                                std::string HTMLColor,
                                                                bool onBootUp,
                                                                bool onAwake,
                                                                bool onSleep,
                                                                bool onShutDown)
{
    int backlightColorSuccess = setLaptopRGBBacklight(effectType, tempo, HTMLColor);
    int backlightPowerSuccess = setLaptopRGBBacklightPowerSettings(onBootUp, onAwake, onSleep, onShutDown);

    if((backlightColorSuccess == -1) || (backlightPowerSuccess == -1))
    {
        return -1;
    }
    return 0;
}

int TUFLaptopRGBBacklightControl::setLaptopRGBBacklightSettings(EffectType effectType,
                                                                Tempo tempo,
                                                                unsigned char brightnessRedDiode,
                                                                unsigned char brightnessGreenDiode,
                                                                unsigned char brightnessBueDiode,
                                                                bool onBootUp,
                                                                bool onAwake,
                                                                bool onSleep,
                                                                bool onShutDown)
{
    int backlightColorSuccess = setLaptopRGBBacklight(effectType, tempo, brightnessRedDiode, brightnessGreenDiode,  brightnessBueDiode);
    int backlightPowerSuccess = setLaptopRGBBacklightPowerSettings(onBootUp, onAwake, onSleep, onShutDown);

    if((backlightColorSuccess == -1) || (backlightPowerSuccess == -1))
    {
        return -1;
    }
    return 0;
}
