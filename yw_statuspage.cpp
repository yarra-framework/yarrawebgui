#include "yw_statuspage.h"


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


ywStatusPage::ywStatusPage(ywApplication* parent)
 : WContainerWidget()
{
    app=parent;

    Wt::WVBoxLayout* statusPageLayout = new Wt::WVBoxLayout();
    statusPageLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(statusPageLayout);

    Wt::WPanel *panel = new Wt::WPanel();
    Wt::WContainerWidget *btncontainer = new Wt::WContainerWidget();

    Wt::WPushButton *button = new Wt::WPushButton("Start", btncontainer);
    button->setStyleClass("btn-primary");
    button->setMargin(2);

    button = new Wt::WPushButton("Stop", btncontainer);
    button->setMargin(2);
    button->setStyleClass("btn-primary");

    button = new Wt::WPushButton("Kill Task", btncontainer);
    button->setMargin(2);
    button->setStyleClass("btn-primary");

    button = new Wt::WPushButton("Refresh", btncontainer);
    button->setStyleClass("btn");
    button->setMargin(2);

    panel->setCentralWidget(btncontainer);
    panel->setMargin(30, Wt::Bottom);

    Wt::WTabWidget *tabW = new Wt::WTabWidget();
    tabW->addTab(new Wt::WTextArea("This is the contents of the first tab."),
             "Status", Wt::WTabWidget::PreLoading);
    tabW->addTab(new Wt::WTextArea("The contents of the tabs are pre-loaded in"
                       " the browser to ensure swift switching."),
             "Server Log", Wt::WTabWidget::PreLoading);
    tabW->addTab(new Wt::WTextArea("You could change any other style attribute of the"
                       " tab widget by modifying the style class."
                       " The style class 'trhead' is applied to this tab."),
             "Task Log", Wt::WTabWidget::PreLoading);


    tabW->setStyleClass("tabwidget");

    Wt::WText* head1=new Wt::WText("<h3>Server Activity</h3>");
    head1->setMargin(6, Wt::Bottom);
    statusPageLayout->addWidget(head1);
    statusPageLayout->addWidget(tabW,1);

    Wt::WText* head2=new Wt::WText("<h3>Runtime Control</h3>");
    head2->setMargin(6, Wt::Bottom);
    head2->setMargin(20, Wt::Top);

    statusPageLayout->addWidget(head2);
    statusPageLayout->addWidget(panel);
}


ywStatusPage* ywStatusPage::createInstance(ywApplication* parent)
{
    return new ywStatusPage(parent);
}

//Wt::StandardButton answer = Wt::WMessageBox::show("Server Shutdown", "<p>Are you sure to shutdown the server?</p>", Wt::Ok | Wt::Cancel);




