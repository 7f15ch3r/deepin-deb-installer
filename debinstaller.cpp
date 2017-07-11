#include "debinstaller.h"
#include "filechoosewidget.h"
#include "singleinstallpage.h"
#include "multipleinstallpage.h"
#include "deblistmodel.h"

#include <QKeyEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>

#include <QApt/DebFile>

using QApt::DebFile;

DWIDGET_USE_NAMESPACE

DebInstaller::DebInstaller(QWidget *parent)
    : DWindow(parent),

      m_fileListModel(new DebListModel(this)),

      m_centralLayout(new QStackedLayout),
      m_fileChooseWidget(new FileChooseWidget)
{
    m_centralLayout->addWidget(m_fileChooseWidget);
    m_centralLayout->setContentsMargins(0, 0, 0, 0);
    m_centralLayout->setSpacing(0);

    setLayout(m_centralLayout);
    setFixedSize(480, 380);
    setWindowTitle(tr("Deepin Deb Installer"));
    setWindowIcon(QIcon::fromTheme("deepin-deb-installer"));
    setTitleIcon(QIcon::fromTheme("deepin-deb-installer").pixmap(24, 24));
    setTitle(QString());
    move(qApp->primaryScreen()->geometry().center() - geometry().center());

    connect(m_fileChooseWidget, &FileChooseWidget::packagesSelected, this, &DebInstaller::onPackagesSelected);
    connect(m_fileListModel, &DebListModel::appendOutputInfo, this, &DebInstaller::onOutputPrinted);
}

DebInstaller::~DebInstaller()
{
}

void DebInstaller::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
#ifdef QT_DEBUG
    case Qt::Key_Escape:        qApp->quit();       break;
#endif
    default:;
    }
}

void DebInstaller::onPackagesSelected(const QStringList &packages)
{
    Q_ASSERT(m_fileListModel->preparedPackages().isEmpty());

    for (const auto &package : packages)
    {
        DebFile *p = new DebFile(package);
        if (!p->isValid())
        {
            qWarning() << "package invalid: " << package;

            delete p;
            continue;
        }

        m_fileListModel->appendPackage(p);
    }

    const int packageCount = m_fileListModel->preparedPackages().size();
    // no packages found
    if (packageCount == 0)
        return;

    if (packageCount == 1)
    {
        // single package install
        SingleInstallPage *singlePage = new SingleInstallPage(m_fileListModel);

        m_centralLayout->addWidget(singlePage);
    } else {
        // multiple packages install
        setTitle(tr("Bulk Install"));

        MultipleInstallPage *multiplePage = new MultipleInstallPage(m_fileListModel);

        m_centralLayout->addWidget(multiplePage);
    }

    // switch to new page.
    m_centralLayout->setCurrentIndex(1);
}

void DebInstaller::onOutputPrinted(const QString &output) const
{
    qDebug() << output;
}
