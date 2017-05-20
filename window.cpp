#include "window.h"

Window::Window()
    : m_VBox(Gtk::ORIENTATION_VERTICAL),
      m_VPanedTemperature(Gtk::ORIENTATION_VERTICAL),
      m_VPanedFan(Gtk::ORIENTATION_VERTICAL) {
  set_border_width(1);
  set_default_size(500, 600);

  Gtk::Window::set_titlebar(m_headerBar);
  add(m_VBox);

  // Add the TreeView, inside a ScrolledWindow:
  m_ScrolledWindow_summary.add(tree->m_TreeView);

  // Only show the scrollbars when they are necessary:
  m_ScrolledWindow_summary.set_policy(Gtk::POLICY_AUTOMATIC,
                                      Gtk::POLICY_AUTOMATIC);

  // Boxes for temperatures and fans:
  m_VPanedTemperature.pack1(*graph_temperatures);
  m_VPanedTemperature.pack2(*graph_fans);

  // Attempt to make that stack:
  m_stack.add(m_ScrolledWindow_summary, "summary", "Summary");
  m_stack.add(m_VPanedTemperature, "temperatures", "Temperatures");
  // m_stack.add(m_VBoxFan, "fans", "Fans");
  m_stackSwitcher.set_stack(m_stack);
  m_VBox.pack_start(m_stack);

  // HeaderBar:
  m_headerBar.set_show_close_button(true);
  m_headerBar.set_custom_title(m_stackSwitcher);

  show_all_children();
}

Window::~Window() {
  delete tree;
  delete graph_temperatures;
  delete graph_fans;
}

void Window::on_button_quit() {
  tree->on_quit();
  hide();
}
