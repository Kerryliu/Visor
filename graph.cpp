#include "graph.h"
#include <glibmm/main.h>
#include <iostream>
#include <list>
#include <vector>

using std::vector;

Graph::Graph(const vector<Device::sensor_reading> &device_readings,
             int device_index, int type)
    : device_readings(device_readings), device_index(device_index), type(type) {
  gen_colors();
  original_y.resize(device_readings.size());
  normalized_y.resize(device_readings.size());
  // sigc::connection conn = Glib::signal_timeout().connect(
  //     sigc::mem_fun(*this, &Graph::update), 1000);
}

Graph::~Graph() {}

const unsigned int Graph::get_type() const { return type; }
const unsigned int Graph::get_device_index() const { return device_index; }

// https://developer.gnome.org/gtkmm-tutorial/stable/chapter-drawingarea.html.en
bool Graph::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
  Gtk::Allocation allocation = get_allocation();
  width = allocation.get_width();
  height = allocation.get_height();
  draw_title(cr);
  draw_legend(cr);
  rectangle_width = width - x_start - right_padding;
  rectangle_height = height - y_start - legend_offset;
  check_resize();
  draw_graph_grid(cr);
  for (unsigned int i = 0; i < device_readings.size(); i++) {
    make_plot(cr, i);
  }
  return true;
}

bool Graph::update() {
  // update values
  for (unsigned int sensor_index = 0; sensor_index < device_readings.size();
       sensor_index++) {
    double temp = device_readings[sensor_index].current_value;
    if (type == TEMPERATURE) {
      temp /= 1000;
    }
    original_y[sensor_index].push_front(temp);
    normalized_y[sensor_index].push_front(y_start + rectangle_height -
                                          temp / max_type_values[type] *
                                              rectangle_height);
    if (normalized_y[sensor_index].size() > 61) {
      original_y[sensor_index].pop_back();
      normalized_y[sensor_index].pop_back();
    }
  }

  // Refresh window;
  auto win = get_window();
  if (win) {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                     get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
  return true;
}

void Graph::check_resize() {
  // On resize, need to scale all previous values
  if (prev_y_axis_pixels != rectangle_height) {
    prev_y_axis_pixels = rectangle_height;
    for (unsigned int i = 0; i < device_readings.size(); i++) {
      std::list<unsigned int>::iterator orig_y_it;
      std::list<unsigned int>::iterator norm_y_it;
      for (orig_y_it = original_y[i].begin(),
          norm_y_it = normalized_y[i].begin();
           orig_y_it != original_y[i].end(); orig_y_it++, norm_y_it++) {
        *norm_y_it =
            y_start + rectangle_height -
            (double)(*orig_y_it) / max_type_values[type] * rectangle_height;
      }
    }
  }
}

void Graph::gen_colors() {
  double r = 0;
  double g = 0;
  double b = 0;
  double stepping = 3 / (double)device_readings.size();
  for (unsigned sensor_index = 0; sensor_index < device_readings.size();
       sensor_index++) {
    if (sensor_index % 3 == 0) { // 0, 3, 6...
      r += stepping;
      if (b == 1) {
        b = 0;
      }
    } else if ((sensor_index + 1) % 3 == 0) { // 1, 4, 5..
      g += stepping;
      if (r == 1) {
        r = 0;
      }
    } else { // 2, 5, 7...
      b += stepping;
      if (g == 1) {
        g = 0;
      }
    }
    colors.push_back({r, g, b});
  }
}

void Graph::update_values(vector<Device::sensor_reading> &device_readings) {
  this->device_readings = device_readings;
  update();
}

void Graph::draw_title(const Cairo::RefPtr<Cairo::Context> &cr) {
  const int left_offset = 10;
  const int top_offset = 10;
  cr->move_to(left_offset, top_offset);
  cr->set_source_rgb(0.4, 0.4, 0.4);

  Pango::FontDescription font;
  font.set_weight(Pango::WEIGHT_BOLD);
  auto layout = create_pango_layout(Device::sensor_types[type]);

  layout->set_font_description(font);
  layout->show_in_cairo_context(cr);
}

void Graph::draw_legend(const Cairo::RefPtr<Cairo::Context> &cr) {
  // First run if no colors have been made: (This needs work)
  const unsigned int bottom_offset = 5;
  const unsigned int side_offset = 50;
  const unsigned int line_spacing = 20;
  const unsigned int working_area = (width - side_offset * 2);
  const unsigned int devices_per_line = 4;
  const unsigned int spacing = working_area / devices_per_line;
  unsigned int num_lines;
  // Calulate spacing between keys:
  num_lines =
      (device_readings.size() + devices_per_line - 1) / devices_per_line;
  // Attempt to draw the damn thing:
  for (unsigned int line_index = 0; line_index < num_lines; line_index++) {
    unsigned int y_coord =
        height - bottom_offset - (line_spacing * (num_lines - line_index));
    Pango::FontDescription font;
    for (unsigned int spacing_index = 0; spacing_index < devices_per_line;
         spacing_index++) {
      unsigned int x_coord = side_offset + spacing * spacing_index;
      unsigned int device_index = spacing_index + devices_per_line * line_index;
      if (device_index >= device_readings.size()) {
        break;
      }
      cr->move_to(x_coord, y_coord);
      cr->set_source_rgb(colors[device_index][0], colors[device_index][1],
                         colors[device_index][2]);
      auto layout = create_pango_layout(device_readings[device_index].name);
      layout->set_font_description(font);
      layout->show_in_cairo_context(cr);
    }
  }

  legend_offset = line_spacing * num_lines + bottom_offset * 2;
}

void Graph::draw_graph_grid(const Cairo::RefPtr<Cairo::Context> &cr) {
  cr->set_line_width(line_width);
  // Vertical:
  cr->set_source_rgb(0.9, 0.9, 0.9);
  const unsigned int line_count =
      ((rectangle_height) / min_line_spacing > max_line_count)
          ? max_line_count
          : rectangle_height / min_line_spacing;
  unsigned int line_spacing = (rectangle_height) / line_count;
  for (unsigned int line_index = 1; line_index < line_count; line_index++) {
    cr->move_to(x_start, y_start + line_spacing * line_index);
    cr->line_to(x_start + rectangle_width + over_shoot,
                y_start + line_spacing * line_index);
  }
  // // Horizontal:
  const unsigned int horizontal_line_count = 5;
  line_spacing = rectangle_width / horizontal_line_count;
  for (unsigned int line_index = 1; line_index < horizontal_line_count;
       line_index++) {
    cr->move_to(x_start + line_spacing * line_index, y_start);
    cr->line_to(x_start + line_spacing * line_index,
                y_start + rectangle_height + over_shoot);
  }
  cr->stroke();

  // Draw outer rectangle:
  cr->set_source_rgb(0.5, 0.5, 0.5);
  cr->move_to(x_start, y_start);
  cr->line_to(x_start + rectangle_width + over_shoot, y_start);
  cr->move_to(x_start, y_start);
  cr->line_to(x_start, y_start + rectangle_height + over_shoot);
  cr->move_to(x_start + rectangle_width, y_start);
  cr->line_to(x_start + rectangle_width,
              y_start + rectangle_height + over_shoot);
  cr->move_to(x_start, y_start + rectangle_height + line_width);
  cr->line_to(x_start + rectangle_width + over_shoot,
              y_start + rectangle_height + line_width);
  cr->stroke();

  // Draw scale on right:
  cr->move_to(rectangle_width + x_start + 5, y_start);
  cr->set_source_rgb(0.4, 0.4, 0.4);
  Pango::FontDescription font;
  font.set_absolute_size(10000);
  auto layout = create_pango_layout(std::to_string(max_type_values[type]) +
                                    type_units[type]);
  layout->set_font_description(font);
  layout->show_in_cairo_context(cr);
}

void Graph::make_plot(const Cairo::RefPtr<Cairo::Context> &cr,
                      unsigned int sensor_index) {

  unsigned int starting_x_value = rectangle_width + x_start;
  double x_axis_pixel_stepping = (double)rectangle_width / 60;

  // Attempt to plot
  cr->set_line_width(line_width);
  cr->set_source_rgb(colors[sensor_index][0], colors[sensor_index][1],
                     colors[sensor_index][2]);
  cr->move_to(starting_x_value, normalized_y[sensor_index].front());
  unsigned int prev_y = 0;
  unsigned int loop_index = 0;
  for (unsigned int y_value : normalized_y[sensor_index]) {
    if (!prev_y) {
      prev_y = y_value;
      cr->line_to(starting_x_value - x_axis_pixel_stepping * loop_index,
                  prev_y);
    } else {
      // This mathy stuff is from gnome-system-monitor. I consider it magic.
      cr->curve_to(
          starting_x_value - ((loop_index - 0.5) * x_axis_pixel_stepping),
          prev_y,
          starting_x_value - ((loop_index - 0.5) * x_axis_pixel_stepping),
          y_value, starting_x_value - x_axis_pixel_stepping * loop_index,
          y_value);
      prev_y = y_value;
    }
    loop_index++;
  }
  cr->set_line_join(Cairo::LineJoin::LINE_JOIN_ROUND);
  cr->stroke();
}
