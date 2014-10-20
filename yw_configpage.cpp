#include "yw_configpage.h"
#include "yw_serverinterface.h"
#include "yw_application.h"
#include "yw_helper.h"

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
#include <Wt/WTimer>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <boost/exception/error_info.hpp>

#include <sys/types.h>
#include <sys/stat.h>

namespace fs = boost::filesystem;



ywConfigPage::ywConfigPage(ywApplication* parent)
 : WContainerWidget()
{
    app=parent;

    serverStatusLabel = new Wt::WText();
    serverStatusLabel->setInline(false);
    serverStatusLabel->setTextFormat(Wt::XHTMLUnsafeText);
    serverStatusLabel->hide();
    serverStatusLabel->setText("<button type=\"button\" class=\"close\" data-dismiss=\"alert\">&times;</button>It is advisable to shutdown the server before modifying any configuration files.");
    serverStatusLabel->setStyleClass("alert alert-dismissable alert-warning");

    Wt::WVBoxLayout* outerLayout = new Wt::WVBoxLayout();
    this->setLayout(outerLayout);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    Wt::WHBoxLayout* configPageLayout = new Wt::WHBoxLayout();
    //this->setLayout(configPageLayout);

    outerLayout->addWidget(serverStatusLabel,0);
    outerLayout->addLayout(configPageLayout,1);

    Wt::WStackedWidget *configContents=new Wt::WStackedWidget();
    configContents->setMargin(30, Wt::Bottom);
    configContents->setMargin(30, Wt::Left);

    Wt::WMenu *configMenu = new Wt::WMenu(configContents, Wt::Vertical, this);
    configMenu->setStyleClass("nav nav-pills nav-stacked");
    configMenu->setWidth(200);

    configMenu->addItem("Reconstruction Modes", new Wt::WTextArea("TODO"));
    configMenu->addItem("Mode List",  new ywConfigPageModeList(this));
    configMenu->addItem("Server Settings", new ywConfigPageServer(this));

    configPageLayout->addWidget(configMenu);
    configPageLayout->addWidget(configContents,1);
}


ywConfigPage* ywConfigPage::createInstance(ywApplication* parent)
{      
    return new ywConfigPage(parent);
}


void ywConfigPage::refreshStatus()
{
    if (ywServerInterface::isServerRunning(app->configuration->yarraPath))
    {
        serverStatusLabel->show();
    }
    else
    {
        serverStatusLabel->hide();
    }

}


void ywConfigPage::showErrorMessage(WString errorMessage)
{
    Wt::WMessageBox *messageBox = new Wt::WMessageBox
       ("Error",
        "<p>Unfortunately, the operation failed due to the following reason:</p>"
        "<p>"+errorMessage+"</p>",
        Wt::Critical, Wt::Ok);

    messageBox->setModal(true);
    messageBox->buttonClicked().connect(std::bind([=] () {
        delete messageBox;
    }));
    messageBox->show();
}




ywConfigPageServer::ywConfigPageServer(ywConfigPage* pageParent)
 : WContainerWidget()
{
    parent=pageParent;

    Wt::WVBoxLayout* subLayout = new Wt::WVBoxLayout();
    this->setLayout(subLayout);
    subLayout->setContentsMargins(0, 0, 0, 0);

    editor = new Wt::WTextArea();
    subLayout->insertWidget(0,editor,1);
    editor->setJavaScriptMember("spellcheck","false");

    WContainerWidget* innerBtnContainer=new WContainerWidget();
    innerBtnContainer->setMargin(10, Wt::Top);

    saveBtn=new Wt::WPushButton("Save", innerBtnContainer);
    saveBtn->setMargin(2);
    saveBtn->setStyleClass("btn-primary");
    saveBtn->clicked().connect(std::bind([=] () {
        writeServerConfig(editor->text());
    }));

    Wt::WPushButton* refreshBtn=new Wt::WPushButton("Refresh", innerBtnContainer);
    refreshBtn->setStyleClass("btn");
    refreshBtn->setMargin(2);
    refreshBtn->clicked().connect(std::bind([=] () {
        refreshEditor();
    }));

    refreshEditor();

    subLayout->insertWidget(1,innerBtnContainer,0);
}


void ywConfigPageServer::refreshEditor()
{
    editor->setText(readServerConfig());
}


WString ywConfigPageServer::readServerConfig()
{
    WString fileName=parent->app->configuration->yarraPath+"/YarraServer.ini";

    // Read the task file into a WString
    std::string line;
    std::ifstream configfile(fileName.toUTF8());
    WString configContent="";

    if(!configfile)
    {
        saveBtn->setEnabled(false);
        return WString("Unable to read configuration file");
    }
    else
    {
        while (std::getline(configfile, line))
        {
            configContent+=WString::fromUTF8(line)+"\n";
        }
    }
    configfile.close();
    saveBtn->setEnabled(true);

    return configContent;
}


bool ywConfigPageServer::writeServerConfig(WString newConfig)
{
    WString fileName=parent->app->configuration->yarraPath+"/YarraServer.ini";

    if (!fs::exists(fileName.toUTF8()))
    {
        parent->showErrorMessage("Cannot find configuration file.");
        WTimer::singleShot(500, this, &ywConfigPageServer::refreshEditor);
        return false;
    }

    // First, create a lockfile in the queue directory
    if (!ywHelper::lockFile(fileName.toUTF8()))
    {
        parent->showErrorMessage("Locking the file not possible. The file is probably being edited by another user.");
        WTimer::singleShot(500, this, &ywConfigPageServer::refreshEditor);
        return false;
    }

    // Write the content to the file
    std::ofstream configFile(fileName.toUTF8(), std::ofstream::out | std::ofstream::trunc);
    configFile << newConfig.toUTF8();
    configFile.flush();
    configFile.close();

    ywHelper::unlockFile(fileName.toUTF8());

    Wt::WMessageBox *messageBox = new Wt::WMessageBox
       ("Configuration Saved",
        "<p>The server configuration has been saved. The server needs to be restarted before the new settings become effective.</p>",
        Wt::Information, Wt::Ok);
    messageBox->setModal(true);
    messageBox->buttonClicked().connect(std::bind([=] () {
        delete messageBox;
    }));
    messageBox->show();

    return true;
}



ywConfigPageModeList::ywConfigPageModeList(ywConfigPage* pageParent)
 : WContainerWidget()
{
    parent=pageParent;

    Wt::WVBoxLayout* subLayout = new Wt::WVBoxLayout();
    this->setLayout(subLayout);
    subLayout->setContentsMargins(0, 0, 0, 0);

    editor = new Wt::WTextArea();
    subLayout->insertWidget(0,editor,1);
    editor->setJavaScriptMember("spellcheck","false");

    WContainerWidget* innerBtnContainer=new WContainerWidget();
    innerBtnContainer->setMargin(10, Wt::Top);

    saveBtn=new Wt::WPushButton("Save", innerBtnContainer);
    saveBtn->setMargin(2);
    saveBtn->setStyleClass("btn-primary");
    saveBtn->clicked().connect(std::bind([=] () {
        writeModeList(editor->text());
    }));

    Wt::WPushButton* refreshBtn=new Wt::WPushButton("Refresh", innerBtnContainer);
    refreshBtn->setStyleClass("btn");
    refreshBtn->setMargin(2);
    refreshBtn->clicked().connect(std::bind([=] () {
        refreshEditor();
    }));

    refreshEditor();

    subLayout->insertWidget(1,innerBtnContainer,0);
}


void ywConfigPageModeList::refreshEditor()
{
    editor->setText(readModeList());
}


WString ywConfigPageModeList::readModeList()
{
    WString fileName=parent->app->configuration->yarraQueuePath+"/YarraModes.cfg";

    // Read the task file into a WString
    std::string line;
    std::ifstream configfile(fileName.toUTF8());
    WString configContent="";

    if(!configfile)
    {
        saveBtn->setEnabled(false);
        return WString("Unable to read mode file");
    }
    else
    {
        while (std::getline(configfile, line))
        {
            configContent+=WString::fromUTF8(line)+"\n";
        }
    }
    configfile.close();
    saveBtn->setEnabled(true);

    return configContent;
}


bool ywConfigPageModeList::writeModeList(WString newConfig)
{
    WString fileName=parent->app->configuration->yarraQueuePath+"/YarraModes.cfg";

    if (!fs::exists(fileName.toUTF8()))
    {
        parent->showErrorMessage("Cannot find mode file.");
        WTimer::singleShot(500, this, &ywConfigPageModeList::refreshEditor);
        return false;
    }

    // First, create a lockfile in the queue directory
    if (!ywHelper::lockFile(fileName.toUTF8()))
    {
        parent->showErrorMessage("Locking the file not possible. The file is probably being edited by another user.");
        WTimer::singleShot(500, this, &ywConfigPageModeList::refreshEditor);
        return false;
    }

    // TODO: chmod 740 on file name!
    if (chmod(fileName.toUTF8().data(),S_IRUSR | S_IWUSR) != 0)
    {
        parent->showErrorMessage("Cannot change write permission of mode file. Check file permissions.");
        WTimer::singleShot(500, this, &ywConfigPageModeList::refreshEditor);
        ywHelper::unlockFile(fileName.toUTF8());
        return false;
    }

    // Write the content to the file
    std::ofstream configFile(fileName.toUTF8(), std::ofstream::out | std::ofstream::trunc);

    bool writeError=false;

    // Error handling
    if (!configFile.is_open())
    {
        writeError=true;
        parent->showErrorMessage("Cannot write mode list. Check server configuration and file permissions.");
    }

    if (!writeError)
    {
        configFile << newConfig.toUTF8();
        configFile.flush();
        configFile.close();
    }

    // chmod 440 on file name
    chmod(fileName.toUTF8().data(),S_IRUSR | S_IRGRP);

    ywHelper::unlockFile(fileName.toUTF8());

    if (!writeError)
    {
        Wt::WMessageBox *messageBox = new Wt::WMessageBox
           ("Mode List Saved",
            "<p>The mode list has been updated. Please make sure that matching mode files exist in the mode directory.</p>",
            Wt::Information, Wt::Ok);
        messageBox->setModal(true);
        messageBox->buttonClicked().connect(std::bind([=] () {
            delete messageBox;
        }));
        messageBox->show();
    }
    else
    {
        WTimer::singleShot(500, this, &ywConfigPageModeList::refreshEditor);
    }

    return true;
}

