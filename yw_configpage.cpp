#include "yw_configpage.h"
#include "yw_configpage_ymgenerator.h"
#include "yw_configpage_modules.h"
#include "yw_configpage_update.h"
#include "yw_serverinterface.h"
#include "yw_application.h"
#include "yw_helper.h"

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WTable>
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
#include <Wt/WComboBox>
#include <Wt/WLabel>
#include <Wt/WRegExpValidator>
#include <Wt/WServer>

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
    serverStatusLabel->setText("<button type=\"button\" class=\"close\" data-dismiss=\"alert\">&times;</button>It is advisable to shutdown the server before modifying the configuration.");
    serverStatusLabel->setStyleClass("alert alert-dismissable alert-warning");

    Wt::WVBoxLayout* outerLayout = new Wt::WVBoxLayout();
    this->setLayout(outerLayout);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    Wt::WHBoxLayout* configPageLayout = new Wt::WHBoxLayout();

    outerLayout->addWidget(serverStatusLabel,0);
    outerLayout->addLayout(configPageLayout,1);

    Wt::WStackedWidget *configContents=new Wt::WStackedWidget();
    configContents->setMargin(15, Wt::Bottom);
    configContents->setMargin(30, Wt::Left);

    configMenu = new Wt::WMenu(configContents, Wt::Vertical, this);
    configMenu->setStyleClass("nav nav-pills nav-stacked");
    configMenu->setWidth(200);

    page0=new ywConfigPageModes(this);    
    configMenu->addItem("Reconstruction Modes", page0)->triggered().connect(std::bind([=] () {
        refreshSubpage();
    }));
    pages.push_back(PAGE_MODES);

    page1=new ywConfigPageModeList(this);
    configMenu->addItem("Mode List", page1)->triggered().connect(std::bind([=] () {
        refreshSubpage();
    }));
    pages.push_back(PAGE_MODELIST);

    page2=new ywConfigPageServer(this);
    configMenu->addItem("Server Settings", page2)->triggered().connect(std::bind([=] () {
        refreshSubpage();
    }));
    pages.push_back(PAGE_SERVERSETTINGS);

    // Show the server list tab only if the file YarraServerList.cfg is existing
    WString serverListName=app->configuration->yarraQueuePath+"/YarraServerList.cfg";
    if (fs::exists(serverListName.toUTF8()))
    {
        pageServerList=new ywConfigPageServerList(this);
        configMenu->addItem("Server List", pageServerList)->triggered().connect(std::bind([=] () {
            refreshSubpage();
        }));
        pages.push_back(PAGE_SERVERLIST);
    }
    else
    {
        pageServerList=0;
    }

    pageUpdate=new ywConfigPageUpdate(this);
    configMenu->addItem("Server Update", pageUpdate)->triggered().connect(std::bind([=] () {
        refreshSubpage();
    }));
    pages.push_back(PAGE_SERVERUPDATE);

    pageServerData=new ywConfigPageServerData(this);
    configMenu->addItem("Information", pageServerData)->triggered().connect(std::bind([=] () {
        refreshSubpage();
    }));
    pages.push_back(PAGE_SERVERDATA);

    pageModules=new ywConfigPageModules(this);
    configMenu->addItem("Installed Modules", pageModules)->triggered().connect(std::bind([=] () {
        refreshSubpage();
    }));
    pages.push_back(PAGE_MODULES);

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

    refreshSubpage();
}


void ywConfigPage::refreshSubpage()
{
    // Find out which page is currently displayed
    PAGES currentPage=PAGE_MODES;

    if ((configMenu->currentIndex()>=0) && (configMenu->currentIndex()<pages.size()))
    {
        currentPage=pages.at(configMenu->currentIndex());
    }

    // Then update the corresponding page
    switch (currentPage)
    {
    case PAGE_MODES:
    default:
        page0->refreshPage();
        break;
    case PAGE_MODELIST:
        page1->refreshEditor();
        break;
    case PAGE_SERVERSETTINGS:
        page2->refreshEditor();
        break;
    case PAGE_SERVERUPDATE:
        pageUpdate->refreshPage();
        break;
    case PAGE_MODULES:
        pageModules->refreshPage();
        break;
    case PAGE_SERVERLIST:
        pageServerList->refreshEditor();
        break;
    case PAGE_SERVERDATA:
        pageServerData->refreshPage();
        break;
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


ywConfigPageServerData::ywConfigPageServerData(ywConfigPage* pageParent)
 : WContainerWidget()
{
    parent=pageParent;
    Wt::WVBoxLayout* subLayout = new Wt::WVBoxLayout();
    this->setLayout(subLayout);
    subLayout->setContentsMargins(0, 0, 0, 0);

    Wt::WText* heading=new Wt::WText("<h3>Server Information</h3>");
    heading->setMargin(6, Wt::Bottom);
    subLayout->insertWidget(0,heading,0);

    table = new Wt::WTable();
    table->setHeaderCount(0);
    subLayout->addWidget(table,0);
    subLayout->addStretch(1);

    refreshPage();
}


Wt::WText* ywConfigPageServerData::addText(Wt::WString text)
{
    Wt::WText* entry = new Wt::WText(text);
    return entry;
}


void ywConfigPageServerData::refreshPage()
{
    table->clear();

    // Check if Mercurial is installed
    int ret = system("which hg");
    Wt::WString mercurial = "<span style=\"background-color: #00aa00 !important;\" class=\"label label-info\">Installed</span>";
    if (ret != 0)
    {
        mercurial = "<span style=\"background-color: #aa0000 !important;\" class=\"label label-info\">Not installed</span>";
    }

    /*
    // Is Git available in the path?
    // Hidden for now, as only hg is supported for module repos at the moment
    ret = system("which git");
    Wt::WString git = "Installed";
    if (ret != 0) {
        git = "Not installed";
    }
    */

    // Check if Matlab is installed

    // TODO: Seems not to work
    // TODO: Determine Matlab version

    //bool file_exists = fs::is_regular_file(parent->app->configuration->yarraMatlabPath.toUTF8());

    bool matlabExists = fs::exists(parent->app->configuration->matlabBinaryPath.toUTF8());
    Wt::WString matlab = "<span style=\"background-color: #00aa00 !important;\" class=\"label label-info\">Installed</span>";
    if (!matlabExists)
    {
        matlab = "<span style=\"background-color: #aa0000 !important;\" class=\"label label-info\">Not installed</span> &nbsp; (expected in " + parent->app->configuration->matlabBinaryPath.toUTF8()+")";
    }

    // Show the public SSH key, if available
    fs::path pubkey_path = fs::path(getenv("HOME")) / ".ssh/id_rsa.pub";
    Wt::WString pubkey = "";

    bool pubkeyExists = fs::is_regular_file(pubkey_path);
    if (!pubkeyExists)
    {
        pubkey += "No public key found.";
    }
    else
    {
        std::string line;
        std::ifstream pubkey_file(pubkey_path.string());

        if(!pubkey_file)
        {
            pubkey += "Error displaying public key.";
        }
        else
        {
            while (std::getline(pubkey_file, line))
            {
                pubkey+=WString::fromUTF8(line)+"\n";
            }
        }
        pubkey_file.close();
        pubkey += "";
    }

    // Show results
    table->elementAt(0, 0)->addWidget(addText("Operating System:"));
    table->elementAt(0, 1)->addWidget(addText("To-be-done"));

    table->elementAt(1, 0)->addWidget(addText("Physical Memory:"));
    table->elementAt(1, 1)->addWidget(addText("To-be-done"));

    table->elementAt(2, 0)->addWidget(addText("CPU Cores:"));
    table->elementAt(2, 1)->addWidget(addText("To-be-done"));

    table->elementAt(3, 0)->addWidget(addText("Offis DCMTK:"));
    table->elementAt(3, 1)->addWidget(addText("To-be-done"));

    table->elementAt(4, 0)->addWidget(addText("Matlab:"));
    table->elementAt(4, 1)->addWidget(addText(matlab));

    table->elementAt(5, 0)->addWidget(addText("Mercurial:"));
    table->elementAt(5, 1)->addWidget(addText(mercurial));

    table->elementAt(6, 0)->addWidget(addText("Public Key:"));

    Wt::WTextArea* pubkeyEdit=new Wt::WTextArea(pubkey);
    pubkeyEdit->setReadOnly(true);
    table->elementAt(6, 1)->addWidget(pubkeyEdit);

    // TODO: Add space between rows

    //table->elementAt(1, 0)->addWidget(new Wt::WText("Git:"));
    //table->elementAt(1, 1)->addWidget(new Wt::WText(git));
}


ywConfigPageServer::ywConfigPageServer(ywConfigPage* pageParent)
 : WContainerWidget()
{
    parent=pageParent;

    Wt::WVBoxLayout* subLayout = new Wt::WVBoxLayout();
    this->setLayout(subLayout);
    subLayout->setContentsMargins(0, 0, 0, 0);

    Wt::WText* heading=new Wt::WText("<h3>Server Settings</h3>");
    heading->setMargin(6, Wt::Bottom);
    subLayout->insertWidget(0,heading,0);

    editor = new Wt::WTextArea();
    subLayout->insertWidget(1,editor,1);
    editor->setJavaScriptMember("spellcheck","false");

    WContainerWidget* innerBtnContainer=new WContainerWidget();
    innerBtnContainer->setMargin(10, Wt::Top);

    Wt::WHBoxLayout* innerLayout = new Wt::WHBoxLayout();
    innerBtnContainer->setLayout(innerLayout);
    innerLayout->setContentsMargins(0, 0, 0, 0);
    innerLayout->setSpacing(4);

    saveBtn=new Wt::WPushButton("Save", innerBtnContainer);
    saveBtn->setStyleClass("btn-primary");
    saveBtn->clicked().connect(std::bind([=] () {
        writeServerConfig(editor->text());
    }));

    Wt::WPushButton* refreshBtn=new Wt::WPushButton("Refresh", innerBtnContainer);
    refreshBtn->setStyleClass("btn");
    refreshBtn->clicked().connect(std::bind([=] () {
        refreshEditor();
    }));

    innerLayout->insertWidget(0,saveBtn,0,Wt::AlignLeft);
    innerLayout->insertWidget(1,refreshBtn,0,Wt::AlignLeft);
    innerLayout->insertWidget(2,new Wt::WContainerWidget,1);

    refreshEditor();

    subLayout->insertWidget(2,innerBtnContainer,0);
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

    // Reload the new server settings into the webgui
    parent->app->configuration->loadServerConfiguration();

    return true;
}


ywConfigPageModeList::ywConfigPageModeList(ywConfigPage* pageParent)
 : WContainerWidget()
{
    parent=pageParent;

    Wt::WVBoxLayout* subLayout = new Wt::WVBoxLayout();
    this->setLayout(subLayout);
    subLayout->setContentsMargins(0, 0, 0, 0);

    Wt::WText* heading=new Wt::WText("<h3>Mode List</h3>");
    heading->setMargin(6, Wt::Bottom);
    subLayout->insertWidget(0,heading,0);

    Wt::WText* infoLabel = new Wt::WText();
    infoLabel->setInline(false);
    infoLabel->setTextFormat(Wt::XHTMLUnsafeText);
    infoLabel->setText("<span class=\"label label-primary\">NOTICE</span>&nbsp;&nbsp;&nbsp;Editing the mode list directly is deprecated. Use the <u>Update Mode List</u> mechanism instead.");
    infoLabel->setMargin(10, Wt::Bottom);

    infoLabel->clicked().connect(std::bind([=] () {
        parent->configMenu->select(0);
    }));
    infoLabel->decorationStyle().setCursor(PointingHandCursor);
    subLayout->insertWidget(1,infoLabel,0);

    editor = new Wt::WTextArea();
    subLayout->insertWidget(2,editor,1);
    editor->setJavaScriptMember("spellcheck","false");

    WContainerWidget* innerBtnContainer=new WContainerWidget();
    innerBtnContainer->setMargin(10, Wt::Top);
    Wt::WHBoxLayout* innerLayout = new Wt::WHBoxLayout();
    innerBtnContainer->setLayout(innerLayout);
    innerLayout->setContentsMargins(0, 0, 0, 0);
    innerLayout->setSpacing(4);

    saveBtn=new Wt::WPushButton("Save");
    saveBtn->setStyleClass("btn-primary");
    saveBtn->clicked().connect(std::bind([=] () {
        writeModeList(editor->text());
    }));

    Wt::WPushButton* refreshBtn=new Wt::WPushButton("Refresh");
    refreshBtn->setStyleClass("btn");
    refreshBtn->clicked().connect(std::bind([=] () {
        refreshEditor();
    }));

    innerLayout->insertWidget(0,saveBtn,0,Wt::AlignLeft);
    innerLayout->insertWidget(1,refreshBtn,0,Wt::AlignLeft);
    innerLayout->insertWidget(2,new Wt::WContainerWidget,1);

    refreshEditor();

    subLayout->insertWidget(3,innerBtnContainer,0);
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
    if (parent->app->configuration->disableModeEditing)
    {
        Wt::WMessageBox::show("Security Policy", "Modification of modes via the WebGUI has been disabled.", Wt::Ok);
        return true;
    }


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

    // chmod 740 on file name!
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


ywConfigPageServerList::ywConfigPageServerList(ywConfigPage* pageParent)
 : WContainerWidget()
{
    parent=pageParent;

    Wt::WVBoxLayout* subLayout = new Wt::WVBoxLayout();
    this->setLayout(subLayout);
    subLayout->setContentsMargins(0, 0, 0, 0);

    Wt::WText* heading=new Wt::WText("<h3>Server List</h3>");
    heading->setMargin(6, Wt::Bottom);
    subLayout->insertWidget(0,heading,0);

    editor = new Wt::WTextArea();
    subLayout->insertWidget(1,editor,1);
    editor->setJavaScriptMember("spellcheck","false");

    WContainerWidget* innerBtnContainer=new WContainerWidget();
    innerBtnContainer->setMargin(10, Wt::Top);
    Wt::WHBoxLayout* innerLayout = new Wt::WHBoxLayout();
    innerBtnContainer->setLayout(innerLayout);
    innerLayout->setContentsMargins(0, 0, 0, 0);
    innerLayout->setSpacing(4);

    saveBtn=new Wt::WPushButton("Save");
    saveBtn->setStyleClass("btn-primary");
    saveBtn->clicked().connect(std::bind([=] () {
        writeServerList(editor->text());
    }));

    Wt::WPushButton* refreshBtn=new Wt::WPushButton("Refresh");
    refreshBtn->setStyleClass("btn");
    refreshBtn->clicked().connect(std::bind([=] () {
        refreshEditor();
    }));

    innerLayout->insertWidget(0,saveBtn,0,Wt::AlignLeft);
    innerLayout->insertWidget(1,refreshBtn,0,Wt::AlignLeft);
    innerLayout->insertWidget(2,new Wt::WContainerWidget,1);

    refreshEditor();

    subLayout->insertWidget(2,innerBtnContainer,0);
}


void ywConfigPageServerList::refreshEditor()
{
    editor->setText(readServerList());
}


WString ywConfigPageServerList::readServerList()
{
    WString fileName=parent->app->configuration->yarraQueuePath+"/YarraServerList.cfg";

    // Read the task file into a WString
    std::string line;
    std::ifstream serverListFile(fileName.toUTF8());
    WString fileContent="";

    if(!serverListFile)
    {
        saveBtn->setEnabled(false);
        return WString("Unable to read server list file");
    }
    else
    {
        while (std::getline(serverListFile, line))
        {
            fileContent+=WString::fromUTF8(line)+"\n";
        }
    }
    serverListFile.close();
    saveBtn->setEnabled(true);

    return fileContent;
}


bool ywConfigPageServerList::writeServerList(WString newConfig)
{
    if (parent->app->configuration->disableModeEditing)
    {
        Wt::WMessageBox::show("Security Policy", "Modification of configuration via the WebGUI has been disabled.", Wt::Ok);
        return true;
    }

    WString fileName=parent->app->configuration->yarraQueuePath+"/YarraServerList.cfg";

    if (!fs::exists(fileName.toUTF8()))
    {
        parent->showErrorMessage("Cannot find server list file.");
        WTimer::singleShot(500, this, &ywConfigPageServerList::refreshEditor);
        return false;
    }

    // First, create a lockfile in the queue directory
    if (!ywHelper::lockFile(fileName.toUTF8()))
    {
        parent->showErrorMessage("Locking the file not possible. The file is probably being edited by another user.");
        WTimer::singleShot(500, this, &ywConfigPageServerList::refreshEditor);
        return false;
    }

    // chmod 640 on file name!
    if (chmod(fileName.toUTF8().data(),S_IRUSR | S_IWUSR) != 0)
    {
        parent->showErrorMessage("Cannot change write permission of server list file. Check file permissions.");
        WTimer::singleShot(500, this, &ywConfigPageServerList::refreshEditor);
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
           ("Server List Saved",
            "<p>The server list has been updated.</p>",
            Wt::Information, Wt::Ok);
        messageBox->setModal(true);
        messageBox->buttonClicked().connect(std::bind([=] () {
            delete messageBox;
        }));
        messageBox->show();
    }
    else
    {
        WTimer::singleShot(500, this, &ywConfigPageServerList::refreshEditor);
    }

    return true;
}


ywConfigPageModes::ywConfigPageModes(ywConfigPage* pageParent)
 : WContainerWidget()
{
    parent=pageParent;

    Wt::WVBoxLayout* subLayout = new Wt::WVBoxLayout();
    this->setLayout(subLayout);
    subLayout->setContentsMargins(0, 0, 0, 0);

    Wt::WText* heading=new Wt::WText("<h3>Reconstruction Modes</h3>");
    heading->setMargin(6, Wt::Bottom);
    subLayout->insertWidget(0,heading,0);

    modeList=new Wt::WComboBox();
    modeList->activated().connect(this, &ywConfigPageModes::showMode);
    modeList->addStyleClass("combo");
    subLayout->insertWidget(1,modeList,0);

    editor = new Wt::WTextArea();
    subLayout->insertWidget(2,editor,1);
    editor->setJavaScriptMember("spellcheck","false");

    WContainerWidget* innerBtnContainer=new WContainerWidget();
    innerBtnContainer->setMargin(10, Wt::Top);
    Wt::WHBoxLayout* innerLayout = new Wt::WHBoxLayout();
    innerBtnContainer->setLayout(innerLayout);
    innerLayout->setContentsMargins(0, 0, 0, 0);
    innerLayout->setSpacing(4);

    Wt::WPushButton* addBtn=new Wt::WPushButton("Add New", innerBtnContainer);
    addBtn->setStyleClass("btn-primary");
    addBtn->clicked().connect(std::bind([=] () {
        addMode();
    }));

    Wt::WPushButton* saveBtn=new Wt::WPushButton("Save", innerBtnContainer);
    saveBtn->setStyleClass("btn-primary");
    saveBtn->clicked().connect(std::bind([=] () {
        saveMode();
    }));

    Wt::WPushButton* deleteBtn=new Wt::WPushButton("Delete", innerBtnContainer);
    deleteBtn->setStyleClass("btn-primary");
    deleteBtn->clicked().connect(std::bind([=] () {
        deleteMode();
    }));

    Wt::WPushButton* refreshBtn=new Wt::WPushButton("Refresh", innerBtnContainer);
    refreshBtn->setStyleClass("btn");
    refreshBtn->clicked().connect(std::bind([=] () {
        refresh();
    }));

    Wt::WPushButton* helpBtn=new Wt::WPushButton("Help", innerBtnContainer);
    helpBtn->setStyleClass("btn");
    helpBtn->clicked().connect(std::bind([=] () {
        showHelp();
    }));

    modeListBtn=new Wt::WPushButton("Update Mode List", innerBtnContainer);
    modeListBtn->setStyleClass("btn-primary");
    modeListBtn->clicked().connect(std::bind([=] () {
        generateModeList();
    }));

    innerLayout->insertWidget(0,addBtn,0);
    innerLayout->insertWidget(1,saveBtn,0);
    innerLayout->insertWidget(2,deleteBtn,0);
    innerLayout->insertWidget(3,refreshBtn,0);
    innerLayout->insertWidget(4,new Wt::WContainerWidget,1);
    innerLayout->insertWidget(5,modeListBtn,0);
    innerLayout->insertWidget(6,helpBtn,0);

    subLayout->insertWidget(3,innerBtnContainer,0);

    refreshModes();
    showMode(0);
}


void ywConfigPageModes::refreshModes()
{
    modeList->clear();

    fs::path modeDir(parent->app->configuration->yarraModesPath.toUTF8());
    fs::directory_iterator end_iter;

    typedef std::vector<Wt::WString> result_set_t;
    result_set_t result_set;

    try
    {
        if ( fs::exists(modeDir) && fs::is_directory(modeDir))
        {
            for( fs::directory_iterator dir_iter(modeDir) ; dir_iter != end_iter ; ++dir_iter)
            {
                if ( (fs::is_regular_file(dir_iter->status()) && (dir_iter->path().extension()==YW_EXT_MODE) ))
                {
                    result_set.push_back(WString::fromUTF8(dir_iter->path().stem().generic_string()));
                }
            }
        }
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
    }

    std::sort(result_set.begin(), result_set.end());

    for (size_t i=0; i<result_set.size(); i++)
    {
        modeList->addItem(result_set.at(i));
    }


}



void ywConfigPageModes::showMode(int index)
{
    if ((index<0) || (index>=modeList->count()))
    {
        editor->setText("");
        return;
    }

    WString filename=parent->app->configuration->yarraModesPath+"/"+modeList->itemText(index)+YW_EXT_MODE;
    WString contentText="";

    if (filename.empty())
    {
        contentText="ERROR: Invalid mode file name.";
    }
    else
    {
        std::string line;
        std::ifstream modefile(filename.toUTF8());

        if(!modefile)
        {
            contentText="ERROR: Unable to open mode file (" + filename + ")";
        }
        else
        {
            while (std::getline(modefile, line))
            {
                contentText+=WString::fromUTF8(line)+"\n";
            }
            modefile.close();
        }
    }

    editor->setText(contentText);
    modeList->setCurrentIndex(index);
}


void ywConfigPageModes::refreshPage()
{
    WString oldMode=modeList->currentText();
    refreshModes();

    // Check if the previouly active mode still exists. If so, select it,
    // otherwise go to the first element.
    int modeIndex=modeList->findText(oldMode);
    if (modeIndex<0)
    {
        modeIndex=0;
    }

    showMode(modeIndex);
}


void ywConfigPageModes::deleteMode()
{
    if (parent->app->configuration->disableModeEditing)
    {
        Wt::WMessageBox::show("Security Policy", "Modification of modes via the WebGUI has been disabled.", Wt::Ok);
        return;
    }


    if (modeList->count()==0)
    {
        return;
    }

    Wt::StandardButton answer=Wt::WMessageBox::show("Delete Mode",
                                                    "Are you sure to permanently delete this reconstruction mode?",
                                                    Wt::Ok | Wt::Cancel);
    if (answer==Wt::Ok)
    {
        int index=modeList->currentIndex();

        if ((index<0) || (index>=modeList->count()))
        {
            return;
        }

        WString filename=parent->app->configuration->yarraModesPath+"/"+modeList->itemText(index)+YW_EXT_MODE;

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
            parent->showErrorMessage("A problem occured while deleting the mode file.");
        }
        else
        {
            // Indicate that the mode list needs to be updated
            setIndicateModeUpdate(true);
        }

        refreshModes();
        showMode(0);
    }
}


void ywConfigPageModes::addMode()
{
    if (parent->app->configuration->disableModeEditing)
    {
        Wt::WMessageBox::show("Security Policy", "Modification of modes via the WebGUI has been disabled.", Wt::Ok);
        return;
    }


    Wt::WDialog *addDialog = new Wt::WDialog("Add New Mode");

    Wt::WPushButton *ok = new Wt::WPushButton("OK", addDialog->footer());
    ok->setDefault(true);
    ok->setDisabled(true);

    Wt::WPushButton *cancel = new Wt::WPushButton("Cancel", addDialog->footer());
    cancel->setDefault(false);
    cancel->clicked().connect(addDialog, &Wt::WDialog::reject);

    Wt::WText* errorLabel = new Wt::WText(addDialog->contents());
    errorLabel->setInline(false);
    errorLabel->setTextFormat(Wt::XHTMLUnsafeText);
    errorLabel->hide();
    errorLabel->setText("<button type=\"button\" class=\"close\" data-dismiss=\"alert\">&times;</button>Mode already exists. Select different name.");
    errorLabel->setStyleClass("alert alert-dismissable alert-warning");

    Wt::WContainerWidget* nameContainer=new Wt::WContainerWidget(addDialog->contents());

    Wt::WLabel *nameLabel = new Wt::WLabel("Name:", nameContainer);
    Wt::WLineEdit *nameEdit = new Wt::WLineEdit(nameContainer);
    nameLabel->setBuddy(nameEdit);
    nameEdit->setText("");

    Wt::WRegExpValidator *nameValidator=new Wt::WRegExpValidator("[a-zA-Z0-9_+-]{1,64}");
    nameValidator->setMandatory(true);
    nameEdit->setValidator(nameValidator);

    nameEdit->keyWentUp().connect(std::bind([=] () {
        ok->setDisabled(nameEdit->validate() != Wt::WValidator::Valid);
    }));


    Wt::WLabel *templateLabel = new Wt::WLabel("Template:", addDialog->contents());
    templateLabel->setMargin(10, Wt::Top);
    Wt::WComboBox *templateBox = new Wt::WComboBox(addDialog->contents());
    templateLabel->setBuddy(templateBox);

    templateBox->addItem("-None-");
    for (int i=0; i<modeList->count(); i++)
    {
        templateBox->addItem(modeList->itemText(i));
    }
    templateBox->setCurrentIndex(0);

    ok->clicked().connect(std::bind([=] () {
        if (nameEdit->validate())
        {
            WString newFilename=parent->app->configuration->yarraModesPath+"/"+nameEdit->text()+YW_EXT_MODE;

            if (fs::exists(newFilename.toUTF8()))
            {
                errorLabel->show();
                nameContainer->addStyleClass("has-error");
                nameEdit->setFocus();
            }
            else
            {
                addDialog->accept();
            }
        }
    }));

    addDialog->rejectWhenEscapePressed();
    addDialog->setModal(true);

    // Process the dialog result.
    addDialog->finished().connect(std::bind([=] () {
        if (addDialog->result()==Wt::WDialog::Accepted)
        {
            WString templateMode="";
            if (templateBox->currentIndex()>0)
            {
                templateMode=templateBox->currentText();
            }

            doAddMode(nameEdit->text(), templateMode);
        }
        delete addDialog;
    }));

    addDialog->resize(600,Wt::WLength::Auto);
    addDialog->refresh();
    addDialog->show();
}


void ywConfigPageModes::doAddMode(WString name, WString templateMode)
{
    WString fullFilename=parent->app->configuration->yarraModesPath+"/"+name+YW_EXT_MODE;

    if (!ywHelper::lockFile(fullFilename))
    {
        parent->showErrorMessage("Locking mode file not possible.");
        return;
    }

    if (templateMode.empty())
    {
        // Create empty file
        std::ofstream modefile(fullFilename.toUTF8());
        modefile << ";##  YarraServer -- Reconstruction Mode Definition" << std::endl;
        modefile << ";##" << std::endl << std::endl;
        modefile.close();
    }
    else
    {
        // Copy existing file
        WString fullTemplateFilename=parent->app->configuration->yarraModesPath+"/"+templateMode+YW_EXT_MODE;

        try
        {
            boost::filesystem::copy(fullTemplateFilename.toUTF8(), fullFilename.toUTF8());
        }
        catch(const boost::filesystem::filesystem_error& e)
        {
            parent->showErrorMessage("Error copying template file.");
        }
    }

    ywHelper::unlockFile(fullFilename);

    // Open the newely created file in the editor
    refreshModes();
    int modeIndex=modeList->findText(name);
    showMode(modeIndex);

    // Indicate that the mode list needs to be updated
    setIndicateModeUpdate(true);
}


void ywConfigPageModes::saveMode()
{
    if (parent->app->configuration->disableModeEditing)
    {
        Wt::WMessageBox::show("Security Policy", "Modification of modes via the WebGUI has been disabled.", Wt::Ok);
        return;
    }


    if (modeList->count()==0)
    {
        return;
    }

    WString fileName=parent->app->configuration->yarraModesPath+"/"+modeList->currentText()+YW_EXT_MODE;
    WString fileContent=editor->text();

    if (!fs::exists(fileName.toUTF8()))
    {
        parent->showErrorMessage("Cannot find mode file.");
        WTimer::singleShot(500, this, &ywConfigPageModes::refresh);
        return;
    }

    // First, create a lockfile in the mode directory (to make sure nobody else is modifying the file)
    if (!ywHelper::lockFile(fileName.toUTF8()))
    {
        parent->showErrorMessage("Locking the mode file not possible. It is probably being edited by another user.");
        WTimer::singleShot(500, this, &ywConfigPageModes::refresh);
        return;
    }

    // Write the content to the file
    std::ofstream modeFile(fileName.toUTF8(), std::ofstream::out | std::ofstream::trunc);
    modeFile << fileContent.toUTF8();
    modeFile.flush();
    modeFile.close();

    ywHelper::unlockFile(fileName.toUTF8());

    Wt::WMessageBox *messageBox = new Wt::WMessageBox
       ("Mode Saved","<p>The reconstruction mode has been updated.</p>",
        Wt::Information, Wt::Ok);
    messageBox->setModal(true);
    messageBox->buttonClicked().connect(std::bind([=] () {
        delete messageBox;
    }));
    messageBox->show();

    // Indicate that the mode list needs to be updated
    setIndicateModeUpdate(true);
}


void ywConfigPageModes::generateModeList()
{
    Wt::StandardButton answer=Wt::WMessageBox::show("Update Mode List",
                                                    "<p>This function will generate an updated YarraModes.cfg file from the [ClientConfig] sections in existing mode files.</p>\
                                                    <p><strong>Warning:</strong> This will overwrite your existing YarraModes.cfg file. </p>\
                                                    <p>Are you sure to continue?</p>",
                                                    Wt::Ok | Wt::Cancel);
    if (answer==Wt::Ok)
    {
        ywConfigPageYMGenerator instance(parent->app);
        instance.perform();
        setIndicateModeUpdate(false);
    }
}


void ywConfigPageModes::showHelp()
{
    Wt::WDialog *infoDialog = new Wt::WDialog("Available Macros");
    infoDialog->setResizable(true);

    Wt::WPushButton *ok = new Wt::WPushButton("Close", infoDialog->footer());
    ok->setDefault(false);

    ok->clicked().connect(std::bind([=] () {
        infoDialog->accept();
    }));

    infoDialog->rejectWhenEscapePressed();
    infoDialog->setModal(false);

    infoDialog->finished().connect(std::bind([=] () {
        delete infoDialog;
    }));

    Wt::WVBoxLayout* scrollLayout = new Wt::WVBoxLayout();
    infoDialog->contents()->setLayout(scrollLayout);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    // Add table with content, adapt size of window
    Wt::WScrollArea* scrollArea = new Wt::WScrollArea();
    scrollLayout->addWidget(scrollArea,1);

    Wt::WText* infoText = new Wt::WText();
    //infoText->setTextFormat(Wt::XHTMLUnsafeText);
    infoText->setWordWrap(false);
    infoText->setText("\
                      <table class=\"table table-striped table-hover\"> \
                      <tbody> \
                      <tr> \
                      <td><strong>Macro</strong></td> \
                      <td><strong>Meaning</strong></td> \
                      </tr> \
                      <tr> \
                      <td>%rif</td> \
                      <td>Name of the input file (TWIX file), including extension (.dat)</td> \
                      </tr> \
                      <tr> \
                      <td>%rid</td> \
                      <td>Path of the input file (normally yarra/work) without trailing &#8220;/&#8221;</td> \
                      </tr> \
                      <tr> \
                      <td>%rin</td> \
                      <td>Name of the input file without extension</td> \
                      </tr> \
                      <tr> \
                      <td>%rit</td> \
                      <td>Name if the task file, including the extension (.task, .task_prio, .task_night)</td> \
                      </tr> \
                      <tr> \
                      <td>%rod</td> \
                      <td>Output directory for the reconstruction module</td> \
                      </tr> \
                      <tr> \
                      <td>%pid</td> \
                      <td>For post-processing modules: Path with the input files</td> \
                      </tr> \
                      <tr> \
                      <td>%pod</td> \
                      <td>For post-processing modules: Output path</td> \
                      </tr> \
                      <tr> \
                      <td>%td</td> \
                      <td>For transfer modules: Path with the input files that should be transferred</td> \
                      </tr> \
                      <tr> \
                      <td>%bd</td> \
                      <td>Directory where the binaries of the core modules are located (usually yarra/modules)</td> \
                      </tr> \
                      <tr> \
                      <td>%bu</td> \
                      <td>Directory where binaries of user-installed modules are located (usually yarra/modules_user)</td> \
                      </tr> \
                      <tr> \
                      <td>%md</td> \
                      <td>Path to the mode files (usually yarra/modes)</td> \
                      </tr> \
                      <tr> \
                      <td>%mf</td> \
                      <td>Name of the mode file of the current reconstruction mode</td> \
                      </tr> \
                      <tr> \
                      <td>%mc</td> \
                      <td>Name of the mode file including the path (%md/%mc)</td> \
                      </tr> \
                      <tr> \
                      <td>%tmp</td> \
                      <td>Path where modules can create temporary files (deleted after each step)</td> \
                      </tr> \
                      <tr> \
                      <td>%vacc</td> \
                      <td>Submitted accession number</td> \
                      </tr> \
                      <tr> \
                      <td>%vparam</td> \
                      <td>Submitted free parameter value (if provided by mode)</td> \
                      </tr> \
                      <tr> \
                      <td>%vuid</td> \
                      <td>Unique task ID (incl processing time stamp)</td> \
                      </tr> \
                      <tr> \
                      <td>%vtid</td> \
                      <td>Task ID without time stamp (not unique if task sent twice)</td> \
                      </tr> \
                      <tr> \
                      <td>%hq</td> \
                      <td>Quote mark character (needed for nested Matlab calls with arguments)</td> \
                      </tr> \
                      <tr> \
                      <td>%hmb</td> \
                      <td>Matlab binary (as defined in server configuration)</td> \
                      </tr> \
                      </tbody>  \
                      </table> \
                      ");

    scrollArea->setWidget(infoText);

    infoDialog->resize(780,WLength(80,Wt::WLength::Percentage));
    infoDialog->show();
}


void ywConfigPageModes::setIndicateModeUpdate(bool status)
{
    // Change the color of the "Update Mode List" button to indicate that the mode
    // list needs to be updated (after changing, adding, or deleting a mode).
    if (status)
    {
        modeListBtn->setAttributeValue("style","background-color:#FF6A13;border-color:#FF6A13;");
    }
    else
    {
        modeListBtn->setAttributeValue("style","background-color:#580F8B;border-color:#580F8B;");
    }
}

