#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc<2)
    {
        // Usage information
        cout << endl;
        cout << "YarraWebGUI Daemonizer" << endl;
        cout << "----------------------" << endl << endl;
        cout << "Launches the command given in the arguments as daemonized process." << endl;
        cout << "Use carefully!" << endl << endl;
        exit(0);
    }
    else
    {
        // Prepare the arguments for the later process call
        char** myArgs=new char*[argc];
        for (int i=0; i<argc-1; i++)
        {
            myArgs[i]=argv[i+1];
        }
        myArgs[argc-1]=0;

        // Daemonize the current process
        if (daemon(1,1)==-1)
        {
            cout << "!E#" << endl;
            exit(0);
        }

        // Execute the process and wait for the return
        execvp(myArgs[0], myArgs);

        // Clear the array
        delete[] myArgs;
        exit(0);
    }

    return 0;
}
