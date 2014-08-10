#include <Wt/WPushButton>
#include <Wt/WLabel>

#include "yw_loginpage.h"
#include "yw_application.h"
#include "yw_configuration.h"


ywLoginPage::ywLoginPage(ywApplication* parent)
 : WContainerWidget()
{
    app=parent;

    Wt::WVBoxLayout *layout = new Wt::WVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);

    WContainerWidget* panelcontainer=new WContainerWidget();
    panelcontainer->resize(250, 300);

    Wt::WPanel *panel = new Wt::WPanel(panelcontainer);
    panel->addStyleClass("centered");
    panel->setTitle("YarraServer: "+app->configuration->serverName);
    panel->addStyleClass("modal-content");
    //panel->addStyleClass("panel-primary");

    // TODO: Define CSS class for background color
    WContainerWidget* haligncontainer=new WContainerWidget();
    haligncontainer->decorationStyle().setBackgroundColor(WColor(128,128,128));
    Wt::WHBoxLayout *halignlayout=new Wt::WHBoxLayout();
    halignlayout->setContentsMargins(0, 0, 0, 0);
    haligncontainer->setLayout(halignlayout);

    WContainerWidget* dummyContainer1=new WContainerWidget();
    dummyContainer1->decorationStyle().setBackgroundColor(WColor(128,128,128));
    WContainerWidget* dummyContainer2=new WContainerWidget();
    dummyContainer2->decorationStyle().setBackgroundColor(WColor(128,128,128));

    layout->addWidget(dummyContainer1,1);
    layout->addWidget(haligncontainer);
    layout->addWidget(dummyContainer2,1);

    halignlayout->addWidget(new WContainerWidget(),1);
    halignlayout->addWidget(panelcontainer);
    halignlayout->addWidget(new WContainerWidget(),1);

    WContainerWidget* innercontainer=new WContainerWidget();
    innercontainer->addStyleClass("form-group");

    Wt::WVBoxLayout *innerlayout=new Wt::WVBoxLayout();
    innerlayout->setContentsMargins(0, 0, 0, 0);
    innerlayout->setSpacing(0);

    Wt::WLabel* nameLabel = new Wt::WLabel("User: ");
    nameLabel->setMargin(2,Wt::Bottom);
    nameEdit=new Wt::WLineEdit();
    nameLabel->setBuddy(nameEdit);
    nameEdit->setFirstFocus();

    pwdContainer=new WContainerWidget();

    pwdLabel = new Wt::WLabel("Password: ",pwdContainer);
    pwdLabel->setMargin(2,Wt::Bottom);
    pwdLabel->setMargin(10,Wt::Top);
    pwdLabel->addStyleClass("control-label");

    pwdEdit=new Wt::WLineEdit(pwdContainer);
    pwdEdit->setEchoMode(Wt::WLineEdit::Password);
    pwdEdit->enterPressed().connect(this, &ywLoginPage::loginClick);
    pwdLabel->setBuddy(pwdEdit);

    nameEdit->enterPressed().connect(std::bind([=] () {
        pwdEdit->setFocus();
        }));


    Wt::WPushButton* loginButton = new Wt::WPushButton("Login");
    loginButton->setStyleClass("btn-primary");
    loginButton->setMargin(18,Wt::Top);
    loginButton->setMaximumSize(70,Wt::WLength::Auto);
    loginButton->clicked().connect(this, &ywLoginPage::loginClick);
    loginButton->setDefault(true);

    innerlayout->addWidget(nameLabel);
    innerlayout->addWidget(nameEdit);
    innerlayout->addWidget(pwdContainer);
    innerlayout->addStretch(1);
    innerlayout->addWidget(loginButton,0,Wt::AlignRight);

    innercontainer->setLayout(innerlayout);
    panel->setCentralWidget(innercontainer);

    nameEdit->setFocus();
}


ywLoginPage* ywLoginPage::createInstance(ywApplication* parent)
{
    return new ywLoginPage(parent);
}


void ywLoginPage::loginClick()
{
    WString loginName=nameEdit->text();
    WString loginPassword=pwdEdit->text();

    ywUser* loginUser=app->configuration->validateUser(loginName, loginPassword);

    if (loginUser==0)
    {
        // Login information is not correct
        pwdContainer->addStyleClass("has-error");
        pwdEdit->setText("");
        pwdEdit->setFocus();
    }
    else
    {
        app->currentUser=loginUser->name;
        app->currentLevel=loginUser->level;
        app->performLogin();
    }
}

