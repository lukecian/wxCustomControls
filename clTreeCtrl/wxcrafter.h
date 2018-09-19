//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxcrafter.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CLTREECTRL_CLTREECTRL_WXCRAFTER_BASE_CLASSES_H
#define _CLTREECTRL_CLTREECTRL_WXCRAFTER_BASE_CLASSES_H

#include "clDataViewListCtrl.h"
#include "clTreeCtrl.h"
#include <map>
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/dataview.h>
#include <wx/frame.h>
#include <wx/icon.h>
#include <wx/iconbndl.h>
#include <wx/imaglist.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/treectrl.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/xrc/xmlres.h>
#if wxVERSION_NUMBER >= 2900
#include <wx/persist.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/toplevel.h>
#include <wx/persist/treebook.h>
#endif

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif

class MainFrameBaseClass : public wxFrame
{
public:
    enum {
        ID_TOGGLE_THEMES = 10001,
        ID_ZEBRA = 10002,
        ID_DV_DELETE_ALL_ITEMS = 10003,
        ID_FILL_WITH_5000_ENTRIES = 10004,
        ID_DV_OPEN_FOLDER = 10005,
        ID_SINGLE_SELECTION = 10006,
        ID_HIDE_ROOT = 10007,
        ID_PREV_SIBLING = 10008,
        ID_NATIVE_HEADER = 10009,
        ID_SHOW_SB_ON_FOCUS = 10010,
        ID_NEXT_SIBLING = 10011,
        ID_SELECT_CHILDREN = 10012,
        ID_EXPAND_ALL = 10013,
        ID_OPEN_FOLDER = 10014,
    };

protected:
    wxPanel* m_mainPanel;
    clTreeCtrl* m_treeCtrl;
    clDataViewListCtrl* m_dataView;
    wxTextCtrl* m_textCtrlLog;
    wxMenuBar* m_myMenuBar;
    wxMenu* menu_clTreeCtrl;
    wxMenuItem* m_menuItem13;
    wxMenuItem* m_separator1;
    wxMenuItem* m_menuItemExpandAll;
    wxMenuItem* m_menuItemCollapseAll;
    wxMenuItem* m_menuItemFirstVisible;
    wxMenuItem* m_menuItemNextVisibleItem;
    wxMenuItem* m_menuItemEnsureVisible;
    wxMenuItem* m_menuItemSelectChildren;
    wxMenuItem* m_menuItemNextSibling;
    wxMenuItem* m_menuItemPrevSibling;
    wxMenuItem* m_menuItemDeleteAllItems;
    wxMenuItem* m_separator2;
    wxMenuItem* m_menuItemExit;
    wxMenu* menu_clDataViewListCtrl;
    wxMenuItem* m_menuItem83;
    wxMenuItem* m_menuItem87;
    wxMenuItem* m_menuItem89;
    wxMenu* menu_Style;
    wxMenuItem* m_menuItemZebra;
    wxMenuItem* m_menuItemThemes;
    wxMenuItem* m_menuItemHideRoot;
    wxMenuItem* m_menuItem67;
    wxMenuItem* m_menuItemSingleSelection;
    wxMenuItem* m_menuItemShowSBOnFocus;
    wxMenuItem* m_menuItemHideHeaders;
    wxMenuItem* m_menuItemNativeHeader;
    wxMenuItem* m_menuItemSupportSearch;

protected:
    virtual void OnOpenFolder(wxCommandEvent& event) { event.Skip(); }
    virtual void OnExpandAll(wxCommandEvent& event) { event.Skip(); }
    virtual void OnCollapseAll(wxCommandEvent& event) { event.Skip(); }
    virtual void OnFirstVisible(wxCommandEvent& event) { event.Skip(); }
    virtual void OnNextVisible(wxCommandEvent& event) { event.Skip(); }
    virtual void OnEnsureItemVisible(wxCommandEvent& event) { event.Skip(); }
    virtual void OnSelectChildren(wxCommandEvent& event) { event.Skip(); }
    virtual void OnNextSibling(wxCommandEvent& event) { event.Skip(); }
    virtual void OnPrevSibling(wxCommandEvent& event) { event.Skip(); }
    virtual void OnDeleteAllItems(wxCommandEvent& event) { event.Skip(); }
    virtual void OnExit(wxCommandEvent& event) { event.Skip(); }
    virtual void OnDVOpenFolder(wxCommandEvent& event) { event.Skip(); }
    virtual void OnFillWith500Entries(wxCommandEvent& event) { event.Skip(); }
    virtual void OnDVDeleteAllItems(wxCommandEvent& event) { event.Skip(); }
    virtual void OnZebraColouring(wxCommandEvent& event) { event.Skip(); }
    virtual void OnToggleTheme(wxCommandEvent& event) { event.Skip(); }
    virtual void OnHideRoot(wxCommandEvent& event) { event.Skip(); }
    virtual void OnSingleSelection(wxCommandEvent& event) { event.Skip(); }
    virtual void OnShowSBOnFocus(wxCommandEvent& event) { event.Skip(); }
    virtual void OnHideHeaders(wxCommandEvent& event) { event.Skip(); }
    virtual void OnNativeHeader(wxCommandEvent& event) { event.Skip(); }
    virtual void OnMenuitemsupportsearchMenuSelected(wxCommandEvent& event) { event.Skip(); }

public:
    clTreeCtrl* GetTreeCtrl() { return m_treeCtrl; }
    clDataViewListCtrl* GetDataView() { return m_dataView; }
    wxTextCtrl* GetTextCtrlLog() { return m_textCtrlLog; }
    wxPanel* GetMainPanel() { return m_mainPanel; }
    wxMenuBar* GetMyMenuBar() { return m_myMenuBar; }
    MainFrameBaseClass(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("My Frame"),
                       const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 300),
                       long style = wxCAPTION | wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxSYSTEM_MENU |
                                    wxCLOSE_BOX);
    virtual ~MainFrameBaseClass();
};

class MyImages : public wxImageList
{
protected:
    // Maintain a map of all bitmaps representd by their name
    std::map<wxString, wxBitmap> m_bitmaps;
    // The requested image resolution (can be one of @2x, @1.5x, @1.25x or an empty string (the default)
    wxString m_resolution;
    int m_imagesWidth;
    int m_imagesHeight;

protected:
public:
    MyImages();
    const wxBitmap& Bitmap(const wxString& name) const
    {
        if(!m_bitmaps.count(name + m_resolution)) return wxNullBitmap;
        return m_bitmaps.find(name + m_resolution)->second;
    }

    void SetBitmapResolution(const wxString& res = wxEmptyString) { m_resolution = res; }

    virtual ~MyImages();
};

#endif
