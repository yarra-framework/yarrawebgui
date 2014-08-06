#ifndef YW_CONFIGPAGE_H
#define YW_CONFIGPAGE_H

#include <Wt/WContainerWidget>

class ywApplication;

using namespace Wt;


class ywConfigPage : public Wt::WContainerWidget
{
public:
    ywConfigPage(ywApplication* parent);

    static ywConfigPage* createInstance(ywApplication *parent);

    ywApplication* app;
};


#endif // YW_CONFIGPAGE_H
