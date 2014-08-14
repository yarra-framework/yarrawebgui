#ifndef YW_QUEUEPAGE_H
#define YW_QUEUEPAGE_H

#include <Wt/WContainerWidget>

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

    void showInfo (WString taskName);

    Wt::WVBoxLayout* failtaskLayout;
    Wt::WVBoxLayout* taskLayout;
    Wt::WTabWidget*  tabWidget;

    WPanel* createQueuePanel(WString title, int mode);

    ywApplication* app;
};


#endif // YW_QUEUEPAGE_H
