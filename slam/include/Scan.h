#ifndef SLAM_PROJECT_SCAN_H
#define SLAM_PROJECT_SCAN_H

#define INTENSITIES_SIZE 399

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <ostream>

using namespace boost::property_tree;


class Beam {
private:
    uint8_t *intensities;
    double time;
    double angle;

public:
    ~Beam() {
        delete [] intensities;
        intensities = nullptr;
    }

    Beam(uint8_t *intensities, double time, double angle) : intensities(intensities), time(time), angle(angle) {}

    [[nodiscard]] uint8_t* getIntensities() const { return intensities; }
    [[nodiscard]] double getTime() const { return time; }
    [[nodiscard]] double getAngle() const { return angle; }

    friend std::ostream &operator<<(std::ostream &os, const Beam &beam);
    static Beam* importJson(const ptree &p);
};

class Scan {
private:
    double step_dist;
    std::vector<const Beam*> beams;

public:
    ~Scan() {
        for (const Beam* beam: beams)
            delete beam;
    }

    explicit Scan(double step_dist) : step_dist(step_dist), beams() {}
    [[nodiscard]] std::vector<const Beam*> getBeams() const { return beams; }
    [[nodiscard]] double getStepDist() const { return step_dist; }

    static Scan* importJson(std::istream &stream);
    void addBeam(const Beam *beam) { beams.push_back(beam); }
    friend std::ostream &operator<<(std::ostream &os, const Scan &scan);
};



#endif //SLAM_PROJECT_SCAN_H
