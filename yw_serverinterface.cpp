#include <stdio.h>
#include <unistd.h>

#include "yw_serverinterface.h"
#include "yw_application.h"



ywServerInterface::ywServerInterface()
{
    serverStatus=STATUS_UKNOWN;
    serverReply="";
    serverStatusText="";
    serverTaskLog="";
}


void ywServerInterface::setAppInstance(ywApplication* myApp)
{
    app=myApp;
}


WString ywServerInterface::getServerLogFilename()
{
    return app->configuration->yarraLogPath+"/"+"yarra.log";
}


WString ywServerInterface::getTaskLogFilename()
{
    return serverTaskLog;
}


bool ywServerInterface::updateStatus()
{
    serverStatusText="Unknown";
    serverTaskLog="";
    serverRunning=false;
    WString statusCmd=app->configuration->yarraPath+"/ServerCtrl -d --p";

    FILE* process=popen(statusCmd.toUTF8().data(), "r");
    if (process==NULL)
    {
        return false;
    }

    serverReply="";

    char buffer[1028];

    while (fgets(buffer, 1028, process) != NULL)
    {
        serverReply+=WString(buffer);
    }

    pclose(process);

    // Check if server is idle. That might be the most frequent case
    if (serverReply.toUTF8().find("!I#")!=string::npos)
    {
        serverRunning=true;
        serverStatus=STATUS_IDLE;
        serverStatusText="Server is running and is idle.";
        return true;
    }

    // Check if the server might be down
    if (serverReply.toUTF8().find("!F#")!=string::npos)
    {
        serverRunning=false;
        serverStatus=STATUS_DOWN;
        serverStatusText="Server is down.";
        return true;
    }

    // Check if there is an error communicating with the server (unlikely)
    if (serverReply.toUTF8().find("!E#")!=string::npos)
    {
        serverRunning=false;
        serverStatus=STATUS_UKNOWN;
        serverStatusText="Error reading server status (possibly due to former crash). Try restarting server.";
        return true;
    }

    // OK, the remaining case is that the server is processing a case
    serverRunning=true;
    serverStatusText="Server is running and is processing task.";
    serverTaskLog=serverReply;
    serverStatus=STATUS_PROCESSING;

    if (serverReply.toUTF8().find("!D#")!=string::npos)
    {
        serverStatus=STATUS_PROCESSINGDOWN;
        serverStatusText+=" Will shut down afterwards.";
    }
    // Remove the trailing CR/LF and possibly the #D notification
    int tasknameLength=serverTaskLog.toUTF8().find(".log")+4;
    serverTaskLog=WString(serverTaskLog.toUTF8().erase(tasknameLength,string::npos));

    serverTaskID=serverTaskLog;
    int cutPos=serverTaskID.toUTF8().find(".log");
    serverTaskID=WString(serverTaskID.toUTF8().erase(cutPos,string::npos));
    cutPos=serverTaskID.toUTF8().find_last_of("/");
    serverTaskID=WString(serverTaskID.toUTF8().erase(0,cutPos+1));

    return true;
}


bool ywServerInterface::startServer()
{
    WString statusCmd="./yarradexec sudo start yarra";

    //system(statusCmd.toUTF8().data());

    FILE* process=popen(statusCmd.toUTF8().data(), "r");
    if (process==NULL)
    {
        return false;
    }

    // Wait for end of process
    pclose(process);

    return true;
}


bool ywServerInterface::ywServerInterface::stopServer()
{
    WString statusCmd="./yarradexec sudo stop yarra";

    FILE* process=popen(statusCmd.toUTF8().data(), "r");
    if (process==NULL)
    {
        return false;
    }

    // No waiting for the end of the process because the stop yarra command will
    // only return when the process has been finished
    pclose(process);

    return true;
}


bool ywServerInterface::killServer()
{
    WString statusCmd="./yarradexec " + app->configuration->yarraPath+"/ServerCtrl -h --p";

    FILE* process=popen(statusCmd.toUTF8().data(), "r");
    if (process==NULL)
    {
        return false;
    }

    pclose(process);

    return true;
}


