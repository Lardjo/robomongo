#include "robomongo/gui/widgets/explorer/ExplorerDatabaseTreeItem.h"

#include "robomongo/core/domain/MongoDatabase.h"
#include "robomongo/core/domain/MongoCollection.h"
#include "robomongo/core/AppRegistry.h"
#include "robomongo/core/Core.h"
#include "robomongo/core/EventBus.h"
#include "robomongo/gui/widgets/explorer/ExplorerCollectionTreeItem.h"
#include "robomongo/gui/widgets/explorer/ExplorerDatabaseCategoryTreeItem.h"
#include "robomongo/gui/GuiRegistry.h"

using namespace Robomongo;

ExplorerDatabaseTreeItem::ExplorerDatabaseTreeItem(MongoDatabase *database) :
    QObject(),
    _database(database),
    _bus(AppRegistry::instance().bus())
{
    _bus->subscribe(this, MongoDatabase_CollectionListLoadedEvent::Type, _database);

    setText(0, _database->name());
    setIcon(0, GuiRegistry::instance().databaseIcon());
	setExpanded(true);
	setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

    _collectionFolderItem = new ExplorerDatabaseCategoryTreeItem(Collections, this);
    _collectionFolderItem->setText(0, "Collections");
    _collectionFolderItem->setIcon(0, GuiRegistry::instance().folderIcon());
    _collectionFolderItem->setExpanded(true);
    _collectionFolderItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    addChild(_collectionFolderItem);

    _javascriptFolderItem = new ExplorerDatabaseCategoryTreeItem(Functions, this);
    _javascriptFolderItem->setText(0, "Functions");
    _javascriptFolderItem->setIcon(0, GuiRegistry::instance().folderIcon());
    _javascriptFolderItem->setExpanded(true);
    _javascriptFolderItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    addChild(_javascriptFolderItem);

    _filesFolderItem = new ExplorerDatabaseCategoryTreeItem(Files, this);
    _filesFolderItem->setText(0, "Files");
    _filesFolderItem->setIcon(0, GuiRegistry::instance().folderIcon());
    _filesFolderItem->setExpanded(true);
    _filesFolderItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    addChild(_filesFolderItem);

    _usersFolderItem = new ExplorerDatabaseCategoryTreeItem(Users, this);
    _usersFolderItem->setText(0, "Users");
    _usersFolderItem->setIcon(0, GuiRegistry::instance().folderIcon());
    _usersFolderItem->setExpanded(true);
    _usersFolderItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    addChild(_usersFolderItem);
}

void ExplorerDatabaseTreeItem::expandCollections()
{
    _database->loadCollections();
}

void ExplorerDatabaseTreeItem::handle(MongoDatabase_CollectionListLoadedEvent *event)
{
    QList<MongoCollection *> collections = event->collections;

    clearCollectionFolderItems();
    createCollectionSystemFolderItem();

    for (int i = 0; i < collections.size(); ++i) {
        MongoCollection *collection = collections.at(i);

        if (collection->isSystem()) {
            addSystemCollectionItem(collection);
        } else {
            addCollectionItem(collection);
        }
    }

    showCollectionSystemFolderIfNeeded();
}

void ExplorerDatabaseTreeItem::clearCollectionFolderItems()
{
    int itemCount = _collectionFolderItem->childCount();
    for (int i = 0; i < itemCount; ++i) {
        QTreeWidgetItem *item = _collectionFolderItem->child(0);
        _collectionFolderItem->removeChild(item);
        delete item;
    }
}

void ExplorerDatabaseTreeItem::createCollectionSystemFolderItem()
{
    _collectionSystemFolderItem = new QTreeWidgetItem();
    _collectionSystemFolderItem->setIcon(0, GuiRegistry::instance().folderIcon());
    _collectionSystemFolderItem->setText(0, "System");
    _collectionFolderItem->addChild(_collectionSystemFolderItem);
}

void ExplorerDatabaseTreeItem::addCollectionItem(MongoCollection *collection)
{
    ExplorerCollectionTreeItem *collectionItem = new ExplorerCollectionTreeItem(collection);
    _collectionFolderItem->addChild(collectionItem);
}

void ExplorerDatabaseTreeItem::addSystemCollectionItem(MongoCollection *collection)
{
    ExplorerCollectionTreeItem *collectionItem = new ExplorerCollectionTreeItem(collection);
    _collectionSystemFolderItem->addChild(collectionItem);
}

void ExplorerDatabaseTreeItem::showCollectionSystemFolderIfNeeded()
{
    _collectionSystemFolderItem->setHidden(_collectionSystemFolderItem->childCount() == 0);
}
