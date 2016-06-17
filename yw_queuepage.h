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
        MODE_PROC    =0,
        MODE_NORMAL  =1,
        MODE_PRIO    =2,
        MODE_NIGHT   =3,
        MODE_FAIL    =4,
        MODE_FINISHED=5
    };

    ywQueuePage(ywApplication* parent);

    static ywQueuePage* createInstance(ywApplication *parent);

    void refreshLists();
    void refreshQueueList();
    void refreshFailList();
    void refreshFinishedList();

    void tabChanged(int);
    void clearQueueList();
    void clearFailList();
    void clearFinishedList();

    void updateTaskInformation(WString taskName, WText* taskWidget, int taskType);

    void showInfo (WString taskName, int mode);
    void changePriority(WString taskName, int currentPriority, int newPriority);
    void deleteTask(WString taskName, int mode);
    void restartTask(WString taskName, int mode);
    void pushbackTask(WString taskName, int mode);

    void patchTask(WString taskName, int mode);
    void doPatchTask(WString taskName, int mode, WString newACC, WString newNotifications);

    void editTaskFile(WString taskName, int mode);
    void doEditTask(WString taskName, int mode, WString newContent);

    Wt::WVBoxLayout* taskLayout;
    Wt::WVBoxLayout* failtaskLayout;
    Wt::WVBoxLayout* finishedtaskLayout;
    Wt::WTabWidget*  tabWidget;

    WPanel* createQueuePanel(WString title, int mode);

    ywApplication* app;
    WString queuePath;
    WString failPath;
    WString workPath;
    WString storagePath;

    bool lockTask(WString taskName);
    bool unlockTask(WString taskName);
    bool isTaskLocked(WString taskName);

    void showErrorMessage(WString errorMessage);
    WString getFullTaskFileName(WString taskName, int mode);
    WString getTaskFileName(WString taskName, int mode);
    WString getFolderTaskFile(WString taskName);
    WString getWorkTaskFile();

    bool getAllFilesOfTask(WString taskFileName, WStringList& fileList);

};



#endif // YW_QUEUEPAGE_H
