#include "yw_configpage_update.h"
#include "yw_servermanifest.h"
#include "yw_application.h"

#include <Wt/WMessageBox>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WPushButton>
#include <Wt/WImage>


ywConfigPageUpdate::ywConfigPageUpdate(ywConfigPage* pageParent)
{
    parent=pageParent;

    Wt::WVBoxLayout* subLayout = new Wt::WVBoxLayout();
    this->setLayout(subLayout);
    subLayout->setContentsMargins(0, 0, 0, 0);

    // Add heading
    Wt::WText* head=new Wt::WText("<h3>Server Information</h3>");
    head->setMargin(16, Wt::Bottom);
    subLayout->addWidget(head,0);

    // Add version information with logo
    WContainerWidget* infoContainer=new WContainerWidget();
    Wt::WHBoxLayout* infoLayout = new Wt::WHBoxLayout();
    infoContainer->setLayout(infoLayout);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(4);

    Wt::WImage* logoImage=new Wt::WImage("style/yarra_logo.png");
    logoImage->setMinimumSize(87,75);
    logoImage->setMaximumSize(87,75);
    infoLayout->addWidget(logoImage,0);
    infoLayout->addSpacing(20);

    infoText=new Wt::WText("");
    infoLayout->addWidget(infoText,1);
    subLayout->addWidget(infoContainer,0);
    subLayout->addSpacing(10);

    // Add into text with link to homepage
    WString homepageTextContent="Documentation and release history available at <a href=\"http://yarra.rocks\" target=\"_blank\">http://yarra.rocks</a>.<br />";
    homepageTextContent+="Visit the <a href=\"https://yarra.rocks/forum/index.php\" target=\"_blank\">Yarra Forum</a> to get support and provide feedback.";
    Wt::WText* homepageText=new Wt::WText(homepageTextContent);
    subLayout->addWidget(homepageText,0);

    // Add button for checking for updates
    WContainerWidget* checkUpdateContainer=new WContainerWidget();
    Wt::WHBoxLayout* checkUpdateLayout=new Wt::WHBoxLayout();
    checkUpdateContainer->setLayout(checkUpdateLayout);
    checkUpdateLayout->setContentsMargins(0, 0, 0, 0);
    checkUpdateLayout->setSpacing(4);
    checkUpdateContainer->setMargin(30, Wt::Top);

    checkUpdatesBtn=new Wt::WPushButton("Check for Available Update");
    checkUpdatesBtn->setStyleClass("btn-sm");
    checkUpdatesBtn->clicked().connect(std::bind([=] () {
        checkForUpdates();
    }));
    checkUpdateLayout->addWidget(checkUpdatesBtn, 0);
    checkUpdateLayout->addStretch(1);

    subLayout->addWidget(checkUpdateContainer, 0);

    // Add label for informing about available updates
    updateText=new Wt::WText("");
    // Add CSS class to get bottom border
    updateText->addStyleClass("log-frame");
    updateText->setMargin(30, Wt::Top);
    updateText->setMargin(10, Wt::Bottom);
    subLayout->addWidget(updateText,1);

    // Add panel with button to install update
    WContainerWidget* innerBtnContainer=new WContainerWidget();
    Wt::WHBoxLayout* innerLayout = new Wt::WHBoxLayout();
    innerBtnContainer->setLayout(innerLayout);
    innerLayout->setContentsMargins(0, 0, 0, 0);
    innerLayout->setSpacing(4);

    installUpdateBtn=new Wt::WPushButton("Install Server Update");
    installUpdateBtn->setStyleClass("btn-primary");
    installUpdateBtn->clicked().connect(std::bind([=] () {
        installUpdate();
    }));

    innerLayout->insertWidget(0,installUpdateBtn,0,Wt::AlignLeft);
    innerLayout->insertWidget(1,new Wt::WContainerWidget,1);

    subLayout->addWidget(innerBtnContainer,0);
}


void ywConfigPageUpdate::refresh()
{
    // Update the version information
    updateInfoText();
}


void ywConfigPageUpdate::updateInfoText()
{
    ywServerManifest manifest;
    WString text="";

    if (!manifest.readManifest(parent->app->configuration->yarraPath))
    {
        text="ERROR: Server information not available.";
        checkUpdatesBtn->setDisabled(true);
        installUpdateBtn->setDisabled(true);
    }
    else
    {
        text=manifest.renderInformation();
    }

    infoText->setText(text);
}


void ywConfigPageUpdate::checkForUpdates()
{
    WString resultText="";

    resultText="<p><span class=\"label label-success\">Up-to-date</span>&nbsp; This server is up to date.</p>";
    resultText="<p><span class=\"label label-warning\">Update Available</span>&nbsp; An update for this server is available (Version 0.3).</p> <p><a href=\"https://yarra.rocks/doc/download\" target=\"_blank\">Download the update from the Yarra Page</a>, and install it by clicking the button at the bottom.</p>";
    resultText="<p><span class=\"label label-danger\">No Connection</span>&nbsp; Unable to retrieve update information.</p>";

    // TODO

    updateText->setText(resultText);
}


void ywConfigPageUpdate::installUpdate()
{
    // TODO

    Wt::WMessageBox::show("Coming Soon", "This function has not been implemented yet.", Wt::Ok);
}

