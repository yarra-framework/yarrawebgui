#ifndef YW_CONFIGPAGE_UPDATE_H
#define YW_CONFIGPAGE_UPDATE_H

#include <Wt/WContainerWidget>
#include <Wt/WFileUpload>
#include <Wt/WProgressBar>
#include <Wt/WPanel>
#include <Wt/WText>

using namespace Wt;


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

    void refresh();

    void updateInfoText();

    void checkForUpdates();
    void installUpdate();

};


#endif // YW_CONFIGPAGE_UPDATE_H
