#include "yw_modulemanifest.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace fs = boost::filesystem;


ywModuleManifest::ywModuleManifest()
{
    name="";
    version="";
    author="";
    description="";
    homepage="";
    downloadURL="";
}



bool ywModuleManifest::readManifest(WString filename)
{
    boost::property_tree::ptree manifestFile;

    // Check if the yarra config file can be found! Otherwise, stop the webgui
    if (!fs::exists(filename.toUTF8()))
    {
        return false;
    }

    boost::property_tree::ini_parser::read_ini(filename.toUTF8(), manifestFile);

    name=WString::fromUTF8(manifestFile.get<std::string>("YarraModule.Name",WString(name).toUTF8()));
    version=WString::fromUTF8(manifestFile.get<std::string>("YarraModule.Version",WString(version).toUTF8()));
    author=WString::fromUTF8(manifestFile.get<std::string>("YarraModule.Author",WString(author).toUTF8()));
    description=WString::fromUTF8(manifestFile.get<std::string>("YarraModule.Description",WString(description).toUTF8()));
    homepage=WString::fromUTF8(manifestFile.get<std::string>("YarraModule.Homepage",WString(homepage).toUTF8()));
    downloadURL=WString::fromUTF8(manifestFile.get<std::string>("YarraModule.DDownloadURL",WString(downloadURL).toUTF8()));

    // If no name is provided in the manifest, use the filename
    if (name.empty())
    {
        try
        {
            // Extract filename from path
            boost::filesystem::path fileLocation = boost::filesystem::path{filename.toUTF8()};
            name=fileLocation.stem().generic_string();
        }
        catch(const std::exception & e)
        {
            name="Unknown";
        }
    }

    return true;
}


WString ywModuleManifest::renderInformation()
{
    WString infoString="";

    WString displayName=name;
    if (displayName.empty())
    {

    }

    infoString= "<strong>"+displayName+"</strong><br />";
    infoString+="<table>";
    infoString+="<tr><td>Version:</td><td>"+version+"</td></tr>";
    infoString+="</table>";


    return infoString;
}



