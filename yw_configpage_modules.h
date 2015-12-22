#ifndef YW_CONFIGPAGE_MODULES_H
#define YW_CONFIGPAGE_MODULES_H

#include <Wt/WContainerWidget>
#include <Wt/WFileUpload>
#include <Wt/WProgressBar>
#include <Wt/WTree>
#include <Wt/WTreeNode>
#include <Wt/WIconPair>
#include <Wt/WTreeNode>
#include <Wt/WPanel>

// Needed to get avoid of "undefined reference to `boost::filesystem::detail::copy_file()`"
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>

using namespace Wt;


class ywConfigPage;

class ywConfigPageModules : public Wt::WContainerWidget
{
public:
    ywConfigPageModules(ywConfigPage* pageParent);

    ywConfigPage* parent;
    Wt::WTree* moduleTree;
    Wt::WPanel* infoPanel;
    Wt::WText* infoText;
    Wt::WPushButton* deleteBtn;
    boost::filesystem::path userModulesPath;
    boost::filesystem::path coreModulesPath;


    void showUploadModuleDialog();
    void refreshModuleTree();
    void deleteSelectedModules();

    void refresh();

    bool isServerOffline();

    void buildCoreModuleTree(Wt::WTreeNode* baseNode);
    void buildUserModuleTree(Wt::WTreeNode* baseNode);

    Wt::WString getModuleInfo(Wt::WString name, bool isUserModule);
    bool isSufficientDiskSpaceAvailable(size_t neededSpace);

};


#endif // YW_CONFIGPAGE_MODULES_H
