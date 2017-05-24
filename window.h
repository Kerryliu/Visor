#ifndef WINDOW_H
#define WINDOW_H

#include "graph.h"
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
  vector<Device> devices;
  vector<vector<vector<Device::sensor_reading>>> all_readings;
  vector<string> device_names;
  const std::string file_path = "/sys/class/hwmon/";
  bool stop_work = false;
  vector<Gtk::Box> m_Notebook_Boxes;
  vector<vector<std::unique_ptr<Graph>>> m_Notebook_Graphs;

  Glib::Dispatcher m_Dispatcher;
  std::unique_ptr<std::thread> m_WorkerThread =
      std::make_unique<std::thread>([this] { update_values(); });

  std::unique_ptr<Tree> tree;
  Gtk::Box m_VBox;
  Gtk::HeaderBar m_headerBar;
  Gtk::StackSwitcher m_stackSwitcher;
  Gtk::Stack m_stack;
  Gtk::Notebook m_Notebook;
  Gtk::ScrolledWindow m_ScrolledWindow;

  void update_values();
  void update_all();
  void on_button_quit();
};

#endif
