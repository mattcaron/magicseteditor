//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2008 Twan van Laarhoven and "coppro"              |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

#ifndef HEADER_DATA_ACTION_SET
#define HEADER_DATA_ACTION_SET

/** @file data/action/set.hpp
 *
 *  Actions operating on Sets
 */

// ----------------------------------------------------------------------------- : Includes

#include <util/prec.hpp>
#include <util/action_stack.hpp>
#include <data/action/generic.hpp>

class Set;
DECLARE_POINTER_TYPE(Card);
DECLARE_POINTER_TYPE(StyleSheet);
DECLARE_POINTER_TYPE(Field);
DECLARE_POINTER_TYPE(Value);
DECLARE_TYPEOF_COLLECTION(GenericAddAction<CardP>::Step);

// ----------------------------------------------------------------------------- : Add card

/// An Action the changes the card list of a set
class CardListAction : public Action {
  public:
	inline CardListAction(Set& set) : set(set) {}
	
  protected:
	Set& set; // the set owns this action, so the set will not be destroyed before this
	          // therefore we don't need a smart pointer
};

/// Adding a new card to a set
class AddCardAction : public CardListAction {
  public:
	/// Add a newly allocated card
	AddCardAction(Set& set);
	AddCardAction(AddingOrRemoving, Set& set, const CardP& card);
	AddCardAction(AddingOrRemoving, Set& set, const vector<CardP>& cards);
	
	virtual String getName(bool to_undo) const;
	virtual void   perform(bool to_undo);
	
	const GenericAddAction<CardP> action;
};

// ----------------------------------------------------------------------------- : Reorder cards

/// Change the position of a card in the card list by swapping two cards
class ReorderCardsAction : public CardListAction {
  public:
	ReorderCardsAction(Set& set, size_t card_id1, size_t card_id2);
	
	virtual String getName(bool to_undo) const;
	virtual void   perform(bool to_undo);
	
  //private:
	const size_t card_id1, card_id2;	///< Positions of the two cards to swap
};

// ----------------------------------------------------------------------------- : Change stylesheet

/// An action that affects the rendering/display/look of a set or cards in the set
class DisplayChangeAction : public Action {
  public:
	virtual String getName(bool to_undo) const;
	virtual void   perform(bool to_undo);
};

/// Changing the style of a a card
class ChangeCardStyleAction : public DisplayChangeAction {
  public:
	ChangeCardStyleAction(const CardP& card, const StyleSheetP& stylesheet);
	
	virtual String getName(bool to_undo) const;
	virtual void   perform(bool to_undo);
	
  //private:
	CardP                   card;         ///< The affected card
	StyleSheetP             stylesheet;   ///< Its old stylesheet
	bool                    has_styling;  ///< Its old has_styling
	IndexMap<FieldP,ValueP> styling_data; ///< Its old styling data
};

/// Changing the style of a set to that of a card
class ChangeSetStyleAction : public DisplayChangeAction {
  public:
	ChangeSetStyleAction(Set& set, const CardP& card);
	
	virtual String getName(bool to_undo) const;
	virtual void   perform(bool to_undo);
	
  private:
	Set&        set;			///< The affected set
	CardP       card;			///< The card whos stylesheet is copied to the set
	StyleSheetP stylesheet;		///< The old stylesheet of the set
};

/// Changing the styling of a card to become custom/non-custom
/** i.e. toggle card->has_styling */
class ChangeCardHasStylingAction : public DisplayChangeAction {
  public:
	ChangeCardHasStylingAction(Set& set, const CardP& card);
	
	virtual String getName(bool to_undo) const;
	virtual void   perform(bool to_undo);
	
  //private:
	Set&                    set;          ///< The set to copy styling from
	CardP                   card;         ///< The affected card
	IndexMap<FieldP,ValueP> styling_data; ///< The old styling of the card
};

// ----------------------------------------------------------------------------- : EOF
#endif
