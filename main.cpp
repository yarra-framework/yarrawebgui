#include <functional>
#include <iostream>

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WNavigationBar>
#include <Wt/WBootstrapTheme>
#include <Wt/WStackedWidget>
#include <Wt/WMenu>
#include <Wt/WPopupMenu>
#include <Wt/WSplitButton>
#include <Wt/WPopupMenu>
#include <Wt/WPopupMenuItem>
#include <Wt/WMessageBox>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WOverlayLoadingIndicator>
#include <Wt/WPanel>
#include <Wt/WTabWidget>
#include <Wt/WTextArea>

#include "yw_application.h"


// The global configuration instance
ywConfiguration configurationInstance;


WApplication* createApplication(const WEnvironment& env)
{
    ywApplication* app=new ywApplication(env);
    app->prepare(&configurationInstance);
    return (WApplication*) app;
}


int main(int argc, char **argv)
{
    configurationInstance.loadConfiguration();
    if (!configurationInstance.isValid())
    {
        std::cout << "ERROR: Unable to start WebGUI" << std::endl;
        return 1;
    }

    // Prepare the arguments for calling the WRun commands
    char arg1[] = "--docroot=""html""";
    char arg2[] = "--http-address";
    char arg3[] = "0.0.0.0";
    char arg4[] = "--http-port";

    // Overwrite the port number with the setting from the configuration file
    char arg5[8] = "8080";
    strcpy(arg5,configurationInstance.port.toUTF8().data());

    // Pass path to local configuration file (needed to set maximum allowed upload size)
    char arg6[] = "--config=wt_config.xml";

    int argCount=7;
    char* args[7];
    args[0]=argv[0];
    args[1]=arg1;
    args[2]=arg2;
    args[3]=arg3;
    args[4]=arg4;
    args[5]=arg5;
    args[6]=arg6;

    return WRun(argCount, args, &createApplication);
    //return WRun(argc, argv, &createApplication);
}

