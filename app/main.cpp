#include "reentry/simulation.hpp"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void usage(const char* program) {
    std::cout << "Usage: " << program << " [options]\n"
              << "  --altitude M       Initial altitude (default 120000)\n"
              << "  --velocity M/S     Initial velocity (default 7500)\n"
              << "  --angle DEG        Flight-path angle (default -6)\n"
              << "  --mass KG          Vehicle mass (default 2000)\n"
              << "  --area M2          Reference area (default 3)\n"
              << "  --nose-radius M    Nose radius (default 0.5)\n"
              << "  --dt S             RK4 time step (default 0.05)\n"
              << "  --duration S       Maximum duration (default 2000)\n"
              << "  --output FILE      CSV destination; '-' writes stdout\n";
}

double number(const char* text, const std::string& option) {
    char* end = nullptr;
    const double value = std::strtod(text, &end);
    if (end == text || *end != '\0') throw std::invalid_argument("invalid value for " + option);
    return value;
}
}  // namespace

int main(int argc, char** argv) {
    reentry::Vehicle vehicle;
    reentry::InitialConditions initial;
    reentry::SimulationOptions options;
    std::string output_path = "reentry.csv";
    try {
        for (int i = 1; i < argc; ++i) {
            const std::string option = argv[i];
            if (option == "--help" || option == "-h") { usage(argv[0]); return 0; }
            if (i + 1 >= argc) throw std::invalid_argument("missing value for " + option);
            const char* value = argv[++i];
            if (option == "--altitude") initial.altitude_m = number(value, option);
            else if (option == "--velocity") initial.velocity_m_s = number(value, option);
            else if (option == "--angle") initial.flight_path_angle_deg = number(value, option);
            else if (option == "--mass") vehicle.mass_kg = number(value, option);
            else if (option == "--area") vehicle.reference_area_m2 = number(value, option);
            else if (option == "--nose-radius") vehicle.nose_radius_m = number(value, option);
            else if (option == "--dt") options.time_step_s = number(value, option);
            else if (option == "--duration") options.maximum_time_s = number(value, option);
            else if (option == "--output") output_path = value;
            else throw std::invalid_argument("unknown option: " + option);
        }

        const auto samples = reentry::Simulator{}.run(vehicle, initial, options);
        std::ofstream file;
        std::ostream* output = &std::cout;
        if (output_path != "-") {
            file.open(output_path);
            if (!file) throw std::runtime_error("could not open output file: " + output_path);
            output = &file;
        }
        *output << "time_s,altitude_m,velocity_m_s,flight_path_angle_deg,downrange_m,mach,"
                   "density_kg_m3,drag_coefficient,drag_acceleration_m_s2,heat_flux_w_m2\n";
        *output << std::setprecision(10);
        for (const auto& s : samples) {
            *output << s.time_s << ',' << s.altitude_m << ',' << s.velocity_m_s << ','
                    << s.flight_path_angle_deg << ',' << s.downrange_m << ',' << s.mach << ','
                    << s.density_kg_m3 << ',' << s.drag_coefficient << ','
                    << s.drag_acceleration_m_s2 << ',' << s.heat_flux_w_m2 << '\n';
        }
        std::cerr << "Wrote " << samples.size() << " trajectory samples to " << output_path << '\n';
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        usage(argv[0]);
        return 1;
    }
}
