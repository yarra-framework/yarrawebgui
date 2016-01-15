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

using namespace Wt;


class ywConfigPage;
class ywServerManifest;

class ZipArchive;


class ywConfigPageUpdate : public Wt::WContainerWidget
{
public:
    ywConfigPageUpdate(ywConfigPage* pageParent);

    ywConfigPage* parent;

    Wt::WText* infoText;
    Wt::WText* updateText;
    Wt::WPushButton* checkUpdatesBtn;
    Wt::WPushButton* installUpdateBtn;
    Wt::WTimer* uiUpdateTimer;
    Wt::WDialog* uploadModuleDialog;

    void refreshPage();

    void updateInfoText();

    void checkForUpdates();
    void installUpdate();

    void handleHttpResponse(boost::system::error_code error, const Http::Message& response);

    void checkUploadedFile(WString uploadedFilename, WString originalFilename);

    bool removeInstalledVersion(ywServerManifest& installedManifest);
    bool installUpdate(ywServerManifest& updateManifest, std::shared_ptr<ZipArchive> zipFile);

};


#endif // YW_CONFIGPAGE_UPDATE_H
