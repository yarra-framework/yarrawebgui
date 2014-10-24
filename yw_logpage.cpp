#include "yw_logpage.h"
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
#include <Wt/WComboBox>
#include <Wt/WScrollArea>
#include <Wt/WFileResource>
#include <Wt/WBreak>

#include <boost/filesystem.hpp>


ywLogPage::ywLogPage(ywApplication* parent)
 : WContainerWidget()
{
    app=parent;

    Wt::WVBoxLayout* pageLayout = new Wt::WVBoxLayout();
    pageLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(pageLayout);

    Wt::WText* head1=new Wt::WText("<h3>Previous Task Logs</h3>");
    head1->setMargin(6, Wt::Bottom);

    logselectCombobox=new Wt::WComboBox();
    logselectCombobox->activated().connect(this, &ywLogPage::showLog);
    logselectCombobox->addStyleClass("combo");

    WContainerWidget* logContainer=new WContainerWidget();
    Wt::WVBoxLayout*  logLayout=new Wt::WVBoxLayout();
    logContainer->setLayout(logLayout);
    logWidget=new Wt::WText();
    logWidget->setTextFormat(Wt::PlainText);
    logWidget->setWordWrap(false);
    logScrollArea=new Wt::WScrollArea();
    logScrollArea->setWidget(logWidget);
    logLayout->addWidget(logScrollArea);
    logLayout->setContentsMargins(11, 11, 11, 16);
    logContainer->addStyleClass("log-frame");

    WContainerWidget* innerBtnContainer=new WContainerWidget();
    Wt::WHBoxLayout*  innerLayout=new Wt::WHBoxLayout();
    innerLayout->setContentsMargins(0, 0, 0, 0);
    innerLayout->setSpacing(4);

    downloadButton =new Wt::WPushButton("Download");
    downloadButton->setStyleClass("btn-primary");
    downloadButton->setLink(WLink((Wt::WResource*) &downloadRes));
    innerLayout->addWidget(downloadButton,0);

    if (app->currentLevel==ywApplication::YW_USERLEVEL_ADMIN)
    {
        deleteButton =new Wt::WPushButton("Delete");
        deleteButton->setStyleClass("btn-primary");
        deleteButton->clicked().connect(this, &ywLogPage::deleteLog);
        innerLayout->addWidget(deleteButton,0);
    }

    Wt::WPushButton* button=0;
    button =new Wt::WPushButton("Refresh");
    button->setStyleClass("btn");
    button->clicked().connect(this, &ywLogPage::refreshLogs);
    innerLayout->addWidget(button,0);

    if (app->currentLevel==ywApplication::YW_USERLEVEL_ADMIN)
    {
        Wt::WPushButton* purgeLogsButton =new Wt::WPushButton("Purge Old Logs");
        purgeLogsButton->setStyleClass("btn-primary");
        purgeLogsButton->clicked().connect(this, &ywLogPage::purgeLogs);
        innerLayout->addStretch(1);
        innerLayout->addWidget(purgeLogsButton,0);
    }

    downloadRes.setMimeType("plain/text");
    downloadRes.suggestFileName("task.log");

    innerBtnContainer->setLayout(innerLayout);
    innerBtnContainer->setMargin(30, Wt::Bottom);

    pageLayout->addWidget(head1);
    pageLayout->addWidget(logselectCombobox);
    pageLayout->addWidget(logContainer,1);
    pageLayout->addWidget(innerBtnContainer);

    previousItem="";
    refreshLogs();
    logselectCombobox->setCurrentIndex(0);
}


ywLogPage* ywLogPage::createInstance(ywApplication* parent)
{
    return new ywLogPage(parent);
}


void ywLogPage::deleteLog()
{
    Wt::StandardButton answer=Wt::WMessageBox::show("Delete Task Log",
                                                    "Are you sure to permanently delete this log file?",
                                                    Wt::Ok | Wt::Cancel);
    if (answer==Wt::Ok)
    {
        int index=logselectCombobox->currentIndex();

        if ((index<0) || (index>=logselectCombobox->count()))
        {
            return;
        }

        WString filename=app->configuration->yarraLogPath+"/"+logselectCombobox->itemText(index);

        bool deleteError=false;
        try
        {
            deleteError=!boost::filesystem::remove(filename.toUTF8());
        }
        catch(const boost::filesystem::filesystem_error& e)
        {
            deleteError=true;
        }

        if (deleteError)
        {
            Wt::WMessageBox messageBox("Error", "A problem occured while deleting the file.",
                                       Wt::Information, Wt::Ok);
            messageBox.setModal(true);
            messageBox.setIcon(Wt::Critical);
            messageBox.buttonClicked().connect(&messageBox, &WMessageBox::accept);
            messageBox.exec();
        }

        refreshLogs();
    }
}


void ywLogPage::refreshLogs()
{
    if (logselectCombobox->currentIndex()>=0)
    {
        previousItem=logselectCombobox->currentText();
    }

    logselectCombobox->clear();

    // Read log directory, except yarra.log
    namespace fs = boost::filesystem;
    fs::path logDir(app->configuration->yarraLogPath.toUTF8());
    fs::directory_iterator end_iter;

    typedef std::multimap<std::time_t,  boost::filesystem::path> result_set_t;
    result_set_t result_set;

    try
    {
        if ( fs::exists(logDir) && fs::is_directory(logDir))
        {
            // TODO: Check if file extension is ".log"
            for( fs::directory_iterator dir_iter(logDir) ; dir_iter != end_iter ; ++dir_iter)
            {
                if ( (fs::is_regular_file(dir_iter->status()) && (dir_iter->path().filename()!="yarra.log") ))
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
        WString fileName=WString((*ii).second.filename().generic_string());
        logselectCombobox->addItem(fileName);
        i++;

        // Only show max 100 entries to avoid slow speed.
        if (i>100)
        {
            break;
        }
    }

    // Focus the previous icon in the list (if it still exists). It might be
    // at a different position now.
    if (!previousItem.empty())
    {
        int newIndex=logselectCombobox->findText(previousItem);
        if (newIndex>=0)
        {
            logselectCombobox->setCurrentIndex(newIndex);
        }
    }

    if (logselectCombobox->count()==0)
    {

    }
    else
    {
        showLog(logselectCombobox->currentIndex());
    }
}


void ywLogPage::showLog(int index)
{
    if ((index<0) || (index>=logselectCombobox->count()))
    {
        return;
    }

    WString filename=app->configuration->yarraLogPath+"/"+logselectCombobox->itemText(index);
    WString widgetText="";

    if (filename.empty())
    {
        widgetText="ERROR: Invalid filename.";
    }
    else
    {
        std::string line;
        std::ifstream logfile(filename.toUTF8());

        if(!logfile)
        {
            widgetText="ERROR: Unable to open task log file (" + filename + ")";
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

    logWidget->setText(widgetText);
    downloadRes.setFileName(filename.toUTF8());
    downloadRes.suggestFileName("task.log");
}


void ywLogPage::purgeLogs()
{
    Wt::StandardButton answer=Wt::WMessageBox::show("Purge Old Logs",
                                                    "<p><strong>Warning:</strong> This operation will delete all logs files older than 14 days.</p>\
                                                    <p>Are you sure to permanently delete these files?</p>",
                                                    Wt::Ok | Wt::Cancel);
    if (answer==Wt::Ok)
    {
        WString command="find "+app->configuration->yarraLogPath+"/*.log -mtime +14 -exec rm -f {} \\;";
        int result=system(command.toUTF8().data());

        if (result==-1)
        {
            Wt::WMessageBox *messageBox = new Wt::WMessageBox
               ("Error", "<p>Unfortunately, deleting the log file was not possible.</p>",
                Wt::Critical, Wt::Ok);

            messageBox->setModal(true);
            messageBox->buttonClicked().connect(std::bind([=] () {
                delete messageBox;
            }));
            messageBox->show();
        }
        refreshLogs();
    }
}
