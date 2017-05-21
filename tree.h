#ifndef TREE_H
#define TREE_H

#include "device.h"
#include <gtkmm.h>
#include <iostream>

using std::vector;

class Tree {
public:
  Gtk::TreeView m_TreeView;
  Tree(vector<vector<vector<Device::sensor_reading>>> &all_readings,
       vector<string> &device_names);
  ~Tree();

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
};

#endif
