#include "packageslistdelegate.h"
#include "deblistmodel.h"

#include <QPainter>

PackagesListDelegate::PackagesListDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
    const QIcon icon = QIcon::fromTheme("application-vnd.debian.binary-package", QIcon::fromTheme("debian-swirl"));
    m_packageIcon = icon.pixmap(32, 32);
}

void PackagesListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    painter->fillRect(option.rect, Qt::gray);

    const int content_x = 45;

    // draw top border
    if (index.row())
    {
        const QPoint start(content_x, option.rect.top());
        const QPoint end(option.rect.right() - 10, option.rect.top());

        painter->setPen(QColor(0, 0, 0, 255 * .05));
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->drawLine(start, end);
    }

    painter->setRenderHint(QPainter::Antialiasing);

    // draw package icon
    const int x = 5;
    const int y = option.rect.top() + (option.rect.height() - m_packageIcon.height()) / 2;
    painter->drawPixmap(x, y, m_packageIcon);

    // draw package name
    QRect name_rect = option.rect;
    name_rect.setLeft(content_x);
    name_rect.setHeight(name_rect.height() / 2);

    const QString name = index.data(DebListModel::PackageNameRole).toString();
    const QFont old_font = painter->font();
    QFont f = old_font;
    f.setWeight(QFont::Bold);
    painter->setFont(f);
    const QString name_str = painter->fontMetrics().elidedText(name, Qt::ElideRight, 306);
    const QRectF name_bounding_rect = painter->boundingRect(name_rect, name_str, Qt::AlignLeft | Qt::AlignBottom);
    painter->setPen(Qt::black);
    painter->drawText(name_rect, name_str, Qt::AlignLeft | Qt::AlignBottom);
    painter->setFont(old_font);

    // draw package version
    const int version_x = name_bounding_rect.right() + 8;
    QRect version_rect = name_rect;
    version_rect.setLeft(version_x);
    version_rect.setRight(option.rect.right() - 85);
    const QString version = index.data(DebListModel::PackageVersionRole).toString();
    const QString version_str = painter->fontMetrics().elidedText(version, Qt::ElideRight, version_rect.width());
    painter->drawText(version_rect, version_str, Qt::AlignLeft | Qt::AlignBottom);

    // install status
    const int install_stat = index.data(DebListModel::PackageOperateStatusRole).toInt();
    if (install_stat != DebListModel::Prepare)
    {
        QRect install_status_rect = option.rect;
        install_status_rect.setRight(option.rect.right() - 15);
        install_status_rect.setLeft(option.rect.right() - 80);

        switch (install_stat)
        {
        case DebListModel::Operating:
            painter->setPen(QColor(124, 124, 124));
            painter->drawText(install_status_rect, "Installing", Qt::AlignVCenter | Qt::AlignRight);
            break;
        case DebListModel::Success:
            painter->setPen(QColor(65, 117, 5));
            painter->drawText(install_status_rect, "Success", Qt::AlignVCenter | Qt::AlignRight);
            break;
        default:
            painter->setPen(QColor(255, 109, 109));
            painter->drawText(install_status_rect, "Failed", Qt::AlignVCenter | Qt::AlignRight);
            break;
        }
    }

    // draw package info
    QRect info_rect = option.rect;
    info_rect.setLeft(content_x);
    info_rect.setTop(name_rect.bottom() + 1 + 3);
    if (install_stat != DebListModel::Failed)
    {
        const QString info_str = painter->fontMetrics().elidedText(index.data(DebListModel::PackageDescriptionRole).toString(), Qt::ElideRight, 306);
        painter->setPen(QColor(90, 90, 90));
        painter->drawText(info_rect, info_str, Qt::AlignLeft | Qt::AlignTop);
    } else {
        const QString info_str = painter->fontMetrics().elidedText(index.data(DebListModel::PackageFailReasonRole).toString(), Qt::ElideRight, 306);
        painter->setPen(QColor(255, 109, 109));
        painter->drawText(info_rect, info_str, Qt::AlignLeft | Qt::AlignTop);
    }
}

QSize PackagesListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
