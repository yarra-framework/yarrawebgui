#include "yw_modulemanifest.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace fs = boost::filesystem;


ywModuleManifest::ywModuleManifest()
{
    name          ="";
    version       ="";
    author        ="";
    description   ="";
    homepage      ="";
    downloadURL   ="";
    requiresMatlab=false;
}


bool ywModuleManifest::readManifest(WString filename)
{
    boost::property_tree::ptree manifestFile;

    // Check for existence of the manifest file
    if (!fs::exists(filename.toUTF8()))
    {
        return false;
    }

    try
    {
        // Read all values from the manifest file
        boost::property_tree::ini_parser::read_ini(filename.toUTF8(), manifestFile);

        name       =WString::fromUTF8(manifestFile.get<std::string>("YarraModule.Name",       WString(name).toUTF8()));
        version    =WString::fromUTF8(manifestFile.get<std::string>("YarraModule.Version",    WString(version).toUTF8()));
        author     =WString::fromUTF8(manifestFile.get<std::string>("YarraModule.Author",     WString(author).toUTF8()));
        description=WString::fromUTF8(manifestFile.get<std::string>("YarraModule.Description",WString(description).toUTF8()));
        homepage   =WString::fromUTF8(manifestFile.get<std::string>("YarraModule.Homepage",   WString(homepage).toUTF8()));
        downloadURL=WString::fromUTF8(manifestFile.get<std::string>("YarraModule.DownloadURL",WString(downloadURL).toUTF8()));
        requiresMatlab=manifestFile.get<bool>("YarraModule.RequiresMatlab",requiresMatlab);
    }
    catch(const std::exception & e)
    {
        return false;
    }

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

    infoString= "<div style=\"margin-bottom: 6px; \"><strong>"+name+"</strong>";

    if (requiresMatlab)
    {
        infoString+="&nbsp;&nbsp;&nbsp;&nbsp;<span style=\"background-color: #2f7eb2 !important;\" class=\"label label-info\">MATLAB</span>";
    }
    infoString+="<br /></div>";



    if (!description.empty())
    {
        infoString+="<div style=\"margin-bottom: 6px; \"><i>"+description+"</i></div>";
    }

    infoString+="<table>";

    if (!version.empty())
    {
        infoString+="<tr><td style=\"padding-right: 6px;\">Version: </td><td>"+version+"</td></tr>";
    }
    if (!author.empty())
    {
        infoString+="<tr><td style=\"padding-right: 6px;\" style=\"padding-right: 6px;\">Author: </td><td>"+author+"</td></tr>";
    }
    if (!homepage.empty())
    {
        infoString+="<tr><td style=\"padding-right: 6px;\">Homepage: </td><td><a href=\"http://"+homepage+"\"  target=\"_blank\">"+homepage+"</a></td></tr>";
    }

    infoString+="</table>";

    return infoString;
}



