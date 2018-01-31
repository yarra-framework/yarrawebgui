#include "yw_servermanifest.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <sstream>
#include <locale>


namespace fs = boost::filesystem;


ywServerManifest::ywServerManifest(WString yarraPath)
{
    localYarraPath=yarraPath;

    id="";
    version="0";
    releaseDate="1/1/2016";
    versionServerCore="0";
    versionWebGUI="0";
    minimumVersionForUpdate="";
    buildOS="";
    filesToRemoveForUpdate.clear();
}


bool ywServerManifest::readManifest(WString filename)
{
    // If no filename is given as argument, read the manifest file
    // from the local Yarra installation
    if (filename.empty())
    {
        filename=localYarraPath+"/"+YW_SERVER_MANIFEST;
    }

    // Check if the YarraServer manifest file can be found
    if (!fs::exists(filename.toUTF8()))
    {
        return false;
    }


    boost::property_tree::ptree manifestFile;

    try
    {
        // Read the values from the manifest file
        boost::property_tree::ini_parser::read_ini(filename.toUTF8(), manifestFile);

        // Check if the file is a valid server manifest file
        id=WString::fromUTF8(manifestFile.get<std::string>("YarraServer.ID",WString("").toUTF8()));

        if (id!=WString(YW_SERVER_MANIFEST_ID))
        {
            return false;
        }

        // Read all entries of the manifest
        version          =WString::fromUTF8(manifestFile.get<std::string>("YarraServer.Version"          ,WString(version).toUTF8()));
        releaseDate      =WString::fromUTF8(manifestFile.get<std::string>("YarraServer.ReleaseDate"      ,WString(releaseDate).toUTF8()));
        versionServerCore=WString::fromUTF8(manifestFile.get<std::string>("YarraServer.VersionServerCore",WString(versionServerCore).toUTF8()));
        versionWebGUI    =WString::fromUTF8(manifestFile.get<std::string>("YarraServer.VersionWebGUI"    ,WString(versionWebGUI).toUTF8()));
        buildOS          =WString::fromUTF8(manifestFile.get<std::string>("YarraServer.BuildOS"          ,WString(buildOS).toUTF8()));

        minimumVersionForUpdate=WString::fromUTF8(manifestFile.get<std::string>("YarraServer.MinimumVersionForUpdate",WString(minimumVersionForUpdate).toUTF8()));

        // Read the list of files that should be removed before installing an update
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, manifestFile.get_child("RemoveForUpdate"))
        {
            filesToRemoveForUpdate.push_back(WString::fromUTF8(boost::lexical_cast<std::string>(v.first.data())));
        }
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

#ifdef UBUNTU_1604
    infoString+="<p style=\"line-height: 1.3; \">Build for Ubuntu 16.04 LTS<br />";
#else
    infoString+="<p style=\"line-height: 1.3; \">Build for Ubuntu 12.04 / 14.04 LTS<br />";
#endif

    infoString+="Released on "+releaseDate+"</p>";
    infoString+="<p style=\"line-height: 1.3; \">Server Core v"+versionServerCore+"<br />";
    infoString+="WebGUI v"+versionWebGUI+"</p>";

    return infoString;
}


bool ywServerManifest::requiresUpdate(WString latestVersionString)
{
    // Convert version strings into float values
    float currentVersion=versionStringToFloat(version);
    float latestVersion =versionStringToFloat(latestVersionString);

    // Compare the float values. Update only if the given version is larger
    return (latestVersion>currentVersion);
}


bool ywServerManifest::canUpdateVersion(WString installedVersionString)
{
    // Convert version strings into float values and remove possible characters
    float minimumVersionNeeded=versionStringToFloat(minimumVersionForUpdate);
    float installedVersion=versionStringToFloat(installedVersionString);

    // Compare float values. If installed version is too late, the update
    // cannot be performed automatically
    if (installedVersion<minimumVersionNeeded)
    {
        return false;
    }

    return true;
}


float ywServerManifest::versionStringToFloat(WString versionString)
{
    float versionFloat=0.0f;

    // Remove characters from version string (in case a beta tag has been added)
    std::string versionStdString=versionString.toUTF8();
    boost::remove_erase_if(versionStdString, !boost::is_any_of("0123456789."));

    // Convert into float, independent of locale
    std::istringstream versionStr(versionStdString);
    versionStr.imbue(std::locale("C"));
    versionStr >> versionFloat;

    return versionFloat;
}


bool ywServerManifest::checkBuildOS()
{
    bool buildOSMatches=false;

#ifdef UBUNTU_1604
    if (buildOS=="1604")
    {
        buildOSMatches=true;
    }
#else
    if ((buildOS=="") || (buildOS=="1404") || (buildOS=="1204"))
    {
        buildOSMatches=true;
    }
#endif

    return buildOSMatches;
}

