#include "QtGuider.h"
#include "QtFlexWidget.h"
#include "QtFlexHelper.h"
#include "QtDockGuider.h"
#include "QtDockWidget.h"
#include "QtFlexManager.h"
#include <QtCore/QSettings>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include <QTreeWidget>
#include <QToolBar>
#include <QTableWidget>
#include "ui_qtguider.h"

QtCentral::QtCentral(QWidget* parent) : QWidget(parent)
{
    setObjectName("Central");

    QHBoxLayout* box = new QHBoxLayout(this);
    box->setContentsMargins(0, 0, 0, 0);

    connect(FlexManager::instance(), SIGNAL(flexWidgetCreated(FlexWidget*)), SLOT(on_flexWidgetCreated(FlexWidget*)));
    connect(FlexManager::instance(), SIGNAL(dockWidgetCreated(DockWidget*)), SLOT(on_dockWidgetCreated(DockWidget*)));
    connect(FlexManager::instance(), SIGNAL(flexWidgetDestroying(FlexWidget*)), SLOT(on_flexWidgetDestroying(FlexWidget*)));
    connect(FlexManager::instance(), SIGNAL(dockWidgetDestroying(DockWidget*)), SLOT(on_dockWidgetDestroying(DockWidget*)));


    QSettings settings("QtFlex5", "QtGuider");

    QByteArray content = settings.value("Flex").toByteArray();

    const char* bytes = content;

    QMap<QString, QWidget*> parents;

    parents[objectName()] = this;

    //if (content.isEmpty())
    {
        createOne();
    }
    //else
    //{
    //    FlexManager::instance()->load(content, parents);
    //}
}

void QtCentral::createOne()
{
	auto content = FlexManager::instance()->createFlexWidget(Flex::ToolView, this, Flex::widgetFlags(), "M");
    layout()->addWidget(content);
}

void QtCentral::createTwo()
{
    auto splitter = new QSplitter(this);
    auto l = FlexManager::instance()->createFlexWidget(Flex::HybridView, this, Flex::widgetFlags(), "L");
    auto r = FlexManager::instance()->createFlexWidget(Flex::HybridView, this, Flex::widgetFlags(), "R");
    splitter->addWidget(l);
    splitter->addWidget(r);
    layout()->addWidget(splitter);
}

void QtCentral::on_flexWidgetCreated(FlexWidget* flexWidget)
{
    if (flexWidget->objectName() == "M")
    {
        layout()->addWidget(flexWidget);
    }
    else
    {
        flexWidget->show();
    }
}

void QtCentral::on_dockWidgetCreated(DockWidget* dockWidget)
{
    /*if (dockWidget->objectName() == "View-1")
    {
        if (dockWidget->widget() != _widget)
        {
            dockWidget->setWidget(_widget);
        }
    }*/
}

void QtCentral::on_flexWidgetDestroying(FlexWidget* flexWidget)
{
}

void QtCentral::on_dockWidgetDestroying(DockWidget* dockWidget)
{
    /*if (dockWidget->objectName() == "View-1")
    {
        if (dockWidget->widget() == _widget)
        {
            dockWidget->detachWidget(_widget);
        }
    }*/
}

class QtGuiderImpl
{
public:
    Ui::QtGuiderClass ui;
};

QtGuider::QtGuider(QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags), impl(new QtGuiderImpl)
{
    qApp->setProperty("window", QVariant::fromValue<QObject*>(this));

    impl->ui.setupUi(this);

    for (int i = 0; i < 15; i++)
    {
        impl->ui.menuFile->addAction(QString("File-%1").arg(i), this, SLOT(actionFile_X_triggered()));
    }

    for (int i = 0; i < 15; i++)
    {
        impl->ui.menuView->addAction(QString("View-%1").arg(i), this, SLOT(actionView_X_triggered()));
    }

    setCentralWidget(new QtCentral(this));

    //auto docker1 = new QDockWidget("Docker1", this);
    //docker1->setWidget(new QWidget(this));
    //auto docker2 = new QDockWidget("Docker2", this);
    //docker2->setWidget(new QWidget(this));

    //addDockWidget(Qt::LeftDockWidgetArea, docker1);
    //addDockWidget(Qt::LeftDockWidgetArea, docker2);

    setGeometry(QApplication::desktop()->availableGeometry().adjusted(100, 100, -100, -100));
}

QtGuider::~QtGuider()
{

}

void QtGuider::openFile_N(int n)
{
    QString dockWidgetName = QString("File-%1").arg(n);

    DockWidget* widget = nullptr;

    if ((widget = FlexManager::instance()->dockWidget(dockWidgetName)) != nullptr)
    {
        widget->activate();
    }
    else
    {
        if (!FlexManager::instance()->restore(dockWidgetName))
        {
            FlexWidget* flexWidget = FlexManager::instance()->createFlexWidget(Flex::FileView, Flex::parent(Flex::FileView), Flex::windowFlags());
            DockWidget* dockWidget = FlexManager::instance()->createDockWidget(Flex::FileView, flexWidget, Flex::widgetFlags(), dockWidgetName);
			
            dockWidget->setViewMode(Flex::FileView);
            dockWidget->setWindowTitle(dockWidgetName);
            flexWidget->addDockWidget(dockWidget);
            flexWidget->show();
            flexWidget->move(geometry().center() - flexWidget->rect().center());
        }
    }
}

void QtGuider::openView_N(int n)
{
    QString dockWidgetName = QString("View-%1").arg(n);

    DockWidget* widget = nullptr;

    if ((widget = FlexManager::instance()->dockWidget(dockWidgetName)) != nullptr)
    {
        widget->activate();
    }
    else
    {
        if (!FlexManager::instance()->restore(dockWidgetName))
        {
            FlexWidget* flexWidget = FlexManager::instance()->createFlexWidget(Flex::ToolView, Flex::parent(Flex::ToolView), Flex::windowFlags());
            DockWidget* dockWidget = FlexManager::instance()->createDockWidget(Flex::ToolView, flexWidget, Flex::widgetFlags(), dockWidgetName);
            
			switch (n)
			{
			case 0:
				fillSolutionExplorer(dockWidget->widget());
				break;
			case 1:
				createTree(dockWidget->widget());
				break;
			case 2:
				createTable(dockWidget->widget());
				break;
			default:
				break;
			}
			
			dockWidget->setViewMode(Flex::ToolView);
			dockWidget->setWindowTitle(dockWidgetName); 
            flexWidget->addDockWidget(dockWidget);
            flexWidget->show();
            flexWidget->move(geometry().center() - flexWidget->rect().center());
        }
    }
}

void QtGuider::on_actionFile_N_triggered()
{
    static int i = 0;
    for (;; i++)
    {
        QString dockWidgetName = QString("File-%1").arg(i);

        if (FlexManager::instance()->hasDockWidget(dockWidgetName))
        {
            continue;
        }

        openFile_N(i);

        break;
    }
}

void QtGuider::on_actionView_N_triggered()
{
    static int i = 0;
    for (;; i++)
    {
        QString dockWidgetName = QString("View-%1").arg(i);

        if (FlexManager::instance()->hasDockWidget(dockWidgetName))
        {
            continue;
        }

        openView_N(i);

        break;
    }
}

void QtGuider::on_actionTool_N_triggered()
{
    FlexManager::instance()->flexWidget("M")->clearDockSites();
}

void QtGuider::actionFile_X_triggered()
{
    openFile_N(static_cast<QAction*>(sender())->text().split("-")[1].toInt());
}

void QtGuider::actionView_X_triggered()
{
    openView_N(static_cast<QAction*>(sender())->text().split("-")[1].toInt());
}

void QtGuider::on_actionEdit_Undo_triggered()
{
    static int i = 0;
    for (;; i++)
    {
        QString dockWidgetName = QString("FileTool-%1").arg(i);

        if (FlexManager::instance()->hasDockWidget(dockWidgetName))
        {
            continue;
        }

        if (!FlexManager::instance()->restore(dockWidgetName))
        {
            DockWidget* dockWidget = FlexManager::instance()->createDockWidget(Flex::FileView, Flex::parent(Flex::FileView), Flex::windowFlags());
            dockWidget->setViewMode(Flex::FileView);
            dockWidget->setWindowTitle(dockWidgetName);
            dockWidget->show();
            dockWidget->move(geometry().center() - dockWidget->rect().center());
        }

        break;
    }
}

void QtGuider::on_actionEdit_Redo_triggered()
{
    static int i = 0;
    for (;; i++)
    {
        QString dockWidgetName = QString("ViewTool-%1").arg(i);

        if (FlexManager::instance()->hasDockWidget(dockWidgetName))
        {
            continue;
        }

        if (!FlexManager::instance()->restore(dockWidgetName))
        {
            DockWidget* dockWidget = FlexManager::instance()->createDockWidget(Flex::ToolView, Flex::parent(Flex::ToolView), Flex::windowFlags());
            dockWidget->setViewMode(Flex::ToolView);
            dockWidget->setWindowTitle(dockWidgetName);
            dockWidget->show();
            dockWidget->move(geometry().center() - dockWidget->rect().center());
        }

        break;
    }
}

void QtGuider::closeEvent(QCloseEvent* evt)
{
    QByteArray content = FlexManager::instance()->save();

    QSettings settings("QtFlex5", "QtGuider");

    settings.setValue("Flex", content);

    FlexManager::instance()->close();
}

void QtGuider::fillSolutionExplorer(QWidget * w){
	QWidget* baseSheet = w;
	/*QToolBar* toolBar = new QToolBar();
	toolBar->setIconSize(QSize(20, 20));
	QAction* act = toolBar->addAction(QIcon(":/res/backward.svg"), QString("Back"));
	act->setToolTip(tr("Back"));
	act = toolBar->addAction(QIcon(":/res/forward.svg"), QString("Forward"));
	act->setToolTip(tr("Forward"));*/

	//m_panelSolutionExplorer->setWidget(baseSheet);
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setMargin(1);
	layout->setSpacing(0);

	QTreeWidget* treeWidget = new QTreeWidget;
	treeWidget->setAlternatingRowColors(false);

	treeWidget->setMinimumWidth(190);
	treeWidget->setHeaderHidden(true);

	QStringList strings;
	strings.append(tr("Solution 'qtitandocking'"));
	QTreeWidgetItem* mainItem = new QTreeWidgetItem(strings);
	mainItem->setExpanded(true);
	mainItem->setIcon(0, QIcon(":/res/solution.svg"));
	treeWidget->insertTopLevelItem(0, mainItem);
	treeWidget->expandItem(mainItem);

	strings.clear();
	strings.append(tr("DockApplication"));
	QTreeWidgetItem* item = new QTreeWidgetItem(mainItem, strings);
	QFont font = item->font(0);
	font.setBold(true);
	item->setFont(0, font);
	item->setIcon(0, QIcon(":/res/cpp_project.png"));
	item->setExpanded(true);
	treeWidget->insertTopLevelItem(1, item);
	treeWidget->expandItem(item);

	strings.clear();
	strings.append(tr("Source Files"));
	QTreeWidgetItem* itemSource = new QTreeWidgetItem(item, strings);
	itemSource->setIcon(0, QIcon(":/res/filterfolderOpen.png"));
	itemSource->setExpanded(true);
	treeWidget->insertTopLevelItem(1, itemSource);
	treeWidget->expandItem(item);

	strings.clear();
	strings.append(tr("main.cpp"));
	item = new QTreeWidgetItem(itemSource, strings);
	item->setIcon(0, QIcon(":/res/cpp.png"));
	treeWidget->insertTopLevelItem(1, item);
	treeWidget->expandItem(item);

	strings.clear();
	strings.append(tr("mainwindow.cpp"));
	item = new QTreeWidgetItem(itemSource, strings);
	item->setIcon(0, QIcon(":/res/cpp.png"));
	treeWidget->insertTopLevelItem(1, item);
	treeWidget->expandItem(item);

	strings.clear();
	strings.append(tr("mainwindow.h"));
	item = new QTreeWidgetItem(itemSource, strings);
	item->setIcon(0, QIcon(":/res/cpp.png"));
	treeWidget->insertTopLevelItem(1, item);
	treeWidget->expandItem(item);

	strings.clear();
	strings.append(tr("mdichild.cpp"));
	item = new QTreeWidgetItem(itemSource, strings);
	item->setIcon(0, QIcon(":/res/cpp.png"));
	treeWidget->insertTopLevelItem(1, item);
	treeWidget->expandItem(item);

	strings.clear();
	strings.append(tr("mdichild.cpp"));
	item = new QTreeWidgetItem(itemSource, strings);
	item->setIcon(0, QIcon(":/res/cpp.png"));
	treeWidget->insertTopLevelItem(1, item);
	treeWidget->expandItem(item);

	//layout->addWidget(toolBar);
	layout->addWidget(treeWidget, 1);
	baseSheet->setLayout(layout);
}

void QtGuider::createTree(QWidget * w){
	QTreeWidget* treeWidget = new QTreeWidget(w);
	treeWidget->setAlternatingRowColors(true);

	treeWidget->setMinimumWidth(190);
	treeWidget->setHeaderHidden(true);

	QStringList strings;
	strings.append(tr("Item 1"));
	QTreeWidgetItem* mainItem = new QTreeWidgetItem(strings);
	mainItem->setCheckState(0, Qt::Checked);
	mainItem->setExpanded(false);
	mainItem->setIcon(0, QIcon(":/res/open16x16.png"));
	treeWidget->insertTopLevelItem(0, mainItem);

	strings.clear();
	strings.append(tr("Item 2"));
	QTreeWidgetItem* item = new QTreeWidgetItem(mainItem, strings);
	item->setCheckState(0, Qt::Checked);
	item->setIcon(0, QIcon(":/res/open16x16.png"));
	item->setExpanded(true);
	treeWidget->insertTopLevelItem(1, item);

	strings.clear();
	strings.append(tr("Item 3"));
	item = new QTreeWidgetItem(mainItem, strings);
	item->setCheckState(0, Qt::Checked);
	item->setIcon(0, QIcon(":/res/open16x16.png"));
	item->setExpanded(true);
	treeWidget->insertTopLevelItem(1, item);

	strings.clear();
	strings.append(tr("Item 4"));
	item = new QTreeWidgetItem(mainItem, strings);
	item->setCheckState(0, Qt::Checked);
	item->setIcon(0, QIcon(":/res/open16x16.png"));
	item->setExpanded(true);
	treeWidget->insertTopLevelItem(1, item);

	strings.clear();
	strings.append(tr("Item 5"));
	item = new QTreeWidgetItem(mainItem, strings);
	item->setCheckState(0, Qt::Checked);
	item->setIcon(0, QIcon(":/res/open16x16.png"));
	item->setExpanded(true);
	treeWidget->insertTopLevelItem(1, item);

	strings.clear();
	strings.append(tr("Item 6"));
	item = new QTreeWidgetItem(mainItem, strings);
	item->setCheckState(0, Qt::Checked);
	item->setIcon(0, QIcon(":/res/open16x16.png"));
	item->setExpanded(true);
	treeWidget->insertTopLevelItem(1, item);
	treeWidget->expandAll();

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setMargin(1);
	layout->setSpacing(0);

	layout->addWidget(treeWidget, 1);
	w->setLayout(layout);
}

void QtGuider::createTable(QWidget * w){
	QTableWidget* tableWidget = new QTableWidget(w);
	tableWidget->setAlternatingRowColors(true);

	tableWidget->setColumnCount(11);
	tableWidget->setRowCount(6);
	tableWidget->setShowGrid(false);
	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Name")));
	tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Path")));
	tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Optimized")));
	tableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem(tr("User Code")));
	tableWidget->setHorizontalHeaderItem(4, new QTableWidgetItem(tr("Symbol Status")));
	tableWidget->setHorizontalHeaderItem(5, new QTableWidgetItem(tr("Symbol File")));
	tableWidget->setHorizontalHeaderItem(6, new QTableWidgetItem(tr("Order")));
	tableWidget->setHorizontalHeaderItem(7, new QTableWidgetItem(tr("Version")));
	tableWidget->setHorizontalHeaderItem(8, new QTableWidgetItem(tr("Timestamp")));
	tableWidget->setHorizontalHeaderItem(9, new QTableWidgetItem(tr("Addresswewe")));
	tableWidget->setHorizontalHeaderItem(10, new QTableWidgetItem(tr("Process")));

	int height = tableWidget->horizontalHeader()->sizeHint().height();

	tableWidget->setRowHeight(0, height);
	tableWidget->setVerticalHeaderItem(0, new QTableWidgetItem(QIcon(":/res/modules.png"), tr("")));
	tableWidget->setItem(0, 0, new QTableWidgetItem(tr("ntdll.dll")));
	tableWidget->setItem(0, 1, new QTableWidgetItem(tr("C:/Windows/System32/ntdll.dll")));
	tableWidget->setItem(0, 2, new QTableWidgetItem(tr("N/A")));
	tableWidget->setItem(0, 3, new QTableWidgetItem(tr("N/A")));
	tableWidget->setItem(0, 4, new QTableWidgetItem(tr("Symbols loaded.")));
	tableWidget->setItem(0, 5, new QTableWidgetItem(tr("C:/Qtitan/customstyles.pdb")));
	tableWidget->setItem(0, 6, new QTableWidgetItem(tr("1")));
	tableWidget->setItem(0, 7, new QTableWidgetItem(tr("6.1.7600.16385 (win7_rtm.090713-1255)")));
	tableWidget->setItem(0, 8, new QTableWidgetItem(tr("27.10.2010 8:32")));
	tableWidget->setItem(0, 9, new QTableWidgetItem(tr("773A0000-774DD000")));
	tableWidget->setItem(0, 10, new QTableWidgetItem(tr("[3376] customstyles.exe: Native")));

	tableWidget->setRowHeight(1, height);
	tableWidget->setVerticalHeaderItem(1, new QTableWidgetItem(QIcon(":/res/modules.png"), tr("")));
	tableWidget->setItem(1, 0, new QTableWidgetItem(tr("ntdll.dll")));
	tableWidget->setItem(1, 1, new QTableWidgetItem(tr("C:/Windows/System32/kernel32.dll")));
	tableWidget->setItem(1, 2, new QTableWidgetItem(tr("N/A")));
	tableWidget->setItem(1, 3, new QTableWidgetItem(tr("N/A")));
	tableWidget->setItem(1, 4, new QTableWidgetItem(tr("No symbols loaded.")));
	tableWidget->setItem(1, 5, new QTableWidgetItem(tr("")));
	tableWidget->setItem(1, 6, new QTableWidgetItem(tr("2")));
	tableWidget->setItem(1, 7, new QTableWidgetItem(tr("6.1.7600.16385 (win7_rtm.090713-1255)")));
	tableWidget->setItem(1, 8, new QTableWidgetItem(tr("27.10.2010 8:32")));
	tableWidget->setItem(1, 9, new QTableWidgetItem(tr("872A0000-766DD000")));
	tableWidget->setItem(1, 10, new QTableWidgetItem(tr("[3376] customstyles.exe: Native")));

	tableWidget->setRowHeight(2, height);
	tableWidget->setVerticalHeaderItem(2, new QTableWidgetItem(QIcon(":/res/modules.png"), tr("")));
	tableWidget->setItem(2, 0, new QTableWidgetItem(tr("kernelBase.dll")));
	tableWidget->setItem(2, 1, new QTableWidgetItem(tr("C:/Windows/System32/kernelBase.dll")));
	tableWidget->setItem(2, 2, new QTableWidgetItem(tr("N/A")));
	tableWidget->setItem(2, 3, new QTableWidgetItem(tr("N/A")));
	tableWidget->setItem(2, 4, new QTableWidgetItem(tr("No symbols loaded.")));
	tableWidget->setItem(2, 5, new QTableWidgetItem(tr("")));
	tableWidget->setItem(2, 6, new QTableWidgetItem(tr("3")));
	tableWidget->setItem(2, 7, new QTableWidgetItem(tr("6.1.7600.16385 (win7_rtm.090713-1255)")));
	tableWidget->setItem(2, 8, new QTableWidgetItem(tr("08.10.2010 8:32")));
	tableWidget->setItem(2, 9, new QTableWidgetItem(tr("772A0000-566DD000")));
	tableWidget->setItem(2, 10, new QTableWidgetItem(tr("[3376] customstyles.exe: Native")));

	tableWidget->setRowHeight(3, height);
	tableWidget->setVerticalHeaderItem(3, new QTableWidgetItem(QIcon(":/res/modules.png"), tr("")));
	tableWidget->setItem(3, 0, new QTableWidgetItem(tr("QtCoreed4.dll")));
	tableWidget->setItem(3, 1, new QTableWidgetItem(tr("C:/Qt/4.6.2/Bin/QtCored4.dll")));
	tableWidget->setItem(3, 2, new QTableWidgetItem(tr("N/A")));
	tableWidget->setItem(3, 3, new QTableWidgetItem(tr("N/A")));
	tableWidget->setItem(3, 4, new QTableWidgetItem(tr("Symbols loaded.")));
	tableWidget->setItem(3, 5, new QTableWidgetItem(tr("C:/Qt/4.6.2/lib/QtCored4.pdb")));
	tableWidget->setItem(3, 6, new QTableWidgetItem(tr("4")));
	tableWidget->setItem(3, 7, new QTableWidgetItem(tr("6.1.7600.16385 (win7_rtm.090713-1255)")));
	tableWidget->setItem(3, 8, new QTableWidgetItem(tr("08.10.2010 8:32")));
	tableWidget->setItem(3, 9, new QTableWidgetItem(tr("772A0000-566DD000")));
	tableWidget->setItem(3, 10, new QTableWidgetItem(tr("[3376] customstyles.exe: Native")));

	tableWidget->setRowHeight(4, height);
	tableWidget->setVerticalHeaderItem(4, new QTableWidgetItem(QIcon(":/res/modules.png"), tr("")));
	tableWidget->setItem(4, 0, new QTableWidgetItem(tr("user32.dll")));
	tableWidget->setItem(4, 1, new QTableWidgetItem(tr("C:/Windows/System32/user32.dll")));
	tableWidget->setItem(4, 2, new QTableWidgetItem(tr("N/A")));
	tableWidget->setItem(4, 3, new QTableWidgetItem(tr("N/A")));
	tableWidget->setItem(4, 4, new QTableWidgetItem(tr("No symbols loaded.")));
	tableWidget->setItem(4, 5, new QTableWidgetItem(tr("")));
	tableWidget->setItem(4, 6, new QTableWidgetItem(tr("3")));
	tableWidget->setItem(4, 7, new QTableWidgetItem(tr("6.1.7600.16385 (win7_rtm.090713-1255)")));
	tableWidget->setItem(4, 8, new QTableWidgetItem(tr("08.10.2010 8:32")));
	tableWidget->setItem(4, 9, new QTableWidgetItem(tr("552A0000-544AD000")));
	tableWidget->setItem(4, 10, new QTableWidgetItem(tr("[3376] customstyles.exe: Native")));

	tableWidget->setRowHeight(5, height);
	tableWidget->setVerticalHeaderItem(5, new QTableWidgetItem(QIcon(":/res/modules.png"), tr("")));
	tableWidget->setItem(5, 0, new QTableWidgetItem(tr("usp10.dll")));
	tableWidget->setItem(5, 1, new QTableWidgetItem(tr("C:/Windows/System32/usp10.dll")));
	tableWidget->setItem(5, 2, new QTableWidgetItem(tr("N/A")));
	tableWidget->setItem(5, 3, new QTableWidgetItem(tr("N/A")));
	tableWidget->setItem(5, 4, new QTableWidgetItem(tr("No symbols loaded.")));
	tableWidget->setItem(5, 5, new QTableWidgetItem(tr("")));
	tableWidget->setItem(5, 6, new QTableWidgetItem(tr("3")));
	tableWidget->setItem(5, 7, new QTableWidgetItem(tr("6.1.7600.16385 (win7_rtm.090713-1255)")));
	tableWidget->setItem(5, 8, new QTableWidgetItem(tr("08.10.2010 8:32")));
	tableWidget->setItem(5, 9, new QTableWidgetItem(tr("552A0000-544AD000")));
	tableWidget->setItem(5, 10, new QTableWidgetItem(tr("[3376] customstyles.exe: Native")));

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setMargin(1);
	layout->setSpacing(0);

	layout->addWidget(tableWidget,1);
	w->setLayout(layout);
}