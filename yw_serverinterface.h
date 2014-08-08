#ifndef YW_SERVERINTERFACE_H
#define YW_SERVERINTERFACE_H

#include <Wt/WString>

class ywApplication;

using namespace Wt;


class ywServerInterface
{
public:

    enum {
        STATUS_UKNOWN        =0,
        STATUS_DOWN          =1,
        STATUS_IDLE          =2,
        STATUS_PROCESSING    =3,
        STATUS_PROCESSINGDOWN=4
    };


    ywServerInterface();
    void setAppInstance(ywApplication* myApp);

    WString getServerLogFilename();
    WString getTaskLogFilename();

    bool updateStatus();

    bool startServer();
    bool stopServer();
    bool killServer();

    int serverStatus;
    WString serverReply;
    WString serverStatusText;
    WString serverTaskLog;
    WString serverTaskID;
    bool    serverRunning;

    ywApplication* app;

};

#endif // YW_SERVERINTERFACE_H
