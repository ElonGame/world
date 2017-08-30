#include "Environment2D.h"

Environment2D::Environment2D() {

}

Environment2D::~Environment2D() {

}

void Environment2D::setMap(Map * map) {
	_map = std::unique_ptr<Map>(map);
}

void Environment2D::setGround(Ground * ground) {
	_ground = std::unique_ptr<Ground>(ground);
}