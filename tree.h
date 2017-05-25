#ifndef TREE_H
#define TREE_H

#include "device.h"
#include <gtkmm.h>
#include <iostream>
#include <memory>

using std::vector;

class Tree {
public:
  Gtk::TreeView m_TreeView;
  Tree(vector<vector<vector<Device::sensor_reading>>> &all_readings,
       vector<string> &device_names);

  void update_tree_view(
      vector<vector<vector<Device::sensor_reading>>> &all_readings);

private:
  vector<vector<vector<Device::sensor_reading>>> all_readings;
  vector<string> device_names;
  Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;

  class ModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    ModelColumns() {
      add(m_pixbuf);
      add(m_col_name);
      add(m_col_cur_val);
      add(m_col_min_val);
      add(m_col_max_val);
      add(m_col_avg_val);
    }

    // Tree model columns:
    std::unique_ptr<Gtk::TreeView::Column> sensor_column =
        std::make_unique<Gtk::TreeView::Column>(
            Gtk::TreeView::Column("Sensor"));
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> m_pixbuf;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_cur_val;
    Gtk::TreeModelColumn<Glib::ustring> m_col_min_val;
    Gtk::TreeModelColumn<Glib::ustring> m_col_max_val;
    Gtk::TreeModelColumn<Glib::ustring> m_col_avg_val;
  };

  ModelColumns m_Columns;

  void make_tree_view();
};

#endif
