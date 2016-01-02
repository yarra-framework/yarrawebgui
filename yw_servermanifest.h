#ifndef YW_SERVERMANIFEST_H
#define YW_SERVERMANIFEST_H

#include <Wt/WString>

using namespace Wt;


class ywServerManifest
{
public:
    ywServerManifest();

    WString version;
    WString releaseDate;
    WString versionServerCore;
    WString versionWebGUI;

    bool readManifest(WString yarraPath);
    WString renderInformation();
};

#endif // YW_SERVERMANIFEST_H
