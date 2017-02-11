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
class ywConfigPageServerList;
class ywConfigPageServerData;


class ywConfigPage : public Wt::WContainerWidget
{
public:

    enum PAGES
    {
        PAGE_MODES=0,
        PAGE_MODELIST,
        PAGE_SERVERSETTINGS,
        PAGE_SERVERUPDATE,
        PAGE_MODULES,
        PAGE_SERVERLIST,
        PAGE_SERVERDATA
    };

    ywConfigPage(ywApplication* parent);

    static ywConfigPage* createInstance(ywApplication *parent);

    void refreshStatus();
    void refreshSubpage();
    void showErrorMessage(WString errorMessage);

    ywApplication* app;

    Wt::WMenu *configMenu;
    Wt::WText* serverStatusLabel;

    std::vector<PAGES> pages;

    ywConfigPageModes*      page0;
    ywConfigPageModeList*   page1;
    ywConfigPageServer*     page2;
    ywConfigPageModules*    pageModules;
    ywConfigPageUpdate*     pageUpdate;
    ywConfigPageServerList* pageServerList;
    ywConfigPageServerData* pageServerData;
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


class ywConfigPageServerList : public Wt::WContainerWidget
{
public:
    ywConfigPageServerList(ywConfigPage* pageParent);

    ywConfigPage* parent;
    Wt::WPushButton* saveBtn;
    Wt::WTextArea* editor;

    WString readServerList();
    bool writeServerList(WString newConfig);
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

    void refreshPage();
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



class ywConfigPageServerData : public Wt::WContainerWidget
{
public:
    ywConfigPageServerData(ywConfigPage* pageParent);

    ywConfigPage* parent;
    Wt::WTable* table;

    Wt::WText* addText(Wt::WString text);
    void refreshPage();
};


#endif // YW_CONFIGPAGE_H


