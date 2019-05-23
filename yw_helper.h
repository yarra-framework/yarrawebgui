#ifndef YW_HELPER_H
#define YW_HELPER_H

#include <Wt/WString>

class ywHelper
{
public:
    ywHelper();

    static bool lockFile      (Wt::WString fullFilename);
    static bool unlockFile    (Wt::WString fullFilename);
    static bool isLocked      (Wt::WString fullFilename);
    static bool isFolderLocked(Wt::WString fullPath);

};

#endif // YW_HELPER_H
