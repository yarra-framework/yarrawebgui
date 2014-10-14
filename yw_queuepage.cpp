#include "yw_queuepage.h"
#include "yw_application.h"

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WLabel>
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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <boost/exception/error_info.hpp>

#include <iostream>
#include <fstream>


namespace fs = boost::filesystem;



ywQueuePage::ywQueuePage(ywApplication* parent)
 : WContainerWidget()
{
    app=parent;
    queuePath=app->configuration->yarraQueuePath;
    failPath=app->configuration->yarraFailPath;
    workPath=app->configuration->yarraWorkPath;


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


ywQueuePage* ywQueuePage::createInstance(ywApplication* parent)
{
    return new ywQueuePage(parent);
}


void ywQueuePage::tabChanged(int)
{
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
            showInfo(title,mode);
        }));

        if (mode==MODE_FAIL)
        {
            popup->addItem("Restart")->triggered().connect(std::bind([=] () {
                restartTask(title, mode);
            }));
        }

        if ((mode!=MODE_PROC) && (mode!=MODE_FAIL))
        {
            popup->addSeparator();
            if (mode!=MODE_NORMAL)
            {
                popup->addItem("Set mode to NORMAL")->triggered().connect(std::bind([=] () {
                    changePriority(title, mode, MODE_NORMAL);
                }));
            }
            if (mode!=MODE_PRIO)
            {
                popup->addItem("Set mode to PRIORITY")->triggered().connect(std::bind([=] () {
                    changePriority(title, mode, MODE_PRIO);
                }));
            }
            if (mode!=MODE_NIGHT)
            {
                popup->addItem("Set mode to NIGHT")->triggered().connect(std::bind([=] () {
                    changePriority(title, mode, MODE_NIGHT);
                }));
            }
            popup->addSeparator();
            popup->addItem("Change ACC / notification")->triggered().connect(std::bind([=] () {
                patchTask(title, mode);
            }));
        }

        if (mode!=MODE_PROC)
        {
            popup->addItem("Delete")->triggered().connect(std::bind([=] () {
                deleteTask(title, mode);
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
    WString infoText="";
    WString fileName=getFullTaskFileName(taskName, taskType);

    if (taskType==MODE_FAIL)
    {
        // Find the full name of the task file (which might be .task, .task_prio, .task_night)
        fileName=getFailTaskFile(fileName);
    }

    if (taskType==MODE_PROC)
    {
        // Find the full name of the task file in the work directory (which can be .task, .task_prio, .task_night)
        fileName=getWorkTaskFile();
    }


    if (fs::exists(fileName.toUTF8()))
    {
        try
        {
            boost::property_tree::ptree taskfile;
            boost::property_tree::ini_parser::read_ini(fileName.toUTF8(), taskfile);

            infoText="<strong>Patient:</strong> " + WString::fromUTF8(taskfile.get<std::string>("Task.PatientName","Unknown"));
            WString acc=WString::fromUTF8(taskfile.get<std::string>("Task.ACC",""));
            if (!acc.empty())
            {
                infoText+="&nbsp;&nbsp; <strong>ACC#:</strong> "+acc;
            }
            infoText+="<br />";
            infoText+="<strong>Mode:</strong> " + WString::fromUTF8(taskfile.get<std::string>("Task.ReconName","Unknown"));
            WString paramValue=WString::fromUTF8(taskfile.get<std::string>("Task.ParamValue",""));
            if (paramValue!="0")
            {
                infoText+="&nbsp;&nbsp; <strong>Parameter:</strong> "+paramValue;
            }
        }
        catch(const boost::property_tree::ptree_error &e)
        {
            infoText="Error reading task information";
        }
    }
    else
    {
        infoText="Error finding task information";
    }


    taskWidget->setText(infoText);
}



WString ywQueuePage::getFailTaskFile(WString taskName)
{
    const string& ext_prio  =YW_EXT_TASKPRIO;
    const string& ext_normal=YW_EXT_TASK;
    const string& ext_night =YW_EXT_TASKNIGHT;

    fs::path failDir(taskName.toUTF8());
    fs::directory_iterator fail_iter;

    WString failTaskName="";

    try
    {
        if ( fs::exists(failDir) && fs::is_directory(failDir))
        {
            for( fs::directory_iterator dir_iter(failDir) ; dir_iter != fail_iter ; ++dir_iter)
            {
                if ( (fs::is_regular_file(dir_iter->status())) &&
                     ( (dir_iter->path().extension()==ext_prio) || (dir_iter->path().extension()==ext_normal) ||
                       (dir_iter->path().extension()==ext_night) )
                   )
                {
                    failTaskName=dir_iter->path().generic_string();
                    break;
                }
            }
        }
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        failTaskName="";
    }

    return failTaskName;
}


WString ywQueuePage::getWorkTaskFile()
{
    const string& ext_prio  =YW_EXT_TASKPRIO;
    const string& ext_normal=YW_EXT_TASK;
    const string& ext_night =YW_EXT_TASKNIGHT;

    fs::path workDir(workPath.toUTF8());
    fs::directory_iterator work_iter;

    WString workTaskName="";

    try
    {
        if ( fs::exists(workDir) && fs::is_directory(workDir))
        {
            for( fs::directory_iterator dir_iter(workDir) ; dir_iter != work_iter ; ++dir_iter)
            {
                if ( (fs::is_regular_file(dir_iter->status())) &&
                     ( (dir_iter->path().extension()==ext_prio) || (dir_iter->path().extension()==ext_normal) ||
                       (dir_iter->path().extension()==ext_night) )
                   )
                {
                    workTaskName=dir_iter->path().generic_string();
                    break;
                }
            }
        }
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        workTaskName="";
    }

    return workTaskName;
}


void ywQueuePage::showInfo (WString taskName, int mode)
{
    WString infoText="";
    WString fileName=getFullTaskFileName(taskName, mode);

    if (mode==MODE_FAIL)
    {
        // Find the full name of the task file (which might be .task, .task_prio, .task_night)
        fileName=getFailTaskFile(fileName);
    }

    if (fs::exists(fileName.toUTF8()))
    {
        try
        {
            boost::property_tree::ptree taskfile;
            boost::property_tree::ini_parser::read_ini(fileName.toUTF8(), taskfile);

            infoText="<strong>Patient:</strong> " + WString::fromUTF8(taskfile.get<std::string>("Task.PatientName","Unknown"));

            WString acc=WString::fromUTF8(taskfile.get<std::string>("Task.ACC",""));
            if (!acc.empty())
            {
                infoText+="<br /><strong>ACC#:</strong> "+acc;
            }

            infoText+="<br /><strong>Protocol:</strong> "+WString::fromUTF8(taskfile.get<std::string>("Task.ScanProtocol","0"));
            infoText+="<br />";
            infoText+="<br /><strong>Mode:</strong> " + WString::fromUTF8(taskfile.get<std::string>("Task.ReconName","Unknown"));
            infoText+="<br /><strong>Parameter:</strong> " + WString::fromUTF8(taskfile.get<std::string>("Task.ParamValue","0"));
            infoText+="<br /><strong>Notifications:</strong> "+WString::fromUTF8(taskfile.get<std::string>("Task.EMailNotification",""));
            infoText+="<br />";
            infoText+="<br /><strong>System:</strong> "+WString::fromUTF8(taskfile.get<std::string>("Information.SystemName",""));
            infoText+="<br /><strong>Submitted:</strong> "+WString::fromUTF8(taskfile.get<std::string>("Information.TaskDate","")) + "&nbsp;&nbsp;" +
                                                                                WString::fromUTF8(taskfile.get<std::string>("Information.TaskTime",""));
            WString scanSizeStr="";
            int scanSize=taskfile.get<int>("Information.ScanFileSize",0)/(1024*1024);
            if (scanSize<1024)
            {
                scanSizeStr=WString("{1} MB").arg(scanSize);
            }
            else
            {
                scanSize=scanSize/1024;
                scanSizeStr=WString("{1} GB").arg(scanSize);
            }

            infoText+="<br /><strong>Scan Size:</strong> ~"+scanSizeStr;
            infoText+="<br />";

        }
        catch(const boost::property_tree::ptree_error &e)
        {
            infoText="Error reading task information";
        }
    }
    else
    {
        infoText="Error finding task information";
    }


    Wt::WDialog *dialog = new Wt::WDialog("Task Information");

    Wt::WText* textWidget=new Wt::WText();
    Wt::WScrollArea* textScroll =new Wt::WScrollArea(dialog->contents());
    textScroll->setWidget(textWidget);
    textWidget->setWordWrap(false);
    textWidget->setText(infoText);

    Wt::WPushButton *ok = new Wt::WPushButton("OK", dialog->footer());
    ok->setDefault(true);

    ok->clicked().connect(std::bind([=] () {
       delete dialog;
    }));

    dialog->resize(600,440);
    dialog->refresh();
    dialog->setResizable(true);
    dialog->setModal(true);
    dialog->show();
}


void ywQueuePage::refreshFailList()
{
    // Clear both pages to free-up memory
    failtaskLayout->clear();
    WTimer::singleShot(0, this, &ywQueuePage::clearQueueList);

    WText* failedLabel=new WText("Failed Tasks");
    failtaskLayout->addWidget(failedLabel,Wt::Left);


    // Read log directory, except yarra.log
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
    const string& ext_prio  =YW_EXT_TASKPRIO;
    const string& ext_normal=YW_EXT_TASK;
    const string& ext_night =YW_EXT_TASKNIGHT;

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


WString ywQueuePage::getFullTaskFileName(WString taskName, int mode)
{
    switch (mode)
    {
    case MODE_NORMAL:
    default:
        return queuePath+"/"+taskName+YW_EXT_TASK;
        break;
    case MODE_PRIO:
        return queuePath+"/"+taskName+YW_EXT_TASKPRIO;
        break;
    case MODE_NIGHT:
        return queuePath+"/"+taskName+YW_EXT_TASKNIGHT;
        break;
    case MODE_FAIL:
        return failPath+"/"+taskName;
        break;
    case MODE_PROC:
        return workPath;
    }
}


WString ywQueuePage::getTaskFileName(WString taskName, int mode)
{
    switch (mode)
    {
    case MODE_NORMAL:
    default:
        return taskName+YW_EXT_TASK;
        break;
    case MODE_PRIO:
        return taskName+YW_EXT_TASKPRIO;
        break;
    case MODE_NIGHT:
        return taskName+YW_EXT_TASKNIGHT;
        break;
    case MODE_FAIL:
    case MODE_PROC:
        return taskName;
        break;
    }
}


void ywQueuePage::changePriority(WString taskName, int currentPriority, int newPriority)
{
    WString errorMessage="";

    // Create lock file
    if (!lockTask(taskName))
    {
        errorMessage="Could not lock task. Possibly, the task is currently modified by another user.";
        showErrorMessage(errorMessage);
        return;
    }

    // Rename the current task file
    WString currentName=getFullTaskFileName(taskName, currentPriority);
    WString newName=getFullTaskFileName(taskName, newPriority);

    try
    {
        fs::rename(currentName.toUTF8(),newName.toUTF8());
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        errorMessage="Error changing task file.";
        showErrorMessage(errorMessage);
    }

    // Remove lock file
    if (!unlockTask(taskName))
    {
        errorMessage="Could not remove lock file. Please contact the administrator.";
        showErrorMessage(errorMessage);
        return;
    }

    // Launch timer in 100 msec and update status
    WTimer::singleShot(100, this, &ywQueuePage::refreshLists);
}



void ywQueuePage::deleteTask(WString taskName, int mode)
{
    if (mode==MODE_FAIL)
    {
        Wt::WMessageBox box1("Delete Task","<p>Are you sure to delete this task?</p>\
                             <p><strong>Warning:</strong> The data will be lost permanently.</p>", Wt::Question, Wt::No | Wt::Yes);
        box1.setDefaultButton(Wt::No);
        box1.buttonClicked().connect(&box1, &WMessageBox::accept);
        box1.exec();

        if (box1.buttonResult()==Wt::Yes)
        {
            // For failed tasks, we only need to delete all file in the task directory
            // and finally delete the task directory

            WString taskPath=getFullTaskFileName(taskName, MODE_FAIL);

            fs::path taskDir(taskPath.toUTF8());
            fs::directory_iterator end_iter;

            // The multimap should be replaced with a normal STL vector here
            typedef std::multimap<int, boost::filesystem::path> result_set_t;
            result_set_t result_set;

            // Get a list of all files in the task directory
            try
            {
                if ( fs::exists(taskDir) && fs::is_directory(taskDir))
                {
                    for( fs::directory_iterator dir_iter(taskDir) ; dir_iter != end_iter ; ++dir_iter)
                    {
                        if (fs::is_regular_file(dir_iter->status()))
                        {
                            result_set.insert(result_set_t::value_type(0,*dir_iter));
                        }
                    }
                }
            }
            catch(const boost::filesystem::filesystem_error& e)
            {
            }

            bool deleteError=false;

            // First check if there is any file already existing in the queue directory
            for (result_set_t::iterator ii=result_set.begin(); ii!=result_set.end(); ++ii)
            {
                WString deleteFile=taskPath+"/"+WString((*ii).second.filename().generic_string());

                try
                {
                    fs::remove(deleteFile.toUTF8());
                }
                catch(const boost::filesystem::filesystem_error& e)
                {
                    deleteError=true;
                }
            }

            if (deleteError)
            {
                showErrorMessage("Error deleting task files.");
            }
            else
            {
                // Now, remove the empty directory
                try
                {
                    fs::remove(taskPath.toUTF8());
                }
                catch(const boost::filesystem::filesystem_error& e)
                {
                    showErrorMessage("Error removing empty task directory.");
                }
            }

            // Launch timer in 100 msec and update status
            WTimer::singleShot(100, this, &ywQueuePage::refreshLists);
        }
    }
    else
    {
        Wt::WMessageBox box1("Delete Task","<p>Are you sure to delete this task?</p>", Wt::Question, Wt::No | Wt::Yes);
        box1.setDefaultButton(Wt::No);
        box1.buttonClicked().connect(&box1, &WMessageBox::accept);
        box1.exec();

        if (box1.buttonResult()==Wt::Yes)
        {
            Wt::WMessageBox box2("Move to Fail List?","<p>Do you want to keep the task in the fail list?</p>\
                                 <p><strong>Warning:</strong> Without moving the task to the fail list, the data will be lost permanently.</p>",
                                 Wt::Question, Wt::Yes | Wt::No | Wt::Cancel);
            box2.setDefaultButton(Wt::Cancel);
            box2.buttonClicked().connect(&box2, &WMessageBox::accept);
            box2.exec();

            if (box2.buttonResult()==Wt::Yes)
            {
                // NOTE: Check if a folder with the case already exists. If so, add time stamp
                //       according to Yarra UID scheme

                // TODO: Create folder in fail directory and move files there

                // Launch timer in 100 msec and update status
                WTimer::singleShot(100, this, &ywQueuePage::refreshLists);
            }
            if (box2.buttonResult()==Wt::No)
            {
                // Just Delete data files in queue directory without

                // First, create a lockfile in the queue directory
                if (!lockTask(taskName))
                {
                    showErrorMessage("Locking the task not possible.");
                    return;
                }

                // Create list of all files belonging to the task
                WStringList fileList;
                WString taskFileName=getFullTaskFileName(taskName, mode);
                fileList.push_back(getTaskFileName(taskName, mode));
                getAllFilesOfTask(taskFileName, fileList);

                bool deleteError=false;
                for (int i=0; i<fileList.size(); i++)
                {
                    WString currentFile=queuePath+"/"+fileList.at(i);
                    try
                    {
                        fs::remove(currentFile.toUTF8());
                    }
                    catch(const boost::filesystem::filesystem_error& e)
                    {
                        deleteError=true;
                    }
                }
                if (deleteError)
                {
                    showErrorMessage("Error deleting task files from queue.");
                }

                unlockTask(taskName);

                // Launch timer in 100 msec and update status
                WTimer::singleShot(100, this, &ywQueuePage::refreshLists);
            }
        }
    }
}


bool ywQueuePage::getAllFilesOfTask(WString taskFileName, WStringList& fileList)
{
    if (!fs::exists(taskFileName.toUTF8()))
    {
        return false;
    }

    try
    {
        boost::property_tree::ptree taskfile;
        boost::property_tree::ini_parser::read_ini(taskFileName.toUTF8(), taskfile);

        WString scanFile=WString::fromUTF8(taskfile.get<std::string>("Task.ScanFile",""));
        if (scanFile.empty())
        {
            // There always needs to be a scanfile. Otherwise, the task was invalid
            return false;
        }
        fileList.push_back(scanFile);

        int adjustmentFilesCount=taskfile.get<int>("Task.AdjustmentFilesCount",0);

        // Prevent unplausible settings
        if ((adjustmentFilesCount<0) || (adjustmentFilesCount>99))
        {
            adjustmentFilesCount=0;
        }

        for (int i=0; i< adjustmentFilesCount; i++)
        {
            WString currFile=WString::fromUTF8(taskfile.get<std::string>(WString("AdjustmentFiles.{1}").arg(i).toUTF8(),""));
            if (!currFile.empty())
            {
                fileList.push_back(currFile);
            }
        }
    }
    catch(const boost::property_tree::ptree_error &e)
    {
        return false;
    }

    return true;
}


void ywQueuePage::patchTask(WString taskName, int mode)
{
    Wt::WMessageBox::show("Coming soon!", "<p>Not implemented yet.</p>", Wt::Ok);
}


void ywQueuePage::restartTask(WString taskName, int mode)
{   
    Wt::WMessageBox box1("Restart Task","<p>Are you sure to move the task back to the task queue?</p>", Wt::Question, Wt::No | Wt::Yes);
    box1.setDefaultButton(Wt::No);
    box1.buttonClicked().connect(&box1, &WMessageBox::accept);
    box1.exec();

    if (box1.buttonResult()==Wt::Yes)
    {
        WString taskPath=getFullTaskFileName(taskName, MODE_FAIL);

        fs::path taskDir(taskPath.toUTF8());
        fs::directory_iterator end_iter;

        typedef std::multimap<int, boost::filesystem::path> result_set_t;
        result_set_t result_set;

        try
        {
            if ( fs::exists(taskDir) && fs::is_directory(taskDir))
            {
                for( fs::directory_iterator dir_iter(taskDir) ; dir_iter != end_iter ; ++dir_iter)
                {
                    if (fs::is_regular_file(dir_iter->status()))
                    {
                        result_set.insert(result_set_t::value_type(0,*dir_iter));
                    }
                }
            }
        }
        catch(const boost::filesystem::filesystem_error& e)
        {
        }


        bool existingFile=false;

        // First check if there is any file already existing in the queue directory
        for (result_set_t::iterator ii=result_set.begin(); ii!=result_set.end(); ++ii)
        {
            WString scanName=queuePath+"/"+WString((*ii).second.filename().generic_string());

            if (fs::exists(scanName.toUTF8()))
            {
                existingFile=true;
                break;
            }
        }

        if (existingFile)
        {
            showErrorMessage("Some of the task files already exist in the queue directory.");
            return;
        }

        // First, create a lockfile in the queue directory
        if (!lockTask(taskName))
        {
            showErrorMessage("Locking the task not possible.");
            return;
        }

        bool copyError=false;

        WString test="";

        // First check if there is any file already existing in the queue directory
        for (result_set_t::iterator ii=result_set.begin(); ii!=result_set.end(); ++ii)
        {
            WString copyFile=WString((*ii).second.filename().generic_string());
            WString sourceFile=taskPath+"/"+copyFile;
            WString targetFile=queuePath+"/"+copyFile;

            try
            {
                fs::rename(sourceFile.toUTF8(),targetFile.toUTF8());
            }
            catch(const boost::filesystem::filesystem_error& e)
            {
                copyError=true;
            }
        }

        if (copyError)
        {
            showErrorMessage("Error moving task files.");
        }
        else
        {
            // Now, remove the empty directory
            try
            {
                fs::remove(taskPath.toUTF8());
            }
            catch(const boost::filesystem::filesystem_error& e)
            {
                showErrorMessage("Error removing empty task directory.");
            }
        }

        unlockTask(taskName);

        // Launch timer in 100 msec and update status
        WTimer::singleShot(100, this, &ywQueuePage::refreshLists);
    }
}


bool ywQueuePage::isTaskLocked(WString taskName)
{
    WString fullName=queuePath+"/"+taskName+YW_EXT_LOCK;

    if (fs::exists(fullName.toUTF8()))
    {
        return true;
    }

    return false;
}


bool ywQueuePage::lockTask(WString taskName)
{
    WString fullName=queuePath+"/"+taskName+YW_EXT_LOCK;

    // File already locked
    if (fs::exists(fullName.toUTF8()))
    {
        return false;
    }

    std::ofstream lockfile(fullName.toUTF8());
    lockfile << "LOCK" << std::endl;
    lockfile.close();

    return true;
}


bool ywQueuePage::unlockTask(WString taskName)
{
    WString fullName=queuePath+"/"+taskName+YW_EXT_LOCK;

    bool result=false;

    try
    {
        if (!fs::exists(fullName.toUTF8()))
        {
            return true;
        }

        result=(fs::remove(fullName.toUTF8()));
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        result=false;
    }

    return result;
}


void ywQueuePage::showErrorMessage(WString errorMessage)
{
    Wt::WMessageBox *messageBox = new Wt::WMessageBox
       ("Error",
        "<p>Unfortunately, the operation failed due to the following reason:</p>"
        "<p>"+errorMessage+"</p>",
        Wt::Critical, Wt::Ok);

    messageBox->setModal(true);
    messageBox->buttonClicked().connect(std::bind([=] () {
        delete messageBox;
        WTimer::singleShot(100, this, &ywQueuePage::refreshLists);
    }));
    messageBox->show();
}


