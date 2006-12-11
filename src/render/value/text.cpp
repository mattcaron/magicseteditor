//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2006 Twan van Laarhoven                           |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

// ----------------------------------------------------------------------------- : Includes

#include <render/value/text.hpp>
#include <render/card/viewer.hpp>

// ----------------------------------------------------------------------------- : TextValueViewer

void TextValueViewer::draw(RotatedDC& dc) {
	drawFieldBorder(dc);
	v.prepare(dc, value().value(), style(), viewer.getContext());
	v.draw(dc, style(), DRAW_NORMAL);
}

void TextValueViewer::onValueChange() {
	v.reset();
}

void TextValueViewer::onStyleChange() {
	v.reset();
}
