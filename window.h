#ifndef WINDOW_H
#define WINDOW_H

#include <gtkmm.h>
#include "device.h"
#include <experimental/filesystem>
#include <iostream>
#include <vector>

class Window : public Gtk::Window {
public:
  Window();
  virtual ~Window();

protected:
  int test = 0;
  std::vector<Device> devices;
  std::string path = "/sys/class/hwmon/";
  bool update_tree_view(int x);
  void make_tree_view();
  // Signal handlers:
  void on_button_quit();
  void on_treeview_row_activated(const Gtk::TreeModel::Path &path,
                                 Gtk::TreeViewColumn *column);

  // Tree model columns:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    ModelColumns() {
      add(m_col_name);
      add(m_col_value);
    }

    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_value;
  };

  ModelColumns m_Columns;

  // Child widgets:
  Gtk::Box m_VBox;

  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::TreeView m_TreeView;
  Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;

  Gtk::ButtonBox m_ButtonBox;
  Gtk::Button m_Button_Quit;
};

#endif
