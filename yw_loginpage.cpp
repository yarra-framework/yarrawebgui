#include <Wt/WPushButton>

#include "yw_loginpage.h"
#include "yw_application.h"



ywLoginPage::ywLoginPage(ywApplication* parent)
 : WContainerWidget()
{
    app=parent;

    Wt::WVBoxLayout *layout = new Wt::WVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    Wt::WPanel *panel = new Wt::WPanel();
    panel->addStyleClass("centered");
    panel->resize(400, 250);

    WContainerWidget* haligncontainer=new WContainerWidget();
    Wt::WHBoxLayout *halignlayout=new Wt::WHBoxLayout();
    halignlayout->setContentsMargins(0, 0, 0, 0);
    haligncontainer->setLayout(halignlayout);

    layout->addWidget(new WContainerWidget(),1);
    layout->addWidget(haligncontainer);
    layout->addWidget(new WContainerWidget(),1);

    halignlayout->addWidget(new WContainerWidget(),1);
    halignlayout->addWidget(panel);
    halignlayout->addWidget(new WContainerWidget(),1);

    WContainerWidget* innercontainer=new WContainerWidget();

    Wt::WPushButton *loginButton = new Wt::WPushButton("Login", innercontainer);
    loginButton->setStyleClass("btn-primary");
    loginButton->clicked().connect(this, &ywLoginPage::loginClick);

    panel->setCentralWidget(innercontainer);

}


ywLoginPage* ywLoginPage::createInstance(ywApplication* parent)
{
    return new ywLoginPage(parent);
}


void ywLoginPage::loginClick()
{
    app->performLogin();
}
