#include "graph.h"
#include <gtkmm/drawingarea.h>
#include <iostream>
#include <vector>

using std::vector;

Graph::Graph(const vector<Device::sensor_reading> &device_readings, int type)
    : type(type), device_readings(device_readings) {
  gen_colors();
}

Graph::~Graph() {}

// https://developer.gnome.org/gtkmm-tutorial/stable/chapter-drawingarea.html.en
bool Graph::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
  Gtk::Allocation allocation = get_allocation();
  width = allocation.get_width();
  height = allocation.get_height();
  draw_title(cr);
  int bottom_offset = draw_legend(cr);
  draw_graph_grid(cr, bottom_offset);
  return true;
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

void Graph::draw_title(const Cairo::RefPtr<Cairo::Context> &cr) {
  const int left_offset = 10;
  const int top_offset = 10;
  const double fifty_shades_of_grey = 0.4;
  cr->move_to(left_offset, top_offset);
  cr->set_source_rgb(fifty_shades_of_grey, fifty_shades_of_grey,
                     fifty_shades_of_grey);

  Pango::FontDescription font;
  font.set_weight(Pango::WEIGHT_BOLD);
  auto layout = create_pango_layout(Device::sensor_types[type]);

  layout->set_font_description(font);
  layout->show_in_cairo_context(cr);
}

int Graph::draw_legend(const Cairo::RefPtr<Cairo::Context> &cr) {
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
  int legend_offset = line_spacing * num_lines + bottom_offset;
  return legend_offset;
}

const vector<unsigned int>
Graph::draw_graph_grid(const Cairo::RefPtr<Cairo::Context> &cr,
                       int legend_offset) {
  // Draw outer rectangle:
  const int left_offset = 0;
  int bottom_offset = 20;
  unsigned int rectangle_height = height - 50;
  unsigned int rectangle_width = width - 100;
  unsigned int x_coord = (width - rectangle_width) / 2 + left_offset;
  unsigned int y_coord = (height - rectangle_height) / 2 + bottom_offset;
  const double fifty_shades_of_grey = 0.9;
  cr->set_source_rgb(fifty_shades_of_grey, fifty_shades_of_grey,
                     fifty_shades_of_grey);
  cr->rectangle(x_coord, y_coord, rectangle_width,
                rectangle_height - legend_offset);
  cr->fill();

  // Draw inner rectangle:
  const unsigned int inner_offset = 2;
  rectangle_height -= inner_offset;
  rectangle_width -= inner_offset;
  x_coord = (width - rectangle_width) / 2 + left_offset;
  y_coord = (height - rectangle_height) / 2 + bottom_offset;
  cr->set_source_rgb(1, 1, 1);
  cr->rectangle(x_coord, y_coord, rectangle_width,
                rectangle_height - legend_offset);
  cr->fill();

  // Draw lines:
  cr->set_line_width(1);
  cr->set_source_rgb(fifty_shades_of_grey, fifty_shades_of_grey,
                     fifty_shades_of_grey);
  // Vertical:
  const unsigned int max_line_count = 10;
  const unsigned int min_line_spacing = 40;
  unsigned int line_count =
      ((rectangle_height) / min_line_spacing > max_line_count)
          ? max_line_count
          : rectangle_height / min_line_spacing;
  unsigned int line_spacing = (rectangle_height - legend_offset) / line_count;
  for (unsigned int line_index = 1; line_index < line_count; line_index++) {
    cr->move_to(x_coord, y_coord + line_spacing * line_index);
    cr->line_to(x_coord + rectangle_width, y_coord + line_spacing * line_index);
  }
  // Horizontal:
  const unsigned int horizontal_line_count = 5;
  line_spacing = rectangle_width / horizontal_line_count;
  for (unsigned int line_index = 1; line_index < horizontal_line_count;
       line_index++) {
    cr->move_to(x_coord + line_spacing * line_index, y_coord);
    cr->line_to(x_coord + line_spacing * line_index,
                y_coord + rectangle_height - legend_offset);
  }
  cr->stroke();
  return {x_coord, y_coord, rectangle_width, rectangle_height - legend_offset};
}

void Graph::make_plot(vector<unsigned int> &rectangle_points,
                      int sensor_index) {
  unsigned int y_axis_pixels = rectangle_points[3] - rectangle_points[1];
  unsigned int x_axis_pixels = rectangle_points[2] - rectangle_points[0];
}
