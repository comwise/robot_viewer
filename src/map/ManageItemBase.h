#ifndef __COMWISE__ROBOT_VIEWER__MANAGE_ITEM_BASE__H__
#define __COMWISE__ROBOT_VIEWER__MANAGE_ITEM_BASE__H__

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QList>
#include <QDateTime>

template <class T>
class ManageItemBase
{
public:
    ManageItemBase() : basic_id_(0) {}
    virtual ~ManageItemBase() {}

    virtual void SetBasicID(int basic_id) { basic_id_ = basic_id; }

    virtual void SetSence(QGraphicsScene *sce);
    virtual void SetItemsFlag(QGraphicsItem::GraphicsItemFlag flag, bool enable);
    virtual void SetItemsSelected(bool select);
    virtual void SetItemScale(double scale);
    virtual void SetItemVisible(bool enable);

    virtual bool AddItem(T *item);
    virtual bool DelItem(T *item);
    virtual bool ClearItems();

    virtual QList<T *> GetItems();
    virtual QList<T *> GetSelectItems();

    virtual QGraphicsItem *FindItemInItems(const QList<QGraphicsItem *> &items, int type);
    virtual QList<T *> FindItemsInItems(const QList<QGraphicsItem *> &items, int type);

protected:
    unsigned int GetUnusedId();
    QGraphicsScene *base_sce_{nullptr};
    QList<T *> items_;
    int basic_id_{0};
};

template <class T>
void ManageItemBase<T>::SetItemsSelected(bool isSelect)
{
    foreach (T *item, items_) {
        if (item) {
            item->setSelected(isSelect);
        }
    }
}

template <class T>
void ManageItemBase<T>::SetItemsFlag(QGraphicsItem::GraphicsItemFlag flag, bool enable)
{
    foreach (T *item, items_) {
        if (item) {
            item->setFlag(flag, enable);
        }
    }
}

template <class T>
void ManageItemBase<T>::SetItemScale(double scale)
{
    foreach (T *item, items_) {
        if (item) {
            item->setScale(scale);
        }
    }
}

template <class T>
void ManageItemBase<T>::SetItemVisible(bool enable)
{
    foreach (T *item, items_) {
        if (item) {
            item->setVisible(enable);
        }
    }
}

template <class T>
unsigned int ManageItemBase<T>::GetUnusedId()
{
    if (items_.empty()) {
        return basic_id_ + 1;
    }

    char max_id[1024 * 8] = {0};
    memset(max_id, 0x00, 1024 * 8);
    for (int i = 0; i < items_.size(); ++i) {
        T *item = dynamic_cast<T *>(items_[i]);
        if (item) {
            *(max_id + item->getID()) = 0x01; //此处有bug
        }
    }

    for (unsigned int i = basic_id_; i < 1024 * 8; ++i) {
        if (!(*(max_id + 1 + i))) {
            return i + 1;
        }
    }
    return 1024 * 8;
}

template <class T>
void ManageItemBase<T>::SetSence(QGraphicsScene *sce)
{
    base_sce_ = sce;
}

template <class T>
bool ManageItemBase<T>::AddItem(T *item)
{
    if (item && base_sce_) {
        items_ << item;
        base_sce_->addItem(item);
    }
    return true;
}

template <class T>
bool ManageItemBase<T>::DelItem(T *item)
{
    int index = items_.indexOf(item);
    if (index != -1) {
        items_.removeAt(index);
        if (base_sce_) {
            base_sce_->removeItem(item);
        }
        delete item;
        item = nullptr;
        return true;
    }
    return false;
}

template <class T>
bool ManageItemBase<T>::ClearItems()
{
    foreach (T *item, items_)
    {
        DelItem(item);
    }
    items_.clear();
    return true;
}

template <class T>
QList<T *> ManageItemBase<T>::GetItems()
{
    return items_;
}

template <class T>
QList<T *> ManageItemBase<T>::GetSelectItems()
{
    QList<T *> select_items;
    foreach (T *item, items_) {
        if (item && item->isSelected()) {
            select_items << item;
        }
    }
    return select_items;
}

template <class T>
QGraphicsItem *ManageItemBase<T>::FindItemInItems(const QList<QGraphicsItem *> &itemAll, int type)
{
    foreach (QGraphicsItem *item, itemAll) {
        if (item && item->type() == type) {
            return item;
        }
    }
    return NULL;
}

template <class T>
QList<T *> ManageItemBase<T>::FindItemsInItems(const QList<QGraphicsItem *> &itemAll, int type)
{
    QList<T *> item_list;
    item_list.clear();
    foreach (QGraphicsItem *item, itemAll) {
        if (item && (item->type() == type)) {
            item_list << (qgraphicsitem_cast<T *>(item));
        }
    }
    return item_list;
}



#endif //__COMWISE__ROBOT_VIEWER__MANAGE_ITEM_BASE__H__
