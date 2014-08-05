#ifndef YW_LOGINPAGE_H
#define YW_LOGINPAGE_H

#include <Wt/WContainerWidget>

class ywApplication;

using namespace Wt;


class ywLoginPage : public Wt::WContainerWidget
{
public:
    ywLoginPage(ywApplication* parent);

    static ywLoginPage* createInstance(ywApplication *parent);

    void loginClick();

    ywApplication* app;

};

#endif // YW_LOGINPAGE_H
