#ifndef YW_CONFIGPAGE_MODULES_H
#define YW_CONFIGPAGE_MODULES_H

#include <Wt/WContainerWidget>
#include <Wt/WFileUpload>
#include <Wt/WProgressBar>
#include <Wt/WTree>
#include <Wt/WTreeNode>
#include <Wt/WIconPair>

// Needed to get avoid of "undefined reference to `boost::filesystem::detail::copy_file()`"
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>


class ywConfigPage;

class ywConfigPageModules : public Wt::WContainerWidget
{
public:
    ywConfigPageModules(ywConfigPage* pageParent);

    ywConfigPage* parent;
    Wt::WTree *folderTree;
    boost::filesystem::path userModulesPath;

    void showUploadModuleDialog();
    void refreshFolderTree();
    void deleteSelectedModules();

    void refresh();
};


#endif // YW_CONFIGPAGE_MODULES_H
