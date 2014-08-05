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



class ywApplication : public WApplication
{
public:
    ywApplication(const WEnvironment& env);

    void performLogin();
    void performLogout();
    void requestLogout();

    void showAbout();

    WString userName;

private:
    ywLoginPage* loginPage;

    WLineEdit* nameEdit_;
    WText* greeting_;

    void greet();
};

#endif // YW_APPLICATION_H
