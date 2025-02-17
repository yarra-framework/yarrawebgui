#ifndef YW_GLOBAL_H
#define YW_GLOBAL_H

#define YW_VERSION   "0.20b3"

#define YW_CONFIGFILE  "YarraWebGUI.ini"
#define YW_YARRACONFIG "YarraServer.ini"

#define YW_EXT_TASK      ".task"
#define YW_EXT_TASKPRIO  ".task_prio"
#define YW_EXT_TASKNIGHT ".task_night"

#define YW_EXT_LOCK      ".lock"
#define YW_EXT_MODE      ".mode"
#define YW_EXT_RESUME    ".resume"

#define YW_EXT_MANIFEST       ".ymf"
#define YW_SERVER_MANIFEST    "YarraServer.ymf"
#define YW_SERVER_MANIFEST_ID "YarraServer_Release"


#include <Wt/WString>

// String list, as used at various places in the code
typedef std::vector<Wt::WString> WStringList;


#endif // YW_GLOBAL_H

