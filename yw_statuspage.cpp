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
#include <Wt/WTimer>

#include "yw_statuspage.h"
#include "yw_application.h"
#include "yw_serverinterface.h"


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
    statusWidget=new Wt::WTemplate();
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
    app->server.updateStatus();

    WString widgetText="<p>";

    switch (app->server.serverStatus)
    {
    case ywServerInterface::STATUS_UKNOWN:
    default:
        widgetText+="<span class=\"label label-danger\">ERROR</span>";
        break;
    case ywServerInterface::STATUS_DOWN:
        widgetText+="<span class=\"label label-danger\">DOWN</span>";
        break;
    case ywServerInterface::STATUS_IDLE:
        widgetText+="<span class=\"label label-success\">ACTIVE</span>&nbsp;<span class=\"label label-primary\">IDLE</span>";
        break;
    case ywServerInterface::STATUS_PROCESSING:
        widgetText+="<span class=\"label label-success\">ACTIVE</span>&nbsp;<span class=\"label label-info\">PROCESSING</span>";
        break;
    case ywServerInterface::STATUS_PROCESSINGDOWN:
        widgetText+="<span class=\"label label-success\">ACTIVE</span>&nbsp;<span class=\"label label-info\">PROCESSING</span>&nbsp;";
        widgetText+="<span class=\"label label-warning\">SHUTDOWN</span>";
        break;
    }

    widgetText+="</p><p><strong>Name: &nbsp;</strong>" + app->configuration->serverName + " (Server Type " + app->configuration->serverType + ")";
    widgetText+="<br /><strong>Status: &nbsp;</strong>" + app->server.serverStatusText;

    if  ((app->server.serverStatus==ywServerInterface::STATUS_PROCESSING) || (app->server.serverStatus==ywServerInterface::STATUS_PROCESSINGDOWN))
    {
        widgetText+="<br /><strong>Task: </strong>" + app->server.serverTaskID + "<br />";
    }

    widgetText+="</p>";


    statusWidget->setTemplateText(widgetText,XHTMLUnsafeText);
}



void ywStatusPage::refreshServerLog()
{
    WString serverLogFilename=app->server.getServerLogFilename();

    WString widgetText="";

    std::string line;
    std::ifstream logfile(serverLogFilename.toUTF8());

    if(!logfile) //Always test the file open.
    {
        widgetText="ERROR: Unable to open server log file (" + serverLogFilename + ")";
    }
    else
    {
        while (std::getline(logfile, line))
        {
            widgetText+=WString::fromUTF8(line)+"\n";
        }
        logfile.close();
    }

    serverLogWidget->setText(widgetText);
}


void ywStatusPage::refreshTaskLog()
{
    app->server.updateStatus();
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
            widgetText="ERROR: Unable to open task log file (" + taskLogFilename + ")";
        }
        else
        {
            while (std::getline(logfile, line))
            {
                widgetText+=WString::fromUTF8(line)+"\n";
            }
            logfile.close();
        }
    }

    taskLogWidget->setText(widgetText);
}


void ywStatusPage::callStartServer()
{
    app->server.updateStatus();

    if (app->server.serverRunning)
    {
        Wt::WMessageBox::show("Already Running",
                              "The server is already running and responsive.", Wt::Ok);
    }
    else
    {
        if (!app->server.startServer())
        {
            Wt::WMessageBox messageBox("Error", "A problem occured while starting the server.",
                                       Wt::Information, Wt::Ok);
            messageBox.setModal(true);
            messageBox.setIcon(Wt::Critical);
            messageBox.buttonClicked().connect(&messageBox, &WMessageBox::accept);
            messageBox.exec();
        }

        // Launch timer in 1 sec and update status
        WTimer::singleShot(1000, this, &ywStatusPage::refreshStatus);
    }
}


void ywStatusPage::callStopServer()
{
    app->server.updateStatus();

    if (app->server.serverStatus==ywServerInterface::STATUS_PROCESSINGDOWN)
    {
        Wt::WMessageBox::show("Server Shutdown",
                              "Server is shutting down after the current task.", Wt::Ok);
    }

    bool taskIsActive=app->server.serverStatus==ywServerInterface::STATUS_PROCESSING;

    Wt::StandardButton answer=Wt::Cancel;
    if (app->server.serverRunning)
    {
        answer=Wt::WMessageBox::show("Server Shutdown",
                                     "Are you sure to shutdown the server? Tasks will not be processed until the server is started again.",
                                     Wt::Ok | Wt::Cancel);
    }
    else
    {
        answer=Wt::WMessageBox::show("Server Shutdown",
                                     "The server is not running or might be unresponsive. Shall a shutdown be requested regardless?",
                                     Wt::Ok | Wt::Cancel);
    }

    if (answer==Wt::Ok)
    {
        if (!app->server.stopServer())
        {
            Wt::WMessageBox messageBox("Error", "A problem occured while stopping the server.",
                                       Wt::Information, Wt::Ok);
            messageBox.setModal(true);
            messageBox.setIcon(Wt::Critical);
            messageBox.buttonClicked().connect(&messageBox, &WMessageBox::accept);
            messageBox.exec();
        }
        else
        {
            if (taskIsActive)
            {
                Wt::WMessageBox::show("Server Shutdown",
                                      "Shutdown has been requested. Server will go down when the task has been completed.", Wt::Ok);
            }
        }

        // Launch timer in 1 sec and update status
        WTimer::singleShot(1000, this, &ywStatusPage::refreshStatus);
    }
}


void ywStatusPage::callKillServer()
{
    Wt::StandardButton answer=Wt::WMessageBox::show("Kill Task",
                                                    "Are you sure to kill the active task? This should only be done after ensuring that the task is unresponsive.",
                                                    Wt::Ok | Wt::Cancel);
    if (answer==Wt::Ok)
    {
        if (!app->server.killServer())
        {
            Wt::WMessageBox messageBox("Error", "A problem occured while killing the server process.",
                                       Wt::Information, Wt::Ok);
            messageBox.setModal(true);
            messageBox.setIcon(Wt::Critical);
            messageBox.buttonClicked().connect(&messageBox, &WMessageBox::accept);
            messageBox.exec();
        }

        // Launch timer in 1 sec and update status
        WTimer::singleShot(1000, this, &ywStatusPage::refreshStatus);
    }
}


