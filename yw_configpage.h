#ifndef YW_CONFIGPAGE_H
#define YW_CONFIGPAGE_H

#include <Wt/WContainerWidget>

class ywApplication;

using namespace Wt;


class ywConfigPage : public Wt::WContainerWidget
{
public:
    ywConfigPage(ywApplication* parent);

    static ywConfigPage* createInstance(ywApplication *parent);

    void refreshStatus();
    void showErrorMessage(WString errorMessage);

    ywApplication* app;

    Wt::WText* serverStatusLabel;
};


class ywConfigPageServer : public Wt::WContainerWidget
{
public:
    ywConfigPageServer(ywConfigPage* pageParent);

    ywConfigPage* parent;
    Wt::WPushButton* saveBtn;
    Wt::WTextArea *editor;

    WString readServerConfig();
    bool writeServerConfig(WString newConfig);
    void refreshEditor();
};


class ywConfigPageModeList : public Wt::WContainerWidget
{
public:
    ywConfigPageModeList(ywConfigPage* pageParent);

    ywConfigPage* parent;
    Wt::WPushButton* saveBtn;
    Wt::WTextArea *editor;

    WString readModeList();
    bool writeModeList(WString newConfig);
    void refreshEditor();
};


#endif // YW_CONFIGPAGE_H


