//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2007 Twan van Laarhoven                           |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

#ifndef HEADER_UTIL_UPDATE_CHECKER
#define HEADER_UTIL_UPDATE_CHECKER

// ----------------------------------------------------------------------------- : Includes

#include <util/prec.hpp>
#include <wx/html/htmlwin.h>

// ----------------------------------------------------------------------------- : Update checking

// Checks for updates if the settings say so
void check_updates();

/// Checks if the current version is the latest version
/** If async==true then checking is done in another thread
 */
void check_updates_now(bool async = true);

/// Show a dialog to inform the user that updates are availible (if there are any)
/** Call check_updates first.
 *  Call this function from an onIdle loop */
void show_update_dialog(Window* parent);

class PackageUpdateList;

DECLARE_POINTER_TYPE(PackageVersionData);

/// A window that displays the updates and allows the user to select some.
class UpdateWindow : public Frame {
  public:
	UpdateWindow();
	void DrawTitles(wxPaintEvent&);

	enum PackageStatus {
		STATUS_INSTALLED,
		STATUS_NOT_INSTALLED,
		STATUS_UPGRADEABLE
	};
	enum PackageAction {
		ACTION_INSTALL,
		ACTION_UNINSTALL,
		ACTION_UPGRADE,
		ACTION_NOTHING,
		ACTION_NEW_MSE
	};

	typedef pair<PackageStatus, PackageAction> PackageData;

	map<PackageVersionDataP, PackageData> package_data;

	void SetDefaultPackageStatus(wxCommandEvent&);
  private:
	DECLARE_EVENT_TABLE();
	PackageUpdateList* package_list;
	wxHtmlWindow* description_window;

	wxStaticText *package_title, *status_title, *new_title;
};

/// Was update data found?
bool update_data_found();
/// Is there an update?
bool update_available();

// ----------------------------------------------------------------------------- : EOF
#endif
