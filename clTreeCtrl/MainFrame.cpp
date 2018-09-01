#include "MainFrame.h"
#include "clTreeCtrl.h"
#include <wx/aboutdlg.h>
#include <wx/dir.h>
#include <wx/dirdlg.h>
#include <wx/msgdlg.h>
#include <wx/numdlg.h>

MainFrame::MainFrame(wxWindow* parent)
    : MainFrameBaseClass(parent)
{
    m_tree = new clTreeCtrl(m_mainPanel);
    clTreeCtrlColours colours = m_tree->GetColours();
    m_tree->SetColours(colours);

    std::vector<wxBitmap> bitmaps;
    MyImages images;
    bitmaps.push_back(images.Bitmap("folder"));
    bitmaps.push_back(images.Bitmap("folder_open"));
    bitmaps.push_back(images.Bitmap("file"));
    m_tree->SetBitmaps(bitmaps);
    m_mainPanel->GetSizer()->Insert(0, m_tree, 1, wxEXPAND);

    m_tree->Bind(wxEVT_TREE_ITEM_EXPANDING, &MainFrame::OnItemExpanding, this);
    m_tree->Bind(wxEVT_TREE_ITEM_EXPANDED, [&](wxTreeEvent& evt) {
        clTreeCtrlNode* node = reinterpret_cast<clTreeCtrlNode*>(evt.GetItem().GetID());
        LogMessage(wxString() << node->GetLabel() << " expanded");
    });
    m_tree->Bind(wxEVT_TREE_ITEM_COLLAPSING, [&](wxTreeEvent& evt) {
        clTreeCtrlNode* node = reinterpret_cast<clTreeCtrlNode*>(evt.GetItem().GetID());
        LogMessage(wxString() << node->GetLabel() << " is collapsing");
    });
    m_tree->Bind(wxEVT_TREE_ITEM_COLLAPSED, [&](wxTreeEvent& evt) {
        clTreeCtrlNode* node = reinterpret_cast<clTreeCtrlNode*>(evt.GetItem().GetID());
        LogMessage(wxString() << node->GetLabel() << " collapsed");
    });
}

MainFrame::~MainFrame()
{
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Close();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxAboutDialogInfo info;
    info.SetCopyright(_("My MainFrame"));
    info.SetLicence(_("GPL v2 or later"));
    info.SetDescription(_("Short description goes here"));
    ::wxAboutBox(info);
}

void MainFrame::LogMessage(const wxString& message)
{
    m_stc15->AddText(message + "\n");
    // m_stc15->ScrollToEnd();
}

void MainFrame::OnOpenFolder(wxCommandEvent& event)
{
    wxString path = wxDirSelector();
    if(path.IsEmpty()) { return; }
    if(!m_tree->IsEmpty()) { return; }
    m_path = path;
    wxTreeItemId root = m_tree->AddRoot(path, 0, 1, new MyItemData(m_path));
    m_tree->AppendItem(root, "dummy-node");
    m_tree->SelectItem(root);
}

void MainFrame::OnItemExpanding(wxTreeEvent& event)
{
    event.Skip();
    wxTreeItemId item = event.GetItem();
    clTreeCtrlNode* node = reinterpret_cast<clTreeCtrlNode*>(item.GetID());
    LogMessage(wxString() << node->GetLabel() << " is expanding");

    clTreeItemIdValue cookie;
    if(m_tree->ItemHasChildren(item)) {
        wxTreeItemId child = m_tree->GetFirstChild(item, cookie);
        if(m_tree->GetItemText(child) == "dummy-node") {
            m_tree->DeleteChildren(item);
            // load the folders
            MyItemData* cd = dynamic_cast<MyItemData*>(m_tree->GetItemData(item));
            wxString filename;
            wxDir dir(cd->GetPath());
            bool cont = dir.GetFirst(&filename);
            while(cont) {
                wxFileName fn(cd->GetPath(), filename);
                if(wxDirExists(fn.GetFullPath())) {
                    // A directory
                    wxTreeItemId folderItem
                        = m_tree->AppendItem(item, filename, 0, 1, new MyItemData(fn.GetFullPath()));
                    m_tree->AppendItem(folderItem, "dummy-node");
                } else {
                    // A file
                    m_tree->AppendItem(item, filename, 2, 2, new MyItemData(fn.GetFullPath()));
                }
                cont = dir.GetNext(&filename);
            }
        }
    }
}
void MainFrame::OnExpandAll(wxCommandEvent& event) { m_tree->ExpandAll(); }
void MainFrame::OnCollapseAll(wxCommandEvent& event) { m_tree->CollapAll(); }
void MainFrame::OnFirstVisible(wxCommandEvent& event)
{
    wxTreeItemId item = m_tree->GetFirstVisibleItem();
    if(item.IsOk()) { wxMessageBox("First visible item: " + m_tree->GetItemText(item)); }
}

void MainFrame::OnNextVisible(wxCommandEvent& event)
{
    wxTreeItemId item = m_tree->GetNextVisible(m_tree->GetFocusedItem());
    if(item.IsOk()) { wxMessageBox("Next visible item (from selection): " + m_tree->GetItemText(item)); }
}
void MainFrame::OnEnsureItemVisible(wxCommandEvent& event)
{
    // long index = wxGetNumberFromUser("Item index:", "Item index:", "clTreeCtrl");
    // wxTreeItemId item = m_tree->RowToItem(index);
    // if(item.IsOk()) { wxMessageBox("Item is: " + m_tree->GetItemText(item)); }
    // m_tree->SelectItem(item);
    // m_tree->EnsureVisible(item);
}