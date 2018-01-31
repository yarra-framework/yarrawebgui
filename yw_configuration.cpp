#include "yw_configuration.h"
#include "yw_global.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <boost/exception/error_info.hpp>
#include <errno.h>
#include <iostream>


namespace fs = boost::filesystem;


ywConfiguration::ywConfiguration()
{       
    configurationValid=false;

    serverName="Unknown";
    port="8080";

    yarraPath                =".";
    disableModuleInstallation=false;
    disableModeEditing       =false;
    disableSupportForum      =false;
    disableYarraNews         =false;

    initServerConfiguration();
}


void ywConfiguration::initServerConfiguration()
{
    yarraLogPath        ="/log";
    yarraModesPath      ="/modes";
    yarraQueuePath      ="/queue";
    yarraWorkPath       ="/work";
    yarraFailPath       ="/fail";
    yarraStoragePath    ="/finished";
    yarraModulesPath    ="/modules";
    yarraModulesUserPath="/modules_user";
    matlabBinaryPath    ="/usr/local/bin/matlab";
}


ywConfiguration::~ywConfiguration()
{
    while (!users.empty())
    {
        ywUser* item=users.back();
        users.pop_back();
        delete item;
        item=0;
    }

    std::cout << std::endl << "YarraServer WebGUI is going down now." << std::endl << std::endl;
}


void ywConfiguration::loadConfiguration()
{    
    std::cout << std::endl << "Reading YarraServer WebGUI configuration..." << std::endl;

    int maxUser=100;
    int userCount=0;

    WString userName="";
    WString userPassword="";
    int userLevel=0;

    try
    {
        boost::property_tree::ptree inifile;
        boost::property_tree::ini_parser::read_ini(YW_CONFIGFILE, inifile);

        userName=WString::fromUTF8(inifile.get<std::string>( WString("User{1}.Name").arg(userCount+1).toUTF8(),""));

        while ((userCount<maxUser) && (userName!=""))
        {
            userPassword=WString::fromUTF8(inifile.get<std::string>( WString("User{1}.Password").arg(userCount+1).toUTF8(),""));
            userLevel=inifile.get<int>( WString("User{1}.Level").arg(userCount+1).toUTF8(),1);

            // Check if userLevel has a valid range
            if ((userLevel<1) || (userLevel>3))
            {
                userLevel=1;
            }

            ywUser* userEntry=new ywUser;
            userEntry->name=userName;
            userEntry->password=userPassword;
            userEntry->level=userLevel;
            users.push_back(userEntry);

            userCount++;
            userName=WString::fromUTF8(inifile.get<std::string>( WString("User{1}.Name").arg(userCount+1).toUTF8(),""));
        }

        // Read main settings
        yarraPath=WString::fromUTF8(inifile.get<std::string>("Setup.YarraPath","."));
        port=WString::fromUTF8(inifile.get<std::string>("Setup.Port","8080"));

        // Read optional security settings
        disableModuleInstallation=inifile.get<bool>("Setup.DisableModuleInstallation",disableModuleInstallation);
        disableModeEditing       =inifile.get<bool>("Setup.DisableModeEditing"       ,disableModeEditing);
        disableSupportForum      =inifile.get<bool>("Setup.DisableSupportForum"      ,disableSupportForum);
        disableYarraNews         =inifile.get<bool>("Setup.DisableYarraNews"         ,disableYarraNews);

        // Now try to read the yarra config file
        // If the yarra config file cannot be loaded, stop the webgui
        if (!loadServerConfiguration())
        {
            configurationValid=false;
            return;
        }

        configurationValid=true;
    }
    catch(const boost::property_tree::ptree_error &e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
        configurationValid=false;
    }

    std::cout << std::endl;
}


bool ywConfiguration::loadServerConfiguration()
{
    // Important: This will reset the server paths to the default values, which will merged with the server location
    //            below. It's important to reinitialize whenever the configuration is refreshed.
    initServerConfiguration();

    try
    {
        WString serverIniFilename=yarraPath+"/"+YW_YARRACONFIG;

        if (!fs::exists(serverIniFilename.toUTF8()))
        {
            std::cout << "ERROR: Can't find YarraServer configuration file " << serverIniFilename << std::endl;
            std::cout << "       Has the path to the YarraServer installation been set correctly?" << std::endl;
            return false;
        }

        boost::property_tree::ptree serverIni;
        boost::property_tree::ini_parser::read_ini(serverIniFilename.toUTF8(), serverIni);
        serverName=WString::fromUTF8(serverIni.get<std::string>("Server.Name","Unknown"));

        // Note: Here the full path is assembled as default value (base path + subfolder)
        yarraLogPath        =WString::fromUTF8(serverIni.get<std::string>("Paths.Log"        ,WString(yarraPath+yarraLogPath        ).toUTF8()));
        yarraModesPath      =WString::fromUTF8(serverIni.get<std::string>("Paths.Modes"      ,WString(yarraPath+yarraModesPath      ).toUTF8()));
        yarraQueuePath      =WString::fromUTF8(serverIni.get<std::string>("Paths.Queue"      ,WString(yarraPath+yarraQueuePath      ).toUTF8()));
        yarraWorkPath       =WString::fromUTF8(serverIni.get<std::string>("Paths.Work"       ,WString(yarraPath+yarraWorkPath       ).toUTF8()));
        yarraFailPath       =WString::fromUTF8(serverIni.get<std::string>("Paths.Fail"       ,WString(yarraPath+yarraFailPath       ).toUTF8()));
        yarraStoragePath    =WString::fromUTF8(serverIni.get<std::string>("Paths.Storage"    ,WString(yarraPath+yarraStoragePath    ).toUTF8()));
        yarraModulesPath    =WString::fromUTF8(serverIni.get<std::string>("Paths.Modules"    ,WString(yarraPath+yarraModulesPath    ).toUTF8()));
        yarraModulesUserPath=WString::fromUTF8(serverIni.get<std::string>("Paths.ModulesUser",WString(yarraPath+yarraModulesUserPath).toUTF8()));

        matlabBinaryPath=WString::fromUTF8(serverIni.get<std::string>("Paths.MatlabBinary",WString(matlabBinaryPath).toUTF8()));
    }
    catch(const boost::property_tree::ptree_error &e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
        return false;
    }

    return true;
}


ywUser* ywConfiguration::validateUser(WString name, WString password)
{
    // Function will return a null pointer if the user does not exists
    // or the password is wrong
    ywUser* userEntry=0;

    for (int i=0; i<users.size(); i++)
    {
        if (users.at(i)->name==name)
        {
            userEntry=users.at(i);
            break;
        }
    }

    if (userEntry==0)
    {
        // A user with this name does not exist
        return 0;
    }

    if (userEntry->password==password)
    {
        return userEntry;
    }
    else
    {
        // Password is incorrect
        return 0;
    }
}


