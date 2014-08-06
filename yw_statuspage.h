#ifndef YW_STATUSPAGE_H
#define YW_STATUSPAGE_H

#include <Wt/WContainerWidget>

class ywApplication;

using namespace Wt;


class ywStatusPage : public Wt::WContainerWidget
{
public:
    ywStatusPage(ywApplication* parent);

    static ywStatusPage* createInstance(ywApplication *parent);

    ywApplication* app;
};

#endif // YW_STATUSPAGE_H
