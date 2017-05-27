#ifndef WINDOW_H
#define WINDOW_H

#include "graph.h"
#include "legend.h"
#include "tree.h"
#include <gtkmm.h>
#include <memory>
#include <thread>
#include <vector>

using std::string;

// Header file is very messy right now...
class Window : public Gtk::Window {
public:
  Window();
  virtual ~Window();

private:
  const std::string file_path = "/sys/class/hwmon/";
  bool stop_work = false;

  vector<Device> devices;
  vector<vector<vector<Device::sensor_reading>>> all_readings;
  vector<string> device_names;

  vector<Gtk::Box> m_Notebook_Boxes;
  vector<vector<std::unique_ptr<Graph>>> m_notebook_graphs;
  vector<vector<std::unique_ptr<Legend>>> m_notebook_legends;
  vector<vector<vector<Gdk::RGBA>>> m_colors;

  Glib::Dispatcher m_Dispatcher;
  std::unique_ptr<std::thread> m_WorkerThread =
      std::make_unique<std::thread>([this] { update_vals(); });

  std::unique_ptr<Tree> tree;
  Gtk::Box m_vbox;
  Gtk::HeaderBar m_headerbar;
  Gtk::StackSwitcher m_stackswitcher;
  Gtk::Stack m_stack;
  Gtk::Notebook m_notebook;
  Gtk::ScrolledWindow m_scrolledwindow;

  void update_vals();
  void update_all();
  vector<Gdk::RGBA> gen_colors(unsigned int size);
  void on_button_quit();
};

#endif
