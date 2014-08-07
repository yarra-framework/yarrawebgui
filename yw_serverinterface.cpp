#include "yw_serverinterface.h"
#include "yw_application.h"



ywServerInterface::ywServerInterface()
{
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
    return "";
}
