#ifndef YW_CONFIGPAGE_UPDATE_H
#define YW_CONFIGPAGE_UPDATE_H

#include <Wt/WContainerWidget>
#include <Wt/WFileUpload>
#include <Wt/WProgressBar>
#include <Wt/WPanel>
#include <Wt/WText>
#include <Wt/WText>
#include <Wt/Http/Client>

using namespace Wt;

#include <boost/system/error_code.hpp>



class ywConfigPage;

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

    void refreshPage();

    void updateInfoText();

    void checkForUpdates();
    void installUpdate();

    void handleHttpResponse(boost::system::error_code error, const Http::Message& response);

};


#endif // YW_CONFIGPAGE_UPDATE_H
