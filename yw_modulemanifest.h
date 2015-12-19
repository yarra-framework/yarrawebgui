#ifndef YW_MODULEMANIFEST_H
#define YW_MODULEMANIFEST_H

#include <Wt/WString>

using namespace Wt;

class ywModuleManifest
{
public:
    ywModuleManifest();

    WString name;
    WString version;
    WString author;
    WString description;
    WString homepage;
    WString downloadURL;

    bool readManifest(WString filename);
    WString renderInformation();

};

#endif // YW_MODULEMANIFEST_H
