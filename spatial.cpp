
#include "spatial.hpp"


Point2D::Point2D() {
}


Point2D::Point2D(double x,  double y) {
    _x = x;
    _y = y;
};


Point2D::~Point2D() {
}


double Point2D::x() {
    return _x;
};


double Point2D::y() {
    return _y;
};


Point2D Point2D::operator=(Point2D p2) {
    return Point2D(p2.x(), p2.y());
};

double Point2D::distance(Point2D another) {
    double dx = x() - another.x();
    double dy = y() - another.y();

    return sqrt( dx*dx + dy*dy);

};


Point3D::Point3D(double x,  double y, double z) {
    _x = x;
    _y = y;
    _z = z;
};


Point3D::~Point3D() {
}


double Point3D::x() {
    return _x;
};


double Point3D::y() {
    return _y;
};


double Point3D::z() {
    return _z;
};


double Point3D::distance(Point3D another) {
    double dx = x() - another.x();
    double dy = y() - another.y();
    double dz = z() - another.z();

    return sqrt( dx*dx + dy*dy + dz*dz);

};


bool Point3D::is_coincident(Point3D another) {

    if (distance(another) > 1e-12) {
        return false; }
    else {
        return true;
    }

};


Segment3D::Segment3D(Point3D start_pt, Point3D end_pt) :
    _start_pt(start_pt), _end_pt(end_pt) {

};


Segment3D::~Segment3D() {
};


Point3D Segment3D::start_pt(){
    return _start_pt;
};


Point3D Segment3D::end_pt(){
    return _end_pt;
};


double Segment3D::dx() {
    return _end_pt.x() - _start_pt.x();
};


double Segment3D::dy() {
    return _end_pt.y() - _start_pt.y();
};


double Segment3D::dz() {
    return _end_pt.z() - _start_pt.z();
};


Vector3D Segment3D::as_vector() {

    return Vector3D(dx(), dy(), dz() );
};


double Segment3D::length() {

    double d_x = dx();
    double d_y = dy();
    double d_z = dz();

    return sqrt( d_x*d_x + d_y*d_y + d_z*d_z );

};


bool Segment3D::is_point_projection_in_segment(Point3D pt_3d) {

    Vector3D pt_vector = Segment3D( start_pt(), pt_3d ).as_vector();
    double scal_prod = as_vector().scalar_prod( pt_vector );
    double segm_length = length();
    if (0 <= scal_prod and scal_prod <= segm_length*segm_length) {
        return true; }
    else {
        return false; };

};


Line3D Segment3D::as_line() {
    Point3D start_pt3d = start_pt();
    Vector3D line_vect = as_vector();
    return Line3D(start_pt3d, line_vect);
};



Vector3D::Vector3D(double x, double y, double z) :
    _x(x), _y(y), _z(z) {
    };


Vector3D::Vector3D( Point3D pt3d_a, Point3D pt3d_b) {

    _x = pt3d_b.x() - pt3d_a.x();
    _y = pt3d_b.y() - pt3d_a.y();
    _z = pt3d_b.z() - pt3d_a.z();

};


Vector3D::~Vector3D() {
};


double Vector3D::x() {
    return _x;
};


double Vector3D::y() {
    return _y;
};


double Vector3D::z() {
    return _z;
};


double Vector3D::length() {
    return sqrt(_x*_x + _y*_y + _z*_z);
};


Vector3D Vector3D::scale(double scale_factor) {

    return Vector3D(_x*scale_factor, _y*scale_factor, _z*scale_factor);
};


Vector3D Vector3D::versor() {
    return scale( 1.0 / length() );
};


double Vector3D::scalar_prod(Vector3D another) {
    return _x*another._x + _y*another._y + _z*another._z;
};


Vector3D Vector3D::vector_prod(Vector3D another) {

    double x = _y * another.z() - _z * another.y();
    double y = _z * another.x() - _x * another.z();
    double z = _x * another.y() - _y * another.x();

    return Vector3D(x, y, z);
};


bool Vector3D::is_colinear(Vector3D another) {
    Vector3D vect_prod = vector_prod(another);
    double norm = vect_prod.length();
    if (norm < 1e-12) {
        return true; }
    else {
        return false; }
};


Point3D Vector3D::move_pt(Point3D pt3d) {

    double x0 = pt3d.x();
    double y0 = pt3d.y();
    double z0 = pt3d.z();

    double dx = _x;
    double dy = _y;
    double dz = _z;

    return Point3D(x0+dx, y0+dy, z0+dz);
};


Line3D::Line3D(Point3D orig_pt3d, Vector3D vers) :
    _orig_pt3d(orig_pt3d), _vers(vers) {

    };


Line3D::~Line3D() {
};


Point3D Line3D::intersect_coplanar(Line3D another) {

    Point3D line_a_start_pt = _orig_pt3d;
    Point3D line_b_start_pt = another._orig_pt3d;

    Vector3D vers_a = _vers;
    Vector3D vers_b = another._vers;

    double delta_distance = 100.0;
    Vector3D displ_vector = vers_b.scale(delta_distance);

    Point3D first_pt = Point3D(line_a_start_pt.x(), line_a_start_pt.y(), line_a_start_pt.z());
    Point3D second_pt = Point3D(line_b_start_pt.x(), line_b_start_pt.y(), line_b_start_pt.z());

    if (second_pt.distance(first_pt) < delta_distance/2.0) {
        second_pt = displ_vector.move_pt(second_pt);
    };

    Point3D third_pt = displ_vector.move_pt(second_pt);
    while(third_pt.distance(first_pt) < delta_distance/2.0) {
       third_pt = displ_vector.move_pt(third_pt);
    };

    CartesianPlane colinear_plane = CartesianPlane(first_pt, second_pt, third_pt);

    //code inspired to: http://geomalgorithms.com/a05-_intersect-1.html#intersect2D_2Segments()
    Vector3D w_vect = Vector3D( line_a_start_pt, line_b_start_pt );
    Vector3D vers_a_perp = colinear_plane.perp_versor_in_plane(vers_a);

    double factor_numerator = - vers_a_perp.scalar_prod(w_vect);
    double factor_denominator = vers_a_perp.scalar_prod(vers_b);

    double factor_scaling = factor_numerator / factor_denominator;

    Point3D intersection_pt3d = vers_b.scale(factor_scaling).move_pt(line_b_start_pt);

    return intersection_pt3d;

};


CartesianPlane::CartesianPlane(Point3D pt_a, Point3D pt_b, Point3D pt_c) {

    Matrix3 matr_a = Matrix3(pt_a.y(), pt_a.z(), 1,
                             pt_b.y(), pt_b.z(), 1,
                             pt_c.y(), pt_c.z(), 1);

    Matrix3 matr_b = Matrix3(pt_a.x(), pt_a.z(), 1,
                             pt_b.x(), pt_b.z(), 1,
                             pt_c.x(), pt_c.z(), 1);

    Matrix3 matr_c = Matrix3(pt_a.x(), pt_a.y(), 1,
                             pt_b.x(), pt_b.y(), 1,
                             pt_c.x(), pt_c.y(), 1);

    Matrix3 matr_d = Matrix3(pt_a.x(), pt_a.y(), pt_a.z(),
                             pt_b.x(), pt_b.y(), pt_b.z(),
                             pt_c.x(), pt_c.y(), pt_c.z());

    _a = matr_a.determinant();
    _b = matr_b.determinant();
    _c = matr_c.determinant();
    _d = matr_d.determinant();

};


CartesianPlane::CartesianPlane(double a, double b, double c, double d) :
    _a(a), _b(b), _c(c), _d(d) {
};


CartesianPlane::~CartesianPlane() {
};


Vector3D CartesianPlane::normal_versor() {

    return Vector3D( _a, _b, _c ).versor();

};


Vector3D CartesianPlane::perp_versor_in_plane(Vector3D inplane_vect3d) {

    return inplane_vect3d.vector_prod( normal_versor() ).versor();
};

Vector3D CartesianPlane::intersect_versor(CartesianPlane another) {

    // TODO: consider the special case of two parallel planes
    Vector3D vers_a = normal_versor();
    Vector3D vers_b = another.normal_versor();

    return vers_a.vector_prod(vers_b).versor();

};


Point3D CartesianPlane::intersect_point3d(CartesianPlane another) {

    Matrix2 matrix_1 = Matrix2(_a, _b, another._a, another._b);
    Matrix2 matrix_2 = Matrix2(_a, _c, another._a, another._c);
    Matrix2 matrix_3 = Matrix2(_b, _c, another._b, another._c);

    double matrix_1_det = matrix_1.determinant();
    double matrix_2_det = matrix_2.determinant();
    double matrix_3_det = matrix_3.determinant();

    // TODO: check formula correctness for case 2 and 3
    double x = 0.0, y = 0.0, z = 0.0;
    if ( fabs( matrix_1_det ) > 1e-12 ) {
        Matrix2 matrix_1b = Matrix2(_d, _b, another._d, another._b);
        Matrix2 matrix_1c = Matrix2(_a, _d, another._a, another._d);
        z = 0.0;
        x = matrix_1b.determinant() / matrix_1_det;
        y = matrix_1c.determinant() / matrix_1_det;
    }
    else if ( fabs( matrix_2_det ) > 1e-12 ) {
        Matrix2 matrix_2b = Matrix2(_d, _c, another._d, another._c);
        Matrix2 matrix_2c = Matrix2(_a, _d, another._a, another._d);
        y = 0.0;
        x = matrix_2b.determinant() / matrix_2_det;
        z = matrix_2c.determinant() / matrix_2_det;
    }
    else if ( fabs( matrix_3_det ) > 1e-12 ) {
        Matrix2 matrix_3b = Matrix2(_d, _c, another._d, another._c);
        Matrix2 matrix_3c = Matrix2(_b, _d, another._b, another._d);
        x = 0.0;
        y = matrix_3b.determinant() / matrix_3_det;
        z = matrix_3c.determinant() / matrix_3_det;
    }

    return Point3D(x, y, z);
};


Line3D CartesianPlane::intersect(CartesianPlane another) {

    // TODO: consider the special case of two parallel planes
    Vector3D vers = intersect_versor(another);
    Point3D pt3d = intersect_point3d(another);

    return Line3D( pt3d, vers);

};





Triangle3D::Triangle3D(Point3D pt_a, Point3D pt_b, Point3D pt_c) :
    _pt_a(pt_a), _pt_b(pt_b), _pt_c(pt_c) {

};


Triangle3D::~Triangle3D() {
};


CartesianPlane Triangle3D::to_cartes_plane() {
    return CartesianPlane(_pt_a, _pt_b, _pt_c);
};





