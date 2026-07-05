#include "UBEditable.h"

#include "domain/shapes/UBEditShapeUndoCommand.h"

#include "core/UBApplication.h"
#include "board/UBBoardController.h"
#include "domain/UBGraphicsScene.h"

UBAbstractEditable::UBAbstractEditable()
{
    mEditMode = false;
}

UBAbstractEditable::~UBAbstractEditable()
{

}

void UBAbstractEditable::addHandle(UBAbstractHandle *handle)
{
    mHandles.push_back(handle);
}

UBAbstractHandle* UBAbstractEditable::getHandle(HandleId id) const
{
    for (auto handle : mHandles)
    {
        if (handle->getId() == id)
        {
            return handle;
        }
    }

    return nullptr;
}

void UBAbstractEditable::showEditMode(bool show)
{
    if(!show){
        for(int i = 0; i < mHandles.size(); i++){
            mHandles.at(i)->hide();
        }
    }else{
        for(int i = 0; i < mHandles.size(); i++){
            mHandles.at(i)->show();
        }
    }

    if (show && !mEditMode)
    {
        // reset and create undo command on transition to edit mode
        mModified = false;
        mUndoCommand = new UBEditShapeUndoCommand{dynamic_cast<UBAbstractGraphicsItem*>(this)};
    }
    else if (!show && mEditMode)
    {
        if (mModified)
        {
            // commit undo command when shape was modified during editing
            mUndoCommand->recordEditedItem();
            UBApplication::undoStack->push(mUndoCommand);
        }
        else
        {
            // delete undo command
            delete mUndoCommand;
        }

        mUndoCommand = nullptr;
    }

    mEditMode = show;
}

void UBAbstractEditable::deactivateEditionMode()
{
    //nop
}

bool UBAbstractEditable::isInEditMode() const
{
    return mEditMode;
}

void UBAbstractEditable::setModified()
{
    mModified = true;
}
