#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "src/readstat.h"

static char *buf_to_file(const char *buf, size_t size) {
  char *name = strdup("/dev/shm/fuzz-XXXXXX");
  int fd = mkstemp(name);
  if (fd < 0) {
    perror("open");
    exit(1);
  }
  size_t pos = 0;
  while (pos < size) {
    int nbytes = write(fd, &buf[pos], size - pos);
    if (nbytes <= 0) {
      perror("write");
      exit(1);
    }
    pos += nbytes;
  }
  if (close(fd) != 0) {
    perror("close");
    exit(1);
  }
  return name;
}

static void handle_error(const char *msg, void *ctx) {
    fprintf(stderr, "%s", msg);
}

static int dump_info(int obs_count, int var_count, void *ctx) {
    printf("Columns: %d\n", var_count);
    printf("Rows: %d\n", obs_count);
    return 0;
}

static int dump_metadata(const char *file_label, const char *original_encoding,
                         time_t timestamp, long version, void *ctx) {
  if (file_label && file_label[0])
    printf("File label: %s\n", file_label);
  if (original_encoding && original_encoding[0])
    printf("Original encoding: %s\n", original_encoding);
  if (timestamp && localtime(&timestamp)) {
    char buffer[128];
    strftime(buffer, sizeof(buffer), "%d %b %Y %H:%M", localtime(&timestamp));
    printf("Timestamp: %s\n", buffer);
  }
  if (version)
    printf("File format version: %ld\n", version);
  return 0;
}

#define RS_FORMAT_UNKNOWN       0x00
#define RS_FORMAT_DTA           0x01
#define RS_FORMAT_SAV           0x02
#define RS_FORMAT_POR           0x04
#define RS_FORMAT_SAS_DATA      0x08
#define RS_FORMAT_SAS_CATALOG   0x10
#define RS_FORMAT_XPORT         0x20
#define RS_FORMAT_CSV           0x40
#define RS_FORMAT_JSON          0x80

void parse_file(const char *file, int format) {
  readstat_parser_t *parser = readstat_parser_init();
  readstat_error_t error = READSTAT_OK;

  readstat_set_error_handler(parser, &handle_error);
  readstat_set_info_handler(parser, &dump_info);
  readstat_set_metadata_handler(parser, &dump_metadata);

  if (format == RS_FORMAT_DTA)
    error = readstat_parse_dta(parser, file, NULL);
  else if (format == RS_FORMAT_SAV)
    error = readstat_parse_sav(parser, file, NULL);
  else if (format == RS_FORMAT_POR)
    error = readstat_parse_por(parser, file, NULL);
  else if (format == RS_FORMAT_SAS_DATA)
    error = readstat_parse_sas7bdat(parser, file, NULL);
  else if (format == RS_FORMAT_SAS_CATALOG)
    error = readstat_parse_sas7bcat(parser, file, NULL);
  else if (format == RS_FORMAT_XPORT)
    error = readstat_parse_xport(parser, file, NULL);

  readstat_parser_free(parser);

  if (error != READSTAT_OK)
    fprintf(stderr, "Error processing %s: %s\n", file,
            readstat_error_message(error));
}

static int fuzz_file(const char *file) {
  // Attempt to parse the file for all formats.
  parse_file(file, RS_FORMAT_DTA);
  parse_file(file, RS_FORMAT_SAV);
  parse_file(file, RS_FORMAT_POR);
  parse_file(file, RS_FORMAT_SAS_DATA);
  parse_file(file, RS_FORMAT_SAS_CATALOG);
  parse_file(file, RS_FORMAT_XPORT);
  return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  char *file = buf_to_file((const char*) data, size);
  (void)fuzz_file(file);
  unlink(file);
  free(file);
  return 0;
}
