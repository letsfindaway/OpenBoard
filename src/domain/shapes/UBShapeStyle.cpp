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


#include "UBShapeStyle.h"


UBShapeStyle::UBShapeStyle(const QColor& lineColorOnLight, const QColor& lineColorOnDark, qreal lineWidth, Qt::PenStyle lineStyle,
                 const QColor& fillColorOnLight, const QColor& fillColorOnDark)
    : mLineColor{lineColorOnLight, lineColorOnDark}
    , mLineWidth{lineWidth}
    , mLineStyle{lineStyle}
    , mFillColor{fillColorOnLight, fillColorOnDark}
{
}

QColor UBShapeStyle::lineColor(bool isDark) const
{
    return mLineColor.color(isDark);
}

void UBShapeStyle::setLineColor(const QColor& lineColorOnLight, const QColor& lineColorOnDark)
{
    mLineColor = {lineColorOnLight, lineColorOnDark};
}

double UBShapeStyle::lineWidth() const
{
    return mLineWidth;
}

void UBShapeStyle::setLineWidth(double lineWidth)
{
    mLineWidth = lineWidth;
}

Qt::PenStyle UBShapeStyle::lineStyle() const
{
    return mLineStyle;
}

void UBShapeStyle::setLineStyle(Qt::PenStyle lineStyle)
{
    mLineStyle = lineStyle;
}

QColor UBShapeStyle::fillColor(bool isDark) const
{
    return mFillColor.color(isDark);
}

void UBShapeStyle::setFillColor(const QColor& fillColorOnLight, const QColor& fillColorOnDark)
{
    mFillColor = {fillColorOnLight, fillColorOnDark};
}

UBShapeStyle UBShapeStyle::intersected(const UBShapeStyle& other)
{
    const auto lineColorOnLight = lineColor(false) == other.lineColor(false) ? lineColor(false) : QColor{};
    const auto lineColorOnDark = lineColor(true) == other.lineColor(true) ? lineColor(true) : QColor{};
    const auto lineWidth = qFuzzyCompare(this->lineWidth(), other.lineWidth()) ? this->lineWidth() : 0;
    const auto lineStyle = this->lineStyle() == other.lineStyle() ? this->lineStyle() :  Qt::NoPen;
    const auto fillColorOnLight = fillColor(false) == other.fillColor(false) ? fillColor(false) : QColor{};
    const auto fillColorOnDark = fillColor(true) == other.fillColor(true) ? fillColor(true) : QColor{};

    return UBShapeStyle{lineColorOnLight, lineColorOnDark, lineWidth, lineStyle, fillColorOnLight, fillColorOnDark};
}

UBShapeStyle::ColorSet::ColorSet(const QColor& colorOnLight, const QColor& colorOnDark)
    : mColorOnLight{colorOnLight}
    , mColorOnDark{colorOnDark}
{
}

QColor UBShapeStyle::ColorSet::color(bool isDark) const
{
    return isDark ? mColorOnDark : mColorOnLight;
}

void UBShapeStyle::ColorSet::setColor(const QColor& colorOnLight, const QColor& colorOnDark)
{
    mColorOnLight = colorOnLight;
    mColorOnDark = colorOnDark;
}

bool UBShapeStyle::ColorSet::operator==(const UBShapeStyle::ColorSet& other) const
{
    return mColorOnLight == other.mColorOnLight && mColorOnDark == other.mColorOnDark;
}

bool operator==(const UBShapeStyle& lhs, const UBShapeStyle& rhs)
{
    return lhs.mLineColor == rhs.mLineColor && lhs.mLineWidth == rhs.mLineWidth && lhs.mLineStyle == rhs.mLineStyle &&
           lhs.mFillColor == rhs.mFillColor;
}
