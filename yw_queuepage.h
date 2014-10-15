#ifndef YW_QUEUEPAGE_H
#define YW_QUEUEPAGE_H

#include <Wt/WContainerWidget>
#include "yw_global.h"


class ywApplication;

using namespace Wt;


class ywQueuePage : public Wt::WContainerWidget
{
public:
    enum {
        MODE_PROC  =0,
        MODE_NORMAL=1,
        MODE_PRIO  =2,
        MODE_NIGHT =3,
        MODE_FAIL  =4
    };

    ywQueuePage(ywApplication* parent);

    static ywQueuePage* createInstance(ywApplication *parent);

    void refreshFailList();
    void refreshQueueList();
    void refreshLists();
    void tabChanged(int);
    void clearFailList();
    void clearQueueList();

    void updateTaskInformation(WString taskName, WText* taskWidget, int taskType);

    void showInfo (WString taskName, int mode);
    void changePriority(WString taskName, int currentPriority, int newPriority);
    void deleteTask(WString taskName, int mode);
    void restartTask(WString taskName, int mode);
    void pushbackTask(WString taskName, int mode);

    void patchTask(WString taskName, int mode);
    void doPatchTask(WString taskName, int mode, WString newACC, WString newNotifications);

    Wt::WVBoxLayout* failtaskLayout;
    Wt::WVBoxLayout* taskLayout;
    Wt::WTabWidget*  tabWidget;

    WPanel* createQueuePanel(WString title, int mode);

    ywApplication* app;
    WString queuePath;
    WString failPath;
    WString workPath;


    bool lockTask(WString taskName);
    bool unlockTask(WString taskName);
    bool isTaskLocked(WString taskName);

    void showErrorMessage(WString errorMessage);
    WString getFullTaskFileName(WString taskName, int mode);
    WString getTaskFileName(WString taskName, int mode);
    WString getFailTaskFile(WString taskName);
    WString getWorkTaskFile();

    bool getAllFilesOfTask(WString taskFileName, WStringList& fileList);

};



#endif // YW_QUEUEPAGE_H
