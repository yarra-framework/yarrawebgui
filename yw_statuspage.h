#ifndef YW_STATUSPAGE_H
#define YW_STATUSPAGE_H

#include <Wt/WContainerWidget>
#include <Wt/WScrollArea>

class ywApplication;

using namespace Wt;


class ywStatusPage : public Wt::WContainerWidget
{
public:

    enum {
        PAGE_STATUS   =0,
        PAGE_SERVERLOG=1,
        PAGE_TASKLOG  =2
    };

    ywStatusPage(ywApplication* parent);

    static ywStatusPage* createInstance(ywApplication *parent);

    void tabChanged(int tab);

    void refreshCurrentTab();
    void refreshStatus();
    void refreshServerLog();
    void refreshTaskLog();

    void callStartServer();
    void callStopServer();
    void callKillServer();

    WTabWidget*  tabWidget;
    WScrollArea* statusScrollArea;
    WTemplate*       statusWidget;
    WScrollArea* serverLogScrollArea;
    WText*       serverLogWidget;
    WScrollArea* taskLogScrollArea;
    WText*       taskLogWidget;

    ywApplication* app;
};

#endif // YW_STATUSPAGE_H
