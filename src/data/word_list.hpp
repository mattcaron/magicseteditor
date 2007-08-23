//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2007 Twan van Laarhoven                           |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

#ifndef HEADER_DATA_WORD_LIST
#define HEADER_DATA_WORD_LIST

// ----------------------------------------------------------------------------- : Includes

#include <util/prec.hpp>
#include <util/reflect.hpp>

DECLARE_POINTER_TYPE(WordListWord);
DECLARE_POINTER_TYPE(WordList);

// ----------------------------------------------------------------------------- : WordList

/// A word in a WordList
class WordListWord : public IntrusivePtrBase<WordListWord> {
  public:
	WordListWord();
	
	String  name;         ///< Name of the list / the word
	bool    line_below;   ///< Line below in the list?
	bool    is_prefix;    ///< Is this a prefix before other words?
	vector<WordListWordP> words; ///< Sublist
	
	inline bool isGroup() const { return !words.empty(); }
	
	DECLARE_REFLECTION();
};

/// A list of words for a drop down box
class WordList : public WordListWord {
  public:
	DECLARE_REFLECTION();
};


// ----------------------------------------------------------------------------- : EOF
#endif
