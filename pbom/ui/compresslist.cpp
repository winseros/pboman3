#include "compresslist.h"
#include <QFileInfo>
#include <QHeaderView>
#include <QKeyEvent>

namespace pboman3::ui {
    constexpr int colTitleIndex = 0;
    constexpr int colExtensionIndex = 1;
    constexpr int colCompressIndex = 2;

    CompressList::CompressList(QWidget* parent)
        : QTreeWidget(parent),
          descriptors_(nullptr) {
        connect(this, &CompressList::itemChanged, this, &CompressList::onItemChanged);
    }

    void CompressList::setDataSource(NodeDescriptors* descriptors) {
        for (int i = 0; i < descriptors->count(); i++) {
            const NodeDescriptor& descriptor = descriptors->at(i);
            auto* item = new CompressListItem(i);
            item->setText(colTitleIndex, descriptor.path().toString());
            item->setText(colExtensionIndex, QFileInfo(descriptor.path().last()).suffix());
            item->setCheckState(colCompressIndex, descriptor.binarySource()->isCompressed()
                                                      ? Qt::Checked
                                                      : Qt::Unchecked);
            addTopLevelItem(item);
        }
        setHeaderLabels(QList<QString>{"File", "Extension", "Compress"});
        header()->setStretchLastSection(false);
        setSortingEnabled(true);

        descriptors_ = descriptors;
    }

    void CompressList::showEvent(QShowEvent* event) {
        int width = header()->width();
        for (int i = 1; i < header()->count(); i++)
            width -= header()->sectionSize(i);
        header()->resizeSection(0, width);
    }

    void CompressList::keyReleaseEvent(QKeyEvent* event) {
        if (event->modifiers().testFlag(Qt::NoModifier)
            && event->key() == Qt::Key_Space) {
            toggleSelectedItems();
        } else {
            QTreeWidget::keyReleaseEvent(event);
        }
    }

    void CompressList::toggleSelectedItems() const {
        const QList<QTreeWidgetItem*> items = selectedItems();

        int checkedCount = 0;
        int uncheckedCount = 0;

        for (const QTreeWidgetItem* item : items) {
            if (item->checkState(colCompressIndex) == Qt::Checked)
                checkedCount++;
            else
                uncheckedCount++;
        }

        bool compress;
        if (checkedCount == uncheckedCount) {
            compress = items.first()->checkState(colCompressIndex) == Qt::Unchecked;
        } else {
            compress = uncheckedCount > checkedCount;
        }

        for (QTreeWidgetItem* item : items) {
            item->setCheckState(colCompressIndex, compress ? Qt::Checked : Qt::Unchecked);
        }
    }

    void CompressList::onItemChanged(QTreeWidgetItem* item, int column) const {
        if (column == colCompressIndex) {
            const auto* changed = dynamic_cast<CompressListItem*>(item);
            assert(changed && "Must not be null");
            NodeDescriptor& descriptor = (*descriptors_)[changed->id()];
            descriptor.setCompressed(item->checkState(column) == Qt::Checked);
        }
    }
}
