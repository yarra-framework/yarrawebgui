#ifndef YW_QUEUEPAGE_H
#define YW_QUEUEPAGE_H

#include <Wt/WContainerWidget>

class ywApplication;

using namespace Wt;


class ywQueuePage : public Wt::WContainerWidget
{
public:
    ywQueuePage(ywApplication* parent);

    static ywQueuePage* createInstance(ywApplication *parent);

    ywApplication* app;
};


#endif // YW_QUEUEPAGE_H
