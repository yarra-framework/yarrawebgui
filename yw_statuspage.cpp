#include <iostream>
#include <fstream>
#include <string>

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
#include <Wt/WScrollArea>

#include "yw_statuspage.h"
#include "yw_application.h"


ywStatusPage::ywStatusPage(ywApplication* parent)
 : WContainerWidget()
{
    app=parent;

    Wt::WVBoxLayout* statusPageLayout = new Wt::WVBoxLayout();
    statusPageLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(statusPageLayout);

    Wt::WPanel *panel = new Wt::WPanel();
    Wt::WContainerWidget *btncontainer = new Wt::WContainerWidget();

    Wt::WPushButton *button = new Wt::WPushButton("Start", btncontainer);
    button->setStyleClass("btn-primary");
    button->setMargin(2);
    button->clicked().connect(this, &ywStatusPage::callStartServer);

    button = new Wt::WPushButton("Stop", btncontainer);
    button->setMargin(2);
    button->setStyleClass("btn-primary");
    button->clicked().connect(this, &ywStatusPage::callStopServer);

    button = new Wt::WPushButton("Kill Task", btncontainer);
    button->setMargin(2);
    button->setStyleClass("btn-primary");
    button->clicked().connect(this, &ywStatusPage::callKillServer);

    button = new Wt::WPushButton("Refresh", btncontainer);
    button->setStyleClass("btn");
    button->setMargin(2);
    button->clicked().connect(this, &ywStatusPage::refreshCurrentTab);

    panel->setCentralWidget(btncontainer);
    panel->setMargin(30, Wt::Bottom);

    tabWidget = new Wt::WTabWidget();

    // Tab for the status log
    WContainerWidget* statusContainer=new WContainerWidget();
    Wt::WVBoxLayout*  statusLayout=new Wt::WVBoxLayout();
    statusContainer->setLayout(statusLayout);
    statusWidget=new Wt::WText();
    statusScrollArea=new Wt::WScrollArea();
    statusScrollArea->setWidget(statusWidget);
    statusLayout->addWidget(statusScrollArea);

    // Tab for the server log
    WContainerWidget* serverLogContainer=new WContainerWidget();
    Wt::WVBoxLayout*  serverLogLayout=new Wt::WVBoxLayout();
    serverLogContainer->setLayout(serverLogLayout);
    serverLogWidget=new Wt::WText();
    serverLogWidget->setTextFormat(Wt::PlainText);
    serverLogWidget->setWordWrap(false);
    serverLogScrollArea=new Wt::WScrollArea();
    //serverLogScrollArea->setHorizontalScrollBarPolicy(Wt::WScrollArea::ScrollBarAlwaysOn);
    //serverLogScrollArea->setScrollBarPolicy(Wt::WScrollArea::ScrollBarAlwaysOn);
    serverLogScrollArea->setWidget(serverLogWidget);
    serverLogLayout->addWidget(serverLogScrollArea);

    // Tab for the task log
    WContainerWidget* taskLogContainer=new WContainerWidget();
    Wt::WVBoxLayout*  taskLogLayout=new Wt::WVBoxLayout();
    taskLogContainer->setLayout(taskLogLayout);
    taskLogWidget=new Wt::WText();
    taskLogWidget->setTextFormat(Wt::PlainText);
    taskLogWidget->setWordWrap(false);
    taskLogScrollArea=new Wt::WScrollArea();
    taskLogScrollArea->setWidget(taskLogWidget);
    taskLogLayout->addWidget(taskLogScrollArea);

    tabWidget->addTab(statusContainer,    "Status",     Wt::WTabWidget::PreLoading);
    tabWidget->addTab(serverLogContainer, "Server Log", Wt::WTabWidget::PreLoading);
    tabWidget->addTab(taskLogContainer,   "Task Log",   Wt::WTabWidget::PreLoading);
    tabWidget->currentChanged().connect(this, &ywStatusPage::tabChanged);
    tabWidget->setStyleClass("tabwidget");

    Wt::WText* head1=new Wt::WText("<h3>Server Activity</h3>");
    head1->setMargin(6, Wt::Bottom);
    statusPageLayout->addWidget(head1);
    statusPageLayout->addWidget(tabWidget,1);

    Wt::WText* head2=new Wt::WText("<h3>Runtime Control</h3>");
    head2->setMargin(6, Wt::Bottom);
    head2->setMargin(20, Wt::Top);

    statusPageLayout->addWidget(head2);
    statusPageLayout->addWidget(panel);

    refreshCurrentTab();
}


ywStatusPage* ywStatusPage::createInstance(ywApplication* parent)
{
    return new ywStatusPage(parent);
}


void ywStatusPage::tabChanged(int tab)
{
    refreshCurrentTab();
}


void ywStatusPage::refreshCurrentTab()
{
    switch (tabWidget->currentIndex())
    {
    case PAGE_STATUS:
    default:
        refreshStatus();
        break;
    case PAGE_SERVERLOG:
        refreshServerLog();
        break;
    case PAGE_TASKLOG:
        refreshTaskLog();
        break;
    }
}


void ywStatusPage::refreshStatus()
{
    WString widgetText="";

    widgetText="Server is active and responsive.";

    statusWidget->setText(widgetText);
}



void ywStatusPage::refreshServerLog()
{
    WString serverLogFilename=app->server.getServerLogFilename();

    WString widgetText="";

    std::string line;
    std::ifstream logfile(serverLogFilename.toUTF8());

    if(!logfile) //Always test the file open.
    {
        widgetText="ERROR: Unable to open server log file.";
    }
    else
    {
        while (std::getline(logfile, line))
        {
            widgetText+=WString::fromUTF8(line)+"\n";
        }
    }

    serverLogWidget->setText(widgetText);
}


void ywStatusPage::refreshTaskLog()
{
    WString taskLogFilename=app->server.getTaskLogFilename();

    WString widgetText="";

    if (taskLogFilename.empty())
    {
        widgetText="No task is processed currently.";
    }
    else
    {
        std::string line;
        std::ifstream logfile(taskLogFilename.toUTF8());

        if(!logfile)
        {
            widgetText="ERROR: Unable to open task log file.";
        }
        else
        {
            while (std::getline(logfile, line))
            {
                widgetText+=WString::fromUTF8(line)+"\n";
            }
        }
    }

    taskLogWidget->setText(widgetText);
}


void ywStatusPage::callStartServer()
{
    bool serverRunning=false;
    // TODO: Check if server is already running

    if (serverRunning)
    {
        Wt::WMessageBox::show("Already Running",
                              "<p>The server is already running and responsive.</p>", Wt::Ok);
    }
    else
    {
        // TODO: Call command to start server

        // TODO: Launch timer in 1 sec and update status
    }
}


void ywStatusPage::callStopServer()
{
    Wt::StandardButton answer=Wt::WMessageBox::show("Server Shutdown",
                                                    "<p>Are you sure to shutdown the server?</p><p>Tasks will not be processed until the server is started again.</p>",
                                                    Wt::Ok | Wt::Cancel);
    if (answer==Wt::Ok)
    {
        // Call shutdown command

        Wt::WMessageBox::show("Server Shutdown",
                              "<p>Shutdown of the server has been requested.</p><p>If a task is active, it will go down when the task has been completed.</p>", Wt::Ok);

        // TODO: Launch timer in 1 sec and update status
    }
}


void ywStatusPage::callKillServer()
{
    Wt::StandardButton answer=Wt::WMessageBox::show("Kill Task",
                                                    "<p>Are you sure to kill the active task?</p><p>This should only be done after ensuring that the task is unresponsive.</p>",
                                                    Wt::Ok | Wt::Cancel);
    if (answer==Wt::Ok)
    {
        // TODO: Kill the current job
    }

}


