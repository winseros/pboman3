#include "conflictslist.h"
#include <QFileInfo>
#include <QHeaderView>
#include <QMenu>

namespace pboman3 {
    constexpr int colTitleIndex = 0;
    constexpr int colExtensionIndex = 1;
    constexpr int colResolutionIndex = 2;

    ConflictsList::ConflictsList(QWidget* parent)
        : QTreeWidget(parent),
          descriptors_(nullptr),
          conflicts_(nullptr) {
        connect(this, &ConflictsList::customContextMenuRequested, this, &ConflictsList::onContextMenuRequested);

        setUpUiActions();
    }

    void ConflictsList::setUpUiActions() {
        actionSkip_ = QSharedPointer<QAction>(new QAction(resolutionToText(ConflictResolution::Skip)));
        actionSkip_->setShortcut(QKeySequence(QKeyCombination(Qt::Modifier::CTRL, Qt::Key_K)));
        actionSkip_->setShortcutContext(Qt::WidgetShortcut);
        addAction(actionSkip_.get());
        connect(actionSkip_.get(), &QAction::triggered, this, &ConflictsList::onSkipClick);

        actionCopy_ = QSharedPointer<QAction>(new QAction(resolutionToText(ConflictResolution::Copy)));
        actionCopy_->setShortcut(QKeySequence(QKeyCombination(Qt::Modifier::CTRL, Qt::Key_C)));
        actionCopy_->setShortcutContext(Qt::WidgetShortcut);
        addAction(actionCopy_.get());
        connect(actionCopy_.get(), &QAction::triggered, this, &ConflictsList::onCopyClick);

        actionReplace_ = QSharedPointer<QAction>(new QAction(resolutionToText(ConflictResolution::Replace)));
        actionReplace_->setShortcut(QKeySequence(QKeyCombination(Qt::Modifier::CTRL, Qt::Key_R)));
        actionReplace_->setShortcutContext(Qt::WidgetShortcut);
        addAction(actionReplace_.get());
        connect(actionReplace_.get(), &QAction::triggered, this, &ConflictsList::onReplaceClick);
    }

    void ConflictsList::setDataSource(NodeDescriptors* descriptors, ConflictsParcel* conflicts) {
        for (int i = 0; i < descriptors->count(); i++) {
            const NodeDescriptor& descriptor = descriptors->at(i);
            const ConflictResolution resolution = conflicts->getResolution(descriptor);
            if (resolution != ConflictResolution::Unset) {
                auto* item = new ConflictsListItem(i);
                item->setText(colTitleIndex, descriptor.path().toString());
                item->setText(colExtensionIndex, QFileInfo(descriptor.path().last()).suffix());
                item->setText(colResolutionIndex, resolutionToText(resolution));
                addTopLevelItem(item);
            }
        }
        setHeaderLabels(QList<QString>{"File", "Extension", "Resolution"});
        header()->setStretchLastSection(false);
        setSortingEnabled(true);

        conflicts_ = conflicts;
        descriptors_ = descriptors;
    }

    void ConflictsList::showEvent(QShowEvent* event) {
        int width = header()->width();
        for (int i = 1; i < header()->count(); i++)
            width -= header()->sectionSize(i);
        header()->resizeSection(0, width);
    }

    void ConflictsList::onContextMenuRequested(const QPoint& point) const {
        QMenu menu;
        menu.addAction(actionSkip_.get());
        menu.addAction(actionCopy_.get());
        menu.addAction(actionReplace_.get());
        menu.exec(this->mapToGlobal(point));
    }

    void ConflictsList::onSkipClick() const {
        setConflictResolution(ConflictResolution::Skip);
    }

    void ConflictsList::onCopyClick() const {
        setConflictResolution(ConflictResolution::Copy);
    }

    void ConflictsList::onReplaceClick() const {
        setConflictResolution(ConflictResolution::Replace);
    }

    void ConflictsList::setConflictResolution(ConflictResolution resolution) const {
        const QList<QTreeWidgetItem*> selected = selectedItems();
        for (QTreeWidgetItem* item : selected) {
            item->setText(colResolutionIndex, resolutionToText(resolution));

            auto* selectedItem = dynamic_cast<ConflictsListItem*>(item);
            assert(selectedItem && "Must not be null");
            const NodeDescriptor& descriptor = descriptors_->at(selectedItem->id());
            conflicts_->setResolution(descriptor, resolution);
        }
    }

    QString ConflictsList::resolutionToText(ConflictResolution resolution) {
        switch (resolution) {
            case ConflictResolution::Skip:
                return "Keep original";
            case ConflictResolution::Copy:
                return "Put as a copy";
            case ConflictResolution::Replace:
                return "Replace";
            case ConflictResolution::Unset:
                return "";
        }
        return "";
    }
}
