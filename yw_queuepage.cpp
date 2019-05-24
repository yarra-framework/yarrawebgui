#include "yw_queuepage.h"
#include "yw_application.h"
#include "yw_helper.h"

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
#include <Wt/WRegExpValidator>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <boost/exception/error_info.hpp>

#include <iostream>
#include <fstream>
#include <ctime>
#include <time.h>


namespace fs = boost::filesystem;



ywQueuePage::ywQueuePage(ywApplication* parent)
 : WContainerWidget()
{
    app=parent;
    queuePath  =app->configuration->yarraQueuePath;
    failPath   =app->configuration->yarraFailPath;
    workPath   =app->configuration->yarraWorkPath;
    storagePath=app->configuration->yarraStoragePath;
    resumePath =app->configuration->yarraResumePath;

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

    // Finished List container
    WContainerWidget* finishedContainer=new WContainerWidget();
    Wt::WVBoxLayout*  finishedLayout=new Wt::WVBoxLayout();
    finishedLayout->setContentsMargins(0, 0, 0, 0);
    finishedContainer->setLayout(finishedLayout);

    Wt::WContainerWidget* finishedcenterContainer = new Wt::WContainerWidget();
    Wt::WHBoxLayout* finishedcenterLayout = new Wt::WHBoxLayout();
    finishedcenterLayout->setContentsMargins(0, 20, 0, 0);
    finishedcenterLayout->setSpacing(0);
    finishedcenterContainer->setLayout(finishedcenterLayout);

    Wt::WContainerWidget* finishedtaskContainer = new Wt::WContainerWidget();
    finishedtaskLayout = new Wt::WVBoxLayout();
    finishedtaskLayout->setContentsMargins(0, 0, 0, 0);
    finishedtaskLayout->setSpacing(4);
    finishedtaskContainer->setLayout(finishedtaskLayout);

    Wt::WScrollArea* finishedscrollArea=new Wt::WScrollArea();
    finishedscrollArea->setWidget(finishedcenterContainer);
    finishedcenterLayout->addWidget(finishedtaskContainer, 0, Wt::AlignCenter);
    finishedLayout->addWidget(finishedscrollArea);

    // Resume List container
    WContainerWidget* resumedContainer=0;
    Wt::WContainerWidget* resumedtaskContainer=0;

    if (app->configuration->yarraEnableResume)
    {
        resumedContainer = new WContainerWidget();
        Wt::WVBoxLayout*  resumedLayout = new Wt::WVBoxLayout();
        resumedLayout->setContentsMargins(0, 0, 0, 0);
        resumedContainer->setLayout(resumedLayout);

        Wt::WContainerWidget* resumedcenterContainer = new Wt::WContainerWidget();
        Wt::WHBoxLayout* resumedcenterLayout = new Wt::WHBoxLayout();
        resumedcenterLayout->setContentsMargins(0, 20, 0, 0);
        resumedcenterLayout->setSpacing(0);
        resumedcenterContainer->setLayout(resumedcenterLayout);

        resumedtaskContainer = new Wt::WContainerWidget();
        resumedtaskLayout = new Wt::WVBoxLayout();
        resumedtaskLayout->setContentsMargins(0, 0, 0, 0);
        resumedtaskLayout->setSpacing(4);
        resumedtaskContainer->setLayout(resumedtaskLayout);

        Wt::WScrollArea* resumedscrollArea=new Wt::WScrollArea();
        resumedscrollArea->setWidget(resumedcenterContainer);
        resumedcenterLayout->addWidget(resumedtaskContainer, 0, Wt::AlignCenter);
        resumedLayout->addWidget(resumedscrollArea);
    }

    tabIndexQueue   =-1;
    tabIndexResumed =-1;
    tabIndexFail    =-1;
    tabIndexFinished=-1;

    // Tab widget
    tabWidget = new Wt::WTabWidget();

    tabWidget->addTab(queueContainer,      "Running & Scheduled", Wt::WTabWidget::PreLoading);
    tabIndexQueue=tabWidget->count()-1;

    if (app->configuration->yarraEnableResume)
    {
        tabWidget->addTab(resumedContainer,"Suspended",           Wt::WTabWidget::PreLoading);
        tabIndexResumed=tabWidget->count()-1;
    }

    tabWidget->addTab(finishedContainer,   "Succeeded",           Wt::WTabWidget::PreLoading);
    tabIndexFinished=tabWidget->count()-1;

    tabWidget->addTab(failContainer,       "Failed",              Wt::WTabWidget::PreLoading);
    tabIndexFail=tabWidget->count()-1;

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

    Wt::WText* heading=new Wt::WText("<h3>Task Queue</h3>");
    heading->setMargin(6, Wt::Bottom);
    queuePageLayout->addWidget(heading);
    queuePageLayout->addWidget(tabWidget,1);
    queuePageLayout->addWidget(btnContainer);

    taskContainer->resize(800,Wt::WLength::Auto);
    taskContainer->setMinimumSize(600,Wt::WLength::Auto);

    failtaskContainer->resize(800,Wt::WLength::Auto);
    failtaskContainer->setMinimumSize(600,Wt::WLength::Auto);
    failtaskContainer->setMaximumSize(800,Wt::WLength::Auto);

    finishedtaskContainer->resize(800,Wt::WLength::Auto);
    finishedtaskContainer->setMinimumSize(600,Wt::WLength::Auto);
    finishedtaskContainer->setMaximumSize(800,Wt::WLength::Auto);

    if (app->configuration->yarraEnableResume)
    {
        resumedtaskContainer->resize(800,Wt::WLength::Auto);
        resumedtaskContainer->setMinimumSize(600,Wt::WLength::Auto);
        resumedtaskContainer->setMaximumSize(800,Wt::WLength::Auto);
    }

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
    if (tabWidget->currentIndex()==tabIndexQueue)
    {
        refreshQueueList();
    }
    if (tabWidget->currentIndex()==tabIndexFinished)
    {
        refreshFinishedList();
    }
    if (tabWidget->currentIndex()==tabIndexFail)
    {
        refreshFailList();
    }
    if (tabWidget->currentIndex()==tabIndexResumed)
    {
        refreshResumedList();
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
    case MODE_FINISHED:
        panel->addStyleClass("panelqueue panel-finished");
        break;
    case MODE_RESUMED:
        panel->addStyleClass("panelqueue panel-resumed");
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

        if ((mode==MODE_FAIL) || (mode==MODE_FINISHED))
        {            
            popup->addSeparator();
            popup->addItem("Restart")->triggered().connect(std::bind([=] () {
                restartTask(title, mode);
            }));
        }

        if (mode==MODE_RESUMED)
        {
            popup->addSeparator();
            popup->addItem("Resume without delay")->triggered().connect(std::bind([=] () {
                clearResumeDelay(title);
            }));
            popup->addItem("Pause / Unpause")->triggered().connect(std::bind([=] () {
                pauseResumeTask(title);
            }));
            popup->addSeparator();
        }

        if ((mode!=MODE_PROC) && (mode!=MODE_FAIL) && (mode!=MODE_FINISHED)
             && (mode!=MODE_RESUMED))
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
            popup->addItem("Push back")->triggered().connect(std::bind([=] () {
                pushbackTask(title, mode);
            }));
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

            if ((app->currentLevel==ywApplication::YW_USERLEVEL_ADMIN) && (mode!=MODE_RESUMED))
            {
                popup->addSeparator();
                popup->addItem("Edit task file")->triggered().connect(std::bind([=] () {
                    editTaskFile(title, mode);
                }));
            }
        }

        WPushButton* button = new Wt::WPushButton("Edit");
        button->setMenu(popup);
        button->addStyleClass("btn btn-primary btn-xs");
        btnLayout->addWidget(button,0, Wt::AlignLeft);
    }

    return panel;
}


void ywQueuePage::pauseResumeTask(WString taskName)
{
    WString folderName=resumePath+"/"+taskName;
    WString resumeFilename=getFolderResumeFile(folderName);

    if (resumeFilename.empty())
    {
        showErrorMessage("Resume information is missing.");
        return;
    }
    if (ywHelper::isFolderLocked(folderName))
    {
        showErrorMessage("Task is currently in use (locked).");
        return;
    }
    if (!ywHelper::lockFile(folderName+"/resume.lock"))
    {
        showErrorMessage("Unable to lock task.");
        return;
    }

    try
    {
        boost::property_tree::ptree resumefile;
        boost::property_tree::ini_parser::read_ini(resumeFilename.toUTF8(), resumefile);

        // Read current state and flip
        bool pauseValue=resumefile.get<bool>("Information.Paused",false);
        resumefile.put("Information.Paused", !pauseValue);

        boost::property_tree::ini_parser::write_ini(resumeFilename.toUTF8(), resumefile);
    }
    catch(const boost::property_tree::ptree_error &e)
    {
    }

    ywHelper::unlockFile(folderName+"/resume.lock");
    refreshResumedList();
}


void ywQueuePage::clearResumeDelay(WString taskName)
{
    WString folderName=resumePath+"/"+taskName;
    WString resumeFilename=getFolderResumeFile(folderName);

    if (resumeFilename.empty())
    {
        showErrorMessage("Resume information is missing.");
        return;
    }
    if (ywHelper::isFolderLocked(folderName))
    {
        showErrorMessage("Task is currently in use (locked).");
        return;
    }
    if (!ywHelper::lockFile(folderName+"/resume.lock"))
    {
        showErrorMessage("Unable to lock task.");
        return;
    }

    try
    {
        boost::property_tree::ptree resumefile;
        boost::property_tree::ini_parser::read_ini(resumeFilename.toUTF8(), resumefile);
        resumefile.put("Information.NextRetry", "");
        boost::property_tree::ini_parser::write_ini(resumeFilename.toUTF8(), resumefile);
    }
    catch(const boost::property_tree::ptree_error &e)
    {
    }

    ywHelper::unlockFile(folderName+"/resume.lock");
    refreshResumedList();
}


void ywQueuePage::updateTaskInformation(WString taskName, WText* taskWidget,int taskType)
{
    WString infoText="";
    WString fileName=getFullTaskFileName(taskName, taskType);

    if ((taskType==MODE_FAIL) || (taskType==MODE_FINISHED) || (taskType==MODE_RESUMED))
    {
        // Find the full name of the task file (which might be .task, .task_prio, .task_night)
        fileName=getFolderTaskFile(fileName);
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

            if (taskType==MODE_RESUMED)
            {
                infoText+=getResumedTaskInformation(taskName);
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


WString ywQueuePage::getResumedTaskInformation(WString taskName)
{
    WString information="<br /><strong>Status:</strong> ";

    WString folderName=resumePath+"/"+taskName;
    WString resumeFilename=getFolderResumeFile(folderName);

    if (resumeFilename.empty())
    {
        information += "Information missing";
        return information;
    }

    WString resumeStatus="";
    WString resumeNextTry="";
    WString resumeRetries="";

    try
    {
        boost::property_tree::ptree resumefile;
        boost::property_tree::ini_parser::read_ini(resumeFilename.toUTF8(), resumefile);

        int state=resumefile.get<int>("Information.State",0);

        resumeStatus+="Failed during ";
        switch (state)
        {
        case 4:
            resumeStatus+="PostProcessing";
            break;
        case 5:
            resumeStatus+="Transfer";
            break;
        default:
            resumeStatus+="Unknown";
            break;
        }

        resumeNextTry=WString::fromUTF8(resumefile.get<std::string>("Information.NextRetry",""));
        if (resumeNextTry.empty())
        {
            resumeNextTry="ASAP";
        }

        resumeRetries=WString::fromUTF8(resumefile.get<std::string>("Information.Retries",""));

        bool paused=resumefile.get<bool>("Information.Paused",false);
        if (paused)
        {
            resumeNextTry += "&nbsp;<span class=\"label label-warning\">PAUSED</span>";
        }
    }
    catch(const boost::property_tree::ptree_error &e)
    {
        information += "Error reading information";
        return information;
    }

    information += resumeStatus;
    information += "<br /><strong>Attempts:</strong> "+resumeRetries;
    information += "<br /><strong>Next Retry:</strong> "+resumeNextTry;

    return information;
}


WString ywQueuePage::getFolderTaskFile(WString taskName)
{
    const string& ext_prio  =YW_EXT_TASKPRIO;
    const string& ext_normal=YW_EXT_TASK;
    const string& ext_night =YW_EXT_TASKNIGHT;

    fs::path folderDir(taskName.toUTF8());
    fs::directory_iterator folder_iter;

    WString folderTaskName="";

    try
    {
        if ( fs::exists(folderDir) && fs::is_directory(folderDir))
        {
            for( fs::directory_iterator dir_iter(folderDir) ; dir_iter != folder_iter ; ++dir_iter)
            {
                if ( (fs::is_regular_file(dir_iter->status())) &&
                     ( (dir_iter->path().extension()==ext_prio) || (dir_iter->path().extension()==ext_normal) ||
                       (dir_iter->path().extension()==ext_night) )
                   )
                {
                    folderTaskName=dir_iter->path().generic_string();
                    break;
                }
            }
        }
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        folderTaskName="";
    }

    return folderTaskName;
}


WString ywQueuePage::getFolderResumeFile(WString taskName)
{
    const string& ext_resume=YW_EXT_RESUME;

    fs::path folderDir(taskName.toUTF8());
    fs::directory_iterator folder_iter;

    WString folderResumeFilename="";

    try
    {
        if ( fs::exists(folderDir) && fs::is_directory(folderDir))
        {
            for( fs::directory_iterator dir_iter(folderDir) ; dir_iter != folder_iter ; ++dir_iter)
            {
                if ( (fs::is_regular_file(dir_iter->status())) && (dir_iter->path().extension()==ext_resume) )
                {
                    folderResumeFilename=dir_iter->path().generic_string();
                    break;
                }
            }
        }
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        folderResumeFilename="";
    }

    return folderResumeFilename;
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


void ywQueuePage::showInfo(WString taskName, int mode)
{
    WString infoText="";
    WString fileName=getFullTaskFileName(taskName, mode);

    if ((mode==MODE_FAIL) || (mode==MODE_FINISHED) || (mode==MODE_RESUMED))
    {
        // Find the full name of the task file (which might be .task, .task_prio, .task_night)
        fileName=getFolderTaskFile(fileName);
    }

    if (mode==MODE_PROC)
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
            long scanSize=taskfile.get<long>("Information.ScanFileSize",0)/(1024*1024);
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
    WTimer::singleShot(0, this, &ywQueuePage::clearFinishedList);
    WTimer::singleShot(0, this, &ywQueuePage::clearResumedList);

    WText* failedLabel=new WText("Failed Tasks");
    failtaskLayout->addWidget(failedLabel,Wt::Left);

    fs::path failDir(app->configuration->yarraFailPath.toUTF8());
    fs::directory_iterator end_iter;

    typedef std::multimap<std::time_t,  boost::filesystem::path> result_set_t;
    result_set_t result_set;

    try
    {
        if (fs::exists(failDir) && fs::is_directory(failDir))
        {
            for (fs::directory_iterator dir_iter(failDir) ; dir_iter != end_iter ; ++dir_iter)
            {
                if (fs::is_directory(dir_iter->status()))
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


void ywQueuePage::refreshFinishedList()
{
    // Clear both pages to free-up memory
    finishedtaskLayout->clear();
    WTimer::singleShot(0, this, &ywQueuePage::clearQueueList);
    WTimer::singleShot(0, this, &ywQueuePage::clearFailList);
    WTimer::singleShot(0, this, &ywQueuePage::clearResumedList);

    WText* finishedLabel=new WText("Finished Tasks");
    finishedtaskLayout->addWidget(finishedLabel,Wt::Left);

    fs::path finishedDir(app->configuration->yarraStoragePath.toUTF8());
    fs::directory_iterator end_iter;

    typedef std::multimap<std::time_t,  boost::filesystem::path> result_set_t;
    result_set_t result_set;

    try
    {
        if (fs::exists(finishedDir) && fs::is_directory(finishedDir))
        {
            for(fs::directory_iterator dir_iter(finishedDir) ; dir_iter != end_iter ; ++dir_iter)
            {
                if (fs::is_directory(dir_iter->status()))
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
        finishedtaskLayout->addWidget(createQueuePanel(dirName, MODE_FINISHED),Wt::AlignMiddle);
        i++;

        // Only show max 100 entries to avoid slow speed.
        if (i>100)
        {
            break;
        }
    }

    if (i==0)
    {
        finishedLabel->setText("No succeeded tasks found.");
    }
}


void ywQueuePage::clearFailList()
{
    failtaskLayout->clear();
}


void ywQueuePage::clearFinishedList()
{
    finishedtaskLayout->clear();
}


void ywQueuePage::clearQueueList()
{
    taskLayout->clear();
}


void ywQueuePage::clearResumedList()
{
    if (app->configuration->yarraEnableResume)
    {
        resumedtaskLayout->clear();
    }
}


void ywQueuePage::refreshQueueList()
{
    const string& ext_prio  =YW_EXT_TASKPRIO;
    const string& ext_normal=YW_EXT_TASK;
    const string& ext_night =YW_EXT_TASKNIGHT;

    taskLayout->clear();
    WTimer::singleShot(0, this, &ywQueuePage::clearFailList);
    WTimer::singleShot(0, this, &ywQueuePage::clearFinishedList);
    WTimer::singleShot(0, this, &ywQueuePage::clearResumedList);

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
            scheduledLabel->setText("No tasks running or scheduled currently.");
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
    case MODE_FINISHED:
        return storagePath+"/"+taskName;
        break;
    case MODE_PROC:
        return workPath;
        break;
    case MODE_RESUMED:
        return resumePath+"/"+taskName;
        break;
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
    case MODE_PROC:
    case MODE_FAIL:
    case MODE_FINISHED:
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
    if ((mode==MODE_FAIL) || (mode==MODE_FINISHED) || (mode==MODE_RESUMED))
    {
        Wt::WMessageBox box1("Delete Task","<p>Are you sure to delete this task?</p>\
                             <p><strong>Warning:</strong> The data will be lost permanently.</p>", Wt::Question, Wt::No | Wt::Yes);
        box1.setDefaultButton(Wt::No);
        box1.buttonClicked().connect(&box1, &WMessageBox::accept);
        box1.exec();

        if (box1.buttonResult()==Wt::Yes)
        {
            // For failed/finished/resume tasks, we only need to delete all file in the task directory
            // and finally delete the task directory

            WString taskPath=getFullTaskFileName(taskName, mode);

            // For resume tasks, we need to create a lock file!
            if (mode==MODE_RESUMED)
            {
                Wt::WMessageBox box2("Move to Fail List?","<p>Do you want to keep the task in the fail list?</p>\
                                     <p><strong>Warning:</strong> Without moving the task to the fail list, the data will be lost permanently.</p>",
                                     Wt::Question, Wt::Yes | Wt::No | Wt::Cancel);
                box2.setDefaultButton(Wt::Yes);
                box2.buttonClicked().connect(&box2, &WMessageBox::accept);
                box2.exec();

                if (box2.buttonResult()==Wt::Cancel)
                {
                    return;
                }

                if (ywHelper::isFolderLocked(taskPath))
                {
                    showErrorMessage("Task is currently in use (locked).");
                    return;
                }
                if (!ywHelper::lockFile(taskPath+"/resume.lock"))
                {
                    showErrorMessage("Unable to lock task.");
                    return;
                }

                if (box2.buttonResult()==Wt::Yes)
                {
                    // TODO: Move the scan files to the fail folder
                    if (!moveResumedtoFailed(taskPath))
                    {
                        showErrorMessage("Error while moving task to failed folder. Files will remain in /resume folder.");

                        if (!ywHelper::unlockFile(taskPath+"/resume.lock"))
                        {
                            showErrorMessage("Unable to unlock task.");
                        }

                        return;
                    }
                }
            }

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
                        if (fs::is_regular_file(dir_iter->status()) || fs::is_directory(dir_iter->status()))
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

            // Delete all items inside the task directory
            for (result_set_t::iterator ii=result_set.begin(); ii!=result_set.end(); ++ii)
            {
                WString deleteFile=taskPath+"/"+WString((*ii).second.filename().generic_string());

                try
                {
                    fs::remove_all(deleteFile.toUTF8());
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
            box2.setDefaultButton(Wt::Yes);
            box2.buttonClicked().connect(&box2, &WMessageBox::accept);
            box2.exec();

            if (box2.buttonResult()==Wt::Yes)
            {
                // Create folder in fail directory and move files there

                // First, create a lockfile in the queue directory
                if (!lockTask(taskName))
                {
                    showErrorMessage("Locking the task not possible.");
                    return;
                }

                // NOTE: Check if a folder with the case already exists. If so, add time stamp
                //       according to Yarra UID scheme

                WString folderName=failPath+"/"+taskName;

                // If the folder already exists, create a unique ID and append it to the
                // taskname. This folder should never exist.
                if (fs::exists(folderName.toUTF8()))
                {
                    // Get the time stamp (including ms)
                    struct timeval  beg;
                    struct tm      *t;
                    gettimeofday( &beg, NULL );
                    t = localtime( &beg.tv_sec );
                    char tbuf[20];
                    sprintf(tbuf, "%02d%02d%02d%03d", t->tm_hour, t->tm_min, t->tm_sec, ((int) beg.tv_usec)/1000);

                    // Get the date stamp
                    time_t rawtime;
                    struct tm * timeinfo;
                    time (&rawtime);
                    timeinfo = localtime(&rawtime);
                    char dbuf[20];
                    strftime(dbuf,20,"%d%m%y",timeinfo);

                    WString uniqueID=WString::fromUTF8(dbuf)+WString::fromUTF8(tbuf);
                    folderName+="_"+uniqueID;
                }

                if (fs::exists(folderName.toUTF8()))
                {
                    // If also the folder with time stamp already exists, then we have a problem.
                    showErrorMessage("Unable to find unique name in fail path.");
                    unlockTask(taskName);
                    return;
                }

                bool error=false;
                try
                {
                    fs::create_directory(folderName.toUTF8());
                }
                catch(const boost::filesystem::filesystem_error& e)
                {
                    showErrorMessage("Unable to create unique folder in fail path.");
                    error=true;
                }

                if (!error)
                {
                    // Create list of all files belonging to the task
                    WStringList fileList;
                    WString taskFileName=getFullTaskFileName(taskName, mode);
                    fileList.push_back(getTaskFileName(taskName, mode));
                    getAllFilesOfTask(taskFileName, fileList);

                    for (int i=0; i<fileList.size(); i++)
                    {
                        WString copyFile=fileList.at(i);
                        WString sourceFile=queuePath+"/"+copyFile;
                        WString targetFile=folderName+"/"+copyFile;

                        try
                        {
                            fs::rename(sourceFile.toUTF8(),targetFile.toUTF8());
                        }
                        catch(const boost::filesystem::filesystem_error& e)
                        {
                            error=true;
                        }
                    }
                    if (error)
                    {
                        showErrorMessage("Error moving files to fail queue.");
                    }
                }

                // Delete the lock file
                unlockTask(taskName);

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


bool ywQueuePage::moveResumedtoFailed(WString resumedPath)
{
    WStringList fileList;
    WString     taskFilename=getFolderTaskFile(resumedPath);
    fs::path    taskFile(taskFilename.toUTF8());
    WString     taskName=WString(taskFile.stem().generic_string());

    // Remove the path from the full task filename and append to list
    fileList.push_back(WString(taskFile.filename().generic_string()));

    if (!getAllFilesOfTask(taskFilename, fileList))
    {
        return false;
    }

    WString  folderName=failPath+"/"+taskName;

    // If the folder already exists, create a unique ID and append it to the
    // taskname. This folder should never exist.
    if (fs::exists(folderName.toUTF8()))
    {
        // Get the time stamp (including ms)
        struct timeval  beg;
        struct tm      *t;
        gettimeofday( &beg, NULL );
        t = localtime( &beg.tv_sec );
        char tbuf[20];
        sprintf(tbuf, "%02d%02d%02d%03d", t->tm_hour, t->tm_min, t->tm_sec, ((int) beg.tv_usec)/1000);

        // Get the date stamp
        time_t rawtime;
        struct tm * timeinfo;
        time (&rawtime);
        timeinfo = localtime(&rawtime);
        char dbuf[20];
        strftime(dbuf,20,"%d%m%y",timeinfo);

        WString uniqueID=WString::fromUTF8(dbuf)+WString::fromUTF8(tbuf);
        folderName+="_"+uniqueID;
    }

    if (fs::exists(folderName.toUTF8()))
    {
        // If also the folder with time stamp already exists, then we have a problem.
        return false;
    }

    try
    {
        fs::create_directory(folderName.toUTF8());
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        return false;
    }

    // Now move all files listed in the task file to the fail folder.
    // The remaining files (created during the recon process) will be
    // deleted by the parent function.
    for (int i=0; i<fileList.size(); i++)
    {
        WString copyFile=fileList.at(i);
        WString sourceFile=resumedPath+"/"+copyFile;
        WString targetFile=folderName+"/"+copyFile;

        try
        {
            fs::rename(sourceFile.toUTF8(),targetFile.toUTF8());
        }
        catch(const boost::filesystem::filesystem_error& e)
        {
            return false;
        }
    }

    return true;
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
    WString accValue="";
    WString notificationsValue="";

    // First, try to read the currently existing settings
    WString fileName=getFullTaskFileName(taskName, mode);
    WString errorText="";
    if (fs::exists(fileName.toUTF8()))
    {
        try
        {
            boost::property_tree::ptree taskfile;
            boost::property_tree::ini_parser::read_ini(fileName.toUTF8(), taskfile);

            accValue=WString::fromUTF8(taskfile.get<std::string>("Task.ACC",""));
            notificationsValue=WString::fromUTF8(taskfile.get<std::string>("Task.EMailNotification",""));
        }
        catch(const boost::property_tree::ptree_error &e)
        {
            errorText="Error reading task information";
        }
    }
    else
    {
        errorText="Error finding task information";
    }

    // Abort operation if reading the existing values was not successful
    if (!errorText.empty())
    {
        showErrorMessage(errorText);
        return;
    }

    // Remove enclosing quotation marks, as created by QT for several items
    std::string notificationMod=notificationsValue.toUTF8();
    if (notificationMod[0]=='"')
    {
        notificationMod.erase(0, 1);
    }
    if (notificationMod[notificationMod.length()-1]=='"')
    {
        notificationMod.erase(notificationMod.length()-1, 1);
    }
    notificationsValue=WString::fromUTF8(notificationMod);

    Wt::WDialog *patchDialog = new Wt::WDialog("Modify Task");

    Wt::WPushButton *ok = new Wt::WPushButton("OK", patchDialog->footer());
    ok->setDefault(true);

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", patchDialog->footer());
    cancel->setDefault(false);
    cancel->clicked().connect(patchDialog, &Wt::WDialog::reject);

    Wt::WLabel *accLabel = new Wt::WLabel("ACC#:", patchDialog->contents());
    Wt::WLineEdit *accEdit = new Wt::WLineEdit(patchDialog->contents());
    accLabel->setBuddy(accEdit);
    accEdit->setText(accValue);

    Wt::WRegExpValidator *accValidator=new Wt::WRegExpValidator("[a-zA-Z0-9]{0,16}");
    accValidator->setMandatory(true);
    accEdit->setValidator(accValidator);

    accEdit->keyWentUp().connect(std::bind([=] () {
        ok->setDisabled(accEdit->validate() != Wt::WValidator::Valid);
    }));

    ok->clicked().connect(std::bind([=] () {
        if (accEdit->validate())
        {
            patchDialog->accept();
        }
    }));

    Wt::WLabel *notificationLabel = new Wt::WLabel("Notifications:", patchDialog->contents());
    notificationLabel->setMargin(10, Wt::Top);
    Wt::WLineEdit *notificationEdit = new Wt::WLineEdit(patchDialog->contents());
    notificationLabel->setBuddy(notificationEdit);
    notificationEdit->setText(notificationsValue);

    patchDialog->rejectWhenEscapePressed();
    patchDialog->setModal(true);

    // Process the dialog result.
    patchDialog->finished().connect(std::bind([=] () {
        if (patchDialog->result()==Wt::WDialog::Accepted)
        {
            doPatchTask(taskName, mode, accEdit->text(), notificationEdit->text());
        }
        delete patchDialog;
    }));

    patchDialog->resize(600,Wt::WLength::Auto);
    patchDialog->refresh();
    patchDialog->show();
}


void ywQueuePage::doPatchTask(WString taskName, int mode, WString newACC, WString newNotifications)
{
    WString taskFilename=getFullTaskFileName(taskName, mode);

    // First, create a lockfile in the queue directory
    if (!lockTask(taskName))
    {
        showErrorMessage("Locking the task not possible.");
        return;
    }

    // Save the last modification time (so that the processing order is preserved)
    std::time_t modtime = std::time(0);
    try
    {
        fs::path p(taskFilename.toUTF8());
        modtime=fs::last_write_time(p);
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        showErrorMessage("Unable to retrieve task modification time. Check Samba configuration.");
    }

    // First, try to read the currently existing settings
    bool error=false;
    if (fs::exists(taskFilename.toUTF8()))
    {
        try
        {
            boost::property_tree::ptree taskfile;
            boost::property_tree::ini_parser::read_ini(taskFilename.toUTF8(), taskfile);

            taskfile.put("Task.ACC", newACC.toUTF8());

            if (newNotifications.toUTF8().find(',')!=string::npos)
            {
                newNotifications="\""+newNotifications+"\"";
            }
            taskfile.put("Task.EMailNotification", newNotifications.toUTF8());

            boost::property_tree::ini_parser::write_ini(taskFilename.toUTF8(), taskfile);
        }
        catch(const boost::property_tree::ptree_error &e)
        {
            error=true;
            showErrorMessage("Error changing task information.");
        }
    }
    else
    {
        error=true;
        showErrorMessage("Error finding task information.");
    }

    // Now recover the previous file modification time (to preserve the processsing order)
    try
    {
        fs::path p(taskFilename.toUTF8());
        fs::last_write_time(p,modtime);
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        showErrorMessage("Reverting task modification time not possible. Check your Samba configuration.");
    }

    unlockTask(taskName);

    // Launch timer in 100 msec and update status
    WTimer::singleShot(100, this, &ywQueuePage::refreshLists);
}


void ywQueuePage::pushbackTask(WString taskName, int mode)
{
    WString taskFilename=getFullTaskFileName(taskName, mode);

    if (!fs::exists(taskFilename.toUTF8()))
    {
        showErrorMessage("Could not find task.");
        return;
    }

    // First, create a lockfile in the queue directory
    if (!lockTask(taskName))
    {
        showErrorMessage("Locking the task not possible.");
        return;
    }

    // Get current time
    std::time_t n = std::time(0);

    try
    {
        fs::path p(taskFilename.toUTF8());
        fs::last_write_time(p,n);
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        showErrorMessage("Pushing back task not possible. Check your Samba configuration.");
    }

    unlockTask(taskName);

    // Launch timer in 100 msec and update status
    WTimer::singleShot(100, this, &ywQueuePage::refreshLists);
}


void ywQueuePage::restartTask(WString taskName, int mode)
{   
    Wt::WMessageBox box1("Restart Task","<p>Are you sure to move the task back to the task queue?</p>", Wt::Question, Wt::No | Wt::Yes);
    box1.setDefaultButton(Wt::No);
    box1.buttonClicked().connect(&box1, &WMessageBox::accept);
    box1.exec();

    if (box1.buttonResult()==Wt::Yes)
    {
        WString taskPath=getFullTaskFileName(taskName, mode);

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


void ywQueuePage::editTaskFile(WString taskName, int mode)
{
    WString fileName=getFullTaskFileName(taskName, mode);

    if ((mode==MODE_FAIL) || (mode==MODE_FINISHED))
    {
        // Find the full name of the task file (which might be .task, .task_prio, .task_night)
        fileName=getFolderTaskFile(fileName);
    }

    if (!fs::exists(fileName.toUTF8()))
    {
        showErrorMessage("Error finding task information");
        return;
    }

    // Read the task file into a WString
    std::string line;
    std::ifstream taskfile(fileName.toUTF8());
    WString taskContent="";

    if(!taskfile)
    {
        showErrorMessage("Unable to read task file");
        return;
    }
    else
    {
        while (std::getline(taskfile, line))
        {
            taskContent+=WString::fromUTF8(line)+"\n";
        }
    }
    taskfile.close();

    Wt::WDialog *editDialog = new Wt::WDialog("Edit Task File");

    Wt::WPushButton *save = new Wt::WPushButton("Save", editDialog->footer());
    save->setDefault(true);
    save->clicked().connect(editDialog, &Wt::WDialog::accept);

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", editDialog->footer());
    cancel->setDefault(false);
    cancel->clicked().connect(editDialog, &Wt::WDialog::reject);

    Wt::WVBoxLayout* contentLayout=new Wt::WVBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);

    Wt::WTextArea *editor = new Wt::WTextArea();
    editor->setText(taskContent);
    contentLayout->insertWidget(0,editor);
    editor->setJavaScriptMember("spellcheck","false");

    editDialog->contents()->setLayout(contentLayout);

    editDialog->rejectWhenEscapePressed();
    editDialog->setModal(true);
    editDialog->setResizable(true);

    // Process the dialog result.
    editDialog->finished().connect(std::bind([=] () {
        if (editDialog->result()==Wt::WDialog::Accepted)
        {
            // Call subfunction to write the changes
            doEditTask(taskName, mode, editor->text());
        }
        delete editDialog;
    }));

    editDialog->resize(700,500);
    editDialog->refresh();
    editDialog->show();
}


void ywQueuePage::doEditTask(WString taskName, int mode, WString newContent)
{
    WString fileName=getFullTaskFileName(taskName, mode);

    if ((mode==MODE_FAIL) || (mode==MODE_FINISHED))
    {
        // Find the full name of the task file (which might be .task, .task_prio, .task_night)
        fileName=getFolderTaskFile(fileName);
    }

    if (!fs::exists(fileName.toUTF8()))
    {
        showErrorMessage("Cannot find task information anymore.");
        return;
    }

    // TODO: Implement lock mechanism for file in fail or finished directory
    //       via generic lock mechanism (for any file)
    if ((mode!=MODE_FAIL) && (mode!=MODE_FINISHED))
    {
        // First, create a lockfile in the queue directory
        if (!lockTask(taskName))
        {
            showErrorMessage("Locking the task not possible.");
            return;
        }
    }

    // Write the content to the file
    std::ofstream taskFile(fileName.toUTF8(), std::ofstream::out | std::ofstream::trunc);
    taskFile << newContent.toUTF8();
    taskFile.flush();
    taskFile.close();

    // TODO: Implement lock mechanism for file in fail directory
    //       via generic lock mechanism (for any file)
    if ((mode!=MODE_FAIL) && (mode!=MODE_FINISHED))
    {
        unlockTask(taskName);
    }

    // Launch timer in 100 msec and update status
    WTimer::singleShot(100, this, &ywQueuePage::refreshLists);
}


void ywQueuePage::refreshResumedList()
{
    if (!app->configuration->yarraEnableResume)
    {
        return;
    }

    // Clear both pages to free-up memory
    resumedtaskLayout->clear();
    WTimer::singleShot(0, this, &ywQueuePage::clearQueueList);
    WTimer::singleShot(0, this, &ywQueuePage::clearFailList);
    WTimer::singleShot(0, this, &ywQueuePage::clearFinishedList);

    WText* resumedLabel=new WText("Suspended Tasks");
    resumedtaskLayout->addWidget(resumedLabel,Wt::Left);

    fs::path resumedDir(app->configuration->yarraResumePath.toUTF8());
    fs::directory_iterator end_iter;

    typedef std::multimap<std::time_t,  boost::filesystem::path> result_set_t;
    result_set_t result_set;

    try
    {
        if (fs::exists(resumedDir) && fs::is_directory(resumedDir))
        {
            for(fs::directory_iterator dir_iter(resumedDir) ; dir_iter != end_iter ; ++dir_iter)
            {
                if (fs::is_directory(dir_iter->status()))
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
        resumedtaskLayout->addWidget(createQueuePanel(dirName, MODE_RESUMED),Wt::AlignMiddle);
        i++;

        // Only show max 100 entries to avoid slow speed.
        if (i>100)
        {
            break;
        }
    }

    if (i==0)
    {
        resumedLabel->setText("No suspended tasks found.");
    }
}
