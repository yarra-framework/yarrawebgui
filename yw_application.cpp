#include "yw_application.h"
#include "yw_global.h"

#include <Wt/WTimer>


ywApplication::ywApplication(const WEnvironment& env)
  : WApplication(env)
{
    configuration=0;
    loginPage=0;
    statusPage=0;
    logPage=0;
    configPage=0;
    currentUser="None";
    currentLevel=1;

    // Pass the server interface an pointer for cross access
    server.setAppInstance(this);

    setLoadingIndicator(new Wt::WOverlayLoadingIndicator());

    Wt::WBootstrapTheme *bootstrapTheme = new Wt::WBootstrapTheme(this);
    bootstrapTheme->setVersion(Wt::WBootstrapTheme::Version3);
    bootstrapTheme->setResponsive(true);
    setTheme(bootstrapTheme);
    useStyleSheet("style/bootstrap.min.css");
    useStyleSheet("style/yarra.css");
}


 void ywApplication::prepare(ywConfiguration* configInstance)
{
    configuration=configInstance;

    Wt::WVBoxLayout* layout=new Wt::WVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    root()->setLayout(layout);

    if (!configuration->isValid())
    {
        // Configuration is not valid. Show an error screen to inform the user.
        WVBoxLayout *layout = new Wt::WVBoxLayout();
        layout->setContentsMargins(0, 0, 0, 0);
        root()->setLayout(layout);

        WContainerWidget* innercontainer=new WContainerWidget();
        WHBoxLayout *layout2 = new Wt::WHBoxLayout();
        innercontainer->setLayout(layout2);
        layout->addWidget(new WContainerWidget(),1);
        layout->addWidget(innercontainer);
        layout->addWidget(new WContainerWidget(),1);

        WContainerWidget* innercontainer2=new WContainerWidget();
        WHBoxLayout *layout3 = new Wt::WHBoxLayout();
        innercontainer2->setLayout(layout3);

        layout2->addWidget(new WContainerWidget(),1);
        layout2->addWidget(innercontainer2);
        layout2->addWidget(new WContainerWidget(),1);

        WPanel* panel = new Wt::WPanel();
        panel->addStyleClass("panel panel-warning");
        panel->setTitle("YarraServer: Invalid Configuration");
        innercontainer2->setMaximumSize(500,300);

        WText* textField=new WText();
        textField->setText("The configuration of the YarraServer WebGUI is not valid. Therefore, the WebGUI is not available.<br /><br />Please check the configuration file YarraWebGUI.ini in the YarraServer installation directory.");
        panel->setCentralWidget(textField);

        layout3->addWidget(panel);
    }
    else
    {
        loginPage=ywLoginPage::createInstance(this);
        layout->addWidget(loginPage,1);

        setTitle("YarraServer: " + configuration->serverName);
    }
}


void ywApplication::performLogin()
{
    root()->layout()->clear();
    loginPage=0;

    WContainerWidget* container=new WContainerWidget();
    Wt::WVBoxLayout *layout_inner = new Wt::WVBoxLayout();
    layout_inner->setContentsMargins(0, 0, 0, 0);
    container->setLayout(layout_inner);

    WNavigationBar* navbar=new WNavigationBar();
    navbar->setTitle("YarraServer");
    navbar->setResponsive(true);
    navbar->addStyleClass("main-nav");
    layout_inner->addWidget(navbar);

    Wt::WStackedWidget *contentsStack = new Wt::WStackedWidget();
    contentsStack->addStyleClass("contents");
    contentsStack->setPadding(WLength(20), Wt::Horizontals);
    layout_inner->addWidget(contentsStack,1);

    // Setup up the top menu in the navbar (depending on user role)
    Wt::WMenu *leftMenu = new Wt::WMenu(contentsStack);

    if (currentLevel==YW_USERLEVEL_ADMIN)
    {
        statusPage=ywStatusPage::createInstance(this);
        leftMenu->addItem("Server Status", statusPage)->triggered().connect(std::bind([=] () {
                statusPage->refreshCurrentTab();
        }));
    }

    queuePage=ywQueuePage::createInstance(this);
    leftMenu->addItem("Task Queue", queuePage)->triggered().connect(std::bind([=] () {
        queuePage->refreshLists();
    }));

    if ((currentLevel==YW_USERLEVEL_ADMIN) || (currentLevel==YW_USERLEVEL_RESEARCHER))
    {
        logPage=ywLogPage::createInstance(this);
        leftMenu->addItem("Log Archive", logPage)->triggered().connect(std::bind([=] () {
            logPage->refreshLogs();
        }));
    }

    if (currentLevel==YW_USERLEVEL_ADMIN)
    {
        configPage=ywConfigPage::createInstance(this);
        leftMenu->addItem("Configuration", configPage)->triggered().connect(std::bind([=] () {
            configPage->refreshStatus();
        }));

        // Embed the support forum from the yarra homepage
        if (!configuration->disableSupportForum)
        {
            WText* iframeText=new WText();
            iframeText->setTextFormat(XHTMLUnsafeText);
            iframeText->setText("<iframe src=\"http://yarraframework.com/embed_support\" width=\"100%\" height=\"100%\" style=\"border:none\"></iframe>");

            leftMenu->addItem("Support", iframeText)->triggered().connect(std::bind([=] () {
                iframeText->setText("");
                iframeText->refresh();
                iframeText->setText("<iframe src=\"http://yarraframework.com/embed_support\" width=\"100%\" height=\"100%\" style=\"border:none\"></iframe>");
            }));
        }
    }

    navbar->addMenu(leftMenu);

    // Setup a Right-aligned menu.
    Wt::WMenu *rightMenu = new Wt::WMenu();
    navbar->addMenu(rightMenu, Wt::AlignRight);

    // Create a popup submenu for the Help menu.
    Wt::WPopupMenu *popup = new Wt::WPopupMenu();

    WMenuItem* helpItem=popup->addItem("Help");
    helpItem->setLink(Wt::WLink("http://yarraframework.com/server"));
    helpItem->setLinkTarget(Wt::TargetNewWindow);

    popup->addItem("About")->triggered().connect(this, &ywApplication::showAbout);
    popup->addSeparator();
    popup->addItem("Logout")->triggered().connect(this, &ywApplication::requestLogout);

    Wt::WMenuItem *item = new Wt::WMenuItem("User: "+currentUser);
    item->setMenu(popup);
    rightMenu->addItem(item);

    WVBoxLayout* layout=(WVBoxLayout*) root()->layout();
    layout->addWidget(container,1);
}


void ywApplication::requestLogout()
{
    WTimer::singleShot(0, this, &ywApplication::performLogout);
}


void ywApplication::performLogout()
{
    root()->layout()->clear();
    statusPage=0;
    configPage=0;
    queuePage=0;
    logPage=0;
    configPage=0;

    currentUser="None";
    currentLevel=1;

    loginPage=ywLoginPage::createInstance(this);
    WVBoxLayout* layout=(WVBoxLayout*) root()->layout();
    layout->addWidget(loginPage,1);    
}


void ywApplication::showAbout()
{
    Wt::WMessageBox *messageBox = new Wt::WMessageBox
       ("About",
        "<p>YarraServer WebGUI</p><p>Version "+ WString(YW_VERSION) +" (Build: " + WString(__DATE__) +  ")</p>"
        "<p>Developed by Kai Tobias Block and Roy Wiggins.</p>"
        "<p><a href=\"http://yarraframework.com\" target=\"_blank\">http://yarraframework.com</a></p>",
        Wt::Information, Wt::Ok);

    messageBox->setModal(true);

    messageBox->buttonClicked().connect(std::bind([=] ()
    {
        delete messageBox;
    }));

    messageBox->show();
}
