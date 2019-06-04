#pragma once

#include "world/core/WorldConfig.h"

#include <string>

#include "Color.h"

namespace world {

class WORLDAPI_EXPORT Material {
public:
    Material();

    Material(const std::string &name);

    virtual ~Material();

    void setName(const std::string &name);

    std::string getName() const { return _name; }

    void setKd(double r, double g, double b);

    Color4d getKd() const { return _Kd; }

    void setKa(double r, double g, double b);

    Color4d getKa() const { return _Ka; }

    void setKs(double r, double g, double b);

    Color4d getKs() const { return _Ks; }

    void setMapKd(const std::string &texName);

    std::string getMapKd() const { return _mapKd; }

private:
    std::string _name;

    Color4d _Kd;
    Color4d _Ka;
    Color4d _Ks;
    std::string _mapKd;
};
} // namespace world