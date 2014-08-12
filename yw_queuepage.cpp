#include "yw_queuepage.h"

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
#include <Wt/WString>
#include <Wt/WText>
#include <Wt/WScrollArea>


ywQueuePage::ywQueuePage(ywApplication* parent)
 : WContainerWidget()
{
    app=parent;

    Wt::WVBoxLayout* queuePageLayout = new Wt::WVBoxLayout();
    this->setLayout(queuePageLayout);


    Wt::WTabWidget* tabWidget = new Wt::WTabWidget();

    WContainerWidget* queueContainer=new WContainerWidget();
    Wt::WVBoxLayout*  queueLayout=new Wt::WVBoxLayout();
    queueContainer->setLayout(queueLayout);

    queuePageLayout->addWidget(queueContainer);

    Wt::WContainerWidget* centerContainer = new Wt::WContainerWidget;
    Wt::WHBoxLayout* centerLayout = new Wt::WHBoxLayout();
    centerContainer->setLayout(centerLayout);

    Wt::WContainerWidget* taskContainer = new Wt::WContainerWidget;
    Wt::WVBoxLayout* taskLayout = new Wt::WVBoxLayout();
    taskLayout->setSpacing(4);
    taskContainer->setLayout(taskLayout);

    Wt::WScrollArea* scrollArea=new Wt::WScrollArea();
    scrollArea->setWidget(taskContainer);

    centerLayout->addWidget(new Wt::WContainerWidget,1);
    centerLayout->addWidget(scrollArea);
    centerLayout->addWidget(new Wt::WContainerWidget,1);

    queueLayout->addWidget(centerContainer);


    WContainerWidget* failContainer=new WContainerWidget();
    Wt::WVBoxLayout*  failLayout=new Wt::WVBoxLayout();
    failContainer->setLayout(failLayout);


    tabWidget->addTab(queueContainer, "Scheduled", Wt::WTabWidget::PreLoading);
    tabWidget->addTab(failContainer,  "Failed",    Wt::WTabWidget::PreLoading);
    //tabWidget->currentChanged().connect(this, &ywStatusPage::tabChanged);
    tabWidget->setStyleClass("tabwidget");


    for (int i=0; i<10; i++)
    {
        WPanel* panel=new WPanel();
        panel->setTitle(WString(" GRASP_LIVER#45453#{1}").arg(i));
        panel->setSelectable(true);
        if (i==0)
        {
            panel->addStyleClass("panel-prio");
        }
        else
        {
            if (i>4)
            {
                panel->addStyleClass("panel-night");
            }
            else
            {
                panel->addStyleClass("panel-jobnormal");
            }
        }
        //panel->setMaximumSize(600,Wt::WLength::Auto);

        WText* test=new WText();
        test->setText("This is a test job");
        panel->setCentralWidget(test);
        panel->setCollapsible(true);
        panel->setCollapsed(true);
        taskLayout->addWidget(panel,Wt::AlignMiddle);
    }

    WContainerWidget* btnContainer=new WContainerWidget();
    Wt::WHBoxLayout*  btnLayout=new Wt::WHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnContainer->setLayout(btnLayout);

    WContainerWidget* innerBtnContainer=new WContainerWidget();

    Wt::WPanel *panel = new Wt::WPanel();
    btnLayout->addWidget(panel);
    panel->setCentralWidget(innerBtnContainer);
    panel->setMargin(30, Wt::Bottom);

    Wt::WPushButton* button=0;
    button =new Wt::WPushButton("Refresh", innerBtnContainer);
    button->setStyleClass("btn");
    button->setMargin(2);
    //button->clicked().connect(this, &ywLogPage::refreshLogs);

    //queuePageLayout->addWidget(centerContainer,1);
    queuePageLayout->addWidget(tabWidget,1);
    queuePageLayout->addWidget(btnContainer);

    scrollArea->resize(600,Wt::WLength::Auto);

}


ywQueuePage* ywQueuePage::createInstance(ywApplication* parent)
{
    return new ywQueuePage(parent);
}
