/**************************************************************************/
/*  Copyright 2009 Tim Day                                                */
/*                                                                        */
/*  This file is part of Fracplanet                                       */
/*                                                                        */
/*  Fracplanet is free software: you can redistribute it and/or modify    */
/*  it under the terms of the GNU General Public License as published by  */
/*  the Free Software Foundation, either version 3 of the License, or     */
/*  (at your option) any later version.                                   */
/*                                                                        */
/*  Fracplanet is distributed in the hope that it will be useful,         */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU General Public License for more details.                          */
/*                                                                        */
/*  You should have received a copy of the GNU General Public License     */
/*  along with Fracplanet.  If not, see <http://www.gnu.org/licenses/>.   */
/**************************************************************************/

/*! \file
  \brief Interface (and implementation) for templated Image class.
*/

#ifndef _image_h_
#define _image_h_

#include "common.h"
#include "rgb.h"

class Progress;

//! Class to support specialisation for particular pixel formats
template <typename T> class PixelTraits
{
 public:

  typedef T ComputeType;
  typedef T ScalarType;
  static ScalarType scalar(const T& v) {return v;}
};

template<> class PixelTraits<uchar>
{
 public:

  typedef float ComputeType;
  typedef uchar ScalarType;
  static ScalarType scalar(const uchar& v) {return v;}
};

template<> class PixelTraits<ushort>
{
 public:

  typedef float ComputeType;
  typedef ushort ScalarType;
  static ScalarType scalar(const ushort& v) {return v;}
};

template<> class PixelTraits<ByteRGBA>
{
 public:

  typedef FloatRGBA ComputeType;
  typedef float ScalarType;
  static ScalarType scalar(const ByteRGBA& v) {return (static_cast<float>(v.r)+static_cast<float>(v.g)+static_cast<float>(v.b))/3.0f;}
};

//! Class for 2D raster images of a specified type.
/*! Assumes explicit instantiation.
  NB The base type just refers to storage allocated elsewhere.
  Used to be gratuitously implemented using boost multiarray type,
  but that was too inefficient
  This is still pretty bad byt gcc 4.1 seems to do a very nice job with -O2.
 */
template <typename T> class Raster
{
 public:

  typedef typename PixelTraits<T>::ComputeType ComputeType;
  typedef typename PixelTraits<T>::ScalarType ScalarType;

  static const ScalarType scalar(const T& v) {return PixelTraits<T>::scalar(v);}

  Raster(uint w,uint h,uint p,T* d)
    :_width(w)
    ,_height(h) 
    ,_pitch(p)
    ,_data(d)
    ,_row_end(row_range(h),p)
    ,_const_row_end(row_range(h),p)
    {}

  virtual ~Raster()
    {}

  uint width() const
    {
      return _width;
    }

  uint height() const
    {
      return _height;
    }

  uint pitch() const
    {
      return _pitch;
    }

  bool contiguous() const
    {
      return (_width==_pitch);
    }

  uint contiguous_size() const
    {
      assert(contiguous());
      return _width*_height;
    }

  T* contiguous_begin()
    {
      assert(contiguous());
      return _data;
    }

  const T* contiguous_begin() const
    {
      assert(contiguous());
      return _data;
    }

  T* contiguous_end()
    {
      assert(contiguous());
      return _data+contiguous_size();
    }

  const T* contiguous_end() const
    {
      assert(contiguous());
      return _data+contiguous_size();
    }

  T* row(uint r)
    {
      return _data+r*_pitch;
    }

  const T* row(uint r) const
    {
      return _data+r*_pitch;
    }

  boost::iterator_range<T*> row_range(uint r)
    {
      T*const it=row(r);
      return boost::iterator_range<T*>(it,it+_width);
    }

  boost::iterator_range<const T*> row_range(uint r) const
    {
      const T*const it=row(r);
      return boost::iterator_range<const T*>(it,it+_width);
    }

  class RowIterator : public std::iterator<std::forward_iterator_tag, boost::iterator_range<T*> >
    {
    public:

      RowIterator(const boost::iterator_range<T*>& row, uint p)
	:_row(row)
	,_pitch(p)
	{}

      ~RowIterator()
	{}

      RowIterator& operator=(const RowIterator& it)
	{
	  _row=it._row;
	  assert(_pitch==it._pitch);
	  return (*this);
	}

      bool operator==(const RowIterator& it) const
	{
	  return _row.begin()==it._row.begin();
	}

      bool operator!=(const RowIterator& it) const
	{
	  return _row.begin()!=it._row.begin();
	}

      RowIterator& operator++()
	{
	  _row=boost::iterator_range<T*>
	    (
	     _row.begin()+_pitch,
	     _row.end()+_pitch
	     );
	  return (*this);
	}

      RowIterator operator++(int)
	{
	  RowIterator tmp(*this);
	  _row=boost::iterator_range<T*>
	    (
	     _row.begin()+_pitch,
	     _row.end()+_pitch
	     );
	  return tmp;
	}

      boost::iterator_range<T*>& operator*()
	{
	  return _row;
	}

      boost::iterator_range<T*>* operator->()
	{
	  return &_row;
	}
      
    private:

      boost::iterator_range<T*> _row;

      const uint _pitch;
    };

  RowIterator row_begin()
    {
      return RowIterator(row_range(0),_pitch);
    }

  RowIterator row_end()
    {
      return _row_end;
    }
  
  class ConstRowIterator : public std::iterator<std::forward_iterator_tag, boost::iterator_range<const T*> >
    {
    public:

      ConstRowIterator(const boost::iterator_range<const T*>& row, uint p)
	:_row(row)
	,_pitch(p)
	{}

      ~ConstRowIterator()
	{}

      ConstRowIterator& operator=(const ConstRowIterator& it)
	{
	  _row=it._row;
	  assert(_pitch==it._pitch);
	  return (*this);
	}

      bool operator==(const ConstRowIterator& it) const
	{
	  return _row.begin()==it._row.begin();
	}

      bool operator!=(const ConstRowIterator& it) const
	{
	  return _row.begin()!=it._row.begin();
	}

      ConstRowIterator& operator++()
	{
	  _row=boost::iterator_range<const T*>
	    (
	     _row.begin()+_pitch,
	     _row.end()+_pitch
	     );
	  return (*this);
	}

      ConstRowIterator operator++(int)
	{
	  ConstRowIterator tmp(*this);
	  _row=boost::iterator_range<const T*>
	    (
	     _row.begin()+_pitch,
	     _row.end()+_pitch
	     );
	  return tmp;
	}

      boost::iterator_range<const T*>& operator*()
	{
	  return _row;
	}

      boost::iterator_range<const T*>* operator->()
	{
	  return &_row;
	}
      
    private:

      boost::iterator_range<const T*> _row;

      const uint _pitch;
    };

  ConstRowIterator row_begin() const
    {
      return ConstRowIterator(row_range(0),_pitch);
    }

  ConstRowIterator row_end() const
    {
      return _const_row_end;
    }
  
  //! Clear the image to a constant value.
  void fill(const T& v);

  const ScalarType maximum_scalar_pixel_value() const;

  //! Fill a line segment on the given half-open range [x0,x1), interpolating between the two given values.
  void scan(uint y,float x0,const ComputeType& v0,float x1,const ComputeType& v1);
  
  //! Variant scan, interpolates between two values then process them through function before 
  template <typename V> void scan(uint y,float x0,const V& v0,float x1,const V& v1,const boost::function<ComputeType (const V&)>& fn);

  bool write_ppmfile(const std::string&,Progress*) const;
  bool write_pgmfile(const std::string&,Progress*) const;

 private:

  const uint _width;

  const uint _height;

  const uint _pitch;

  T*const _data;

  const RowIterator _row_end;

  const ConstRowIterator _const_row_end;
};

/*! Scan x0 to x1 into image.
  Pixel centres are at 0.5 , so x0=0.75 goes to pixel 1.
  Rightmost pixel is at width()-0.5.
*/
template <typename T> inline void Raster<T>::scan(uint y,float x0,const ComputeType& v0,float x1,const ComputeType& v1)
{
  assert(x0<=x1);
  
  if (x1<0.5f || width()-0.5f<x0) return;  // Early out for spans off edges
  
  const int ix_min=static_cast<int>(std::max(0.0f        ,ceilf(x0-0.5f)));
  const int ix_max=static_cast<int>(std::min(width()-0.5f,floorf(x1-0.5f)));
  
  const ComputeType kv((v1-v0)/(x1-x0));
  
  T*const row_ptr=row(y);
  for (int ix=ix_min;ix<=ix_max;ix++)
    {
      const ComputeType v(v0+kv*(ix+0.5f-x0));
      row_ptr[ix]=static_cast<T>(v);
    }
}

template <typename T> template <typename V> inline void Raster<T>::scan(uint y,float x0,const V& v0,float x1,const V& v1,const boost::function<ComputeType (const V&)>& fn)
{
  assert(x0<=x1);
  
  if (x1<0.5f || width()-0.5f<x0) return;  // Early out for spans off edges
  
  const int ix_min=static_cast<int>(std::max(0.0f        ,ceilf(x0-0.5f)));
  const int ix_max=static_cast<int>(std::min(width()-0.5f,floorf(x1-0.5f)));
  
  const V kv((v1-v0)/(x1-x0));
  
  T*const row_ptr=row(y);
  for (int ix=ix_min;ix<=ix_max;ix++)
    {
      const ComputeType v(fn(v0+kv*(ix+0.5f-x0)));
      row_ptr[ix]=static_cast<T>(v);
    }
}

template <typename T> class ImageStorage
{
 public:
  ImageStorage(int n)
    :_storage(new T[n])
    {}
  ~ImageStorage()
    {}
 protected:
  boost::scoped_array<T> _storage;
};

template <typename T> class Image : private ImageStorage<T>, public Raster<T>, public boost::noncopyable
{
 public:
  Image(uint w,uint h)
    :ImageStorage<T>(w*h)
    ,Raster<T>(w,h,w,ImageStorage<T>::_storage.get())
    {}
  ~Image()
    {}  
};

#endif
