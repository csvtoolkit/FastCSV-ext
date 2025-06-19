#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "php_fastcsv.h"

zend_class_entry *fastcsv_reader_ce;
zend_class_entry *fastcsv_writer_ce;
zend_class_entry *fastcsv_config_ce;

static inline php_fastcsv_reader_object *php_fastcsv_reader_from_obj(zend_object *obj) {
    return (php_fastcsv_reader_object*)((char*)(obj) - XtOffsetOf(php_fastcsv_reader_object, std));
}

static inline php_fastcsv_writer_object *php_fastcsv_writer_from_obj(zend_object *obj) {
    return (php_fastcsv_writer_object*)((char*)(obj) - XtOffsetOf(php_fastcsv_writer_object, std));
}

static inline php_fastcsv_config_object *php_fastcsv_config_from_obj(zend_object *obj) {
    return (php_fastcsv_config_object*)((char*)(obj) - XtOffsetOf(php_fastcsv_config_object, std));
}

#define Z_FASTCSV_READER_P(zv) php_fastcsv_reader_from_obj(Z_OBJ_P((zv)))
#define Z_FASTCSV_WRITER_P(zv) php_fastcsv_writer_from_obj(Z_OBJ_P((zv)))
#define Z_FASTCSV_CONFIG_P(zv) php_fastcsv_config_from_obj(Z_OBJ_P((zv)))

static zend_object_handlers fastcsv_reader_handlers;
static zend_object_handlers fastcsv_writer_handlers;
static zend_object_handlers fastcsv_config_handlers;

static zend_object *fastcsv_reader_create_object(zend_class_entry *class_type) {
    php_fastcsv_reader_object *intern;
    
    intern = zend_object_alloc(sizeof(php_fastcsv_reader_object), class_type);
    intern->reader = NULL;
    intern->arena = emalloc(sizeof(Arena));
    arena_create(intern->arena, 1024 * 1024);
    
    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);
    
    intern->std.handlers = &fastcsv_reader_handlers;
    
    return &intern->std;
}

static zend_object *fastcsv_writer_create_object(zend_class_entry *class_type) {
    php_fastcsv_writer_object *intern;
    
    intern = zend_object_alloc(sizeof(php_fastcsv_writer_object), class_type);
    intern->writer = NULL;
    intern->arena = emalloc(sizeof(Arena));
    arena_create(intern->arena, 1024 * 1024);
    
    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);
    
    intern->std.handlers = &fastcsv_writer_handlers;
    
    return &intern->std;
}

static zend_object *fastcsv_config_create_object(zend_class_entry *class_type) {
    php_fastcsv_config_object *intern;
    
    intern = zend_object_alloc(sizeof(php_fastcsv_config_object), class_type);
    intern->config = NULL;
    intern->arena = emalloc(sizeof(Arena));
    arena_create(intern->arena, 1024 * 1024);
    
    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);
    
    intern->std.handlers = &fastcsv_config_handlers;
    
    return &intern->std;
}

static void fastcsv_reader_free_object(zend_object *object) {
    php_fastcsv_reader_object *intern = php_fastcsv_reader_from_obj(object);
    
    if (intern->reader) {
        csv_reader_free(intern->reader);
        intern->reader = NULL;
    }
    
    if (intern->arena) {
        arena_destroy(intern->arena);
        efree(intern->arena);
        intern->arena = NULL;
    }
    
    zend_object_std_dtor(&intern->std);
}

static void fastcsv_writer_free_object(zend_object *object) {
    php_fastcsv_writer_object *intern = php_fastcsv_writer_from_obj(object);
    
    if (intern->writer) {
        csv_writer_free(intern->writer);
        intern->writer = NULL;
    }
    
    if (intern->arena) {
        arena_destroy(intern->arena);
        efree(intern->arena);
        intern->arena = NULL;
    }
    
    zend_object_std_dtor(&intern->std);
}

static void fastcsv_config_free_object(zend_object *object) {
    php_fastcsv_config_object *intern = php_fastcsv_config_from_obj(object);
    
    if (intern->config) {
        csv_config_free(intern->config);
        intern->config = NULL;
    }
    
    if (intern->arena) {
        arena_destroy(intern->arena);
        efree(intern->arena);
        intern->arena = NULL;
    }
    
    zend_object_std_dtor(&intern->std);
}

PHP_METHOD(FastCSVReader, __construct) {
    zval *param_zval;
    php_fastcsv_reader_object *intern;
    php_fastcsv_config_object *config_obj;
    CSVConfig *config;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(param_zval)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_READER_P(getThis());
    
    if (Z_TYPE_P(param_zval) == IS_STRING) {
        config = csv_config_create(intern->arena);
        if (!config) {
            zend_throw_exception(zend_ce_exception, "Failed to create default config", 0);
            RETURN_THROWS();
        }
        
        csv_config_set_path(config, Z_STRVAL_P(param_zval));
        
        intern->reader = csv_reader_init_with_config(intern->arena, config);
        if (!intern->reader) {
            zend_throw_exception(zend_ce_exception, "Failed to open CSV file", 0);
            RETURN_THROWS();
        }
    } else if (Z_TYPE_P(param_zval) == IS_OBJECT && instanceof_function(Z_OBJCE_P(param_zval), fastcsv_config_ce)) {
        config_obj = Z_FASTCSV_CONFIG_P(param_zval);
        
        if (!config_obj->config) {
            zend_throw_exception(zend_ce_exception, "Invalid FastCSVConfig object", 0);
            RETURN_THROWS();
        }
        
        intern->reader = csv_reader_init_with_config(intern->arena, config_obj->config);
        if (!intern->reader) {
            zend_throw_exception(zend_ce_exception, "Failed to open CSV file", 0);
            RETURN_THROWS();
        }
    } else {
        zend_throw_exception(zend_ce_exception, "Expected string (file path) or FastCSVConfig object", 0);
        RETURN_THROWS();
    }
}

PHP_METHOD(FastCSVReader, getHeaders) {
    php_fastcsv_reader_object *intern;
    char **headers;
    int header_count;
    int i;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_READER_P(getThis());
    
    if (!intern->reader) {
        zend_throw_exception(zend_ce_exception, "Reader not initialized", 0);
        RETURN_THROWS();
    }
    
    headers = csv_reader_get_headers(intern->reader, &header_count);
    if (!headers || header_count <= 0) {
        RETURN_NULL();
    }
    
    array_init(return_value);
    for (i = 0; i < header_count; i++) {
        if (headers[i]) {
            add_next_index_string(return_value, headers[i]);
        } else {
            add_next_index_null(return_value);
        }
    }
}

PHP_METHOD(FastCSVReader, nextRecord) {
    php_fastcsv_reader_object *intern;
    CSVRecord *record;
    int i;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_READER_P(getThis());
    
    if (!intern->reader) {
        zend_throw_exception(zend_ce_exception, "Reader not initialized", 0);
        RETURN_THROWS();
    }
    
    record = csv_reader_next_record(intern->reader);
    if (!record) {
        RETURN_NULL();
    }
    
    array_init(return_value);
    for (i = 0; i < record->field_count; i++) {
        if (record->fields[i]) {
            add_next_index_string(return_value, record->fields[i]);
        } else {
            add_next_index_null(return_value);
        }
    }
}

/* {{{ proto array|false FastCSVReader::close()
   Close the CSV file */
PHP_METHOD(FastCSVReader, close)
{
    php_fastcsv_reader_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_READER_P(getThis());
    
    if (intern->reader) {
        csv_reader_free(intern->reader);
        intern->reader = NULL;
        RETURN_TRUE;
    }
    
    RETURN_FALSE;
}

PHP_METHOD(FastCSVReader, rewind) {
    php_fastcsv_reader_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_READER_P(getThis());
    
    if (!intern->reader) {
        zend_throw_exception(zend_ce_exception, "Reader not initialized", 0);
        RETURN_THROWS();
    }
    
    csv_reader_rewind(intern->reader);
    RETURN_TRUE;
}

PHP_METHOD(FastCSVReader, setConfig) {
    zval *config_zval;
    php_fastcsv_reader_object *intern;
    php_fastcsv_config_object *config_obj;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(config_zval, fastcsv_config_ce)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_READER_P(getThis());
    config_obj = Z_FASTCSV_CONFIG_P(config_zval);
    
    if (!config_obj->config) {
        zend_throw_exception(zend_ce_exception, "Invalid FastCSVConfig object", 0);
        RETURN_THROWS();
    }
    
    if (intern->reader) {
        int result = csv_reader_set_config(intern->reader, intern->arena, config_obj->config);
        RETURN_BOOL(result == 1);
    } else {
        intern->reader = csv_reader_init_with_config(intern->arena, config_obj->config);
        if (!intern->reader) {
            zend_throw_exception(zend_ce_exception, "Failed to initialize reader with config", 0);
            RETURN_THROWS();
        }
        RETURN_TRUE;
    }
}

PHP_METHOD(FastCSVReader, getRecordCount) {
    php_fastcsv_reader_object *intern;
    long count;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_READER_P(getThis());
    
    if (!intern->reader) {
        zend_throw_exception(zend_ce_exception, "Reader not initialized", 0);
        RETURN_THROWS();
    }
    
    count = csv_reader_get_record_count(intern->reader);
    RETURN_LONG(count);
}

PHP_METHOD(FastCSVReader, getPosition) {
    php_fastcsv_reader_object *intern;
    long position;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_READER_P(getThis());
    
    if (!intern->reader) {
        zend_throw_exception(zend_ce_exception, "Reader not initialized", 0);
        RETURN_THROWS();
    }
    
    position = csv_reader_get_position(intern->reader);
    RETURN_LONG(position);
}

PHP_METHOD(FastCSVReader, seek) {
    zend_long position;
    php_fastcsv_reader_object *intern;
    int result;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(position)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_READER_P(getThis());
    
    if (!intern->reader) {
        zend_throw_exception(zend_ce_exception, "Reader not initialized", 0);
        RETURN_THROWS();
    }
    
    if (position < 0) {
        zend_throw_exception(zend_ce_exception, "Position must be non-negative", 0);
        RETURN_THROWS();
    }
    
    result = csv_reader_seek(intern->reader, position);
    RETURN_BOOL(result == 1);
}

PHP_METHOD(FastCSVReader, hasNext) {
    php_fastcsv_reader_object *intern;
    int has_next;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_READER_P(getThis());
    
    if (!intern->reader) {
        zend_throw_exception(zend_ce_exception, "Reader not initialized", 0);
        RETURN_THROWS();
    }
    
    has_next = csv_reader_has_next(intern->reader);
    RETURN_BOOL(has_next == 1);
}

PHP_METHOD(FastCSVWriter, __construct) {
    zval *param1_zval;
    zval *headers_array;
    php_fastcsv_writer_object *intern;
    php_fastcsv_config_object *config_intern;
    CSVConfig *config;
    char **headers;
    int header_count;
    zval *header_val;
    int i;
    CSVWriterResult result;
    
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(param1_zval)
        Z_PARAM_ARRAY(headers_array)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_WRITER_P(getThis());
    
    if (Z_TYPE_P(param1_zval) == IS_STRING) {
        config = csv_config_create(intern->arena);
        if (!config) {
            zend_throw_exception(zend_ce_exception, "Failed to create default config", 0);
            RETURN_THROWS();
        }
        
        csv_config_set_path(config, Z_STRVAL_P(param1_zval));
    } else if (Z_TYPE_P(param1_zval) == IS_OBJECT && instanceof_function(Z_OBJCE_P(param1_zval), fastcsv_config_ce)) {
        config_intern = Z_FASTCSV_CONFIG_P(param1_zval);
        
        if (!config_intern->config) {
            zend_throw_exception(zend_ce_exception, "Invalid FastCSVConfig object", 0);
            RETURN_THROWS();
        }
        
        config = config_intern->config;
    } else {
        zend_throw_exception(zend_ce_exception, "Expected string (file path) or FastCSVConfig object as first parameter", 0);
        RETURN_THROWS();
    }
    
    header_count = zend_hash_num_elements(Z_ARRVAL_P(headers_array));
    headers = emalloc(header_count * sizeof(char*));
    
    i = 0;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(headers_array), header_val) {
        if (Z_TYPE_P(header_val) == IS_STRING) {
            headers[i] = Z_STRVAL_P(header_val);
            i++;
        }
    } ZEND_HASH_FOREACH_END();
    
    result = csv_writer_init(&intern->writer, config, headers, header_count, intern->arena);
    efree(headers);
    
    if (result != CSV_WRITER_OK) {
        zend_throw_exception(zend_ce_exception, "Failed to create CSV file", 0);
        RETURN_THROWS();
    }
}

PHP_METHOD(FastCSVWriter, writeRecord) {
    zval *fields_array;
    php_fastcsv_writer_object *intern;
    char **fields;
    zend_string **converted_strings;
    int field_count;
    zval *field_val;
    int i;
    CSVWriterResult result;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(fields_array)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_WRITER_P(getThis());
    
    if (!intern->writer) {
        zend_throw_exception(zend_ce_exception, "Writer not initialized", 0);
        RETURN_THROWS();
    }
    
    field_count = zend_hash_num_elements(Z_ARRVAL_P(fields_array));
    if (field_count == 0) {
        RETURN_FALSE;
    }
    
    fields = emalloc(field_count * sizeof(char*));
    converted_strings = ecalloc(field_count, sizeof(zend_string*));
    
    i = 0;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(fields_array), field_val) {
        if (Z_TYPE_P(field_val) == IS_STRING) {
            fields[i] = Z_STRVAL_P(field_val);
            converted_strings[i] = NULL;
        } else if (Z_TYPE_P(field_val) == IS_NULL) {
            fields[i] = "";
            converted_strings[i] = NULL;
        } else {
            converted_strings[i] = zval_get_string(field_val);
            fields[i] = ZSTR_VAL(converted_strings[i]);
        }
        i++;
    } ZEND_HASH_FOREACH_END();
    
    result = csv_writer_write_record(intern->writer, fields, field_count);
    
    for (i = 0; i < field_count; i++) {
        if (converted_strings[i]) {
            zend_string_release(converted_strings[i]);
        }
    }
    
    efree(fields);
    efree(converted_strings);
    
    if (result != CSV_WRITER_OK) {
        zend_throw_exception(zend_ce_exception, csv_writer_error_string(result), 0);
        RETURN_THROWS();
    }
    
    RETURN_TRUE;
}

PHP_METHOD(FastCSVWriter, writeRecordMap) {
    zval *fields_map;
    php_fastcsv_writer_object *intern;
    char **field_names;
    char **field_values;
    zend_string **converted_strings;
    int field_count;
    zend_string *key;
    zval *val;
    int i;
    CSVWriterResult result;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(fields_map)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_WRITER_P(getThis());
    
    if (!intern->writer) {
        zend_throw_exception(zend_ce_exception, "Writer not initialized", 0);
        RETURN_THROWS();
    }
    
    field_count = zend_hash_num_elements(Z_ARRVAL_P(fields_map));
    if (field_count == 0) {
        RETURN_FALSE;
    }
    
    field_names = emalloc(field_count * sizeof(char*));
    field_values = emalloc(field_count * sizeof(char*));
    converted_strings = ecalloc(field_count, sizeof(zend_string*));
    
    i = 0;
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(fields_map), key, val) {
        if (key) {
            field_names[i] = ZSTR_VAL(key);
            if (Z_TYPE_P(val) == IS_STRING) {
                field_values[i] = Z_STRVAL_P(val);
                converted_strings[i] = NULL;
            } else if (Z_TYPE_P(val) == IS_NULL) {
                field_values[i] = "";
                converted_strings[i] = NULL;
            } else {
                converted_strings[i] = zval_get_string(val);
                field_values[i] = ZSTR_VAL(converted_strings[i]);
            }
            i++;
        }
    } ZEND_HASH_FOREACH_END();
    
    result = csv_writer_write_record_map(intern->writer, field_names, field_values, field_count);
    
    for (i = 0; i < field_count; i++) {
        if (converted_strings[i]) {
            zend_string_release(converted_strings[i]);
        }
    }
    
    efree(field_names);
    efree(field_values);
    efree(converted_strings);
    
    if (result != CSV_WRITER_OK) {
        zend_throw_exception(zend_ce_exception, csv_writer_error_string(result), 0);
        RETURN_THROWS();
    }
    
    RETURN_TRUE;
}

PHP_METHOD(FastCSVWriter, flush) {
    php_fastcsv_writer_object *intern;
    CSVWriterResult result;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_WRITER_P(getThis());
    
    if (!intern->writer) {
        zend_throw_exception(zend_ce_exception, "Writer not initialized", 0);
        RETURN_THROWS();
    }
    
    result = csv_writer_flush(intern->writer);
    if (result != CSV_WRITER_OK) {
        zend_throw_exception(zend_ce_exception, csv_writer_error_string(result), 0);
        RETURN_THROWS();
    }
    
    RETURN_TRUE;
}

PHP_METHOD(FastCSVWriter, close) {
    php_fastcsv_writer_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_WRITER_P(getThis());
    
    if (intern->writer) {
        csv_writer_free(intern->writer);
        intern->writer = NULL;
        RETURN_TRUE;
    }
    
    RETURN_FALSE;
}

PHP_METHOD(FastCSVConfig, __construct) {
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    intern->config = csv_config_create(intern->arena);
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Failed to create CSV config", 0);
        RETURN_THROWS();
    }
}

PHP_METHOD(FastCSVConfig, getDelimiter) {
    php_fastcsv_config_object *intern;
    char delimiter_str[2];
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    delimiter_str[0] = csv_config_get_delimiter(intern->config);
    delimiter_str[1] = '\0';
    
    RETURN_STRING(delimiter_str);
}

PHP_METHOD(FastCSVConfig, setDelimiter) {
    char *delimiter;
    size_t delimiter_len;
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(delimiter, delimiter_len)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    if (delimiter_len != 1) {
        zend_throw_exception(zend_ce_exception, "Delimiter must be exactly one character", 0);
        RETURN_THROWS();
    }
    
    csv_config_set_delimiter(intern->config, delimiter[0]);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, getEnclosure) {
    php_fastcsv_config_object *intern;
    char enclosure_str[2];
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    enclosure_str[0] = csv_config_get_enclosure(intern->config);
    enclosure_str[1] = '\0';
    
    RETURN_STRING(enclosure_str);
}

PHP_METHOD(FastCSVConfig, setEnclosure) {
    char *enclosure;
    size_t enclosure_len;
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(enclosure, enclosure_len)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    if (enclosure_len != 1) {
        zend_throw_exception(zend_ce_exception, "Enclosure must be exactly one character", 0);
        RETURN_THROWS();
    }
    
    csv_config_set_enclosure(intern->config, enclosure[0]);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, getEscape) {
    php_fastcsv_config_object *intern;
    char escape_str[2];
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    escape_str[0] = csv_config_get_escape(intern->config);
    escape_str[1] = '\0';
    
    RETURN_STRING(escape_str);
}

PHP_METHOD(FastCSVConfig, setEscape) {
    char *escape;
    size_t escape_len;
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(escape, escape_len)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    if (escape_len != 1) {
        zend_throw_exception(zend_ce_exception, "Escape character must be exactly one character", 0);
        RETURN_THROWS();
    }
    
    csv_config_set_escape(intern->config, escape[0]);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, getPath) {
    php_fastcsv_config_object *intern;
    const char *path;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    path = csv_config_get_path(intern->config);
    RETURN_STRING(path ? path : "");
}

PHP_METHOD(FastCSVConfig, setPath) {
    char *path;
    size_t path_len;
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(path, path_len)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    csv_config_set_path(intern->config, path);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, getOffset) {
    php_fastcsv_config_object *intern;
    int offset;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    offset = csv_config_get_offset(intern->config);
    RETURN_LONG(offset);
}

PHP_METHOD(FastCSVConfig, setOffset) {
    zend_long offset;
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(offset)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    if (offset < 0) {
        zend_throw_exception(zend_ce_exception, "Offset must be non-negative", 0);
        RETURN_THROWS();
    }
    
    csv_config_set_offset(intern->config, (int)offset);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, hasHeader) {
    php_fastcsv_config_object *intern;
    bool has_header;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    has_header = csv_config_has_header(intern->config);
    RETURN_BOOL(has_header);
}

PHP_METHOD(FastCSVConfig, setHasHeader) {
    bool has_header;
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(has_header)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    csv_config_set_has_header(intern->config, has_header);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, getEncoding) {
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    RETURN_LONG(csv_config_get_encoding(intern->config));
}

PHP_METHOD(FastCSVConfig, setEncoding) {
    php_fastcsv_config_object *intern;
    zend_long encoding;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(encoding)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    csv_config_set_encoding(intern->config, (CSVEncoding)encoding);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, getWriteBOM) {
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    RETURN_BOOL(csv_config_get_write_bom(intern->config));
}

PHP_METHOD(FastCSVConfig, setWriteBOM) {
    php_fastcsv_config_object *intern;
    zend_bool writeBOM;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(writeBOM)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    csv_config_set_write_bom(intern->config, writeBOM);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, getStrictMode) {
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    RETURN_BOOL(csv_config_get_strict_mode(intern->config));
}

PHP_METHOD(FastCSVConfig, setStrictMode) {
    php_fastcsv_config_object *intern;
    zend_bool strictMode;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(strictMode)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    csv_config_set_strict_mode(intern->config, strictMode);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, getSkipEmptyLines) {
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    RETURN_BOOL(csv_config_get_skip_empty_lines(intern->config));
}

PHP_METHOD(FastCSVConfig, setSkipEmptyLines) {
    php_fastcsv_config_object *intern;
    zend_bool skipEmptyLines;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(skipEmptyLines)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    csv_config_set_skip_empty_lines(intern->config, skipEmptyLines);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, getTrimFields) {
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    RETURN_BOOL(csv_config_get_trim_fields(intern->config));
}

PHP_METHOD(FastCSVConfig, setTrimFields) {
    php_fastcsv_config_object *intern;
    zend_bool trimFields;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(trimFields)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    csv_config_set_trim_fields(intern->config, trimFields);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, getPreserveQuotes) {
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    RETURN_BOOL(csv_config_get_preserve_quotes(intern->config));
}

PHP_METHOD(FastCSVConfig, setPreserveQuotes) {
    php_fastcsv_config_object *intern;
    zend_bool preserveQuotes;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(preserveQuotes)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    csv_config_set_preserve_quotes(intern->config, preserveQuotes);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, getAutoFlush) {
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    RETURN_BOOL(csv_config_get_auto_flush(intern->config));
}

PHP_METHOD(FastCSVConfig, setAutoFlush) {
    php_fastcsv_config_object *intern;
    zend_bool autoFlush;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(autoFlush)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        zend_throw_exception(zend_ce_exception, "Config not initialized", 0);
        RETURN_THROWS();
    }
    
    csv_config_set_auto_flush(intern->config, autoFlush);
    RETURN_ZVAL(getThis(), 1, 0);
}

/* Arginfo for FastCSVReader methods */
ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_reader_construct, 0, 0, 1)
    ZEND_ARG_INFO(0, configOrPath)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_reader_get_headers, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_reader_next_record, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_reader_close, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_reader_rewind, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_reader_set_config, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, config, FastCSVConfig, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_reader_get_record_count, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_reader_get_position, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_reader_seek, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, position, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_reader_has_next, 0, 0, 0)
ZEND_END_ARG_INFO()

/* Arginfo for FastCSVWriter methods */
ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_writer_construct, 0, 0, 2)
    ZEND_ARG_INFO(0, configOrPath)
    ZEND_ARG_TYPE_INFO(0, headers, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_writer_write_record, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, fields, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_writer_write_record_map, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, fields_map, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_writer_flush, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_writer_close, 0, 0, 0)
ZEND_END_ARG_INFO()

/* Arginfo for FastCSVConfig methods */
ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_get_delimiter, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_set_delimiter, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, delimiter, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_get_enclosure, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_set_enclosure, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, enclosure, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_get_escape, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_set_escape, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, escape, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_get_path, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_set_path, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_get_offset, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_set_offset, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_has_header, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_set_has_header, 0, 0, 1)
    ZEND_ARG_INFO(0, hasHeader)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_get_encoding, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_set_encoding, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, encoding, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_get_write_bom, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_set_write_bom, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, writeBOM, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_get_strict_mode, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_set_strict_mode, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, strictMode, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_get_skip_empty_lines, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_set_skip_empty_lines, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, skipEmptyLines, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_get_trim_fields, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_set_trim_fields, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, trimFields, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_get_preserve_quotes, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_set_preserve_quotes, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, preserveQuotes, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_get_auto_flush, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_config_set_auto_flush, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, autoFlush, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry fastcsv_reader_methods[] = {
    PHP_ME(FastCSVReader, __construct, arginfo_fastcsv_reader_construct, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVReader, getHeaders, arginfo_fastcsv_reader_get_headers, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVReader, nextRecord, arginfo_fastcsv_reader_next_record, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVReader, close, arginfo_fastcsv_reader_close, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVReader, rewind, arginfo_fastcsv_reader_rewind, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVReader, setConfig, arginfo_fastcsv_reader_set_config, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVReader, getRecordCount, arginfo_fastcsv_reader_get_record_count, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVReader, getPosition, arginfo_fastcsv_reader_get_position, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVReader, seek, arginfo_fastcsv_reader_seek, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVReader, hasNext, arginfo_fastcsv_reader_has_next, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

static const zend_function_entry fastcsv_writer_methods[] = {
    PHP_ME(FastCSVWriter, __construct, arginfo_fastcsv_writer_construct, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVWriter, writeRecord, arginfo_fastcsv_writer_write_record, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVWriter, writeRecordMap, arginfo_fastcsv_writer_write_record_map, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVWriter, flush, arginfo_fastcsv_writer_flush, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVWriter, close, arginfo_fastcsv_writer_close, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

static const zend_function_entry fastcsv_config_methods[] = {
    PHP_ME(FastCSVConfig, __construct, arginfo_fastcsv_config_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(FastCSVConfig, getDelimiter, arginfo_fastcsv_config_get_delimiter, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, setDelimiter, arginfo_fastcsv_config_set_delimiter, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, getEnclosure, arginfo_fastcsv_config_get_enclosure, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, setEnclosure, arginfo_fastcsv_config_set_enclosure, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, getEscape, arginfo_fastcsv_config_get_escape, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, setEscape, arginfo_fastcsv_config_set_escape, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, getPath, arginfo_fastcsv_config_get_path, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, setPath, arginfo_fastcsv_config_set_path, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, getOffset, arginfo_fastcsv_config_get_offset, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, setOffset, arginfo_fastcsv_config_set_offset, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, hasHeader, arginfo_fastcsv_config_has_header, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, setHasHeader, arginfo_fastcsv_config_set_has_header, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, getEncoding, arginfo_fastcsv_config_get_encoding, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, setEncoding, arginfo_fastcsv_config_set_encoding, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, getWriteBOM, arginfo_fastcsv_config_get_write_bom, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, setWriteBOM, arginfo_fastcsv_config_set_write_bom, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, getStrictMode, arginfo_fastcsv_config_get_strict_mode, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, setStrictMode, arginfo_fastcsv_config_set_strict_mode, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, getSkipEmptyLines, arginfo_fastcsv_config_get_skip_empty_lines, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, setSkipEmptyLines, arginfo_fastcsv_config_set_skip_empty_lines, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, getTrimFields, arginfo_fastcsv_config_get_trim_fields, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, setTrimFields, arginfo_fastcsv_config_set_trim_fields, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, getPreserveQuotes, arginfo_fastcsv_config_get_preserve_quotes, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, setPreserveQuotes, arginfo_fastcsv_config_set_preserve_quotes, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, getAutoFlush, arginfo_fastcsv_config_get_auto_flush, ZEND_ACC_PUBLIC)
    PHP_ME(FastCSVConfig, setAutoFlush, arginfo_fastcsv_config_set_auto_flush, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(fastcsv) {
    zend_class_entry ce;
    
    INIT_CLASS_ENTRY(ce, "FastCSVReader", fastcsv_reader_methods);
    fastcsv_reader_ce = zend_register_internal_class(&ce);
    fastcsv_reader_ce->create_object = fastcsv_reader_create_object;
    
    memcpy(&fastcsv_reader_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    fastcsv_reader_handlers.offset = XtOffsetOf(php_fastcsv_reader_object, std);
    fastcsv_reader_handlers.free_obj = fastcsv_reader_free_object;
    
    INIT_CLASS_ENTRY(ce, "FastCSVWriter", fastcsv_writer_methods);
    fastcsv_writer_ce = zend_register_internal_class(&ce);
    fastcsv_writer_ce->create_object = fastcsv_writer_create_object;
    
    memcpy(&fastcsv_writer_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    fastcsv_writer_handlers.offset = XtOffsetOf(php_fastcsv_writer_object, std);
    fastcsv_writer_handlers.free_obj = fastcsv_writer_free_object;
    
    INIT_CLASS_ENTRY(ce, "FastCSVConfig", fastcsv_config_methods);
    fastcsv_config_ce = zend_register_internal_class(&ce);
    fastcsv_config_ce->create_object = fastcsv_config_create_object;
    
    memcpy(&fastcsv_config_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    fastcsv_config_handlers.offset = XtOffsetOf(php_fastcsv_config_object, std);
    fastcsv_config_handlers.free_obj = fastcsv_config_free_object;

    // Register encoding constants
    REGISTER_LONG_CONSTANT("CSV_ENCODING_UTF8", CSV_ENCODING_UTF8, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("CSV_ENCODING_UTF16LE", CSV_ENCODING_UTF16LE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("CSV_ENCODING_UTF16BE", CSV_ENCODING_UTF16BE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("CSV_ENCODING_UTF32LE", CSV_ENCODING_UTF32LE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("CSV_ENCODING_UTF32BE", CSV_ENCODING_UTF32BE, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("CSV_ENCODING_ASCII", CSV_ENCODING_ASCII, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("CSV_ENCODING_LATIN1", CSV_ENCODING_LATIN1, CONST_CS | CONST_PERSISTENT);
    
    return SUCCESS;
}

PHP_MINFO_FUNCTION(fastcsv) {
    php_info_print_table_start();
    php_info_print_table_header(2, "FastCSV support", "enabled");
    php_info_print_table_row(2, "FastCSV version", PHP_FASTCSV_VERSION);
    php_info_print_table_end();
}

zend_module_entry fastcsv_module_entry = {
    STANDARD_MODULE_HEADER,
    "fastcsv",
    NULL,
    PHP_MINIT(fastcsv),
    NULL,
    NULL,
    NULL,
    PHP_MINFO(fastcsv),
    PHP_FASTCSV_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_FASTCSV
ZEND_GET_MODULE(fastcsv)
#endif 