#ifndef YW_APPLICATION_H
#define YW_APPLICATION_H

#include <functional>

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WNavigationBar>
#include <Wt/WBootstrapTheme>
#include <Wt/WStackedWidget>
#include <Wt/WMenu>
#include <Wt/WPopupMenu>
#include <Wt/WSplitButton>
#include <Wt/WPopupMenu>
#include <Wt/WPopupMenuItem>
#include <Wt/WMessageBox>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WOverlayLoadingIndicator>
#include <Wt/WPanel>
#include <Wt/WTabWidget>
#include <Wt/WTextArea>
#include <Wt/WString>

using namespace Wt;


#include "yw_loginpage.h"
#include "yw_statuspage.h"
#include "yw_configpage.h"
#include "yw_configuration.h"


class ywApplication : public WApplication
{
public:

    enum {
        YW_USERLEVEL_TECH      =1,
        YW_USERLEVEL_RESEARCHER=2,
        YW_USERLEVEL_ADMIN     =3
    };

    ywApplication(const WEnvironment& env);

    void prepare(ywConfiguration* configInstance);

    void performLogin();
    void performLogout();
    void requestLogout();

    void showAbout();

    ywConfiguration* configuration;
    WString currentUser;
    int     currentLevel;

private:
    ywLoginPage*  loginPage;
    ywStatusPage* statusPage;
    ywConfigPage* configPage;
};

#endif // YW_APPLICATION_H
