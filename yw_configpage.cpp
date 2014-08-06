#include "yw_configpage.h"

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


ywConfigPage::ywConfigPage(ywApplication* parent)
 : WContainerWidget()
{
    app=parent;

    Wt::WHBoxLayout* configPageLayout = new Wt::WHBoxLayout();
    this->setLayout(configPageLayout);

    Wt::WStackedWidget *configContents=new Wt::WStackedWidget();
    configContents->setMargin(30, Wt::Bottom);
    configContents->setMargin(30, Wt::Left);

    Wt::WMenu *configMenu = new Wt::WMenu(configContents, Wt::Vertical, this);
    configMenu->setStyleClass("nav nav-pills nav-stacked");
    configMenu->setWidth(200);

    configMenu->addItem("Internal paths", new Wt::WTextArea("Internal paths contents"));
    configMenu->addItem("Anchor", new Wt::WTextArea("Anchor contents"));
    configMenu->addItem("Stacked widget", new Wt::WTextArea("Stacked widget contents"));
    configMenu->addItem("Tab widget", new Wt::WTextArea("Tab widget contents"));
    configMenu->addItem("Menu", new Wt::WTextArea("Menu contents"));

    configPageLayout->addWidget(configMenu);
    configPageLayout->addWidget(configContents,1);

}


ywConfigPage* ywConfigPage::createInstance(ywApplication* parent)
{
    return new ywConfigPage(parent);
}

