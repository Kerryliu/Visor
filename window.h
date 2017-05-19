#ifndef WINDOW_H
#define WINDOW_H

#include "device.h"
#include <chrono>
#include <experimental/filesystem>
#include <gtkmm.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

using std::vector;
using std::string;

// Header file is very messy right now...
class Window : public Gtk::Window {
public:
  Window();
  virtual ~Window();

protected:
  bool stop_work = false;
  vector<Device> devices;
  const std::string file_path = "/sys/class/hwmon/";
  void make_tree_view();
  void update_values();
  void update_tree_view();
  Glib::Dispatcher m_Dispatcher;
  std::thread *m_WorkerThread;
  // Signal handlers:
  void on_button_quit();
  void on_treeview_row_activated(const Gtk::TreeModel::Path &path,
                                 Gtk::TreeViewColumn *column);

  // Tree model columns:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    ModelColumns() {
      add(m_pixbuf);
      add(m_col_name);
      add(m_col_current_value);
      add(m_col_min_value);
      add(m_col_max_value);
      add(m_col_average_value);
    }

    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> m_pixbuf;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_current_value;
    Gtk::TreeModelColumn<Glib::ustring> m_col_min_value;
    Gtk::TreeModelColumn<Glib::ustring> m_col_max_value;
    Gtk::TreeModelColumn<Glib::ustring> m_col_average_value;
  };

  ModelColumns m_Columns;

  // Child widgets:
  Gtk::Box m_VBox;

  Gtk::HeaderBar m_headerBar;

  Gtk::StackSwitcher m_stackSwitcher;
  Gtk::Stack m_stack;

  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::TreeView m_TreeView;
  Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;

  Gtk::ButtonBox m_ButtonBox;
  Gtk::Button m_Button_Quit;
};

#endif
