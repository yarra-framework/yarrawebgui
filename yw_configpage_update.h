#ifndef YW_CONFIGPAGE_UPDATE_H
#define YW_CONFIGPAGE_UPDATE_H

#include <Wt/WContainerWidget>
#include <Wt/WFileUpload>
#include <Wt/WProgressBar>
#include <Wt/WPanel>
#include <Wt/WText>
#include <Wt/WText>
#include <Wt/Http/Client>

#include <boost/system/error_code.hpp>

#include "yw_global.h"

using namespace Wt;


class ywConfigPage;
class ywServerManifest;

class ZipArchive;


class ywConfigPageUpdate : public Wt::WContainerWidget
{
public:

    enum LogMessageType { INFO, OK, WARNING, ERROR };

    ywConfigPageUpdate(ywConfigPage* pageParent);

    ywConfigPage* parent;

    Wt::WText* infoText;
    Wt::WText* updateText;
    Wt::WPushButton* checkUpdatesBtn;
    Wt::WPushButton* installUpdateBtn;
    Wt::WTimer* uiUpdateTimer;
    Wt::WDialog* uploadModuleDialog;

    WStringList updateLog;

    void refreshPage();
    void updateInfoText();

    void checkForUpdates();
    void installUpdate();

    void handleHttpResponse(boost::system::error_code error, const Http::Message& response);
    void checkUploadAndUpdate(WString uploadedFilename, WString originalFilename);

    bool removeInstalledVersion(ywServerManifest& installedManifest);
    bool installUpdate(ywServerManifest& updateManifest, std::shared_ptr<ZipArchive> zipFile, std::string zipFilename);
    std::string getAbsoluteInstallationPath(std::string pathInPackage);
    void showUpdateResult(bool isSuccess, WString newVersionString="");
    bool isSufficientDiskSpaceAvailable(size_t neededSpace);

    void ulog(WString message, LogMessageType type=INFO);

    void showRestartMessage();
};


inline void ywConfigPageUpdate::ulog(WString message, LogMessageType type)
{
    WString prefix  ="";
    WString preHTML ="";
    WString postHTML="";

    switch (type)
    {
    case OK:
        prefix="OK";
        preHTML ="<span style=\"color: #fff; background: #3fb618; padding-left: 4px; padding-right: 4px;\">";
        postHTML="</span>&nbsp;";
        message=" "+message;
        break;

    case WARNING:
        prefix="WARNING";
        preHTML ="<span style=\"color: #fff; background: #ff7518; padding-left: 4px; padding-right: 4px;\">";
        postHTML="</span>&nbsp;";
        message=" "+message;
        break;

    case ERROR:
        prefix="ERROR";
        preHTML ="<span style=\"color: #fff; background: #ff0039; padding-left: 4px; padding-right: 4px;\">";
        postHTML="</span>&nbsp;";
        message=" "+message;
        break;

    case INFO:
    default:
        break;
    }

    // Display message on screen (captured in upstart log)
    std::cout << prefix.toUTF8() << message.toUTF8() << std::endl;

    // Store message in internal log, so that it can be displayed to the end user
    // Enclose HTML pre/post statements to highlight errors and warnings
    updateLog.push_back(preHTML+prefix+postHTML+message);
}



#endif // YW_CONFIGPAGE_UPDATE_H
