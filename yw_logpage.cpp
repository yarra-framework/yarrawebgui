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

    WContainerWidget* btnContainer=new WContainerWidget();
    Wt::WHBoxLayout*  btnLayout=new Wt::WHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnContainer->setLayout(btnLayout);

    WContainerWidget* innerBtnContainer=new WContainerWidget();

    Wt::WPanel *panel = new Wt::WPanel();
    btnLayout->addWidget(panel);
    panel->setCentralWidget(innerBtnContainer);
    panel->setMargin(30, Wt::Bottom);

    downloadButton =new Wt::WPushButton("Download", innerBtnContainer);
    downloadButton->setMargin(2);
    downloadButton->setStyleClass("btn-primary");
    downloadButton->setLink(WLink((Wt::WResource*) &downloadRes));

    if (app->currentLevel==ywApplication::YW_USERLEVEL_ADMIN)
    {
        deleteButton =new Wt::WPushButton("Delete", innerBtnContainer);
        deleteButton->setMargin(2);
        deleteButton->setStyleClass("btn-primary");
        deleteButton->clicked().connect(this, &ywLogPage::deleteLog);
    }

    Wt::WPushButton* button=0;
    button =new Wt::WPushButton("Refresh", innerBtnContainer);
    button->setStyleClass("btn");
    button->setMargin(2);
    button->clicked().connect(this, &ywLogPage::refreshLogs);

    pageLayout->addWidget(head1);
    pageLayout->addWidget(logselectCombobox);
    pageLayout->addWidget(logContainer,1);
    pageLayout->addWidget(btnContainer);

    downloadRes.setMimeType("plain/text");
    downloadRes.suggestFileName("task.log");

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
                                                    "Are you sure to permaently delete this log file?",
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
        }
    }

    logWidget->setText(widgetText);
    downloadRes.setFileName(filename.toUTF8());
    downloadRes.suggestFileName("task.log");
}



