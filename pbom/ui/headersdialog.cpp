#include "headersdialog.h"
#include <QTreeWidgetItem>
#include <QAction>
#include <QMenu>
#include "util/log.h"

#define LOG(...) LOGGER("ui/HeadersDialog", __VA_ARGS__)

namespace pboman3 {
    constexpr int colName = 0;
    constexpr int colValue = 1;
    constexpr Qt::ItemFlags itemFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

    HeadersDialog::HeadersDialog(HeadersModel* model, QWidget* parent)
        : QDialog(parent),
          ui_(new Ui::HeadersDialog),
          model_(model) {
        ui_->setupUi(this);

        renderHeaderItems();
        setupConnections();
    }

    HeadersDialog::~HeadersDialog() {
        delete ui_;
    }

    void HeadersDialog::accept() {
        LOG(info, "User clicked the Accept button")

        QList<QSharedPointer<PboHeader>> headers;
        headers.reserve(ui_->treeWidget->topLevelItemCount());

        for (int i = 0; i < ui_->treeWidget->topLevelItemCount(); i++) {
            const QTreeWidgetItem* item = ui_->treeWidget->topLevelItem(i);

            if (isValidHeader(item->text(colName), item->text(colValue))) {
                LOG(debug, "Append header, name=", item->text(colName), "value=", item->text(colValue))
                headers.append(QSharedPointer<PboHeader>(new PboHeader(item->text(colName), item->text(colValue))));
            }
        }

        model_->setData(std::move(headers));

        LOG(info, "Accepting the dialog")
        QDialog::accept();
    }

    void HeadersDialog::showEvent(QShowEvent*) {
        const int width = ui_->treeWidget->header()->width();
        ui_->treeWidget->setColumnWidth(0, static_cast<int>(width * 0.45));
    }

    void HeadersDialog::renderHeaderItems() const {
        LOG(info, "Rendering headers")

        ui_->treeWidget->setHeaderLabels(QList<QString>({"Name", "Value"}));
        for (const QSharedPointer<PboHeader>& header : *model_) {
            LOG(debug, "Render header, name=", header->name, "value=", header->value)

            const auto item = new QTreeWidgetItem();
            item->setText(colName, header->name);
            item->setText(colValue, header->value);
            item->setFlags(itemFlags);
            ui_->treeWidget->addTopLevelItem(item);
        }
        LOG(info, "Rendered", ui_->treeWidget->topLevelItemCount(), "items")
    }

    void HeadersDialog::setupConnections() const {
        connect(ui_->actionInsertAbove, &QAction::triggered, this, [this]() { onInsertClick(0); });
        connect(ui_->actionInsertBelow, &QAction::triggered, this, [this]() { onInsertClick(1); });
        connect(ui_->actionMoveUp, &QAction::triggered, this, [this]() { onMoveClick(-1); });
        connect(ui_->actionMoveDown, &QAction::triggered, this, [this]() { onMoveClick(1); });
        connect(ui_->actionInsert, &QAction::triggered, this, [this]() { onInsertClick(1); });
        connect(ui_->actionRemove, &QAction::triggered, this, &HeadersDialog::onRemoveClick);

        ui_->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(ui_->treeWidget, &QTreeWidget::customContextMenuRequested, this, &HeadersDialog::contextMenuRequested);
    }

    void HeadersDialog::contextMenuRequested(const QPoint& pos) const {
        QMenu menu;
        if (ui_->treeWidget->currentIndex().isValid()) {
            LOG(info, "Open the context menu - currentIndex is valid")
            menu.addAction(ui_->actionInsertAbove);
            menu.addAction(ui_->actionInsertBelow);
            menu.addSeparator();
            menu.addAction(ui_->actionMoveUp);
            menu.addAction(ui_->actionMoveDown);
            menu.addSeparator();
            menu.addAction(ui_->actionRemove);
        } else {
            LOG(info, "Open the context menu - currentIndex is invalid")
            menu.addAction(ui_->actionInsert);
        }

        menu.exec(ui_->treeWidget->mapToGlobal(pos));
    }

    bool HeadersDialog::isValidHeader(const QString& name, const QString& value) const {
        return !name.isEmpty() && !value.isEmpty();
    }

    void HeadersDialog::onInsertClick(int index) const {
        LOG(info, "User clicked the Insert button")

        const auto item = new QTreeWidgetItem();
        item->setFlags(itemFlags);

        const QModelIndex selected = ui_->treeWidget->currentIndex();
        if (selected.isValid()) {
            LOG(info, "Inserting a new top level itemm currentIndex:", selected.row(), " index:", index)
            ui_->treeWidget->insertTopLevelItem(selected.row() + index, item);
        } else {
            LOG(info, "Inserting an new top level item at the top")
            ui_->treeWidget->addTopLevelItem(item);
        }

        ui_->treeWidget->setCurrentItem(item);
    }

    void HeadersDialog::onMoveClick(int index) const {
        LOG(info, "User clicked the Move button, index:", index)

        const QModelIndex selected = ui_->treeWidget->currentIndex();
        if (selected.isValid()) {
            index = selected.row() + index;
            LOG(info, "Selection is valid, selectedRow:", selected.row(), ", index:", index)
            if (index >= 0 && index < ui_->treeWidget->topLevelItemCount()) {
                QTreeWidgetItem* item = ui_->treeWidget->takeTopLevelItem(selected.row());
                LOG(info, "Moving the item: name=", item->text(colName), "value=", item->text(colValue), " from the index: ", selected.row(), "to index:", index)
                ui_->treeWidget->insertTopLevelItem(index, item);
                ui_->treeWidget->setCurrentItem(item);
            }
        }
    }

    void HeadersDialog::onRemoveClick() const {
        const QTreeWidgetItem* selected = ui_->treeWidget->currentItem();
        LOG(info, "User clicked the Remove button, name=", selected->text(colName), "value=", selected->text(colValue), "index=", ui_->treeWidget->currentIndex())
        delete selected; //https://doc.qt.io/qt-6/qtreewidgetitem.html#dtor.QTreeWidgetItem
    }
}
