#ifndef vil_dicom_stream_h_
#define vil_dicom_stream_h_
//:
// \file
// \author Amitha Perera

#include <dcmtk/dcmdata/dcistrma.h>
#include <dcmtk/dcmimgle/didocu.h>

class vil_stream;

class vil_dicom_stream_producer
  : public DcmProducer
{
 public:
  vil_dicom_stream_producer( vil_stream* vs );

  virtual ~vil_dicom_stream_producer();

  virtual OFBool good() const;
  virtual OFCondition status() const;
  virtual OFBool eos();
  virtual offile_off_t avail();
  virtual offile_off_t read(void *buf, offile_off_t buflen);
  virtual offile_off_t skip(offile_off_t skiplen);
  virtual void putback(offile_off_t num);

 private:
  vil_stream* vs_;
};


class vil_dicom_stream_factory
  : public DcmInputStreamFactory
{
 public:
  vil_dicom_stream_factory( vil_stream* vs );

  virtual ~vil_dicom_stream_factory();

  virtual DcmInputStream* create() const;

  virtual DcmInputStreamFactory* clone() const
  {
    return new vil_dicom_stream_factory(*this);
  }
  
  virtual DcmInputStreamFactoryType ident() const
  {
      return DFT_DcmInputFileStreamFactory;
  }

 private:
  vil_stream* vs_;
};

class vil_dicom_stream_input
  : public DcmInputStream
{
 public:
  vil_dicom_stream_input( vil_stream* vs );
  virtual ~vil_dicom_stream_input();
  virtual DcmInputStreamFactory* newFactory() const;
};

#endif // vil_dicom_stream_h_
