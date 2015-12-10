#include "yw_configpage_modules.h"
#include "yw_configpage.h"
#include "yw_application.h"
#include "yw_helper.h"

#include <Wt/WMessageBox>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>

#include <ZipLib/ZipFile.h>


ywConfigPageModules::ywConfigPageModules(ywConfigPage* pageParent)
{
    parent=pageParent;
    userModulesPath = boost::filesystem::path{parent->app->configuration->yarraModulesUserPath.toUTF8()};

    Wt::WVBoxLayout* subLayout = new Wt::WVBoxLayout();
    this->setLayout(subLayout);
    subLayout->setContentsMargins(0, 0, 0, 0);

    // TODO: Put into scrollarea

    folderTree = new Wt::WTree();
    folderTree->setSelectionMode(Wt::ExtendedSelection);
    subLayout->insertWidget(0,folderTree,1);

    // TODO: Does not need to be refreshed during constructed, but better when it becomes active
    refreshFolderTree();

    WContainerWidget* innerBtnContainer=new WContainerWidget();
    innerBtnContainer->setMargin(10, Wt::Top);
    Wt::WHBoxLayout* innerLayout = new Wt::WHBoxLayout();
    innerBtnContainer->setLayout(innerLayout);
    innerLayout->setContentsMargins(0, 0, 0, 0);
    innerLayout->setSpacing(4);

    Wt::WPushButton* uploadModuleBtn = new Wt::WPushButton("Install / Update", innerBtnContainer);
    uploadModuleBtn->setStyleClass("btn-primary");
    uploadModuleBtn->clicked().connect(this, &ywConfigPageModules::showUploadModuleDialog);

    Wt::WPushButton* deleteBtn=new Wt::WPushButton("Remove", innerBtnContainer);
    deleteBtn->setStyleClass("btn-primary");
    deleteBtn->clicked().connect(std::bind([=] () {
        deleteSelectedModules();
    }));

    Wt::WPushButton* refreshBtn=new Wt::WPushButton("Refresh", innerBtnContainer);
    refreshBtn->setStyleClass("btn");
    refreshBtn->clicked().connect(std::bind([=] () {
        refreshFolderTree();
    }));

    innerLayout->insertWidget(0,uploadModuleBtn,0);
    innerLayout->insertWidget(1,deleteBtn,0);
    innerLayout->insertWidget(2,refreshBtn,0);
    innerLayout->insertWidget(3,new Wt::WContainerWidget,1);

    subLayout->insertWidget(1,innerBtnContainer,0);
}


void ywConfigPageModules::refresh()
{
    //refresh();
}


void buildFolderTree(const boost::filesystem::path & path, Wt::WTreeNode * dirNode, bool is_toplevel = false)
{
    // TODO: Devide into sections: Core Modules and User Modules

    try
    {

        for (auto dir_entry = boost::filesystem::directory_iterator(path);
             dir_entry != boost::filesystem::directory_iterator(); ++dir_entry)
        {
            bool is_dir = boost::filesystem::is_directory(dir_entry->path());
            Wt::WTreeNode *node = new Wt::WTreeNode(dir_entry->path().filename().string(),
                 is_toplevel ? new Wt::WIconPair("icons/engine-24.png", "icons/engine-24.png", false)
                 : is_dir ? new Wt::WIconPair("icons/yellow-folder-closed.png", "icons/yellow-folder-open.png", false)
                 : nullptr);

            dirNode->addChildNode(node);
            node->setSelectable(is_toplevel);
            node->expand();
            if (is_dir) {
                buildFolderTree(dir_entry->path(), node);
            }
        }
    }
    catch(const std::exception & e)
    {
        // TODO: What to do with exceptions?
    }
}


void ywConfigPageModules::refreshFolderTree()
{
    Wt::WTreeNode *root = new Wt::WTreeNode("User Modules");
    //root->setStyleClass("example-tree");
    folderTree->setTreeRoot(root);

    root->label()->setTextFormat(Wt::PlainText);
    root->setLoadPolicy(Wt::WTreeNode::NextLevelLoading);

    buildFolderTree(userModulesPath, root, true);

    root->expand();
}


void ywConfigPageModules::deleteSelectedModules()
{
    // TODO: Make sure that the server is offline
    // TODO: Make sure that nobody else is deleting module -- add lock mechanism

    if (Wt::WMessageBox::show("Uninstall Module",
        "You are about to delete all selected modules. Are you sure?",  Wt::Ok | Wt::Cancel) == Wt::Ok)
    {
        for (const auto & module : folderTree->selectedNodes())
        {
            auto modulePath = userModulesPath / module->label()->text().toUTF8();

            try
            {
                boost::filesystem::remove_all(modulePath);
            }
            catch(const std::exception & e)
            {
                // TODO: Show error message
            }
        }
        refreshFolderTree();
    }
}


void ywConfigPageModules::showUploadModuleDialog()
{
    // TODO: Make sure that the server is offline
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
    uploadModuleDialog->contents()->setLayout(contentLayout,Wt::AlignTop);


    Wt::WPushButton *uploadBtn = new Wt::WPushButton("Upload", uploadModuleDialog->footer());
    uploadBtn->disable();
    uploadModule->changed().connect(uploadBtn, &Wt::WPushButton::enable);
    uploadBtn->clicked().connect(std::bind([=] () {
        uploadModule->upload();
    }));

    Wt::WPushButton *closeBtn = new Wt::WPushButton("Close", uploadModuleDialog->footer());
    closeBtn->setDefault(true);
    closeBtn->clicked().connect(uploadModuleDialog, &Wt::WDialog::reject);


    uploadModule->fileTooLarge().connect(std::bind([=] () {
        Wt::WMessageBox::show("Module Upload", "The selected file is too large", Wt::Ok);
    }));


    // React to a succesfull uploadModule.
    uploadModule->uploaded().connect(std::bind([=] () {

        std::string uploadedFileName = uploadModule->spoolFileName();
        boost::filesystem::path originalModuleName(uploadModule->clientFileName().toUTF8());

        // TODO: Check if the zip file contains a manifest file
        // TODO: Extract the module name from the manifest file

        auto moduleName = originalModuleName.stem();
        auto modulePath = userModulesPath / moduleName;
        bool updated = false;
        try
        {
            if (boost::filesystem::is_directory(modulePath))
            {
                if (Wt::WMessageBox::show("Module Upload",
                    "A module with the name `" + moduleName.string() + "` exists. Do you want to overwrite it?",
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
            {
                auto zipArchive = ZipFile::Open(uploadedFileName);

                // TODO: Error handling!

                for (int i=0; i<zipArchive->GetEntriesCount(); ++i )
                {
                    auto zipEntry = zipArchive->GetEntry(i);
                    auto outFilePath = modulePath / zipEntry->GetFullName();
                    boost::filesystem::create_directories(outFilePath.parent_path());
                    if (!boost::filesystem::is_directory(outFilePath))
                    {
                        ZipFile::ExtractFile(uploadedFileName, zipEntry->GetFullName(), outFilePath.string());
                        wApp->log("notice") <<  zipEntry->GetFullName() << " -> " << std::hex << uint32_t(zipEntry->GetAttributes());
                        boost::filesystem::permissions( outFilePath,
                            boost::filesystem::perms((uint32_t(zipEntry->GetAttributes()) >> 16) & 0777));
                    }
                }
            }

            refreshFolderTree();
            Wt::WMessageBox::show("Module Upload", "Module '" + moduleName.string()
                                + (updated ?  "' updated successfully" : "' installed successfully"), Wt::Ok);
        }
        catch(const std::exception & e)
        {
            refreshFolderTree();
            Wt::WMessageBox::show("Module Upload", "Module '" + moduleName.string() + "' installation failed: " +e.what(), Wt::Ok);
        }
        uploadModuleDialog->accept();
    }));

    uploadModuleDialog->finished().connect(std::bind([=] () {
        delete uploadModuleDialog;
    }));

    uploadModuleDialog->show();
}

