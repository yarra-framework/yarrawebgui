#include "yw_configpage_modules.h"
#include "yw_configpage.h"
#include "yw_application.h"
#include "yw_helper.h"
#include "yw_modulemanifest.h"

#include <Wt/WMessageBox>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>

#include <ZipLib/ZipFile.h>

#include <sys/statvfs.h>

namespace fs = boost::filesystem;


#define LABEL_MODULES_CORE "<span style=\"color: #580F8B;\">Core Modules</span>"
#define LABEL_MODULES_USER "<span style=\"color: #580F8B;\">User Modules</span>"


ywConfigPageModules::ywConfigPageModules(ywConfigPage* pageParent)
{
    parent=pageParent;
    userModulesPath = boost::filesystem::path{parent->app->configuration->yarraModulesUserPath.toUTF8()};
    coreModulesPath = boost::filesystem::path{parent->app->configuration->yarraModulesPath.toUTF8()};

    Wt::WVBoxLayout* subLayout = new Wt::WVBoxLayout();
    this->setLayout(subLayout);
    subLayout->setContentsMargins(0, 0, 0, 0);

    Wt::WText* head=new Wt::WText("<h3>Installed Yarra Modules</h3>");
    head->setMargin(6, Wt::Bottom);
    subLayout->insertWidget(0,head,0);

    moduleTree = new Wt::WTree();
    moduleTree->setSelectionMode(Wt::SingleSelection);
    Wt::WScrollArea* scrollArea=new Wt::WScrollArea();
    scrollArea->setWidget(moduleTree);
    subLayout->insertWidget(1,scrollArea,1);

    infoText = new Wt::WText();
    infoText->setText("");

    infoPanel = new Wt::WPanel();
    infoPanel->setTitle("Information");
    infoPanel->setMinimumSize(WLength::Auto, 200);
    infoPanel->setMargin(10, Wt::Top);
    infoPanel->setCentralWidget(infoText);
    subLayout->insertWidget(2,infoPanel,0);

    // Does not need to be refreshed during constructor. Updated via Refresh() when tab becomes visible
    // refreshModuleTree();

    WContainerWidget* innerBtnContainer=new WContainerWidget();
    innerBtnContainer->setMargin(10, Wt::Top);
    Wt::WHBoxLayout* innerLayout = new Wt::WHBoxLayout();
    innerBtnContainer->setLayout(innerLayout);
    innerLayout->setContentsMargins(0, 0, 0, 0);
    innerLayout->setSpacing(4);

    Wt::WPushButton* uploadModuleBtn = new Wt::WPushButton("Install / Update", innerBtnContainer);
    uploadModuleBtn->setStyleClass("btn-primary");
    uploadModuleBtn->clicked().connect(this, &ywConfigPageModules::showUploadModuleDialog);

    deleteBtn=new Wt::WPushButton("Remove", innerBtnContainer);
    deleteBtn->setStyleClass("btn-primary");
    deleteBtn->setDisabled(true);
    deleteBtn->clicked().connect(std::bind([=] () {
        deleteSelectedModules();
    }));

    Wt::WPushButton* refreshBtn=new Wt::WPushButton("Refresh", innerBtnContainer);
    refreshBtn->setStyleClass("btn");
    refreshBtn->clicked().connect(std::bind([=] () {
        refreshModuleTree();
    }));

    innerLayout->insertWidget(0,uploadModuleBtn,0);
    innerLayout->insertWidget(1,deleteBtn,0);
    innerLayout->insertWidget(2,refreshBtn,0);
    innerLayout->insertWidget(3,new Wt::WContainerWidget,1);

    subLayout->insertWidget(3,innerBtnContainer,0);

    // React when user clicks on modules
    moduleTree->itemSelectionChanged().connect(std::bind([=] () {
        bool deleteBtnDisabled=true;

        if (moduleTree->selectedNodes().size()>0)
        {
            WTreeNode* selectedNode=*moduleTree->selectedNodes().begin();
            bool isUserModule=false;

            // Enable the Remove button only if a user module has been selected
            if (selectedNode->parentNode()->label()->text()==LABEL_MODULES_USER)
            {
                isUserModule=true;
                deleteBtnDisabled=false;
            }

            // Update the info text based on the selected module
            infoText->setText(getModuleInfo(selectedNode->label()->text(),isUserModule));
        }
        else
        {
            infoText->setText("");
        }

        deleteBtn->setDisabled(deleteBtnDisabled);
    }));
}


void ywConfigPageModules::refreshPage()
{
    refreshModuleTree();
}


void ywConfigPageModules::buildCoreModuleTree(Wt::WTreeNode* baseNode)
{
    // Check if path exists, if not create error message
    bool pathError=false;
    try
    {
        if (!fs::exists(coreModulesPath) || !fs::is_directory(coreModulesPath))
        {
            pathError=true;
        }
    }
    catch(const std::exception & e)
    {
        pathError=true;
    }

    if (pathError)
    {
        Wt::WTreeNode *node = new Wt::WTreeNode("ERROR: Can't access path "+WString(coreModulesPath.generic_string()));
        baseNode->addChildNode(node);
        node->setSelectable(false);
        return;
    }

    // For the core modules, search for manifest files in the modules folder
    try
    {
        const string& ext_manifest=YW_EXT_MANIFEST;

        for (auto dir_entry = boost::filesystem::directory_iterator(coreModulesPath);
             dir_entry != boost::filesystem::directory_iterator(); ++dir_entry)
        {
            bool is_manifest = (boost::filesystem::is_regular_file(dir_entry->path()) && (dir_entry->path().extension()==ext_manifest));

            if (is_manifest)
            {
                Wt::WTreeNode *node = new Wt::WTreeNode(dir_entry->path().stem().string());
                baseNode->addChildNode(node);
                node->setSelectable(true);
            }
        }
    }
    catch(const std::exception & e)
    {
        // TODO: Add error reporting
    }
}



void ywConfigPageModules::buildUserModuleTree(Wt::WTreeNode* baseNode)
{
    // Check if path exists, if not create error message
    bool pathError=false;
    try
    {
        if (!fs::exists(userModulesPath) || !fs::is_directory(userModulesPath))
        {
            pathError=true;
        }
    }
    catch(const std::exception & e)
    {
        pathError=true;
    }

    if (pathError)
    {
        Wt::WTreeNode *node = new Wt::WTreeNode("ERROR: Can't access path "+WString(userModulesPath.generic_string()));
        baseNode->addChildNode(node);
        node->setSelectable(false);
        return;
    }

    // For the user modules, show all existing subfolders in the user module directory
    try
    {
        for (auto dir_entry = boost::filesystem::directory_iterator(userModulesPath);
             dir_entry != boost::filesystem::directory_iterator(); ++dir_entry)
        {
            bool is_dir = boost::filesystem::is_directory(dir_entry->path());

            if (is_dir)
            {
                Wt::WTreeNode *node = new Wt::WTreeNode(dir_entry->path().filename().string());
                baseNode->addChildNode(node);
                node->setSelectable(true);
            }
        }
    }
    catch(const std::exception & e)
    {
        // TODO: Add error reporting
    }
}


void ywConfigPageModules::refreshModuleTree()
{
    Wt::WTreeNode *root = new Wt::WTreeNode("Yarra Modules");

    moduleTree->setTreeRoot(root);

    root->setLoadPolicy(Wt::WTreeNode::NextLevelLoading);
    root->setSelectable(false);
    root->setNodeVisible(false);

    Wt::WTreeNode *coreNode = new Wt::WTreeNode(LABEL_MODULES_CORE);
    root->addChildNode(coreNode);
    coreNode->setSelectable(false);
    buildCoreModuleTree(coreNode);

    Wt::WTreeNode *userNode = new Wt::WTreeNode(LABEL_MODULES_USER);
    root->addChildNode(userNode);
    userNode->setSelectable(false);
    buildUserModuleTree(userNode);

    root->expand();
    coreNode->expand();
    userNode->expand();
}


void ywConfigPageModules::deleteSelectedModules()
{
    if (parent->app->configuration->disableModuleInstallation)
    {
        Wt::WMessageBox::show("Security Policy", "Installation of modules via the WebGUI has been disabled.", Wt::Ok);
        return;
    }

    if (!isServerOffline())
    {
        Wt::WMessageBox::show("Server Online", "The server needs to be offline before modules can be removed.", Wt::Ok);
        return;
    }

    // TODO: Make sure that nobody else is deleting module -- add lock mechanism

    if (Wt::WMessageBox::show("Uninstall Module",
                              "This operation will uninstall the selected module from the server. Are you sure?",
                              Wt::Yes | Wt::No) == Wt::Yes)
    {
        bool error=false;

        for (const auto & module : moduleTree->selectedNodes())
        {
            auto modulePath = userModulesPath / module->label()->text().toUTF8();

            try
            {
                boost::filesystem::remove_all(modulePath);
            }
            catch(const std::exception & e)
            {
                error=true;
            }
        }

        if (error)
        {
            Wt::WMessageBox::show("Error", "Removing the module was not possible. Check file permissions.", Wt::Ok);
        }

        refreshModuleTree();
    }
}


void ywConfigPageModules::showUploadModuleDialog()
{
    if (parent->app->configuration->disableModuleInstallation)
    {
        Wt::WMessageBox::show("Security Policy", "Installation of modules via the WebGUI has been disabled.", Wt::Ok);
        return;
    }

    if (!isServerOffline())
    {
        Wt::WMessageBox::show("Server Online", "The server needs to be offline before modules can be installed.", Wt::Ok);
        return;
    }

    // TODO: Make sure that nobody else is currently uploading -- add lock mechanism.

    Wt::WDialog* uploadModuleDialog = new WDialog("Upload Module");

    uploadModuleDialog->contents()->setMinimumSize(500,80);
    Wt::WVBoxLayout* contentLayout=new Wt::WVBoxLayout();

    contentLayout->addWidget(new Wt::WText("Select Yarra Module file to upload (.zip):"));
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

    Wt::WPushButton *closeBtn = new Wt::WPushButton("Cancel", uploadModuleDialog->footer());
    closeBtn->setDefault(true);
    closeBtn->clicked().connect(uploadModuleDialog, &Wt::WDialog::reject);


    uploadModule->fileTooLarge().connect(std::bind([=] () {
        Wt::WMessageBox::show("Module Upload", "The selected file is too large", Wt::Ok);
    }));


    // React to succesfull upload
    uploadModule->uploaded().connect(std::bind([=] () {

        std::string uploadedFileName = uploadModule->spoolFileName();

        fs::path originalModuleName(uploadModule->clientFileName().toUTF8());
        std::string moduleName = originalModuleName.stem().generic_string();

        try
        {
            auto zipArchive = ZipFile::Open(uploadedFileName);

            // Check is ZIP file contains manifest file and identify module name
            bool validManifestFound=false;
            bool multipleManifests=false;
            size_t requiredSize=0;

            for (int i=0; i<zipArchive->GetEntriesCount(); ++i )
            {
                std::string fileEntry = zipArchive->GetEntry(i)->GetName();
                requiredSize += zipArchive->GetEntry(i)->GetSize();

                std::string searchString = YW_EXT_MANIFEST;
                size_t extPosition=fileEntry.find(searchString);

                if ((extPosition!=string::npos) && (extPosition==fileEntry.length()-4))
                {
                    if (validManifestFound)
                    {
                        // A manifest file has already been found previously. Can't be a valid module
                        validManifestFound=false;
                        multipleManifests=true;
                        break;
                    }

                    // Extract name of manifest file to be used as module name
                    moduleName=fileEntry.substr(0,fileEntry.length()-4);
                    validManifestFound=true;
                }
            }

            if (!validManifestFound)
            {
                WString errorReason=multipleManifests ? "contains invalid manifest files" : "does not contain manifest file";
                Wt::WMessageBox::show("Invalid Module", "The uploaded file is not a valid Yarra Module (" + errorReason + ").", Wt::Ok);
                uploadModuleDialog->reject();
                return;
            }

            // Prevent installation if user is trying to upload the server package
            if (moduleName=="YarraServer")
            {
                Wt::WMessageBox::show("Invalid Module", "This package contains the YarraServer components, and it cannot be installed as a module. Use the Update tab on the left side to install server updates.", Wt::Ok);
                uploadModuleDialog->reject();
                return;
            }

            fs::path modulePath = userModulesPath / moduleName;

            // Check if space available at modulePath is at least requiredSize
            if (!isSufficientDiskSpaceAvailable(requiredSize))
            {
                Wt::WMessageBox::show("Insufficient Disk Space", "There is not enough free space available to install the module.", Wt::Ok);
                uploadModuleDialog->reject();
                return;
            }

            // Check is module is already installed
            bool updated=false;
            if (boost::filesystem::is_directory(modulePath))
            {
                if (Wt::WMessageBox::show("Already Installed",
                                          "A module with name `" + moduleName + "` is already installed. Do you want to overwrite it?",
                                          Wt::Ok | Wt::Cancel) != Wt::Ok)
                {
                    uploadModuleDialog->reject();
                    return;
                }
                boost::filesystem::remove_all(modulePath);
                updated = true;
            }

            boost::filesystem::create_directories(modulePath);

            // Extract and install ZIP file
            for (int i=0; i<zipArchive->GetEntriesCount(); ++i )
            {
                // TODO: Error handling for file permissions etc

                auto zipEntry = zipArchive->GetEntry(i);
                auto outFilePath = modulePath / zipEntry->GetFullName();

                // Create needed folder
                boost::filesystem::create_directories(outFilePath.parent_path());

                if (!boost::filesystem::is_directory(outFilePath))
                {
                    // Extract file from archive
                    ZipFile::ExtractFile(uploadedFileName, zipEntry->GetFullName(), outFilePath.string());

                    // Set file permission to allow execution of binaries

                    // The Unix file permissions are stored in the upper 16 bits of the attributes
                    uint32_t unixPermissions=(uint32_t(zipEntry->GetAttributes()) >> 16);

                    // Check if executable bit is set for the file owner
                    bool isExecutable=(unixPermissions & 0100)==0100;

                    //std::cout <<  zipEntry->GetFullName() << " -> " << std::oct << int(unixPermissions) << std::endl;
                    //std::cout << (isExecutable ? "Executable" : "Not executable") << std::endl;

                    if (isExecutable)
                    {
                        // Set executable bit for extracted file (only for file owner, for security reason)
                        fs::permissions(outFilePath, fs::perms(0700));
                    }
                    else
                    {
                        // If not executable, set to read/write for the server owner only
                        fs::permissions(outFilePath, fs::perms(0600));
                    }
                }
            }

            refreshModuleTree();
            Wt::WMessageBox::show("Module Upload", "Module '" + moduleName
                                + (updated ?  "' updated successfully" : "' installed successfully"), Wt::Ok);
        }
        catch(const std::exception & e)
        {
            refreshModuleTree();
            Wt::WMessageBox::show("Error", "Installation of module '" + moduleName + "' failed. Reason: " +e.what(), Wt::Ok);
        }
        uploadModuleDialog->accept();
    }));


    uploadModuleDialog->finished().connect(std::bind([=] () {
        delete uploadModuleDialog;
    }));


    uploadModuleDialog->show();
}


bool ywConfigPageModules::isServerOffline()
{
    return !ywServerInterface::isServerRunning(parent->app->configuration->yarraPath);
}


WString ywConfigPageModules::getModuleInfo(Wt::WString name, bool isUserModule)
{
    WString moduleInfo="";
    WString manifestFile=name+YW_EXT_MANIFEST;

    if (isUserModule)
    {
        manifestFile=userModulesPath.generic_string()+"/"+name+"/"+manifestFile;
    }
    else
    {
        manifestFile=coreModulesPath.generic_string()+"/"+manifestFile;
    }

    // Read information from manifest file using helper class
    ywModuleManifest manifest;
    if (!manifest.readManifest(manifestFile))
    {
        moduleInfo="Module information not available.";
    }
    else
    {
        moduleInfo=manifest.renderInformation();
    }

    return moduleInfo;
}


bool ywConfigPageModules::isSufficientDiskSpaceAvailable(size_t neededSpace)
{
    struct statvfs64 fiData;
    __fsblkcnt64_t sizeAvailable=0;

    if((statvfs64(userModulesPath.generic_string().c_str(),&fiData)) < 0)
    {
        // statvfs call failed, assuming there is enough space
        return true;
    }
    else
    {
        sizeAvailable=fiData.f_bsize*fiData.f_bavail;

        //std::cout << "Required disk space: " << neededSpace << std::endl;
        //std::cout << "Available disk space: " << sizeAvailable << std::endl;

        if (neededSpace>sizeAvailable)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

