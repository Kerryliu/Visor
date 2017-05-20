#ifndef TREE_H
#define TREE_H

#include "device.h"
#include <chrono>
#include <experimental/filesystem>
#include <gtkmm.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

namespace fs = std::experimental::filesystem;

class Tree {
public:
  Tree();
  ~Tree();
  Gtk::TreeView m_TreeView;
  Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;

  void on_quit();
private:
  vector<Device> devices;
  const std::string file_path = "/sys/class/hwmon/";
  bool stop_work = false;

  Glib::Dispatcher m_Dispatcher;
  std::thread *m_WorkerThread;

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

    // Tree model columns:
    Gtk::TreeView::Column *sensor_column = new Gtk::TreeView::Column("Sensor");
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> m_pixbuf;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_current_value;
    Gtk::TreeModelColumn<Glib::ustring> m_col_min_value;
    Gtk::TreeModelColumn<Glib::ustring> m_col_max_value;
    Gtk::TreeModelColumn<Glib::ustring> m_col_average_value;
  };

  ModelColumns m_Columns;

  void make_tree_view();
  void update_values();
  void update_tree_view();

  void on_treeview_row_activated(const Gtk::TreeModel::Path &path,
                                 Gtk::TreeViewColumn *column);
};

#endif