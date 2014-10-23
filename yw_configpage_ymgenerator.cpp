#include "yw_configpage.h"
#include "yw_configpage_ymgenerator.h"
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
#include <Wt/WComboBox>
#include <Wt/WLabel>
#include <Wt/WRegExpValidator>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <boost/exception/error_info.hpp>
#include <boost/foreach.hpp>

#include <sys/types.h>
#include <sys/stat.h>

namespace fs = boost::filesystem;

ywConfigPageYMGenerator::~ywConfigPageYMGenerator()
{
    freeModeList();
}


void ywConfigPageYMGenerator::freeModeList()
{
    if (orderedModeList.empty())
    {
        return;
    }

    // Discard all entries in the mode list and clear it
    for (ywcModeList::iterator ii=orderedModeList.begin(); ii!=orderedModeList.end(); ++ii)
    {
        delete (*ii).second;
        (*ii).second=0;
    }
    orderedModeList.clear();
}


ywConfigPageYMGenerator::ywConfigPageYMGenerator(ywApplication* parent)
{
    app=parent;
}


void ywConfigPageYMGenerator::showErrorMessage(WString errorMessage)
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



void ywConfigPageYMGenerator::perform()
{
    modeFiles.clear();

    fs::path modeDir(app->configuration->yarraModesPath.toUTF8());
    fs::directory_iterator end_iter;

    typedef std::vector<Wt::WString> result_set_t;

    try
    {
        if ( fs::exists(modeDir) && fs::is_directory(modeDir))
        {
            for( fs::directory_iterator dir_iter(modeDir) ; dir_iter != end_iter ; ++dir_iter)
            {
                if ( (fs::is_regular_file(dir_iter->status()) && (dir_iter->path().extension()==YW_EXT_MODE) ))
                {
                    modeFiles.push_back(WString::fromUTF8(dir_iter->path().stem().generic_string()));
                }
            }
        }
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
    }

    bool lockError=false;
    WString fullName="";

    // First check if any of the lock files is currently locked
    for (size_t i=0; i<modeFiles.size(); i++)
    {
        fullName=app->configuration->yarraModesPath+"/"+modeFiles.at(i)+YW_EXT_MODE;
        if (ywHelper::isLocked(fullName.toUTF8()))
        {
            lockError=true;
        }
    }

    // If yes, another user is working on the mode file. So we can't go on right now.
    if (lockError)
    {
        showErrorMessage("Some files are currently edited by another user.");
        return;
    }

    // Now lock all mode files
    for (size_t i=0; i<modeFiles.size(); i++)
    {
        fullName=app->configuration->yarraModesPath+"/"+modeFiles.at(i)+YW_EXT_MODE;
        if (!ywHelper::lockFile(fullName.toUTF8()))
        {
            lockError=true;
        }
    }
    // If locking the files results in an error, create error message, unlock all files again
    // and return;
    if (lockError)
    {
        showErrorMessage("Locking the mode files was not possible.");
        for (size_t i=0; i<modeFiles.size(); i++)
        {
            fullName=app->configuration->yarraModesPath+"/"+modeFiles.at(i)+YW_EXT_MODE;
            ywHelper::unlockFile(fullName.toUTF8());
        }
        return;
    }

    // Do the action!
    if (!parseModeFiles())
    {
        showErrorMessage("Some of the mode files do not contain a [ClientConfig] section. \
                         The YarraModes.cfg file can only be generated and populated if \
                         these sections are present in all files.");
    }
    else
    {
        generateYMFile();
        showModeTable();
    }

    freeModeList();

    // Unlock all mode file
    for (size_t i=0; i<modeFiles.size(); i++)
    {
        fullName=app->configuration->yarraModesPath+"/"+modeFiles.at(i)+YW_EXT_MODE;
        ywHelper::unlockFile(fullName.toUTF8());
    }
}



bool ywConfigPageYMGenerator::parseModeFiles()
{
    bool parseError=false;

    for (size_t i=0; i<modeFiles.size(); i++)
    {
        WString fileName=app->configuration->yarraModesPath+"/"+modeFiles.at(i)+YW_EXT_MODE;
        boost::property_tree::ptree modecontent;
        boost::property_tree::ini_parser::read_ini(fileName.toUTF8(), modecontent);

        try
        {
            bool isDisabled=modecontent.get<bool>("ClientConfig.Disabled",false);

            if (!isDisabled)
            {
                int sortIndex=modecontent.get<int>("ClientConfig.SortIndex",0);

                ywcModeEntry* item=new ywcModeEntry;
                orderedModeList.insert(ywcModeList::value_type(sortIndex, item));
                item->ID=modeFiles.at(i);

                item->name=WString::fromUTF8(modecontent.get<std::string>("ClientConfig.Name",""));
                item->tag= WString::fromUTF8(modecontent.get<std::string>("ClientConfig.Tag",""));

                WString entry="";
                WString value="";

                BOOST_FOREACH(boost::property_tree::ptree::value_type &v, modecontent.get_child("ClientConfig"))
                {
                    entry=WString::fromUTF8(v.first.data());
                    value=WString::fromUTF8(v.second.data());

                    if ((entry!="Disabled") && (value!="SortIndex"))
                    {
                        item->entries.push_back(entry);
                        item->values.push_back(value);
                    }
                }
            }
        }
        catch(const boost::property_tree::ptree_error &e)
        {
            parseError=true;
            break;
        }
    }

    return !parseError;
}


bool ywConfigPageYMGenerator::generateYMFile()
{
    WString fileName=app->configuration->yarraQueuePath+"/YarraModes.cfg";

    if (!fs::exists(fileName.toUTF8()))
    {
        showErrorMessage("Cannot find mode list.");
        return false;
    }

    // First, create a lockfile in the queue directory
    if (!ywHelper::lockFile(fileName.toUTF8()))
    {
        showErrorMessage("Locking the file not possible. The file is probably being edited by another user.");
        return false;
    }

    if (chmod(fileName.toUTF8().data(),S_IRUSR | S_IWUSR) != 0)
    {
        showErrorMessage("Cannot change write lock of mode list. Check file permissions.");
        ywHelper::unlockFile(fileName.toUTF8());
        return false;
    }

    try
    {
        boost::property_tree::ptree modefile;

        // First write the list of available modes
        int count=0;
        for (ywcModeList::iterator ii=orderedModeList.begin(); ii!=orderedModeList.end(); ++ii)
        {
            modefile.put(WString("Modes.{1}").arg(count).toUTF8(), (*ii).second->ID);
            count++;
        }

        // Now write individual sections
        for (ywcModeList::iterator ii=orderedModeList.begin(); ii!=orderedModeList.end(); ++ii)
        {
            WString prefix=(*ii).second->ID+".";

            for (size_t i=0; i<(*ii).second->entries.size(); i++)
            {
                modefile.put(WString(prefix+(*ii).second->entries.at(i)).toUTF8(), (*ii).second->values.at(i).toUTF8());
            }
        }


        boost::property_tree::ini_parser::write_ini(fileName.toUTF8(), modefile);
    }
    catch(const boost::property_tree::ptree_error &e)
    {
        //error=true;
        showErrorMessage("Error writing mode list.");
    }

    // chmod 440 on file name
    chmod(fileName.toUTF8().data(),S_IRUSR | S_IRGRP);
    ywHelper::unlockFile(fileName.toUTF8());

    return true;
}


void ywConfigPageYMGenerator::showModeTable()
{
    Wt::WDialog *infoDialog = new Wt::WDialog("Generated Mode List");
    infoDialog->setResizable(true);
    infoDialog->setModal(true);
    infoDialog->rejectWhenEscapePressed();

    Wt::WPushButton *ok = new Wt::WPushButton("Close", infoDialog->footer());
    ok->setDefault(true);

    ok->clicked().connect(std::bind([=] () {
        infoDialog->accept();
    }));

    infoDialog->finished().connect(std::bind([=] () {
        delete infoDialog;
    }));

    Wt::WVBoxLayout* scrollLayout = new Wt::WVBoxLayout();
    infoDialog->contents()->setLayout(scrollLayout);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    Wt::WText* infoTitle = new Wt::WText();
    infoTitle->setText("The mode list (YarraModes.cfg) has been generated and contains the following reconstruction modes:");
    infoTitle->setMargin(10, Wt::Bottom);
    scrollLayout->addWidget(infoTitle,0);

    // Add table with content, adapt size of window
    Wt::WScrollArea* scrollArea = new Wt::WScrollArea();
    scrollLayout->addWidget(scrollArea,1);

    Wt::WText* infoText = new Wt::WText();
    infoText->setWordWrap(false);

    WString listContent="<table class=\"table table-striped table-hover\"> \
            <tbody> \
            <tr> \
            <td><strong>Tag</strong></td> \
            <td><strong>Name</strong></td> \
            <td><strong>File</strong></td> \
            </tr>";

    for (ywcModeList::iterator ii=orderedModeList.begin(); ii!=orderedModeList.end(); ++ii)
    {
        listContent+=WString("<tr><td>{1}</td><td>{2}</td><td>{3}.mode</td></tr>").arg((*ii).second->tag).arg((*ii).second->name).arg((*ii).second->ID);
    }

    listContent+="</tbody></table>";
    infoText->setText(listContent);

    scrollArea->setWidget(infoText);
    infoDialog->resize(800,WLength(80,Wt::WLength::Percentage));
    //infoDialog->setMaximumSize(WLength(90,Wt::WLength::Percentage),WLength(90,Wt::WLength::Percentage));
    infoDialog->show();
}

