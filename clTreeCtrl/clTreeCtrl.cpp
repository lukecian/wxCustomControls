#include "clTreeCtrl.h"
#include "clTreeCtrlModel.h"
#include <algorithm>
#include <cmath>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/utils.h>
#include <wx/wupdlock.h>

clTreeCtrl::clTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, wxID_ANY, pos, size, wxWANTS_CHARS | wxTAB_TRAVERSAL)
    , m_model(this)
    , m_treeStyle(style)
{
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);
    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    memDC.SetFont(font);
    wxSize textSize = memDC.GetTextExtent("Tp");
    m_lineHeight = clTreeCtrlNode::Y_SPACER + textSize.GetHeight() + clTreeCtrlNode::Y_SPACER;
    SetIndent(m_lineHeight);
    Bind(wxEVT_IDLE, &clTreeCtrl::OnIdle, this);
    Bind(wxEVT_PAINT, &clTreeCtrl::OnPaint, this);
    Bind(wxEVT_SIZE, &clTreeCtrl::OnSize, this);
    Bind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent& event) { wxUnusedVar(event); });
    Bind(wxEVT_LEFT_DOWN, &clTreeCtrl::OnMouseLeftDown, this);
    Bind(wxEVT_LEFT_DCLICK, &clTreeCtrl::OnMouseLeftDClick, this);
    Bind(wxEVT_MOUSEWHEEL, &clTreeCtrl::OnMouseScroll, this);
    Bind(wxEVT_LEAVE_WINDOW, &clTreeCtrl::OnLeaveWindow, this);
    Bind(wxEVT_KEY_DOWN, &clTreeCtrl::OnKeyDown, this);

    // Initialise default colours
    m_colours.textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    m_colours.selItemTextColour = m_colours.textColour;
    m_colours.selItemBgColour = wxColour("rgb(199,203,209)");
    m_colours.buttonColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW);
    m_colours.hoverBgColour = wxColour("rgb(219,221,224)");
    m_colours.bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
}

clTreeCtrl::~clTreeCtrl()
{
    Unbind(wxEVT_IDLE, &clTreeCtrl::OnIdle, this);
    Unbind(wxEVT_PAINT, &clTreeCtrl::OnPaint, this);
    Unbind(wxEVT_SIZE, &clTreeCtrl::OnSize, this);
    Unbind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent& event) { wxUnusedVar(event); });
    Unbind(wxEVT_LEFT_DOWN, &clTreeCtrl::OnMouseLeftDown, this);
    Unbind(wxEVT_LEFT_DCLICK, &clTreeCtrl::OnMouseLeftDClick, this);
    Unbind(wxEVT_MOUSEWHEEL, &clTreeCtrl::OnMouseScroll, this);
    Unbind(wxEVT_LEAVE_WINDOW, &clTreeCtrl::OnLeaveWindow, this);
    Unbind(wxEVT_KEY_DOWN, &clTreeCtrl::OnKeyDown, this);
}

void clTreeCtrl::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC pdc(this);
    wxGCDC dc(pdc);

    wxRect clientRect = GetClientRect();
    dc.SetPen(m_colours.bgColour);
    dc.SetBrush(m_colours.bgColour);
    dc.DrawRectangle(clientRect);

    int maxItems = GetNumLineCanFitOnScreen();
    if(!m_firstOnScreenItem) { m_firstOnScreenItem = m_model.GetRoot(); }
    clTreeCtrlNode* firstItem = m_firstOnScreenItem;
    if(!firstItem) { return; }
    int y = clientRect.GetY();
    clTreeCtrlNode::Vec_t items;
    m_model.GetNextItems(firstItem, maxItems, items);

    for(size_t i = 0; i < items.size(); ++i) {
        clTreeCtrlNode* curitem = items[i];
        wxRect itemRect = wxRect(0, y, clientRect.GetWidth(), m_lineHeight);
        wxRect buttonRect;
        if(curitem->HasChildren()) {
            buttonRect = wxRect((curitem->GetIndentsCount() * GetIndent()), y, m_lineHeight, m_lineHeight);
        }
        curitem->SetRects(itemRect, buttonRect);
        curitem->Render(dc, m_colours);
        y += m_lineHeight;
    }
    m_model.SetOnScreenItems(items); // Keep track of the visible items
}

void clTreeCtrl::OnSize(wxSizeEvent& event)
{
    Refresh();
    event.Skip();
}

wxTreeItemId clTreeCtrl::AppendItem(
    const wxTreeItemId& parent, const wxString& text, int image, int selImage, wxTreeItemData* data)
{
    wxTreeItemId item = m_model.AppendItem(parent, text, image, selImage, data);
    m_model.StateModified();
    return item;
}

wxTreeItemId clTreeCtrl::AddRoot(const wxString& text, int image, int selImage, wxTreeItemData* data)
{
    wxTreeItemId root = m_model.AddRoot(text, image, selImage, data);
    m_model.StateModified();
    return root;
}

wxTreeItemId clTreeCtrl::GetRootItem() const { return m_model.GetRootItem(); }

void clTreeCtrl::Expand(const wxTreeItemId& item)
{
    if(!item.GetID()) return;
    clTreeCtrlNode* child = reinterpret_cast<clTreeCtrlNode*>(item.GetID());
    if(!child) return;
    child->SetExpanded(true);
    m_model.StateModified();
    Refresh();
}

void clTreeCtrl::Collapse(const wxTreeItemId& item)
{
    if(!item.GetID()) return;
    clTreeCtrlNode* child = reinterpret_cast<clTreeCtrlNode*>(item.GetID());
    if(!child) return;
    child->SetExpanded(false);
    m_model.StateModified();
    Refresh();
}

int clTreeCtrl::GetExpandedLines() { return m_model.GetExpandedLines(); }

void clTreeCtrl::SelectItem(const wxTreeItemId& item, bool select)
{
    m_model.SelectItem(item, select);
    Refresh();
}

void clTreeCtrl::OnMouseLeftDown(wxMouseEvent& event)
{
    event.Skip();
    int flags = 0;
    wxPoint pt = DoFixPoint(event.GetPosition());
    wxTreeItemId where = HitTest(pt, flags);
    if(where.IsOk()) {
        if(flags & wxTREE_HITTEST_ONITEMBUTTON) {
            if(IsExpanded(where)) {
                Collapse(where);
            } else {
                Expand(where);
            }
        } else {
            UnselectAll();
            SelectItem(where, true);
        }
        Refresh();
    }
}

wxTreeItemId clTreeCtrl::HitTest(const wxPoint& point, int& flags) const
{
    flags = 0;
    for(size_t i = 0; i < m_model.GetOnScreenItems().size(); ++i) {
        const clTreeCtrlNode* item = m_model.GetOnScreenItems()[i];
        if(item->GetButtonRect().Contains(point)) {
            flags |= wxTREE_HITTEST_ONITEMBUTTON;
            return wxTreeItemId(const_cast<clTreeCtrlNode*>(item));
        }
        if(item->GetItemRect().Contains(point)) { return wxTreeItemId(const_cast<clTreeCtrlNode*>(item)); }
    }
    return wxTreeItemId();
}

void clTreeCtrl::UnselectAll()
{
    m_model.UnselectAll();
    Refresh();
}

wxPoint clTreeCtrl::DoFixPoint(const wxPoint& pt)
{
    wxPoint point = pt;
    return point;
}

void clTreeCtrl::EnsureVisible(const wxTreeItemId& item)
{
    if(!item.IsOk()) { return; }
    // Make sure that all parents of ítem are expanded
    if(!m_model.ExpandToItem(item)) { return; }
    Refresh();
    CallAfter(&clTreeCtrl::DoEnsureVisible, item);
}

void clTreeCtrl::DoEnsureVisible(const wxTreeItemId& item)
{
    // scroll to the item
    if(!item.IsOk()) { return; }
    clTreeCtrlNode* pNode = reinterpret_cast<clTreeCtrlNode*>(item.GetID());
    if(IsItemVisible(pNode)) { return; }
    EnsureItemVisible(pNode, false); // make it visible at the bottom
    Refresh();
}

void clTreeCtrl::OnMouseLeftDClick(wxMouseEvent& event)
{
    event.Skip();
    int flags = 0;
    wxPoint pt = DoFixPoint(event.GetPosition());
    wxTreeItemId where = HitTest(pt, flags);
    if(where.IsOk()) {
        UnselectAll();
        SelectItem(where, true);
        if(ItemHasChildren(where)) {
            if(IsExpanded(where)) {
                Collapse(where);
            } else {
                Expand(where);
            }
        }
    }
}

bool clTreeCtrl::IsExpanded(const wxTreeItemId& item) const
{
    if(!item.GetID()) return false;
    clTreeCtrlNode* child = reinterpret_cast<clTreeCtrlNode*>(item.GetID());
    if(!child) return false;
    return child->IsExpanded();
}

bool clTreeCtrl::ItemHasChildren(const wxTreeItemId& item) const
{
    if(!item.GetID()) return false;
    clTreeCtrlNode* child = reinterpret_cast<clTreeCtrlNode*>(item.GetID());
    if(!child) return false;
    return child->HasChildren();
}

void clTreeCtrl::SetIndent(int size)
{
    m_model.SetIndentSize(size);
    Refresh();
}

int clTreeCtrl::GetIndent() const { return m_model.GetIndentSize(); }

bool clTreeCtrl::IsEmpty() const { return m_model.IsEmpty(); }

size_t clTreeCtrl::GetChildrenCount(const wxTreeItemId& item, bool recursively) const
{
    if(!item.GetID()) return 0;
    clTreeCtrlNode* node = reinterpret_cast<clTreeCtrlNode*>(item.GetID());
    return node->GetChildrenCount(recursively);
}

void clTreeCtrl::DeleteChildren(const wxTreeItemId& item)
{
    if(!item.GetID()) return;
    clTreeCtrlNode* node = reinterpret_cast<clTreeCtrlNode*>(item.GetID());
    node->RemoveAllChildren();
    m_model.StateModified();
}

wxTreeItemId clTreeCtrl::GetFirstChild(const wxTreeItemId& item, clTreeItemIdValue& cookie) const
{
    if(!item.GetID()) return wxTreeItemId();
    clTreeCtrlNode* node = reinterpret_cast<clTreeCtrlNode*>(item.GetID());
    const clTreeCtrlNode::Vec_t& children = node->GetChildren();
    if(children.empty()) return wxTreeItemId(); // No children
    cookie.nextItem = 1;                        // the next item
    return wxTreeItemId(children[0]);
}

wxTreeItemId clTreeCtrl::GetNextChild(const wxTreeItemId& item, clTreeItemIdValue& cookie) const
{
    if(!item.GetID()) return wxTreeItemId();
    clTreeCtrlNode* node = reinterpret_cast<clTreeCtrlNode*>(item.GetID());
    const clTreeCtrlNode::Vec_t& children = node->GetChildren();
    if((int)children.size() >= cookie.nextItem) return wxTreeItemId();
    wxTreeItemId child(children[cookie.nextItem]);
    cookie.nextItem++;
    return child;
}

wxString clTreeCtrl::GetItemText(const wxTreeItemId& item) const
{
    if(!item.GetID()) return "";
    clTreeCtrlNode* node = reinterpret_cast<clTreeCtrlNode*>(item.GetID());
    return node->GetLabel();
}

wxTreeItemData* clTreeCtrl::GetItemData(const wxTreeItemId& item) const
{
    if(!item.GetID()) return nullptr;
    clTreeCtrlNode* node = reinterpret_cast<clTreeCtrlNode*>(item.GetID());
    return node->GetClientObject();
}

void clTreeCtrl::OnMouseScroll(wxMouseEvent& event)
{
    if(!m_firstOnScreenItem) { return; }
    clTreeCtrlNode::Vec_t items;
    if(event.GetWheelRotation() > 0) { // Scrolling up
        m_model.GetPrevItems(m_firstOnScreenItem, m_scrollTick, items);
        m_firstOnScreenItem = items.front(); // first item
    } else {
        m_model.GetNextItems(m_firstOnScreenItem, m_scrollTick, items);
        m_firstOnScreenItem = items.back(); // the last item
    }
    Refresh();
}

const wxBitmap& clTreeCtrl::GetBitmap(size_t index) const
{
    if(index >= m_bitmaps.size()) {
        static wxBitmap emptyBitmap;
        return emptyBitmap;
    }
    return m_bitmaps[index];
}

void clTreeCtrl::SetBitmaps(const std::vector<wxBitmap>& bitmaps)
{
    m_bitmaps = bitmaps;
    int heighestBitmap = 0;
    for(size_t i = 0; i < m_bitmaps.size(); ++i) {
        heighestBitmap = wxMax(heighestBitmap, m_bitmaps[i].GetScaledHeight());
    }
    heighestBitmap += 2 * clTreeCtrlNode::Y_SPACER;
    m_lineHeight = wxMax(heighestBitmap, m_lineHeight);
    SetIndent(m_lineHeight);
    Refresh();
}

void clTreeCtrl::OnIdle(wxIdleEvent& event)
{
    int flags = 0;
    wxPoint pt = ScreenToClient(::wxGetMousePosition());
    wxTreeItemId item = HitTest(pt, flags);
    if(item.IsOk()) {
        clTreeCtrlNode::Vec_t& items = m_model.GetOnScreenItems();
        clTreeCtrlNode* hoveredNode = reinterpret_cast<clTreeCtrlNode*>(item.GetID());
        bool refreshNeeded = false;
        for(size_t i = 0; i < items.size(); ++i) {
            bool new_state = hoveredNode == items[i];
            bool old_state = items[i]->IsHovered();
            if(!refreshNeeded) { refreshNeeded = (new_state != old_state); }
            items[i]->SetHovered(hoveredNode == items[i]);
        }
        if(refreshNeeded) { Refresh(); }
    }
}

void clTreeCtrl::OnLeaveWindow(wxMouseEvent& event)
{
    event.Skip();
    clTreeCtrlNode::Vec_t& items = m_model.GetOnScreenItems();
    for(size_t i = 0; i < items.size(); ++i) { items[i]->SetHovered(false); }
    Refresh();
}

void clTreeCtrl::SetColours(const clTreeCtrlColours& colours)
{
    m_colours = colours;
    Refresh();
}

void clTreeCtrl::ExpandAllChildren(const wxTreeItemId& item)
{
    wxBusyCursor bc;
    m_model.ExpandAllChildren(item);
    Refresh();
}

void clTreeCtrl::CollapseAllChildren(const wxTreeItemId& item)
{
    wxBusyCursor bc;
    m_model.CollapseAllChildren(item);
    Refresh();
}

wxTreeItemId clTreeCtrl::GetFirstVisibleItem() const
{
    const clTreeCtrlNode::Vec_t& items = m_model.GetOnScreenItems();
    if(items.empty()) { return wxTreeItemId(); }
    return wxTreeItemId(items[0]);
}

wxTreeItemId clTreeCtrl::GetNextVisible(const wxTreeItemId& item) const { return DoGetSiblingVisibleItem(item, true); }
wxTreeItemId clTreeCtrl::GetPrevVisible(const wxTreeItemId& item) const { return DoGetSiblingVisibleItem(item, false); }

wxTreeItemId clTreeCtrl::DoGetSiblingVisibleItem(const wxTreeItemId& item, bool next) const
{
    if(!item.IsOk()) { return wxTreeItemId(); }
    const clTreeCtrlNode::Vec_t& items = m_model.GetOnScreenItems();
    if(items.empty()) { return wxTreeItemId(); }
    clTreeCtrlNode* from = reinterpret_cast<clTreeCtrlNode*>(item.GetID());
    clTreeCtrlNode::Vec_t::const_iterator iter
        = std::find_if(items.begin(), items.end(), [&](clTreeCtrlNode* p) { return p == from; });
    if(next && (iter == items.end())) { return wxTreeItemId(); }
    if(!next && (iter == items.begin())) { return wxTreeItemId(); }
    if(next) {
        ++iter;
        if(iter == items.end()) { return wxTreeItemId(); }
    } else {
        --iter;
        if(iter == items.begin()) { return wxTreeItemId(); }
    }
    return wxTreeItemId(*iter);
}

wxTreeItemId clTreeCtrl::GetSelection() const
{
    const clTreeCtrlNode::Vec_t& items = m_model.GetSelections();
    if(items.empty()) { return wxTreeItemId(); }
    return wxTreeItemId(items[0]);
}

wxTreeItemId clTreeCtrl::GetFocusedItem() const { return GetSelection(); }

size_t clTreeCtrl::GetSelections(wxArrayTreeItemIds& selections) const
{
    const clTreeCtrlNode::Vec_t& items = m_model.GetSelections();
    if(items.empty()) { return 0; }
    std::for_each(items.begin(), items.end(), [&](clTreeCtrlNode* item) { selections.Add(wxTreeItemId(item)); });
    return selections.size();
}

void clTreeCtrl::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();
    if(event.GetKeyCode() == WXK_UP) {
        wxTreeItemId selectedItem = GetSelection();
        selectedItem = m_model.GetItemBefore(selectedItem, true);
        if(selectedItem.IsOk()) {
            m_model.UnselectAll();
            SelectItem(selectedItem);
            EnsureItemVisible(m_model.ToPtr(selectedItem), true);
        }
    } else if(event.GetKeyCode() == WXK_DOWN) {
        wxTreeItemId selectedItem = GetSelection();
        selectedItem = m_model.GetItemAfter(selectedItem, true);
        if(selectedItem.IsOk()) {
            m_model.UnselectAll();
            SelectItem(selectedItem);
            EnsureItemVisible(m_model.ToPtr(selectedItem), false);
        }
    }
}

bool clTreeCtrl::IsItemVisible(clTreeCtrlNode* item) const
{
    const clTreeCtrlNode::Vec_t& onScreenItems = m_model.GetOnScreenItems();
    return (std::find_if(onScreenItems.begin(), onScreenItems.end(), [&](clTreeCtrlNode* p) { return p == item; })
        != onScreenItems.end());
}

void clTreeCtrl::EnsureItemVisible(clTreeCtrlNode* item, bool fromTop)
{
    if(IsItemVisible(item)) { return; }
    if(fromTop) {
        m_firstOnScreenItem = item;
    } else {
        int max_lines_on_screen = GetNumLineCanFitOnScreen();
        clTreeCtrlNode::Vec_t items;
        m_model.GetPrevItems(item, max_lines_on_screen, items);
        if(items.empty()) { return; }
        m_firstOnScreenItem = items[0];
    }
}

int clTreeCtrl::GetNumLineCanFitOnScreen() const
{
    wxRect clientRect = GetClientRect();
    int max_lines_on_screen = ceil(clientRect.GetHeight() / m_lineHeight);
    return max_lines_on_screen;
}