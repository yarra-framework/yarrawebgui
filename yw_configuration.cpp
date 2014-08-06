#include "yw_configuration.h"
#include "yw_global.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <boost/exception/error_info.hpp>
#include <errno.h>
#include <iostream>

ywConfiguration::ywConfiguration()
{       
    configurationValid=false;

    yarraPath=".";
    serverName="Unknown";
    port="8080";
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

        yarraPath=WString::fromUTF8(inifile.get<std::string>("Setup.YarraPath","."));
        port=WString::fromUTF8(inifile.get<std::string>("Setup.Port","8080"));

        // Now try to read the yarra config file
        boost::property_tree::ptree serverIni;
        WString serverFilename=yarraPath+"/"+YW_YARRACONFIG;
        boost::property_tree::ini_parser::read_ini(serverFilename.toUTF8(), serverIni);
        serverName=WString::fromUTF8(serverIni.get<std::string>("Server.Name","Unknown"));
        serverType=WString::fromUTF8(serverIni.get<std::string>("Server.Type","Unknown"));

        configurationValid=true;
    }
    catch(const boost::property_tree::ptree_error &e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
        configurationValid=false;
    }

    std::cout << std::endl;
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


