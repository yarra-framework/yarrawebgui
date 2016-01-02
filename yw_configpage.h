#ifndef YW_CONFIGPAGE_H
#define YW_CONFIGPAGE_H

#include <Wt/WContainerWidget>

class ywApplication;

using namespace Wt;


class ywConfigPageModes;
class ywConfigPageModeList;
class ywConfigPageServer;
class ywConfigPageModules;
class ywConfigPageUpdate;


class ywConfigPage : public Wt::WContainerWidget
{
public:
    ywConfigPage(ywApplication* parent);

    static ywConfigPage* createInstance(ywApplication *parent);

    void refreshStatus();
    void refreshSubpage();
    void showErrorMessage(WString errorMessage);

    ywApplication* app;

    Wt::WMenu *configMenu;
    Wt::WText* serverStatusLabel;

    ywConfigPageModes*    page0;
    ywConfigPageModeList* page1;
    ywConfigPageServer*   page2;
    ywConfigPageModules*  pageModules;
    ywConfigPageUpdate*   pageUpdate;
};


class ywConfigPageServer : public Wt::WContainerWidget
{
public:
    ywConfigPageServer(ywConfigPage* pageParent);

    ywConfigPage* parent;
    Wt::WPushButton* saveBtn;
    Wt::WTextArea* editor;

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
    Wt::WTextArea* editor;

    WString readModeList();
    bool writeModeList(WString newConfig);
    void refreshEditor();
};



class ywConfigPageModes : public Wt::WContainerWidget
{
public:
    ywConfigPageModes(ywConfigPage* pageParent);

    ywConfigPage* parent;
    Wt::WTextArea* editor;
    Wt::WComboBox* modeList;
    Wt::WPushButton* modeListBtn;

    void refresh();
    void refreshModes();
    void showMode(int index);

    void deleteMode();
    void addMode();
    void doAddMode(WString name, WString templateMode);
    void saveMode();
    void generateModeList();
    void showHelp();

    void setIndicateModeUpdate(bool status);
};


#endif // YW_CONFIGPAGE_H


