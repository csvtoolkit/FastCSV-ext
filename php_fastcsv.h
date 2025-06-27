#ifndef PHP_FASTCSV_H
#define PHP_FASTCSV_H

extern zend_module_entry fastcsv_module_entry;
#define phpext_fastcsv_ptr &fastcsv_module_entry

#define PHP_FASTCSV_VERSION "0.0.1"

#ifdef PHP_WIN32
#   define PHP_FASTCSV_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#   define PHP_FASTCSV_API __attribute__ ((visibility("default")))
#else
#   define PHP_FASTCSV_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include "lib/csv_config.h"
#include "lib/csv_reader.h"
#include "lib/csv_writer.h"
#include "lib/arena.h"

// Class entries
extern zend_class_entry *fastcsv_reader_ce;
extern zend_class_entry *fastcsv_writer_ce;
extern zend_class_entry *fastcsv_config_ce;

typedef struct _php_fastcsv_reader_object {
    CSVReader *reader;
    Arena *persistent_arena;
    Arena *temp_arena;
    zend_object std;
} php_fastcsv_reader_object;

typedef struct _php_fastcsv_writer_object {
    CSVWriter *writer;
    Arena *arena;
    zend_object std;
} php_fastcsv_writer_object;

typedef struct _php_fastcsv_config_object {
    CSVConfig *config;
    Arena *arena;
    zend_object std;
} php_fastcsv_config_object;

// Method declarations
PHP_MINIT_FUNCTION(fastcsv);
PHP_MINFO_FUNCTION(fastcsv);

PHP_METHOD(FastCSVReader, __construct);
PHP_METHOD(FastCSVReader, getHeaders);
PHP_METHOD(FastCSVReader, nextRecord);
PHP_METHOD(FastCSVReader, close);
PHP_METHOD(FastCSVReader, rewind);
PHP_METHOD(FastCSVReader, setConfig);
PHP_METHOD(FastCSVReader, getRecordCount);
PHP_METHOD(FastCSVReader, getPosition);
PHP_METHOD(FastCSVReader, seek);

PHP_METHOD(FastCSVWriter, __construct);
PHP_METHOD(FastCSVWriter, writeRecord);
PHP_METHOD(FastCSVWriter, writeRecordMap);
PHP_METHOD(FastCSVWriter, flush);
PHP_METHOD(FastCSVWriter, close);

PHP_METHOD(FastCSVConfig, __construct);
PHP_METHOD(FastCSVConfig, getDelimiter);
PHP_METHOD(FastCSVConfig, setDelimiter);
PHP_METHOD(FastCSVConfig, getEnclosure);
PHP_METHOD(FastCSVConfig, setEnclosure);
PHP_METHOD(FastCSVConfig, getEscape);
PHP_METHOD(FastCSVConfig, setEscape);
PHP_METHOD(FastCSVConfig, getPath);
PHP_METHOD(FastCSVConfig, setPath);
PHP_METHOD(FastCSVConfig, getOffset);
PHP_METHOD(FastCSVConfig, setOffset);
PHP_METHOD(FastCSVConfig, hasHeader);
PHP_METHOD(FastCSVConfig, setHasHeader);
PHP_METHOD(FastCSVConfig, getEncoding);
PHP_METHOD(FastCSVConfig, setEncoding);
PHP_METHOD(FastCSVConfig, getWriteBOM);
PHP_METHOD(FastCSVConfig, setWriteBOM);
PHP_METHOD(FastCSVConfig, getStrictMode);
PHP_METHOD(FastCSVConfig, setStrictMode);
PHP_METHOD(FastCSVConfig, getSkipEmptyLines);
PHP_METHOD(FastCSVConfig, setSkipEmptyLines);
PHP_METHOD(FastCSVConfig, getTrimFields);
PHP_METHOD(FastCSVConfig, setTrimFields);
PHP_METHOD(FastCSVConfig, getPreserveQuotes);
PHP_METHOD(FastCSVConfig, setPreserveQuotes);
PHP_METHOD(FastCSVConfig, getAutoFlush);
PHP_METHOD(FastCSVConfig, setAutoFlush);

#ifdef ZTS
#define FASTCSV_G(v) TSRMG(fastcsv_globals_id, zend_fastcsv_globals *, v)
#else
#define FASTCSV_G(v) (fastcsv_globals.v)
#endif

#endif 