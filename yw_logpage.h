#ifndef YW_LOGPAGE_H
#define YW_LOGPAGE_H

#include <Wt/WContainerWidget>
#include <Wt/WFileResource>


class ywApplication;

using namespace Wt;


class ywLogPage : public Wt::WContainerWidget
{
public:
    ywLogPage(ywApplication* parent);

    static ywLogPage* createInstance(ywApplication *parent);

    Wt::WComboBox* logselectCombobox;
    WScrollArea*   logScrollArea;
    WText*         logWidget;
    ywApplication* app;

    WPushButton*   downloadButton;
    WPushButton*   deleteButton;

    WFileResource  downloadRes;


    WString previousItem;

    void deleteLog();
    void refreshLogs();

    void showLog(int index);

};



#endif // YW_LOGPAGE_H

