
#include "spatial.hpp"


class RectangularDomain {
    Point2D pt2d;  // generation point defined by x and y value
    double rot_alpha_degr;  // rotation angle Alpha, in degrees - with respect to x-axis
    double l_size, m_size;  // l, m domain sizes
public:
    RectangularDomain();
    RectangularDomain(const Point2D&, const double&, const double&, const double&);
    ~RectangularDomain();

};


class RectRegularGrid {
    // ? planar CRS - starting with undefined CRS
    RectangularDomain domain; // rectangular domain
    unsigned int ncols, nrows;  // cel size/number, along l and m directions
    //array // 2D array
public:
    RectRegularGrid();
    RectRegularGrid(const RectangularDomain&, const unsigned int&, const unsigned int&);
    ~RectRegularGrid();
};


class DataRRGrid {

    RectRegularGrid _rrgrid;
    std::vector<double> _data;
    double _nodata_value;

public:
    DataRRGrid();
    DataRRGrid(const RectRegularGrid&, const std::vector<double>, const double);
    ~DataRRGrid();
    RectRegularGrid rrgrid();
    std::vector<double> data();
    double nodata_value();

};


class MeshTriangleStrip {

    std::vector<Point3D> _pts;
    std::vector<std::vector<unsigned int> > _trianglestrips;

public:
    MeshTriangleStrip();
    MeshTriangleStrip(const std::vector<Point3D>&, const std::vector<std::vector<unsigned int> >& );
    ~MeshTriangleStrip();

    std::vector<Point3D> pts();
    std::vector<std::vector<unsigned int> > trianglestrips();

};

