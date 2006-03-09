// This is gel/mrc/vpgl/vpgl_calibration_matrix.txx
#ifndef vpgl_calibration_matrix_txx_
#define vpgl_calibration_matrix_txx_
//:
// \file

//#include <vcl_iostream.h>

#include "vpgl_calibration_matrix.h"

#include <vgl/io/vgl_io_point_2d.h>
#include <vgl/xio/vgl_xio_point_2d.h>
#include <vsl/vsl_basic_xml_element.h>

//--------------------------------------
template <class T>
vpgl_calibration_matrix<T>::vpgl_calibration_matrix() :
  focal_length_( (T)1 ),
  principal_point_( vgl_point_2d<T>( (T)0, (T)0 ) ),
  x_scale_( (T)1 ),
  y_scale_( (T)1 ),
  skew_( (T)0 )
{
}


//--------------------------------------
template <class T>
vpgl_calibration_matrix<T>::vpgl_calibration_matrix( T focal_length, 
  const vgl_point_2d<T>& principal_point, T x_scale, T y_scale, T skew ) :
  focal_length_( focal_length ),
  principal_point_( principal_point ),
  x_scale_( x_scale ),
  y_scale_( y_scale ),
  skew_( skew )
{
  // Make sure the inputs are valid.
  assert( focal_length != 0 );
  assert( x_scale > 0 );
  assert( y_scale > 0 );
}


//--------------------------------------
template <class T>
vpgl_calibration_matrix<T>::vpgl_calibration_matrix( const vnl_matrix_fixed<T,3,3>& K )
{
  // Put the supplied matrix into canonical form and check that it could be a 
  // calibration matrix.
  assert( K(2,2) != (T)0 && K(1,0) == (T)0 && K(2,0) == (T)0 && K(2,1) == (T)0 );
  double scale_factor = (T)1;
  if( K(2,2) != (T)1 ) scale_factor = ((T)1)/K(2,2);
  
  focal_length_ = (T)1; 
  x_scale_ = scale_factor*K(0,0);
  y_scale_ = scale_factor*K(1,1);
  skew_ = scale_factor*K(0,1);
  principal_point_.set( scale_factor*K(0,2), scale_factor*K(1,2) );
  
  assert( ( x_scale_ > 0 && y_scale_ > 0 ) || ( x_scale_ < 0 && y_scale_ < 0 ) );
}


//--------------------------------------
template <class T>
vnl_matrix_fixed<T,3,3> vpgl_calibration_matrix<T>::get_matrix() const
{
  // Construct the matrix as in H&Z.
  vnl_matrix_fixed<T,3,3> K( (T)0 );
  K(0,0) = focal_length_*x_scale_;
  K(1,1) = focal_length_*y_scale_;
  K(2,2) = (T)1;
  K(0,2) = principal_point_.x();
  K(1,2) = principal_point_.y();
  K(0,1) = skew_;
  return K;
}
  
  
//--------------------------------------
template <class T>
void vpgl_calibration_matrix<T>::set_focal_length( T new_focal_length )
{
  assert( new_focal_length != (T)0 );
  focal_length_ = new_focal_length;
}


//--------------------------------------
template <class T>
void vpgl_calibration_matrix<T>::set_principal_point( 
  const vgl_point_2d<T>& new_principal_point )
{
  assert( !new_principal_point.ideal() );
  principal_point_ = new_principal_point;
}


//--------------------------------------
template <class T>
void vpgl_calibration_matrix<T>::set_x_scale( T new_x_scale )
{
  assert( new_x_scale > 0 );
  x_scale_ = new_x_scale;
}


//--------------------------------------
template <class T>
void vpgl_calibration_matrix<T>::set_y_scale( T new_y_scale )
{
  assert( new_y_scale > 0 );
  y_scale_ = new_y_scale;
}


//--------------------------------------
template <class T>
void vpgl_calibration_matrix<T>::set_skew( T new_skew )
{
  skew_ = new_skew;
}

//: Equality test
template <class T> bool vpgl_calibration_matrix<T>::
operator==(vpgl_calibration_matrix<T> const &that) const
{
  if (this == &that) // same object => equal.
    return true;

  return 
    this->focal_length_ == that.focal_length_ &&
    this->principal_point_ == that.principal_point_ &&
    this->x_scale_ == that.x_scale_ && this->y_scale_ == that.y_scale_ && 
    this->skew_ == that.skew_;
}

// I/O :------------------------------------------------

//-------------------------------
template <class T> void vpgl_calibration_matrix<T>::
b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
     vsl_b_read(is, focal_length_);
     vsl_b_read(is, principal_point_);
     vsl_b_read(is, x_scale_);
     vsl_b_read(is, y_scale_);
     vsl_b_read(is, skew_);
    break;
   default:
    vcl_cerr << "I/O ERROR: vpgl_calibration_matrix::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//-------------------------------
//: Binary save self to stream.
// \remark cached_svd_ not written
template <class T> void vpgl_calibration_matrix<T>::
b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, this->version());
  vsl_b_write(os, focal_length_);
  vsl_b_write(os, principal_point_);
  vsl_b_write(os, x_scale_);
  vsl_b_write(os, y_scale_);
  vsl_b_write(os, skew_);
}

//: Binary save
template <class T> void
vsl_b_write(vsl_b_ostream &os, const vpgl_calibration_matrix<T> * p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}


//: Binary load
template <class T> void
vsl_b_read(vsl_b_istream &is, vpgl_calibration_matrix<T>* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new vpgl_calibration_matrix<T>();
    p->b_read(is);
  }
  else
    p = 0;
}

//: xml write
template <class T>
void x_write(vcl_ostream &os, vpgl_calibration_matrix<T> p)
{
  vsl_basic_xml_element element("vpgl_calibration_matrix");
  element.add_attribute("focal_length", (T) p.focal_length());
  element.add_attribute("x_scale", p.x_scale());
  element.add_attribute("y_scale", p.y_scale());
  element.add_attribute("skew", p.skew());
  element.x_write_open(os);
  vgl_point_2d<T> point = p.principal_point();
  x_write(os, point, "principal_point");
  element.x_write_close(os);
}

// Code for easy instantiation.
#undef vpgl_CALIBRATION_MATRIX_INSTANTIATE
#define vpgl_CALIBRATION_MATRIX_INSTANTIATE(T) \
template class vpgl_calibration_matrix<T>; \
template void x_write(vcl_ostream &, vpgl_calibration_matrix<T>)

#endif // vpgl_calibration_matrix_txx_

