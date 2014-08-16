#include "yw_queuepage.h"
#include "yw_application.h"

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
#include <Wt/WText>
#include <Wt/WScrollArea>
#include <Wt/WTimer>

#include <boost/filesystem.hpp>


ywQueuePage::ywQueuePage(ywApplication* parent)
 : WContainerWidget()
{
    app=parent;

    // Page container
    Wt::WVBoxLayout* queuePageLayout = new Wt::WVBoxLayout();
    queuePageLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(queuePageLayout);

    // Queue List container
    WContainerWidget* queueContainer=new WContainerWidget();
    Wt::WVBoxLayout*  queueLayout=new Wt::WVBoxLayout();
    queueLayout->setContentsMargins(0, 0, 0, 0);
    queueContainer->setLayout(queueLayout);

    //queuePageLayout->addWidget(queueContainer);

    Wt::WContainerWidget* taskContainer = new Wt::WContainerWidget();
    taskLayout = new Wt::WVBoxLayout();
    taskLayout->setContentsMargins(0, 0, 0, 0);
    taskLayout->setSpacing(4);
    taskContainer->setLayout(taskLayout);

    Wt::WContainerWidget* centerContainer = new Wt::WContainerWidget();
    Wt::WHBoxLayout* centerLayout = new Wt::WHBoxLayout();
    centerLayout->setContentsMargins(0, 20, 0, 0);
    centerLayout->setSpacing(0);
    centerContainer->setLayout(centerLayout);
    centerLayout->addWidget(taskContainer, 0, Wt::AlignCenter);

    Wt::WScrollArea* scrollArea=new Wt::WScrollArea();
    scrollArea->setWidget(centerContainer);
    queueLayout->addWidget(scrollArea);

    // Failed List container
    WContainerWidget* failContainer=new WContainerWidget();
    Wt::WVBoxLayout*  failLayout=new Wt::WVBoxLayout();
    failLayout->setContentsMargins(0, 0, 0, 0);
    failContainer->setLayout(failLayout);

    Wt::WContainerWidget* failcenterContainer = new Wt::WContainerWidget();
    Wt::WHBoxLayout* failcenterLayout = new Wt::WHBoxLayout();
    failcenterLayout->setContentsMargins(0, 20, 0, 0);
    failcenterLayout->setSpacing(0);
    failcenterContainer->setLayout(failcenterLayout);

    Wt::WContainerWidget* failtaskContainer = new Wt::WContainerWidget();
    failtaskLayout = new Wt::WVBoxLayout();
    failtaskLayout->setContentsMargins(0, 0, 0, 0);
    failtaskLayout->setSpacing(4);
    failtaskContainer->setLayout(failtaskLayout);

    Wt::WScrollArea* failscrollArea=new Wt::WScrollArea();
    failscrollArea->setWidget(failcenterContainer);
    failcenterLayout->addWidget(failtaskContainer, 0, Wt::AlignCenter);
    failLayout->addWidget(failscrollArea);

    // Tab widget
    tabWidget = new Wt::WTabWidget();
    tabWidget->addTab(queueContainer, "Queue List", Wt::WTabWidget::PreLoading);
    tabWidget->addTab(failContainer,  "Fail List",    Wt::WTabWidget::PreLoading);
    tabWidget->currentChanged().connect(this, &ywQueuePage::tabChanged);
    tabWidget->setStyleClass("tabwidget");

    // Button panel
    WContainerWidget* btnContainer=new WContainerWidget();
    Wt::WHBoxLayout*  btnLayout=new Wt::WHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnContainer->setLayout(btnLayout);

    WContainerWidget* innerBtnContainer=new WContainerWidget();

    Wt::WPanel *panel = new Wt::WPanel();
    btnLayout->addWidget(panel);
    panel->setCentralWidget(innerBtnContainer);
    panel->setMargin(30, Wt::Bottom);

    Wt::WPushButton* button=0;
    button =new Wt::WPushButton("Refresh", innerBtnContainer);
    button->setStyleClass("btn");
    button->setMargin(2);
    button->clicked().connect(this, &ywQueuePage::refreshLists);


    queuePageLayout->addWidget(tabWidget,1);
    queuePageLayout->addWidget(btnContainer);

    taskContainer->resize(800,Wt::WLength::Auto);
    taskContainer->setMinimumSize(600,Wt::WLength::Auto);
    failtaskContainer->resize(800,Wt::WLength::Auto);
    failtaskContainer->setMinimumSize(600,Wt::WLength::Auto);
    failtaskContainer->setMaximumSize(800,Wt::WLength::Auto);

    refreshLists();
}


void ywQueuePage::refreshLists()
{
    if (tabWidget->currentIndex()==0)
    {
        refreshQueueList();
    }
    if (tabWidget->currentIndex()==1)
    {
        refreshFailList();
    }
}


WPanel* ywQueuePage::createQueuePanel(WString title, int mode)
{
    WPanel* panel=new WPanel();
    panel->setTitle(title);
    //panel->setSelectable(true);

    switch (mode)
    {
    case MODE_PROC:
        panel->addStyleClass("panelqueue panel-proc");
        break;
    case MODE_NORMAL:
    default:
        panel->addStyleClass("panelqueue panel-jobnormal");
        break;
    case MODE_PRIO:
        panel->addStyleClass("panelqueue panel-prio");
        break;
    case MODE_NIGHT:
        panel->addStyleClass("panelqueue panel-night");
        break;
    case MODE_FAIL:
        panel->addStyleClass("panelqueue panel-fail");
        break;
    }
    panel->addStyleClass("modal-content");
    panel->setCollapsible(true);
    panel->setCollapsed(true);

    WContainerWidget* innerWidget=new WContainerWidget();
    panel->setCentralWidget(innerWidget);


    WText* taskInfo=new WText(innerWidget);
    taskInfo->setText("");

    panel->expanded().connect(std::bind([=] () {
        if (taskInfo->text().empty())
        {
            updateTaskInformation(title, taskInfo, mode);
        }
    }));

    panel->titleBarWidget()->clicked().connect( std::bind([=] () {
        if (taskInfo->text().empty())
        {
            updateTaskInformation(title, taskInfo, mode);
        }
    }));


    if (app->currentLevel>ywApplication::YW_USERLEVEL_TECH)
    {
        WContainerWidget* btnContainer=new WContainerWidget(innerWidget);
        Wt::WHBoxLayout*  btnLayout=new Wt::WHBoxLayout();
        btnLayout->setContentsMargins(0, 6, 0, 0);
        btnLayout->setSpacing(4);
        btnContainer->setLayout(btnLayout);

        Wt::WPopupMenu *popup = new Wt::WPopupMenu();

        popup->addItem("Info")->triggered().connect(std::bind([=] () {
            showInfo(title);
        }));

        if (mode==MODE_FAIL)
        {
            popup->addItem("Restart")->triggered().connect(std::bind([=] () {
            }));
        }
        if ((mode!=MODE_PROC) && (mode!=MODE_FAIL))
        {
            popup->addSeparator();
            if (mode!=MODE_NORMAL)
            {
                popup->addItem("Set mode to NORMAL")->triggered().connect(std::bind([=] () {
                }));
            }
            if (mode!=MODE_PRIO)
            {
                popup->addItem("Set mode to PRIORITY")->triggered().connect(std::bind([=] () {
                }));
            }
            if (mode!=MODE_NIGHT)
            {
                popup->addItem("Set moode to NIGHT")->triggered().connect(std::bind([=] () {
                }));
            }
            popup->addSeparator();
            popup->addItem("Change ACC / notification")->triggered().connect(std::bind([=] () {
            }));
        }

        if (mode!=MODE_PROC)
        {
            popup->addItem("Delete")->triggered().connect(std::bind([=] () {
            }));
        }

        WPushButton* button = new Wt::WPushButton("Edit");
        button->setMenu(popup);
        button->addStyleClass("btn btn-primary btn-xs");
        btnLayout->addWidget(button,0, Wt::AlignLeft);

        //btnLayout->addWidget(new WContainerWidget,1);
    }

    return panel;
}


void ywQueuePage::updateTaskInformation(WString taskName, WText* taskWidget,int taskType)
{
    // TODO
    taskWidget->setText("More features coming soon!");
}


void ywQueuePage::showInfo (WString taskName)
{
    Wt::WMessageBox::show("Selected Case",
                          "The selected case is: " + taskName, Wt::Ok);
}


void ywQueuePage::refreshFailList()
{
    // Clear both pages to free-up memory
    failtaskLayout->clear();
    WTimer::singleShot(0, this, &ywQueuePage::clearQueueList);

    WText* failedLabel=new WText("Failed Tasks");
    failtaskLayout->addWidget(failedLabel,Wt::Left);


    // Read log directory, except yarra.log
    namespace fs = boost::filesystem;
    fs::path failDir(app->configuration->yarraFailPath.toUTF8());
    fs::directory_iterator end_iter;

    typedef std::multimap<std::time_t,  boost::filesystem::path> result_set_t;
    result_set_t result_set;

    try
    {
        if ( fs::exists(failDir) && fs::is_directory(failDir))
        {
            for( fs::directory_iterator dir_iter(failDir) ; dir_iter != end_iter ; ++dir_iter)
            {
                if ( fs::is_directory(dir_iter->status()) )
                {
                    result_set.insert(result_set_t::value_type(fs::last_write_time(dir_iter->path()), *dir_iter));
                }
            }
        }
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
    }

    int i=0;
    // Iterate backwards through the results list
    for (result_set_t::reverse_iterator ii=result_set.rbegin(); ii!=result_set.rend(); ++ii)
    {
        WString dirName=WString((*ii).second.filename().generic_string());
        failtaskLayout->addWidget(createQueuePanel(dirName, MODE_FAIL),Wt::AlignMiddle);
        i++;

        // Only show max 100 entries to avoid slow speed.
        if (i>100)
        {
            break;
        }
    }

    if (i==0)
    {
        failedLabel->setText("No failed tasks found.");
    }
}


void ywQueuePage::clearFailList()
{
    failtaskLayout->clear();
}


void ywQueuePage::clearQueueList()
{
    taskLayout->clear();
}


void ywQueuePage::refreshQueueList()
{
    const string& ext_prio  =".task_prio";
    const string& ext_normal=".task";
    const string& ext_night =".task_night";

    taskLayout->clear();
    WTimer::singleShot(0, this, &ywQueuePage::clearFailList);

    namespace fs = boost::filesystem;

    // ## Check the work dir
    fs::path procDir(app->configuration->yarraWorkPath.toUTF8());
    fs::directory_iterator work_iter;

    WString procTaskName="";

    try
    {
        if ( fs::exists(procDir) && fs::is_directory(procDir))
        {
            for( fs::directory_iterator dir_iter(procDir) ; dir_iter != work_iter ; ++dir_iter)
            {
                if ( (fs::is_regular_file(dir_iter->status())) &&
                     ( (dir_iter->path().extension()==ext_prio) || (dir_iter->path().extension()==ext_normal) ||
                       (dir_iter->path().extension()==ext_night) )
                   )
                {
                    procTaskName=dir_iter->path().stem().generic_string();
                    break;
                }
            }
        }
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
    }

    // Add item if a currently processed task has been found
    if (!procTaskName.empty())
    {
        WPanel* procPanel=createQueuePanel(procTaskName, MODE_PROC);
        WText* processingLabel=new WText("Processing");
        taskLayout->addWidget(processingLabel,Wt::Left);
        taskLayout->addWidget(procPanel,Wt::AlignMiddle);
        procPanel->setMargin(20, Wt::Bottom);
    }


    // ## Now, check the queue dir
    WText* scheduledLabel=new WText("Scheduled Tasks");
    taskLayout->addWidget(scheduledLabel,Wt::Left);

    // First search for the ".task_prio" files in the queue dir
    fs::path queueDir(app->configuration->yarraQueuePath.toUTF8());
    fs::directory_iterator end_iter;

    typedef std::multimap<std::time_t,  boost::filesystem::path> result_set_t;
    result_set_t result_set;

    try
    {
        if ( fs::exists(queueDir) && fs::is_directory(queueDir))
        {
            for( fs::directory_iterator dir_iter(queueDir) ; dir_iter != end_iter ; ++dir_iter)
            {
                if ( (fs::is_regular_file(dir_iter->status()) && (dir_iter->path().extension()==ext_prio)) )
                {
                    result_set.insert(result_set_t::value_type(fs::last_write_time(dir_iter->path()), *dir_iter));
                }
            }
        }
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
    }

    int itemCount=0;
    // Iterate backwards through the results list
    for (result_set_t::iterator ii=result_set.begin(); ii!=result_set.end(); ++ii)
    {
        WString scanName=WString((*ii).second.stem().generic_string());
        taskLayout->addWidget(createQueuePanel(scanName, MODE_PRIO),Wt::AlignMiddle);
        itemCount++;

        // Only show max 100 entries to avoid slow speed.
        if (itemCount>100)
        {
            break;
        }
    }

    // Second, search for the ".task" and ".task_night" files
    result_set.clear();

    try
    {
        if ( fs::exists(queueDir) && fs::is_directory(queueDir))
        {
            for( fs::directory_iterator dir_iter(queueDir) ; dir_iter != end_iter ; ++dir_iter)
            {
                if ( (fs::is_regular_file(dir_iter->status())) &&
                      ( (dir_iter->path().extension()==ext_normal) || (dir_iter->path().extension()==ext_night) ))
                {
                    result_set.insert(result_set_t::value_type(fs::last_write_time(dir_iter->path()), *dir_iter));
                }
            }
        }
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
    }

    // Iterate backwards through the results list
    for (result_set_t::iterator ii=result_set.begin(); ii!=result_set.end(); ++ii)
    {
        WString scanName=WString((*ii).second.stem().generic_string());
        int mode=MODE_NORMAL;
        if ((*ii).second.extension()==ext_night)
        {
            mode=MODE_NIGHT;
        }

        taskLayout->addWidget(createQueuePanel(scanName, mode),Wt::AlignMiddle);
        itemCount++;

        // Only show max 100 entries to avoid slow speed.
        if (itemCount>100)
        {
            break;
        }
    }

    if (itemCount==0)
    {
        if (procTaskName.empty())
        {
            scheduledLabel->setText("No tasks scheduled currently.");
        }
        else
        {
            scheduledLabel->setText("No additional tasks scheduled.");
        }
    }
}


void ywQueuePage::tabChanged(int)
{
    refreshLists();
}


ywQueuePage* ywQueuePage::createInstance(ywApplication* parent)
{
    return new ywQueuePage(parent);
}
