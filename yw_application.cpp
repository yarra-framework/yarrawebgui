#include "yw_application.h"
#include "yw_global.h"

#include <Wt/WTimer>


ywApplication::ywApplication(const WEnvironment& env)
  : WApplication(env)
{
    setLoadingIndicator(new Wt::WOverlayLoadingIndicator());

    Wt::WBootstrapTheme *bootstrapTheme = new Wt::WBootstrapTheme(this);
    bootstrapTheme->setVersion(Wt::WBootstrapTheme::Version3);
    bootstrapTheme->setResponsive(true);
    setTheme(bootstrapTheme);
    useStyleSheet("style/bootstrap.min.css");
    useStyleSheet("style/yarra.css");

    setTitle("YarraServer: 64core_1");

    userName="Admin";

    Wt::WVBoxLayout* layout=new Wt::WVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    root()->setLayout(layout);

    loginPage=ywLoginPage::createInstance(this);
    layout->addWidget(loginPage,1);
}

void ywApplication::greet()
{
    Wt::StandardButton answer = Wt::WMessageBox::show("Server Shutdown", "<p>Are you sure to shutdown the server?</p>", Wt::Ok | Wt::Cancel);
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
    Wt::WAnimation animation(Wt::WAnimation::Fade, Wt::WAnimation::Linear, 200);
    contentsStack->setTransitionAnimation(animation, false);
    layout_inner->addWidget(contentsStack,1);


    Wt::WContainerWidget* statusPage=new  Wt::WContainerWidget();
    Wt::WVBoxLayout* statusPageLayout = new Wt::WVBoxLayout();
    statusPage->setLayout(statusPageLayout);
    statusPageLayout->setContentsMargins(0, 0, 0, 0);

    Wt::WPanel *panel = new Wt::WPanel();
    Wt::WContainerWidget *btncontainer = new Wt::WContainerWidget();

    Wt::WPushButton *button = new Wt::WPushButton("Start", btncontainer);
    button->setStyleClass("btn-primary");
    button->clicked().connect(this, &ywApplication::greet);
    button->setMargin(2);

    button = new Wt::WPushButton("Stop", btncontainer);
    button->setMargin(2);
    button->setStyleClass("btn-primary");

    button = new Wt::WPushButton("Kill Task", btncontainer);
    button->setMargin(2);
    button->setStyleClass("btn-primary");

    button = new Wt::WPushButton("Refresh", btncontainer);
    button->setStyleClass("btn");
    button->setMargin(2);

    panel->setCentralWidget(btncontainer);
    panel->setMargin(30, Wt::Bottom);


    Wt::WTabWidget *tabW = new Wt::WTabWidget();
    tabW->addTab(new Wt::WTextArea("This is the contents of the first tab."),
             "Status", Wt::WTabWidget::PreLoading);
    tabW->addTab(new Wt::WTextArea("The contents of the tabs are pre-loaded in"
                       " the browser to ensure swift switching."),
             "Server Log", Wt::WTabWidget::PreLoading);
    tabW->addTab(new Wt::WTextArea("You could change any other style attribute of the"
                       " tab widget by modifying the style class."
                       " The style class 'trhead' is applied to this tab."),
             "Task Log", Wt::WTabWidget::PreLoading);


    tabW->setStyleClass("tabwidget");

    Wt::WText* head1=new Wt::WText("<h3>Server Activity</h3>");
    head1->setMargin(6, Wt::Bottom);
    statusPageLayout->addWidget(head1);
    statusPageLayout->addWidget(tabW,1);

    Wt::WText* head2=new Wt::WText("<h3>Runtime Control</h3>");
    head2->setMargin(6, Wt::Bottom);
    head2->setMargin(20, Wt::Top);

    statusPageLayout->addWidget(head2);
    statusPageLayout->addWidget(panel);



    Wt::WContainerWidget* configPage=new  Wt::WContainerWidget();
    Wt::WHBoxLayout* configPageLayout = new Wt::WHBoxLayout();
    configPage->setLayout(configPageLayout);

    Wt::WStackedWidget *configContents=new Wt::WStackedWidget();
    configContents->setMargin(30, Wt::Bottom);
    configContents->setMargin(30, Wt::Left);

    Wt::WMenu *configMenu = new Wt::WMenu(configContents, Wt::Vertical, configPage);
    configMenu->setStyleClass("nav nav-pills nav-stacked");
    configMenu->setWidth(200);

    configMenu->addItem("Internal paths", new Wt::WTextArea("Internal paths contents"));
    configMenu->addItem("Anchor", new Wt::WTextArea("Anchor contents"));
    configMenu->addItem("Stacked widget", new Wt::WTextArea("Stacked widget contents"));
    configMenu->addItem("Tab widget", new Wt::WTextArea("Tab widget contents"));
    configMenu->addItem("Menu", new Wt::WTextArea("Menu contents"));

    configPageLayout->addWidget(configMenu);
    configPageLayout->addWidget(configContents,1);


    Wt::WMenu *leftMenu = new Wt::WMenu(contentsStack);
    leftMenu->addItem("Server Status", statusPage);
    leftMenu->addItem("Task Queue", new Wt::WText("Layout content 1"));
    leftMenu->addItem("Log Archive", new Wt::WText("Layout content 2"));
    leftMenu->addItem("Configuration", configPage);
    navbar->addMenu(leftMenu);


    // Setup a Right-aligned menu.
    Wt::WMenu *rightMenu = new Wt::WMenu();
    navbar->addMenu(rightMenu, Wt::AlignRight);

    // Create a popup submenu for the Help menu.
    Wt::WPopupMenu *popup = new Wt::WPopupMenu();

    WMenuItem* helpItem=popup->addItem("Help");
    helpItem->setLink(Wt::WLink("http://ktblock.de/yarra"));
    helpItem->setLinkTarget(Wt::TargetNewWindow);

    popup->addItem("About")->triggered().connect(this, &ywApplication::showAbout);
    popup->addSeparator();
    popup->addItem("Logout")->triggered().connect(this, &ywApplication::requestLogout);

    Wt::WMenuItem *item = new Wt::WMenuItem(userName);
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
    loginPage=ywLoginPage::createInstance(this);
    WVBoxLayout* layout=(WVBoxLayout*) root()->layout();
    layout->addWidget(loginPage,1);
}

void ywApplication::showAbout()
{
    Wt::WMessageBox *messageBox = new Wt::WMessageBox
       ("About",
        "<p>YarraServer WebGUI</p><p>Version "+ WString(YW_VERSION) +" (Build: " + WString(__DATE__) +  ")</p>"
        "<p>Developed by Kai Tobias Block (yarra@ktblock.de)</p>",
        Wt::Information, Wt::Ok);

    messageBox->setModal(false);

    messageBox->buttonClicked().connect(std::bind([=] ()
    {
        delete messageBox;
    }));

    messageBox->show();
}
