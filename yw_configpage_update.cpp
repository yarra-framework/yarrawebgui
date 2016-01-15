#include "yw_configpage_update.h"
#include "yw_servermanifest.h"
#include "yw_application.h"

#include <Wt/WMessageBox>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WPushButton>
#include <Wt/WImage>
#include <Wt/WTimer>
#include <Wt/WException>
#include <Wt/Json/Parser>
#include <Wt/Json/Object>
#include <Wt/Json/Value>

#include <iostream>
#include <sys/statvfs.h>
#include <csignal>

#include <ZipLib/ZipFile.h>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

#define YW_UPDATE_URL     "https://yarra.rocks/release/version.json"
#define YW_YARRAROCKS_URL "https://yarra.rocks"


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

    // The update dialog will be created later
    uploadModuleDialog=0;
}


void ywConfigPageUpdate::refreshPage()
{
    // Update the version information
    updateInfoText();
    updateText->setText("");
}


void ywConfigPageUpdate::updateInfoText()
{
    WString text="";
    ywServerManifest manifest(parent->app->configuration->yarraPath);

    if (!manifest.readManifest())
    {
        text="ERROR: Server information not available.";
        checkUpdatesBtn->setDisabled(true);
        installUpdateBtn->setDisabled(true);
    }
    else
    {
        text=manifest.renderInformation();
        checkUpdatesBtn->setDisabled(false);
        installUpdateBtn->setDisabled(false);
    }

    infoText->setText(text);
}


void ywConfigPageUpdate::checkForUpdates()
{
    parent->app->enableUpdates(true);
    checkUpdatesBtn->setDisabled(true);

    Http::Client *httpClient = new Http::Client(this);

    httpClient->done().connect(boost::bind(&ywConfigPageUpdate::handleHttpResponse, this, _1, _2));

    if (!httpClient->get(YW_UPDATE_URL))
    {
        WString resultText="";
        resultText="<p><span class=\"label label-danger\">No Connection</span>&nbsp; Unable to open connection.</p>";
        updateText->setText(resultText);

        checkUpdatesBtn->setDisabled(true);
        parent->app->enableUpdates(false);
    }
}


void ywConfigPageUpdate::handleHttpResponse(boost::system::error_code error, const Http::Message& response)
{
    WString resultText="";

    if ((!error) && (response.status()==200))
    {
        Json::Object jsonContent;

        try
        {
            Json::parse(response.body(),jsonContent);
            WString latestVersion=jsonContent.get("server_version");
            WString latestLink=jsonContent.get("server_link");
            WString latestURL=YW_YARRAROCKS_URL+latestLink;

            // Read local manifest file
            ywServerManifest manifest(parent->app->configuration->yarraPath);
            if (manifest.readManifest())
            {
                if (manifest.requiresUpdate(latestVersion))
                {
                    resultText= "<p><span class=\"label label-warning\">Update Available</span>&nbsp;&nbsp; An update for this server is available (Version "+latestVersion+").</p>";
                    resultText+="<p><a href=\""+latestURL+"\" target=\"_blank\">Click here to download</a> the update or visit the <a href=\"https://yarra.rocks/doc/download\" target=\"_blank\">Yarra Download page</a>. Afterwards, install it using the button below.</p>";
                }
                else
                {
                    resultText="<p><span class=\"label label-success\">Up-to-date</span>&nbsp;&nbsp; This server is up to date.</p>";
                }
            }
            else
            {
                // Reading the local version should always be possible at this point, but handle errors to be sure
                resultText="<p><span class=\"label label-danger\">Error</span>&nbsp;&nbsp; Unable to identify local version.";
            }
        }
        catch(const std::exception & e)
        {
            resultText="<p><span class=\"label label-danger\">Error</span>&nbsp;&nbsp; Unable to parse update information.</p>";
        }
    }
    else
    {
        resultText="<p><span class=\"label label-danger\">No Connection</span>&nbsp;&nbsp; Unable to retrieve update information";
        if (!error)
        {
            resultText+=WString(" (response {1})").arg(response.status());
        }
        resultText+=".</p>";
    }


    // Grab the application lock, to make sure that the application still exists at this time
    Wt::WApplication::UpdateLock lock(parent->app);

    if (lock)
    {
        // We do have access to the application now, so update the UI controls
        updateText->setText(resultText);
        checkUpdatesBtn->setDisabled(false);

        // Disable server-side updates again
        parent->app->triggerUpdate();
        parent->app->enableUpdates(false);
    }
}


void ywConfigPageUpdate::installUpdate()
{
    // Return if installation of modules/updates has been disabled in the configuration
    if (parent->app->configuration->disableModuleInstallation)
    {
        Wt::WMessageBox::show("Security Policy", "Installation of updates via the WebGUI has been disabled.", Wt::Ok);
        return;
    }

    // Check if server is offline
    if (ywServerInterface::isServerRunning(parent->app->configuration->yarraPath))
    {
        Wt::WMessageBox::show("Server Online", "The server needs to be offline before updates can be installed.", Wt::Ok);
        return;
    }

    // TODO: Show message if functionality has not been finished before next release
    //Wt::WMessageBox::show("Coming Soon", "This function has not been implemented yet.", Wt::Ok);
    //return;


    uploadModuleDialog=new WDialog("Upload Server Update");
    uploadModuleDialog->contents()->setMinimumSize(500,80);
    Wt::WVBoxLayout* contentLayout=new Wt::WVBoxLayout();

    contentLayout->addWidget(new Wt::WText("Select YarraServer package to upload (.zip):"));
    contentLayout->addSpacing(10);

    Wt::WFileUpload* uploadModule = new Wt::WFileUpload();
    uploadModule->setFilters(".zip");

    Wt::WProgressBar* progressBar=new Wt::WProgressBar();
    progressBar->resize(WLength::Auto, 30);
    uploadModule->setProgressBar(progressBar);

    contentLayout->addWidget(uploadModule);
    contentLayout->addStretch(1);
    uploadModuleDialog->contents()->setLayout(contentLayout);

    // Automatically launch upload after selecting file
    uploadModule->changed().connect(std::bind([=] () {
        uploadModule->upload();
    }));

    uploadModule->fileTooLarge().connect(std::bind([=] () {
        Wt::WMessageBox::show("Update Upload", "The selected file is too large", Wt::Ok);
    }));

    // Handler for succesfull upload
    uploadModule->uploaded().connect(std::bind([=] () {
        checkUploadedFile(uploadModule->spoolFileName(), uploadModule->clientFileName());
    }));

    Wt::WPushButton *closeBtn = new Wt::WPushButton("Cancel", uploadModuleDialog->footer());
    closeBtn->setDefault(true);
    closeBtn->clicked().connect(uploadModuleDialog, &Wt::WDialog::reject);

    // Free dialog when it is closed
    uploadModuleDialog->finished().connect(std::bind([=] () {
        delete uploadModuleDialog;
        uploadModuleDialog=0;
    }));

    uploadModuleDialog->show();

}


void ywConfigPageUpdate::checkUploadedFile(WString uploadedFilename, WString originalFilename)
{
    try
    {
        auto zipArchive = ZipFile::Open(uploadedFilename.toUTF8());

        // Check is ZIP file contains manifest file and identify module name
        bool validManifestFound=false;
        size_t requiredSize=0;

        std::string searchString=YW_SERVER_MANIFEST;

        // Loop over all files to see if the archive contains the file "YarraServer.ymf".
        // Also estimate the total size of the files when extracted.
        ZipArchiveEntry::Ptr manifestInZip=0;

        for (int i=0; i<zipArchive->GetEntriesCount(); ++i )
        {
            std::string fileEntry = zipArchive->GetEntry(i)->GetFullName();
            requiredSize += zipArchive->GetEntry(i)->GetSize();

            // Make sure that the archive contains the manifest file
            if (fileEntry.compare(searchString)==0)
            {
                manifestInZip=zipArchive->GetEntry(i);
                validManifestFound=true;
            }
        }

        if (!validManifestFound)
        {
            Wt::WMessageBox::show("Invalid Package", "The uploaded file is not a valid YarraServer package.", Wt::Ok);
            uploadModuleDialog->reject();
            return;
        }

        bool abortUpdate=false;
        WString abortMessage="";

        // TODO: Untested so far!

        // Generate temporary file name and extract manifest file from archive
        boost::filesystem::path tempManifestFile=boost::filesystem::unique_path();
        ZipFile::ExtractFile(uploadedFilename.toUTF8(), manifestInZip->GetName(), tempManifestFile.string());
        std::cout << "Temporary filename: " << tempManifestFile.string() << std::endl; //DEBUG

        // Read extracted manifest file and compare with version numbers form installed version
        ywServerManifest tempManifest(parent->app->configuration->yarraPath);
        if (tempManifest.readManifest(tempManifestFile.string()))
        {
            // TODO: Compare version number to installed version
        }
        else
        {
            // TODO: Show error message.
            abortUpdate=true;
            abortMessage="Update package is corrupt.";
        }

        // Remove extracted temporary manifest file
        // TOOD

        // TODO: Show confirmation dialig. Mention that backup should be created first. Ask for approval to go forward

        if (abortUpdate)
        {
            if (!abortMessage.empty())
            {
                // TODO: Show dialog with message
            }
            return;
        }

        // TODO: Uninstall current version by removing files listed in manifest file of installed version

        // TODO: Iterate through ZIP file and extract files that don't exist in local version. Replace variable paths via current configuration

        // Reboot the webgui by terminating the current instance
        std::cout << std::endl << "## Enforcing restart of WebGUI after server update ##" << std::endl << std::endl;
        killpg(getpid(),SIGTERM);
    }
    catch(const std::exception & e)
    {
        Wt::WMessageBox::show("Error", WString("Installation of update package failed. Reason: ") + e.what(), Wt::Ok);
    }

    // Close the upload dialog
    uploadModuleDialog->accept();
}


