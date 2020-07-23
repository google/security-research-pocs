/* This code is mostly copy-paste from two sources:
 * tiff/libtiff/tools/tiffinfo.c (most of the code)
 * libwebp/v0_2/examples/tiffdec.c (in-memory
 * parsing) A few other parts have been changed/added to make the code fit
 * together.
 */

#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "tiff/libtiff/tif_config.h"
#include "tiff/libtiff/tiff.h"
#include "tiff/libtiff/tiffio.h"
#include "tiff/libtiff/tiffiop.h"

constexpr size_t kMaxMalloc = 250000000;  // 1/4 GB

static bool showdata = false;   // show data
static bool rawdata = false;    // show raw/decoded data
static bool showwords = false;  // show data as bytes/words
static bool readdata = true;    // read data in file
static bool stoponerr = true;   // stop on first read error

typedef struct {
  const uint8_t* data;
  toff_t size;
  toff_t pos;
} MyData;

static int MyClose(thandle_t /* opaque */) { return 0; }

static toff_t MySize(thandle_t opaque) {
  const MyData* const my_data = (MyData*)opaque;
  return my_data->size;
}

static toff_t MySeek(thandle_t opaque, toff_t offset, int whence) {
  MyData* const my_data = (MyData*)opaque;
  offset += (whence == SEEK_CUR) ? my_data->pos
                                 : (whence == SEEK_SET) ? 0 : my_data->size;
  if (offset > my_data->size) return (toff_t)-1;
  my_data->pos = offset;
  return offset;
}

static int MyMapFile(thandle_t /* opaque */, void** /* base */,
                     toff_t* /* size */) {
  return 0;
}

static void MyUnmapFile(thandle_t /* opaque */, void* /* base */,
                        toff_t /* size*/) {}

static tsize_t MyRead(thandle_t opaque, void* dst, tsize_t size) {
  MyData* const my_data = (MyData*)opaque;
  if (my_data->pos + size > my_data->size) {
    size = my_data->size - my_data->pos;
  }
  if (size > 0) {
    memcpy(dst, my_data->data + my_data->pos, size);
    my_data->pos += size;
  }
  return size;
}

static void ShowStrip(tstrip_t strip, unsigned char* pp, uint32 nrow,
                      tsize_t scanline) {
  printf("Strip %lu:\n", (unsigned long)strip);
  while (nrow-- > 0) {
    for (tsize_t cc = 0; cc < scanline; cc++) {
      printf(" %02x", *pp++);
      if (((cc + 1) % 24) == 0) putchar('\n');
    }
    putchar('\n');
  }
}

void TIFFReadContigStripData(TIFF* tif) {
  const tsize_t scanline = TIFFScanlineSize(tif);

  if (TIFFStripSize(tif) > kMaxMalloc) return;
  unsigned char* buf = (unsigned char*)_TIFFmalloc(TIFFStripSize(tif));
  if (buf == nullptr) return;

  uint32 h = 0;
  uint32 rowsperstrip = (uint32)-1;

  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
  TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
  for (uint32 row = 0; row < h; row += rowsperstrip) {
    const uint32 nrow = (row + rowsperstrip > h ? h - row : rowsperstrip);
    tstrip_t strip = TIFFComputeStrip(tif, row, 0);
    if (TIFFReadEncodedStrip(tif, strip, buf, nrow * scanline) < 0) {
      if (stoponerr) break;
    } else if (showdata) {
      ShowStrip(strip, buf, nrow, scanline);
    }
  }
  _TIFFfree(buf);
}

void TIFFReadSeparateStripData(TIFF* tif) {
  tsize_t scanline = TIFFScanlineSize(tif);

  if (TIFFStripSize(tif) > kMaxMalloc) return;
  unsigned char* buf = (unsigned char*)_TIFFmalloc(TIFFStripSize(tif));
  if (buf == nullptr) return;
  uint32 h = 0;
  uint32 rowsperstrip = (uint32)-1;
  tsample_t samplesperpixel = 0;

  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
  TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
  TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
  for (uint32 row = 0; row < h; row += rowsperstrip) {
    for (tsample_t s = 0; s < samplesperpixel; s++) {
      uint32 nrow = (row + rowsperstrip > h ? h - row : rowsperstrip);
      tstrip_t strip = TIFFComputeStrip(tif, row, s);
      if (TIFFReadEncodedStrip(tif, strip, buf, nrow * scanline) < 0) {
        if (stoponerr) break;
      } else if (showdata) {
        ShowStrip(strip, buf, nrow, scanline);
      }
    }
  }
  _TIFFfree(buf);
}

static void ShowTile(uint32 row, uint32 col, tsample_t sample,
                     unsigned char* pp, uint32 nrow, tsize_t rowsize) {
  printf("Tile (%lu,%lu", (unsigned long)row, (unsigned long)col);
  if (sample != (tsample_t)-1) printf(",%u", sample);
  printf("):\n");
  while (nrow-- > 0) {
    for (uint32 cc = 0; cc < (uint32)rowsize; cc++) {
      printf(" %02x", *pp++);
      if (((cc + 1) % 24) == 0) putchar('\n');
    }
    putchar('\n');
  }
}

void TIFFReadContigTileData(TIFF* tif) {
  tsize_t rowsize = TIFFTileRowSize(tif);

  if (TIFFTileSize(tif) > kMaxMalloc) return;
  unsigned char* buf = (unsigned char*)_TIFFmalloc(TIFFTileSize(tif));
  if (buf == nullptr) return;
  uint32 tw = 0, th = 0, w = 0, h = 0;

  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
  TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tw);
  TIFFGetField(tif, TIFFTAG_TILELENGTH, &th);
  for (uint32 row = 0; row < h; row += th) {
    for (uint32 col = 0; col < w; col += tw) {
      if (TIFFReadTile(tif, buf, col, row, 0, 0) < 0) {
        if (stoponerr) break;
      } else if (showdata) {
        ShowTile(row, col, (tsample_t)-1, buf, th, rowsize);
      }
    }
  }
  _TIFFfree(buf);
}

void TIFFReadSeparateTileData(TIFF* tif) {
  const tsize_t rowsize = TIFFTileRowSize(tif);

  if (TIFFTileSize(tif) > kMaxMalloc) return;
  unsigned char* buf = (unsigned char*)_TIFFmalloc(TIFFTileSize(tif));
  if (buf == nullptr) return;
  uint32 tw = 0, th = 0, w = 0, h = 0;
  tsample_t s, samplesperpixel = 0;

  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
  TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tw);
  TIFFGetField(tif, TIFFTAG_TILELENGTH, &th);
  TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
  for (uint32 row = 0; row < h; row += th) {
    for (uint32 col = 0; col < w; col += tw) {
      for (s = 0; s < samplesperpixel; s++) {
        if (TIFFReadTile(tif, buf, col, row, 0, s) < 0) {
          if (stoponerr) break;
        } else if (showdata) {
          ShowTile(row, col, s, buf, th, rowsize);
        }
      }
    }
  }
  _TIFFfree(buf);
}

void TIFFReadData(TIFF* tif) {
  uint16 config = PLANARCONFIG_CONTIG;

  TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
  if (TIFFIsTiled(tif)) {
    if (config == PLANARCONFIG_CONTIG)
      TIFFReadContigTileData(tif);
    else
      TIFFReadSeparateTileData(tif);
  } else {
    if (config == PLANARCONFIG_CONTIG)
      TIFFReadContigStripData(tif);
    else
      TIFFReadSeparateStripData(tif);
  }
}

static void ShowRawBytes(unsigned char* pp, uint32 n) {
  for (uint32 i = 0; i < n; i++) {
    printf(" %02x", *pp++);
    if (((i + 1) % 24) == 0) printf("\n ");
  }
  putchar('\n');
}

static void ShowRawWords(uint16* pp, uint32 n) {
  for (uint32 i = 0; i < n; i++) {
    printf(" %04x", *pp++);
    if (((i + 1) % 15) == 0) printf("\n ");
  }
  putchar('\n');
}

void TIFFReadRawData(TIFF* tif, int bitrev) {
  const tstrip_t nstrips = TIFFNumberOfStrips(tif);
  const char* what = TIFFIsTiled(tif) ? "Tile" : "Strip";
  uint64* stripbc = nullptr;

  TIFFGetField(tif, TIFFTAG_STRIPBYTECOUNTS, &stripbc);

  if (nstrips < 1) return;

  uint32 bufsize = (uint32)stripbc[0];
  if (bufsize > kMaxMalloc) return;
  tdata_t buf = _TIFFmalloc(bufsize);

  for (tstrip_t s = 0; s < nstrips; s++) {
    if (stripbc[s] > bufsize) {
      buf = _TIFFrealloc(buf, (tmsize_t)stripbc[s]);
      bufsize = (uint32)stripbc[s];
    }
    if (buf == nullptr) {
      fprintf(stderr, "Cannot allocate buffer to read strip %lu\n",
              (unsigned long)s);
      break;
    }
    if (TIFFReadRawStrip(tif, s, buf, (tmsize_t)stripbc[s]) < 0) {
      fprintf(stderr, "Error reading strip %lu\n", (unsigned long)s);
      if (stoponerr) break;
    } else if (showdata) {
      if (bitrev) {
        TIFFReverseBits(reinterpret_cast<uint8*>(buf), (tmsize_t)stripbc[s]);
        printf("%s %lu: (bit reversed)\n ", what, (unsigned long)s);
      } else {
        printf("%s %lu:\n ", what, (unsigned long)s);
      }
      if (showwords)
        ShowRawWords((uint16*)buf, (uint32)stripbc[s] >> 1);
      else
        ShowRawBytes((unsigned char*)buf, (uint32)stripbc[s]);
    }
  }
  if (buf != nullptr) _TIFFfree(buf);
}

static void tiffinfo(TIFF* tif, uint16 order, long flags, int is_image) {
  TIFFPrintDirectory(tif, stdout, flags);
  if (!readdata || !is_image) return;
  if (rawdata) {
    if (order) {
      uint16 o;
      TIFFGetFieldDefaulted(tif, TIFFTAG_FILLORDER, &o);
      TIFFReadRawData(tif, o != order);
    } else {
      TIFFReadRawData(tif, 0);
    }
  } else {
    if (order) TIFFSetField(tif, TIFFTAG_FILLORDER, order);
    TIFFReadData(tif);
  }
}

void MyTIFFErrorHandler(const char*, const char*, va_list) {}
void MyTIFFErrorHandlerExt(thandle_t, const char*, const char*, va_list) {}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  TIFFSetErrorHandler(MyTIFFErrorHandler);
  TIFFSetErrorHandlerExt(MyTIFFErrorHandlerExt);
  TIFFSetWarningHandler(MyTIFFErrorHandler);
  TIFFSetWarningHandlerExt(MyTIFFErrorHandlerExt);

  MyData my_data = {data, (toff_t)size, 0};

  TIFF* tif = TIFFClientOpen("memory", "rc", &my_data, MyRead, MyRead, MySeek,
                             MyClose, MySize, MyMapFile, MyUnmapFile);
  if (tif == nullptr) return 0;

  do {
    toff_t offset = 0;

    uint16 order = 0;
    long flags = 0;
    tiffinfo(tif, order, flags, 1);
    if (TIFFGetField(tif, TIFFTAG_EXIFIFD, &offset)) {
      if (TIFFReadEXIFDirectory(tif, offset)) {
        tiffinfo(tif, order, flags, 0);
      }
    }
  } while (TIFFReadDirectory(tif));
  TIFFClose(tif);

  return 0;
}
