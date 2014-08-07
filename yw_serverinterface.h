#ifndef YW_SERVERINTERFACE_H
#define YW_SERVERINTERFACE_H

#include <Wt/WString>

class ywApplication;

using namespace Wt;


class ywServerInterface
{
public:
    ywServerInterface();
    void setAppInstance(ywApplication* myApp);

    WString getServerLogFilename();
    WString getTaskLogFilename();

    ywApplication* app;

};

#endif // YW_SERVERINTERFACE_H
