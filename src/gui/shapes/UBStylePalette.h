/*
 * Copyright (C) 2015-2026 Département de l'Instruction Publique (DIP-SEM)
 * and contributors.
 *
 * This file is part of OpenBoard.
 *
 * OpenBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License,
 * with a specific linking exception for the OpenSSL project's
 * "OpenSSL" library (or with modified versions of it that use the
 * same license as the "OpenSSL" library).
 *
 * OpenBoard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenBoard. If not, see <http://www.gnu.org/licenses/>.
 */


#pragma once

#include <QAction>

#include "domain/shapes/UBShapeStyle.h"
#include "gui/UBFloatingPalette.h"

// forward
class UBToolbarButtonGroup;

class UBStylePalette : public UBFloatingPalette
{
    Q_OBJECT

public:
    UBStylePalette(QWidget* parent);
    virtual ~UBStylePalette();

    UBShapeStyle selectedStyle();

public slots:
    void updateChoice(const UBShapeStyle& style);

signals:
    void styleChanged(const UBShapeStyle& style);

protected:
    virtual int border() override;
    virtual void onCloseButtonClicked() override;

private:
    void init();
    void setLineStyleIconAndConnect(UBToolbarButtonGroup* buttonGroup, int index, Qt::PenStyle style);
    void setTransparentIcon(UBToolbarButtonGroup* buttonGroup, int index);
    QPixmap createPreview(const UBShapeStyle& style) const;

private slots:
    void updateColorPalette();
    void updateButtonColors();
    void updatePreview();
    void colorContextChanged();
    void applyStyle(const UBShapeStyle& style);
    void saveStyle();
    void recallStyle();

private:
    QFormLayout* mFormLayout{nullptr};
    QAction* mToggleStylePalette{nullptr};

    QList<QAction*> mLineColorActions{};
    QList<QAction*> mLineWidthActions{};
    QList<QAction*> mLineStyleActions{};
    QList<QAction*> mSolidFillColorActions{};
    QList<QAction*> mTransparentFillColorActions{};

    UBToolbarButtonGroup* mLineColorChoice{nullptr};
    UBToolbarButtonGroup* mLineWidthChoice{nullptr};
    UBToolbarButtonGroup* mLineStyleChoice{nullptr};
    UBToolbarButtonGroup* mSolidFillColorChoice{nullptr};
    UBToolbarButtonGroup* mTransparentFillColorChoice{nullptr};

    QLabel* mCurrentPreviewLabel{nullptr};
    QLabel* mSavedPreviewLabel{nullptr};

    QPushButton* mSave{nullptr};
    QPushButton* mRecall{nullptr};

    UBShapeStyle mStyle;
    UBShapeStyle mSavedStyle;
};
