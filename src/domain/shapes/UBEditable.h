#ifndef UBEDITABLE_H
#define UBEDITABLE_H

#include <QVector>

#include "UBAbstractHandle.h"

// forward
class UBEditShapeUndoCommand;


class UBAbstractEditable
{
public:
    explicit UBAbstractEditable();

    virtual ~UBAbstractEditable();

    void addHandle(UBAbstractHandle *mhandle);

    UBAbstractHandle* getHandle(HandleId id) const;

    void showEditMode(bool show);

    virtual void updateHandle(UBAbstractHandle *handle) = 0;

    virtual void onBackgroundSceneClick(){ }

    virtual void deactivateEditionMode();

    virtual void focusHandle(UBAbstractHandle *handle){
        Q_UNUSED(handle)
    }

    bool isInEditMode() const;

    void setModified();

protected:
    QVector<UBAbstractHandle*> mHandles;

private:
    bool mEditMode;
    bool mModified{false};
    UBEditShapeUndoCommand* mUndoCommand{nullptr};
};

#endif // UBEDITABLE_H
