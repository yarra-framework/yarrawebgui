#ifndef YW_CONFIGPAGE_YMGENERATOR_H
#define YW_CONFIGPAGE_YMGENERATOR_H

#include "yw_global.h"

#include <Wt/WString>


class ywApplication;

class ywcModeEntry
{
public:
    WString ID;
    WStringList entries;
    WStringList values;
};

typedef std::multimap<int, ywcModeEntry*> ywcModeList;


class ywConfigPageYMGenerator
{
public:
    ywConfigPageYMGenerator(ywApplication* parent);
    ~ywConfigPageYMGenerator();

    void perform();
    void showErrorMessage(WString errorMessage);
    void freeModeList();

    bool parseModeFiles();
    bool generateYMFile();

    ywApplication* app;

    WStringList modeFiles;
    ywcModeList orderedModeList;

};

#endif // YW_CONFIGPAGE_YMGENERATOR_H
