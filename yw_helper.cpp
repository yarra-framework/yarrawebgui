#include "yw_helper.h"

#include <Wt/WString>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <boost/exception/error_info.hpp>

#include <iostream>
#include <fstream>
#include <ctime>
#include <time.h>


#include <yw_global.h>


namespace fs = boost::filesystem;


ywHelper::ywHelper()
{
}


bool ywHelper::lockFile(Wt::WString fullFilename)
{
    fs::path fp(fullFilename.toUTF8());
    fp.replace_extension(YW_EXT_LOCK);

    // File already locked
    if (fs::exists(fp))
    {
        return false;
    }

    std::ofstream lockfile(fp.generic_string());
    lockfile << "LOCK" << std::endl;
    lockfile.close();

    // Check if file exists now
    if (!fs::exists(fp))
    {
        return false;
    }

    return true;
}


bool ywHelper::unlockFile(Wt::WString fullFilename)
{
    fs::path fp(fullFilename.toUTF8());
    fp.replace_extension(YW_EXT_LOCK);

    bool result=false;

    try
    {
        if (!fs::exists(fp))
        {
            return true;
        }

        result=(fs::remove(fp));
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        result=false;
    }

    return result;
}


bool ywHelper::isLocked(Wt::WString fullFilename)
{
    fs::path fp(fullFilename.toUTF8());
    fp.replace_extension(YW_EXT_LOCK);

    if (fs::exists(fp))
    {
        return true;
    }

    return false;
}


bool ywHelper::isFolderLocked(Wt::WString fullPath)
{
    const std::string& ext_lock =YW_EXT_LOCK;

    fs::path folderDir(fullPath.toUTF8());
    fs::directory_iterator folder_iter;

    bool lockFileFound=false;

    try
    {
        if ( fs::exists(folderDir) && fs::is_directory(folderDir))
        {
            for( fs::directory_iterator dir_iter(folderDir) ; dir_iter != folder_iter ; ++dir_iter)
            {
                if ( (fs::is_regular_file(dir_iter->status())) &&   (dir_iter->path().extension()==ext_lock) )
                {
                    lockFileFound=true;
                    break;
                }
            }
        }
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
    }

    return lockFileFound;
}

