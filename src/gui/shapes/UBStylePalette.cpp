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


#include "UBStylePalette.h"

#include "board/UBBoardController.h"
#include "board/UBDrawingController.h"
#include "core/UBApplication.h"
#include "core/UBSettings.h"
#include "domain/UBGraphicsScene.h"
#include "gui/UBBackgroundManager.h"
#include "gui/UBMainWindow.h"
#include "gui/UBToolbarButtonGroup.h"


UBStylePalette::UBStylePalette(QWidget* parent)
    : UBFloatingPalette(Qt::TopLeftCorner, parent)
{
    mToggleStylePalette = UBApplication::boardController->shapeFactory().shapeActions()->actionToggleStylePalette;

    init();

    connect(UBApplication::boardController, &UBBoardController::backgroundChanged, this,
            &UBStylePalette::updateColorPalette);
    connect(UBApplication::boardController, &UBBoardController::activeSceneChanged, this,
            &UBStylePalette::updateColorPalette);
    connect(UBDrawingController::drawingController(), &UBDrawingController::stylusToolChanged, this, [this](int tool){
        if (tool == UBStylusTool::Pen || tool == UBStylusTool::Marker)
        {
            mToggleStylePalette->setChecked(false);
        }
    });

    connect(UBSettings::settings(), &UBSettings::colorContextChanged, this, &UBStylePalette::colorContextChanged);

    connect(this, &UBStylePalette::styleChanged, this, &UBStylePalette::updatePreview);
    connect(this, &UBStylePalette::styleChanged, &UBApplication::boardController->shapeFactory(),
            &UBShapeFactory::applyStyle);

    connect(mToggleStylePalette, &QAction::toggled, this, &QWidget::setVisible);

    auto settings = UBSettings::settings();

    const auto lineColorOnLight = QColor::fromString(settings->value("Board/StyleLineColorOnLight", "black").toString());
    const auto lineColorOnDark = QColor::fromString(settings->value("Board/StyleLineColorOnDark", "white").toString());
    const auto lineWidth = settings->value("Board/StyleLineWidth", 3.).toDouble();
    const auto lineStyle = settings->value("Board/StyleLineStyle", static_cast<int>(Qt::SolidLine)).value<Qt::PenStyle>();
    const auto fillColorOnLight = QColor::fromString(settings->value("Board/StyleFillColorOnLight", "transparent").toString());
    const auto fillColorOnDark = QColor::fromString(settings->value("Board/StyleFillColorOnDark", "transparent").toString());

    updateChoice(UBShapeStyle{lineColorOnLight, lineColorOnDark, lineWidth, lineStyle, fillColorOnLight, fillColorOnDark});
}

UBStylePalette::~UBStylePalette()
{
    auto settings = UBSettings::settings();

    settings->setValue("Board/StyleLineColorOnLight", mStyle.lineColor(false).name(QColor::HexArgb));
    settings->setValue("Board/StyleLineColorOnDark", mStyle.lineColor(true).name(QColor::HexArgb));
    settings->setValue("Board/StyleLineWidth", mStyle.lineWidth());
    settings->setValue("Board/StyleLineStyle", static_cast<int>(mStyle.lineStyle()));
    settings->setValue("Board/StyleFillColorOnLight", mStyle.fillColor(false).name(QColor::HexArgb));
    settings->setValue("Board/StyleFillColorOnDark", mStyle.fillColor(true).name(QColor::HexArgb));

    settings->save();
}

UBShapeStyle UBStylePalette::selectedStyle()
{
    return mStyle;
}

void UBStylePalette::updateChoice(const UBShapeStyle& style)
{
    if (style == mStyle)
    {
        return;
    }

    mStyle = style;

    // search for matching line color
    for (const auto action : mLineColorActions)
    {
        if (action->isVisible() && action->property("color").isValid())
        {
            const auto color = action->property("color").value<QVariantList>();
            const auto match = QColor::fromString(color.at(0).toString()) == style.lineColor(false) &&
                               QColor::fromString(color.at(1).toString()) == style.lineColor(true);
            action->setChecked(match);
        }
    }

    // search for matching line width
    const auto width = style.lineWidth();
    const auto settings = UBSettings::settings();

    mLineWidthActions.at(0)->setChecked(qFuzzyCompare(width, settings->boardPenFineWidth->get().toDouble()));
    mLineWidthActions.at(1)->setChecked(qFuzzyCompare(width, settings->boardPenMediumWidth->get().toDouble()));
    mLineWidthActions.at(2)->setChecked(qFuzzyCompare(width, settings->boardPenStrongWidth->get().toDouble()));

    // search for matching line style
    mLineStyleActions.at(0)->setChecked(style.lineStyle() == Qt::SolidLine);
    mLineStyleActions.at(1)->setChecked(style.lineStyle() == Qt::DashLine);
    mLineStyleActions.at(2)->setChecked(style.lineStyle() == Qt::DotLine);

    // search for matching fill color
    for (const auto action : mSolidFillColorActions + mTransparentFillColorActions)
    {
        if (action->isVisible() && action->property("color").isValid())
        {
            const auto color = action->property("color").value<QVariantList>();
            const auto match = QColor::fromString(color.at(0).toString()) == style.fillColor(false) &&
                               QColor::fromString(color.at(1).toString()) == style.fillColor(true);
            action->setChecked(match);
        }
    }

    updatePreview();
}

int UBStylePalette::border()
{
    return 5;
}

void UBStylePalette::init()
{
    mFormLayout = new QFormLayout{this};
    mFormLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    mFormLayout->setVerticalSpacing(0);

    // Setup title line
    QLabel* titleLabel = new QLabel{tr("<b>Shape Style</b>")};
    titleLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    titleLabel->setIndent(10);
    titleLabel->setTextInteractionFlags(Qt::NoTextInteraction);
    mFormLayout->addRow(titleLabel);

    // Setup line color choice widget
    for (int i = 0; i < UBSettings::maxColorPaletteSize; ++i)
    {
        auto action = new QAction{this};
        action->setCheckable(true);
        mLineColorActions << action;

        connect(action, &QAction::triggered, this,
                [this]()
                {
                    const auto color = sender()->property("color").value<QVariantList>();
                    qDebug() << "Line color is now" << color;
                    auto newStyle = mStyle;
                    newStyle.setLineColor(QColor::fromString(color.at(0).toString()),
                                          QColor::fromString(color.at(1).toString()));
                    applyStyle(newStyle);
                });
    }

    mLineColorActions << UBApplication::mainWindow->actionColorPreferences;

    mLineColorChoice = new UBToolbarButtonGroup(UBApplication::mainWindow->boardToolBar, mLineColorActions, "",
                                                UBSettings::settings()->colorPaletteSize);
    mLineColorChoice->displayText(false);
    mLineColorChoice->layout()->setContentsMargins({});

    QLabel* lineColorLabel = new QLabel{tr("Line color")};
    mFormLayout->addRow(lineColorLabel, mLineColorChoice);

    // Setup line width choice widget
    mLineWidthActions.append(new QAction);
    mLineWidthActions.back()->setCheckable(true);
    mLineWidthActions.back()->setIcon(UBApplication::mainWindow->actionLineSmall->icon());
    connect(mLineWidthActions.back(), &QAction::triggered, this,
            [this]()
            {
                const auto width = UBSettings::settings()->boardPenFineWidth->get().toDouble();
                qDebug() << "Line width is now" << width;
                auto newStyle = mStyle;
                newStyle.setLineWidth(width);
                applyStyle(newStyle);
            });

    mLineWidthActions.append(new QAction);
    mLineWidthActions.back()->setCheckable(true);
    mLineWidthActions.back()->setIcon(UBApplication::mainWindow->actionLineMedium->icon());
    connect(mLineWidthActions.back(), &QAction::triggered, this,
            [this]()
            {
                const auto width = UBSettings::settings()->boardPenMediumWidth->get().toDouble();
                qDebug() << "Line width is now" << width;
                auto newStyle = mStyle;
                newStyle.setLineWidth(width);
                applyStyle(newStyle);
            });

    mLineWidthActions.append(new QAction);
    mLineWidthActions.back()->setCheckable(true);
    mLineWidthActions.back()->setIcon(UBApplication::mainWindow->actionLineLarge->icon());
    connect(mLineWidthActions.back(), &QAction::triggered, this,
            [this]()
            {
                const auto width = UBSettings::settings()->boardPenStrongWidth->get().toDouble();
                qDebug() << "Line width is now" << width;
                auto newStyle = mStyle;
                newStyle.setLineWidth(width);
                applyStyle(newStyle);
            });

    mLineWidthChoice = new UBToolbarButtonGroup(UBApplication::mainWindow->boardToolBar, mLineWidthActions, "");
    mLineWidthChoice->displayText(false);
    mLineWidthChoice->layout()->setContentsMargins({});

    QLabel* lineWidthLabel = new QLabel{tr("Line width")};
    mFormLayout->addRow(lineWidthLabel, mLineWidthChoice);

    // Setup line style choice widget
    for (int i = 0; i < 3; ++i)
    {
        auto action = new QAction{this};
        action->setCheckable(true);
        mLineStyleActions << action;
    }

    mLineStyleChoice = new UBToolbarButtonGroup(UBApplication::mainWindow->boardToolBar, mLineStyleActions, "");
    mLineStyleChoice->displayText(false);
    mLineStyleChoice->layout()->setContentsMargins({});

    setLineStyleIconAndConnect(mLineStyleChoice, 0, Qt::SolidLine);
    setLineStyleIconAndConnect(mLineStyleChoice, 1, Qt::DashLine);
    setLineStyleIconAndConnect(mLineStyleChoice, 2, Qt::DotLine);

    QLabel* lineStyleLabel = new QLabel{tr("Line style")};
    mFormLayout->addRow(lineStyleLabel, mLineStyleChoice);

    // Setup fill color choice widget
    for (int i = 0; i < UBSettings::maxColorPaletteSize; ++i)
    {
        auto action = new QAction{this};
        action->setCheckable(true);
        mSolidFillColorActions << action;

        action = new QAction{this};
        action->setCheckable(true);
        mTransparentFillColorActions << action;
    }

    // append transparent background button to transparent fill colors
    auto action = new QAction;
    action->setCheckable(true);
    mTransparentFillColorActions << action;

    mSolidFillColorChoice = new UBToolbarButtonGroup(UBApplication::mainWindow->boardToolBar, mSolidFillColorActions,
                                                     "", UBSettings::settings()->colorPaletteSize);
    mSolidFillColorChoice->displayText(false);
    mSolidFillColorChoice->layout()->setContentsMargins({});

    mTransparentFillColorChoice =
        new UBToolbarButtonGroup(UBApplication::mainWindow->boardToolBar, mTransparentFillColorActions, "",
                                 UBSettings::settings()->colorPaletteSize + 1);
    mTransparentFillColorChoice->displayText(false);
    mTransparentFillColorChoice->layout()->setContentsMargins({});

    // use a common action group for solid and transparent fill colors
    auto fillColorActionGroup = mSolidFillColorActions.at(0)->actionGroup();

    for (const auto action : mSolidFillColorActions + mTransparentFillColorActions)
    {
        action->setActionGroup(fillColorActionGroup);
        connect(action, &QAction::triggered, this,
                [this]()
                {
                    auto color = sender()->property("color").value<QVariantList>();
                    qDebug() << "Fill color is now" << color;
                    auto newStyle = mStyle;
                    newStyle.setFillColor(QColor::fromString(color.at(0).toString()),
                                          QColor::fromString(color.at(1).toString()));
                    applyStyle(newStyle);
                });
    }

    QLabel* fillColorLabel = new QLabel{tr("Fill color")};
    mFormLayout->addRow(fillColorLabel, mSolidFillColorChoice);
    mFormLayout->addRow({}, mTransparentFillColorChoice);

    updateButtonColors();

    // setup preview
    QWidget* preview = new QWidget{this};
    QHBoxLayout* hbox = new QHBoxLayout{preview};

    mCurrentPreviewLabel = new QLabel;
    hbox->addWidget(mCurrentPreviewLabel);

    mSave = new QPushButton{">"};
    mRecall = new QPushButton{"<"};

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(mSave);
    vbox->addWidget(mRecall);

    hbox->addLayout(vbox);

    mSavedPreviewLabel = new QLabel;
    hbox->addWidget(mSavedPreviewLabel);

    connect(mSave, &QPushButton::clicked, this, &UBStylePalette::saveStyle);
    connect(mRecall, &QPushButton::clicked, this, &UBStylePalette::recallStyle);

    QLabel* previewLabel = new QLabel{tr("Preview")};
    mFormLayout->addRow(previewLabel, preview);

    preview->resize(hbox->sizeHint());

    resize(mFormLayout->sizeHint());
}

void UBStylePalette::setLineStyleIconAndConnect(UBToolbarButtonGroup* buttonGroup, int index, Qt::PenStyle style)
{
    QPixmap pixmap{54, 8};
    pixmap.fill(Qt::transparent);

    QPainter painter{&pixmap};
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QPen pen{Qt::black, 3, style, Qt::RoundCap};
    painter.setPen(pen);
    painter.drawLine(4, 4, 50, 4);

    QIcon icon{pixmap};

    const auto buttonList = buttonGroup->findChildren<QToolButton*>(Qt::FindDirectChildrenOnly);

    if (buttonList.size() > index)
    {
        buttonList.at(index)->setStyleSheet("width: 64");
        buttonList.at(index)->defaultAction()->setIcon(icon);
        buttonList.at(index)->setIconSize(pixmap.size());
    }

    connect(buttonList.at(index)->defaultAction(), &QAction::triggered, this,
            [this, style]()
            {
                qDebug() << "Line style is now" << style;
                auto newStyle = mStyle;
                newStyle.setLineStyle(style);
                applyStyle(newStyle);
            });
}

void UBStylePalette::setTransparentIcon(UBToolbarButtonGroup* buttonGroup, int index)
{
    const QSize iconSize{24, 24};
    const QSize patternSize{8, 8};
    const uint patternColor{0xff808080};

    QImage pattern{patternSize, QImage::Format_ARGB32};
    pattern.fill(Qt::white);

    const auto blockSize = patternSize.width() / 2;

    for (int x = 0; x < blockSize; ++x)
    {
        for (int y = 0; y < blockSize; ++y)
        {
            pattern.setPixel(x, y, patternColor);
            pattern.setPixel(x + blockSize, y + blockSize, patternColor);
        }
    }

    QBrush brush;
    brush.setTextureImage(pattern);

    QPixmap pixmap{iconSize};
    pixmap.fill(Qt::white);

    QPainter painter{&pixmap};
    painter.fillRect(pixmap.rect(), brush);

    QIcon icon{pixmap};

    const auto buttonList = buttonGroup->findChildren<QToolButton*>(Qt::FindDirectChildrenOnly);

    if (buttonList.size() > index)
    {
        buttonList.at(index)->defaultAction()->setIcon(icon);
        buttonList.at(index)->setIconSize(pixmap.size());
    }
}

QPixmap UBStylePalette::createPreview(const UBShapeStyle& style) const
{
    // get the current background
    const auto scene = UBApplication::boardController->activeScene();
    bool isDark{false};
    const UBBackgroundRuling* background{nullptr};

    if (scene)
    {
        isDark = scene->isDarkBackground();
        background = scene->background();
    }
    else
    {
        isDark = UBSettings::settings()->isDarkBackground();
        const auto backgroundUuid = UBSettings::settings()->pageBackgroundUuid();
        const auto bgManager = UBApplication::boardController->backgroundManager();
        background = bgManager->background(backgroundUuid);
    }

    QPixmap pixmap{70, 70};
    pixmap.fill(isDark ? Qt::black : Qt::white);

    QPainter painter{&pixmap};
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    if (background)
    {
        background->draw(&painter, pixmap.rect(), 20., pixmap.rect(), isDark);
    }

    const QPen borderPen{Qt::darkGray, 1};
    painter.setPen(borderPen);
    painter.drawRect(QRect{{0, 0}, pixmap.size()});

    const Qt::PenStyle penStyle =
        style.lineColor(isDark).isValid() && style.lineWidth() > 0 ? style.lineStyle() : Qt::NoPen;
    const QPen pen{style.lineColor(isDark), style.lineWidth(), penStyle, Qt::RoundCap};
    painter.setPen(pen);

    if (style.fillColor(isDark).isValid())
    {
        const QBrush brush{style.fillColor(isDark)};
        painter.setBrush(brush);
    }

    painter.drawLine(5, 50, 30, 10);
    painter.drawEllipse(25, 25, 40, 40);

    return pixmap;
}

void UBStylePalette::updateColorPalette()
{
    updateButtonColors();
    updatePreview();
    mSavedPreviewLabel->setPixmap(createPreview(mSavedStyle));
}

void UBStylePalette::updateButtonColors()
{
    const auto settings = UBSettings::settings();

    const auto isDarkBackground = settings->isDarkBackground();
    const auto solidColors = settings->penColors(isDarkBackground);
    const auto solidColorsOnLight = settings->penColors(false);
    const auto solidColorsOnDark = settings->penColors(true);

    for (int i = 0; i < solidColors.size() && i < mSolidFillColorActions.size(); ++i)
    {
        const auto colorProperty =
            QVariantList{solidColorsOnLight.at(i).name(QColor::HexArgb), solidColorsOnDark.at(i).name(QColor::HexArgb)};

        mLineColorChoice->setColor(solidColors.at(i), i);
        mLineColorActions.at(i)->setProperty("color", colorProperty);

        mSolidFillColorChoice->setColor(solidColors.at(i), i);
        mSolidFillColorActions.at(i)->setProperty("color", colorProperty);
    }

    const auto transparentColors = settings->markerColors(isDarkBackground);
    const auto transparentColorsOnLight = settings->markerColors(false);
    const auto transparentColorsOnDark = settings->markerColors(true);

    for (int i = 0; i < transparentColors.size() && i < mTransparentFillColorActions.size(); ++i)
    {
        mTransparentFillColorChoice->setColor(transparentColors.at(i), i);
        mTransparentFillColorActions.at(i)->setProperty(
            "color", QVariantList{transparentColorsOnLight.at(i).name(QColor::HexArgb),
                                  transparentColorsOnDark.at(i).name(QColor::HexArgb)});
    }

    const auto transparentIndex = settings->colorPaletteSize;
    setTransparentIcon(mTransparentFillColorChoice, transparentIndex);
    mTransparentFillColorActions.at(transparentIndex)
        ->setProperty("color", QVariantList{QColor{Qt::transparent}, QColor{Qt::transparent}});
}

void UBStylePalette::updatePreview()
{
    const auto previewPixmap = createPreview(mStyle);
    mToggleStylePalette->setIcon(QIcon{previewPixmap});
    mCurrentPreviewLabel->setPixmap(previewPixmap);
}

void UBStylePalette::colorContextChanged()
{
    const auto selectableColors = UBSettings::settings()->boardColorPaletteSize->get().toInt();
    mLineColorChoice->setSelectableCount(selectableColors);
    mSolidFillColorChoice->setSelectableCount(selectableColors);
    mTransparentFillColorChoice->setSelectableCount(selectableColors + 1); // + 1 for transparent fill
    updateButtonColors();

    // defer resizing so that sizeHint is already updated
    QTimer::singleShot(0, [this]() { resize(mFormLayout->sizeHint()); });
}

void UBStylePalette::applyStyle(const UBShapeStyle& style)
{
    if (style != mStyle)
    {
        mStyle = style;
        emit styleChanged(mStyle);
    }
}

void UBStylePalette::saveStyle()
{
    mSavedStyle = mStyle;
    mSavedPreviewLabel->setPixmap(createPreview(mSavedStyle));
}

void UBStylePalette::recallStyle()
{
    updateChoice(mSavedStyle);

    emit styleChanged(mStyle);
}
