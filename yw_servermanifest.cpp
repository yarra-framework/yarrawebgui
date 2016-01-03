#include "yw_global.h"
#include "yw_servermanifest.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <iostream>
#include <sstream>
#include <locale>


namespace fs = boost::filesystem;


ywServerManifest::ywServerManifest()
{
    version="0";
    releaseDate="1/1/2016";
    versionServerCore="0";
    versionWebGUI="0";
}


bool ywServerManifest::readManifest(WString yarraPath)
{
    boost::property_tree::ptree manifestFile;
    WString filename=yarraPath+"/"+YW_SERVER_MANIFEST;

    // Check if the YarraServer manifest file can be found
    if (!fs::exists(filename.toUTF8()))
    {
        return false;
    }

    try
    {
        // Read the values from the manifest file
        boost::property_tree::ini_parser::read_ini(filename.toUTF8(), manifestFile);

        version          =WString::fromUTF8(manifestFile.get<std::string>("YarraServer.Version"          ,WString(version).toUTF8()));
        releaseDate      =WString::fromUTF8(manifestFile.get<std::string>("YarraServer.ReleaseDate"      ,WString(releaseDate).toUTF8()));
        versionServerCore=WString::fromUTF8(manifestFile.get<std::string>("YarraServer.VersionServerCore",WString(versionServerCore).toUTF8()));
        versionWebGUI    =WString::fromUTF8(manifestFile.get<std::string>("YarraServer.VersionWebGUI"    ,WString(versionWebGUI).toUTF8()));
    }
    catch(const std::exception & e)
    {
        return false;
    }

    return true;
}


WString ywServerManifest::renderInformation()
{
    WString infoString="";

    infoString ="<div style=\"margin-bottom: 6px; margin-top: -4px; \"><h3 style=\"font-size: 21px; font-weight: 400;\">YarraServer&nbsp;- Version "+version+"</h3></div>";
    infoString+="<p style=\"line-height: 1.3; \">Released on "+releaseDate+"</p>";
    infoString+="<p style=\"line-height: 1.3; \">Server Core v"+versionServerCore+"<br />";
    infoString+="WebGUI v"+versionWebGUI+"</p>";

    return infoString;
}


bool ywServerManifest::requiresUpdate(WString latestVersionString)
{
    float currentVersion=0.0f;
    float latestVersion =0.0f;

    // Remove characeters from version strings (in case a beta tag has been added)
    std::string currentStdString=version.toUTF8();
    boost::remove_erase_if(currentStdString, !boost::is_any_of("0123456789."));

    std::string latestStdString=latestVersionString.toUTF8();
    boost::remove_erase_if(latestStdString,  !boost::is_any_of("0123456789."));

    // Convert into float, independent of locale
    std::istringstream currentStr(currentStdString);
    currentStr.imbue(std::locale("C"));
    currentStr >> currentVersion;

    std::istringstream latestStr(latestStdString);
    latestStr.imbue(std::locale("C"));
    latestStr >> latestVersion;

    // Compare the float number. Update only if the given version is larger
    return (latestVersion>currentVersion);
}


