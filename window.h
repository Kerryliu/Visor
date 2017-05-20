#ifndef WINDOW_H
#define WINDOW_H

#include "tree.h"
#include <gtkmm.h>
#include <iostream>
#include <vector>

using std::vector;
using std::string;

// Header file is very messy right now...
class Window : public Gtk::Window {
public:
  Window();
  virtual ~Window();
protected:
  Tree* tree = new Tree();
  // Signal handlers:
  void on_button_quit();


  // Child widgets:
  Gtk::Box m_VBox;

  Gtk::HeaderBar m_headerBar;

  Gtk::StackSwitcher m_stackSwitcher;
  Gtk::Stack m_stack;

  Gtk::ScrolledWindow m_ScrolledWindow;

  Gtk::ButtonBox m_ButtonBox;
  Gtk::Button m_Button_Quit;
};

#endif
