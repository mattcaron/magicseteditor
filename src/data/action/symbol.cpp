//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2006 Twan van Laarhoven                           |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

// ----------------------------------------------------------------------------- : Includes

#include <data/action/symbol.hpp>
#include <data/action/symbol_part.hpp>

typedef pair<SymbolPartP,SymbolPartCombine> pair_part_combine_t;
typedef pair<SymbolPartP,size_t           > pair_part_size_t;
DECLARE_TYPEOF_COLLECTION(pair_part_combine_t);
DECLARE_TYPEOF_COLLECTION(pair_part_size_t);

// ----------------------------------------------------------------------------- : Moving symbol parts

SymbolPartMoveAction::SymbolPartMoveAction(const set<SymbolPartP>& parts)
	: parts(parts)
	, constrain(false)
{}

String SymbolPartMoveAction::getName(bool toUndo) const {
	return parts.size() == 1 ? _("Move shape") : _("Move shapes");
}

void SymbolPartMoveAction::perform(bool toUndo) {
	// move the points back
	FOR_EACH(p, parts) {
		p->minPos -= moved;
		p->maxPos -= moved;
		FOR_EACH(pnt, p->points) {
			pnt->pos -= moved;
		}
	}
	moved = -moved;
}

void SymbolPartMoveAction::move(const Vector2D& deltaDelta) {
	delta += deltaDelta;
	// Move each point by deltaDelta, possibly constrained
	Vector2D d = constrainVector(delta, constrain);
	Vector2D dd = d - moved;
	FOR_EACH(p, parts) {
		p->minPos += dd;
		p->maxPos += dd;
		FOR_EACH(pnt, p->points) {
			pnt->pos += dd;
		}
	}
	moved = d;
}

// ----------------------------------------------------------------------------- : Rotating symbol parts

SymbolPartMatrixAction::SymbolPartMatrixAction(const set<SymbolPartP>& parts, const Vector2D& center)
	: parts(parts)
	, center(center)
{}

void SymbolPartMatrixAction::transform(const Vector2D& mx, const Vector2D& my) {
	// Transform each point
	FOR_EACH(p, parts) {
		FOR_EACH(pnt, p->points) {
			pnt->pos         = (pnt->pos - center).mul(mx,my) + center;
			pnt->deltaBefore = pnt->deltaBefore.mul(mx,my);
			pnt->deltaAfter  = pnt->deltaAfter .mul(mx,my);
		}
		// bounds change after transforming
		p->calculateBounds();
	}
}


SymbolPartRotateAction::SymbolPartRotateAction(const set<SymbolPartP>& parts, const Vector2D& center)
	: SymbolPartMatrixAction(parts, center)
	, constrain(false)
	, angle(0)
{}

String SymbolPartRotateAction::getName(bool toUndo) const {
	return parts.size() == 1 ? _("Rotate shape") : _("Rotate shapes");
}

void SymbolPartRotateAction::perform(bool toUndo) {
	// move the points back
	rotateBy(-angle);
	angle = -angle;
}

void SymbolPartRotateAction::rotateTo(double newAngle) {
	double oldAngle = angle;
	angle = newAngle;
	// constrain?
	if (constrain) {
		// multiples of 2pi/24 i.e. 24 stops
		double mult = (2 * M_PI) / 24;
		angle = floor(angle / mult + 0.5) * mult;
	}
	if (oldAngle != angle) rotateBy(angle - oldAngle);
}

void SymbolPartRotateAction::rotateBy(double deltaAngle) {
	// Rotation 'matrix'
	transform(
		Vector2D(cos(deltaAngle), -sin(deltaAngle)),
		Vector2D(sin(deltaAngle),  cos(deltaAngle))
	);
}


// ----------------------------------------------------------------------------- : Shearing symbol parts

SymbolPartShearAction::SymbolPartShearAction(const set<SymbolPartP>& parts, const Vector2D& center)
	: SymbolPartMatrixAction(parts, center)
	, constrain(false)
{}

String SymbolPartShearAction::getName(bool toUndo) const {
	return parts.size() == 1 ? _("Shear shape") : _("Shear shapes");
}

void SymbolPartShearAction::perform(bool toUndo) {
	// move the points back
	// the vector shear = (x,y) is used as:
	//  <1 x>
	//  <y 1>
	// inverse is:
	//  <1  -x>  /
	//  <-y  1> / (1 - xy)
	// we have: xy = 0 => (1 - xy) = 1
	shearBy(-shear);
}

void SymbolPartShearAction::move(const Vector2D& deltaShear) {
	shear += deltaShear;
	shearBy(deltaShear);
}

void SymbolPartShearAction::shearBy(const Vector2D& shear) {
	// Shear 'matrix'
	transform(
		Vector2D(1,       shear.x),
		Vector2D(shear.y, 1)
	);
}


// ----------------------------------------------------------------------------- : Scaling symbol parts


SymbolPartScaleAction::SymbolPartScaleAction(const set<SymbolPartP>& parts, int scaleX, int scaleY)
	: parts(parts)
	, constrain(false)
	, scaleX(scaleX), scaleY(scaleY)
{
	// Find min and max coordinates
	oldMin          =  Vector2D::infinity();
	Vector2D oldMax = -Vector2D::infinity();
	FOR_EACH(p, parts) {
		oldMin = piecewise_min(oldMin, p->minPos);
		oldMax = piecewise_max(oldMax, p->maxPos);
	}
	// new == old
	newMin  = newRealMin  = oldMin;
	newSize = newRealSize = oldSize = oldMax - oldMin;
}

String SymbolPartScaleAction::getName(bool toUndo) const {
	return parts.size() == 1 ? _("Scale shape") : _("Scale shapes");
}

void SymbolPartScaleAction::perform(bool toUndo) {
	swap(oldMin,  newMin);
	swap(oldSize, newSize);
	transformAll();
}

void SymbolPartScaleAction::move(const Vector2D& deltaMin, const Vector2D& deltaMax) {
	newRealMin  += deltaMin;
	newRealSize += deltaMax - deltaMin;
	update();
}

void SymbolPartScaleAction::update() {
	// Move each point so the range <oldMin...oldMax> maps to <newMin...newMax>
	// we have already moved to the current <newMin...newMax>
	Vector2D tmpMin = oldMin, tmpSize = oldSize; // the size before any scaling
	         oldMin = newMin; oldSize = newSize; // the size before this move
	// the size after the move
	newMin = newRealMin; newSize = newRealSize;
	if (constrain && scaleX != 0 && scaleY != 0) {
		Vector2D scale = newSize.div(tmpSize);
		scale = constrainVector(scale, true, true);
		newSize = tmpSize.mul(scale);
		newMin += (newRealSize - newSize).mul(Vector2D(scaleX == -1 ? 1 : 0, scaleY == -1 ? 1 : 0));
	}
	// now move all points
	transformAll();
	// restore oldMin/Size
	oldMin = tmpMin;  oldSize = tmpSize;
}

void SymbolPartScaleAction::transformAll() {
	Vector2D scale = newSize.div(oldSize);
	FOR_EACH(p, parts) {
		p->minPos = transform(p->minPos);
		p->maxPos = transform(p->maxPos);
		// make sure that max >= min
		if (p->minPos.x > p->maxPos.x) swap(p->minPos.x, p->maxPos.x);
		if (p->minPos.y > p->maxPos.y) swap(p->minPos.y, p->maxPos.y);
		// scale all points
		FOR_EACH(pnt, p->points) {
			pnt->pos = transform(pnt->pos);
			// also scale handles
			pnt->deltaBefore = pnt->deltaBefore.mul(scale);
			pnt->deltaAfter  = pnt->deltaAfter .mul(scale);
		}
	}
}

// ----------------------------------------------------------------------------- : Change combine mode

CombiningModeAction::CombiningModeAction(const set<SymbolPartP>& parts, SymbolPartCombine mode) {
	FOR_EACH(p, parts) {
		this->parts.push_back(make_pair(p,mode));
	}
}

String CombiningModeAction::getName(bool toUndo) const {
	return _("Change combine mode");
}

void CombiningModeAction::perform(bool toUndo) {
	FOR_EACH(pm, parts) {
		swap(pm.first->combine, pm.second);
	}
}

// ----------------------------------------------------------------------------- : Change name

SymbolPartNameAction::SymbolPartNameAction(const SymbolPartP& part, const String& name)
	: part(part), partName(name)
{}

String SymbolPartNameAction::getName(bool toUndo) const {
	return _("Change shape name");
}

void SymbolPartNameAction::perform(bool toUndo) {
	swap(part->name, partName);
}

// ----------------------------------------------------------------------------- : Add symbol part

AddSymbolPartAction::AddSymbolPartAction(Symbol& symbol, const SymbolPartP& part)
	: symbol(symbol), part(part)
{}

String AddSymbolPartAction::getName(bool toUndo) const {
	return _("Add ") + part->name;
}

void AddSymbolPartAction::perform(bool toUndo) {
	if (toUndo) {
		assert(!symbol.parts.empty());
		symbol.parts.erase (symbol.parts.begin());
	} else {
		symbol.parts.insert(symbol.parts.begin(), part);
	}
}

// ----------------------------------------------------------------------------- : Remove symbol part

RemoveSymbolPartsAction::RemoveSymbolPartsAction(Symbol& symbol, const set<SymbolPartP>& parts)
	: symbol(symbol)
{
	size_t index = 0;
	FOR_EACH(p, symbol.parts) {
		if (parts.find(p) != parts.end()) {
			removals.push_back(make_pair(p, index)); // remove this part
		}
		++index;
	}
}

String RemoveSymbolPartsAction::getName(bool toUndo) const {
	return removals.size() == 1 ? _("Remove shape") : _("Remove shapes");
}

void RemoveSymbolPartsAction::perform(bool toUndo) {
	if (toUndo) {
		// reinsert the parts
		// ascending order, this is the reverse of removal
		FOR_EACH(r, removals) {
			assert(r.second <= symbol.parts.size());
			symbol.parts.insert(symbol.parts.begin() + r.second, r.first);
		}
	} else {
		// remove the parts
		// descending order, because earlier removals shift the rest of the vector
		FOR_EACH_REVERSE(r, removals) {
			assert(r.second < symbol.parts.size());
			symbol.parts.erase(symbol.parts.begin() + r.second);
		}
	}
}

// ----------------------------------------------------------------------------- : Duplicate symbol parts

DuplicateSymbolPartsAction::DuplicateSymbolPartsAction(Symbol& symbol, const set<SymbolPartP>& parts)
	: symbol(symbol)
{
	UInt index = 0;
	FOR_EACH(p, symbol.parts) {
		index += 1;
		if (parts.find(p) != parts.end()) {
			// duplicate this part
			duplications.push_back(make_pair(p->clone(), index));
			index += 1; // the clone also takes up space on the vector
		}
	}
}

String DuplicateSymbolPartsAction::getName(bool toUndo) const {
	return duplications.size() == 1 ? _("Duplicate shape") : _("Duplicate shapes");
}

void DuplicateSymbolPartsAction::perform(bool toUndo) {
	if (toUndo) {
		// remove the clones
		// walk in reverse order, otherwise we will shift the vector
		FOR_EACH_REVERSE(d, duplications) {
			assert(d.second < symbol.parts.size());
			symbol.parts.erase(symbol.parts.begin() + d.second);
		}
	} else {
		// insert the clones
		FOR_EACH(d, duplications) {
			assert(d.second <= symbol.parts.size());
			symbol.parts.insert(symbol.parts.begin() + d.second, d.first);
		}
	}
}

void DuplicateSymbolPartsAction::getParts(set<SymbolPartP>& parts) {
	parts.clear();
	FOR_EACH(d, duplications) {
		parts.insert(d.first);
	}
}

// ----------------------------------------------------------------------------- : Reorder symbol parts

ReorderSymbolPartsAction::ReorderSymbolPartsAction(Symbol& symbol, size_t partId1, size_t partId2)
	: symbol(symbol), partId1(partId1), partId2(partId2)
{}

String ReorderSymbolPartsAction::getName(bool toUndo) const {
	return _("Reorder");
}

void ReorderSymbolPartsAction::perform(bool toUndo) {
	assert(partId1 < symbol.parts.size());
	assert(partId2 < symbol.parts.size());
	swap(symbol.parts[partId1], symbol.parts[partId2]);
}