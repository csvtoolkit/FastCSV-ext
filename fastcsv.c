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
    
    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);
    
    intern->std.handlers = &fastcsv_reader_handlers;
    
    return &intern->std;
}

static zend_object *fastcsv_writer_create_object(zend_class_entry *class_type) {
    php_fastcsv_writer_object *intern;
    
    intern = zend_object_alloc(sizeof(php_fastcsv_writer_object), class_type);
    intern->writer = NULL;
    
    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);
    
    intern->std.handlers = &fastcsv_writer_handlers;
    
    return &intern->std;
}

static zend_object *fastcsv_config_create_object(zend_class_entry *class_type) {
    php_fastcsv_config_object *intern;
    
    intern = zend_object_alloc(sizeof(php_fastcsv_config_object), class_type);
    intern->config = NULL;
    
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
    
    zend_object_std_dtor(&intern->std);
}

static void fastcsv_writer_free_object(zend_object *object) {
    php_fastcsv_writer_object *intern = php_fastcsv_writer_from_obj(object);
    
    if (intern->writer) {
        csv_writer_free(intern->writer);
        intern->writer = NULL;
    }
    
    zend_object_std_dtor(&intern->std);
}

static void fastcsv_config_free_object(zend_object *object) {
    php_fastcsv_config_object *intern = php_fastcsv_config_from_obj(object);
    
    if (intern->config) {
        csv_config_free(intern->config);
        intern->config = NULL;
    }
    
    zend_object_std_dtor(&intern->std);
}

PHP_METHOD(FastCSVReader, __construct) {
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
    
    intern->reader = csv_reader_init_with_config(config_obj->config);
    if (!intern->reader) {
        zend_throw_exception(zend_ce_exception, "Failed to open CSV file", 0);
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
        RETURN_FALSE;
    }
    
    headers = csv_reader_get_headers(intern->reader, &header_count);
    if (!headers) {
        RETURN_FALSE;
    }
    
    array_init(return_value);
    for (i = 0; i < header_count; i++) {
        add_next_index_string(return_value, headers[i]);
    }
}

PHP_METHOD(FastCSVReader, nextRecord) {
    php_fastcsv_reader_object *intern;
    CSVRecord *record;
    int i;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_READER_P(getThis());
    
    if (!intern->reader) {
        RETURN_FALSE;
    }
    
    record = csv_reader_next_record(intern->reader);
    if (!record) {
        RETURN_FALSE;
    }
    
    array_init(return_value);
    for (i = 0; i < record->field_count; i++) {
        add_next_index_string(return_value, record->fields[i]);
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
    }
}

PHP_METHOD(FastCSVReader, rewind) {
    php_fastcsv_reader_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_READER_P(getThis());
    
    if (intern->reader) {
        csv_reader_rewind(intern->reader);
    }
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
        RETURN_FALSE;
    }
    
    if (intern->reader) {
        RETURN_BOOL(csv_reader_set_config(intern->reader, config_obj->config) == 1);
    } else {
        intern->reader = csv_reader_init_with_config(config_obj->config);
        RETURN_BOOL(intern->reader != NULL);
    }
}

PHP_METHOD(FastCSVReader, getRecordCount) {
    php_fastcsv_reader_object *intern;
    long count;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_READER_P(getThis());
    
    if (!intern->reader) {
        RETURN_LONG(-1);
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
        RETURN_LONG(-1);
    }
    
    position = csv_reader_get_position(intern->reader);
    RETURN_LONG(position);
}

PHP_METHOD(FastCSVReader, seek) {
    zend_long position;
    php_fastcsv_reader_object *intern;
    int result;
    CSVRecord *record;
    int i;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(position)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_READER_P(getThis());
    
    if (!intern->reader) {
        RETURN_FALSE;
    }
    
    result = csv_reader_seek(intern->reader, position);
    if (!result) {
        RETURN_FALSE;
    }
    
    // After successful seek, get the record at that position
    record = csv_reader_next_record(intern->reader);
    if (!record) {
        RETURN_FALSE;
    }
    
    array_init(return_value);
    for (i = 0; i < record->field_count; i++) {
        add_next_index_string(return_value, record->fields[i]);
    }
}

PHP_METHOD(FastCSVReader, hasNext) {
    php_fastcsv_reader_object *intern;
    int has_next;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_READER_P(getThis());
    
    if (!intern->reader) {
        RETURN_FALSE;
    }
    
    has_next = csv_reader_has_next(intern->reader);
    RETURN_BOOL(has_next);
}

PHP_METHOD(FastCSVWriter, __construct) {
    zval *config_obj;
    zval *headers_array;
    php_fastcsv_writer_object *intern;
    php_fastcsv_config_object *config_intern;
    char **headers;
    int header_count;
    zval *header_val;
    int i;
    
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(config_obj, fastcsv_config_ce)
        Z_PARAM_ARRAY(headers_array)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_WRITER_P(getThis());
    config_intern = Z_FASTCSV_CONFIG_P(config_obj);
    
    if (!config_intern->config) {
        zend_throw_exception(zend_ce_exception, "Invalid FastCSVConfig object", 0);
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
    
    intern->writer = csv_writer_init(config_intern->config, headers, header_count);
    efree(headers);
    
    if (!intern->writer) {
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
    int result;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(fields_array)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_WRITER_P(getThis());
    
    if (!intern->writer) {
        RETURN_FALSE;
    }
    
    field_count = zend_hash_num_elements(Z_ARRVAL_P(fields_array));
    fields = emalloc(field_count * sizeof(char*));
    converted_strings = ecalloc(field_count, sizeof(zend_string*));
    
    i = 0;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(fields_array), field_val) {
        if (Z_TYPE_P(field_val) == IS_STRING) {
            fields[i] = Z_STRVAL_P(field_val);
            converted_strings[i] = NULL;
        } else {
            // Convert non-string values to string representation
            converted_strings[i] = zval_get_string(field_val);
            fields[i] = ZSTR_VAL(converted_strings[i]);
        }
        i++;
    } ZEND_HASH_FOREACH_END();
    
    result = csv_writer_write_record(intern->writer, fields, field_count);
    
    // Free converted strings
    for (i = 0; i < field_count; i++) {
        if (converted_strings[i]) {
            zend_string_release(converted_strings[i]);
        }
    }
    
    efree(fields);
    efree(converted_strings);
    
    RETURN_BOOL(result == 0);
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
    int result;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(fields_map)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_WRITER_P(getThis());
    
    if (!intern->writer) {
        RETURN_FALSE;
    }
    
    field_count = zend_hash_num_elements(Z_ARRVAL_P(fields_map));
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
            } else {
                // Convert non-string values to string representation
                converted_strings[i] = zval_get_string(val);
                field_values[i] = ZSTR_VAL(converted_strings[i]);
            }
            i++;
        }
    } ZEND_HASH_FOREACH_END();
    
    result = csv_writer_write_record_map(intern->writer, field_names, field_values, field_count);
    
    // Free converted strings
    for (i = 0; i < field_count; i++) {
        if (converted_strings[i]) {
            zend_string_release(converted_strings[i]);
        }
    }
    
    efree(field_names);
    efree(field_values);
    efree(converted_strings);
    
    RETURN_BOOL(result == 0);
}

PHP_METHOD(FastCSVWriter, close) {
    php_fastcsv_writer_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_WRITER_P(getThis());
    
    if (intern->writer) {
        csv_writer_free(intern->writer);
        intern->writer = NULL;
    }
}

PHP_METHOD(FastCSVConfig, __construct) {
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    intern->config = csv_config_create();
    
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
        RETURN_FALSE;
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
    
    if (!intern->config || delimiter_len != 1) {
        RETURN_FALSE;
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
        RETURN_FALSE;
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
    
    if (!intern->config || enclosure_len != 1) {
        RETURN_FALSE;
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
        RETURN_FALSE;
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
    
    if (!intern->config || escape_len != 1) {
        RETURN_FALSE;
    }
    
    csv_config_set_escape(intern->config, escape[0]);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, getPath) {
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        RETURN_FALSE;
    }
    
    RETURN_STRING(csv_config_get_path(intern->config));
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
        RETURN_FALSE;
    }
    
    csv_config_set_path(intern->config, path);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, getOffset) {
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        RETURN_FALSE;
    }
    
    RETURN_LONG(csv_config_get_offset(intern->config));
}

PHP_METHOD(FastCSVConfig, setOffset) {
    zend_long offset;
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(offset)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        RETURN_FALSE;
    }
    
    csv_config_set_offset(intern->config, (int)offset);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(FastCSVConfig, hasHeader) {
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_NONE();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        RETURN_FALSE;
    }
    
    RETURN_BOOL(csv_config_has_header(intern->config));
}

PHP_METHOD(FastCSVConfig, setHasHeader) {
    zend_bool has_header;
    php_fastcsv_config_object *intern;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(has_header)
    ZEND_PARSE_PARAMETERS_END();
    
    intern = Z_FASTCSV_CONFIG_P(getThis());
    
    if (!intern->config) {
        RETURN_FALSE;
    }
    
    csv_config_set_has_header(intern->config, has_header);
    RETURN_ZVAL(getThis(), 1, 0);
}

/* Arginfo for FastCSVReader methods */
ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_reader_construct, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, config, FastCSVConfig, 0)
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
    ZEND_ARG_OBJ_INFO(0, config, FastCSVConfig, 0)
    ZEND_ARG_TYPE_INFO(0, headers, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_writer_write_record, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, fields, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fastcsv_writer_write_record_map, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, fields_map, IS_ARRAY, 0)
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
    PHP_ME(FastCSVWriter, close, arginfo_fastcsv_writer_close, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

static const zend_function_entry fastcsv_config_methods[] = {
    PHP_ME(FastCSVConfig, __construct, arginfo_fastcsv_config_construct, ZEND_ACC_PUBLIC)
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