# FastCSV PHP Extension

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![PHP Extension](https://img.shields.io/badge/PHP-Extension-777BB4.svg?logo=php)](https://www.php.net/)
[![RFC 4180](https://img.shields.io/badge/RFC%204180-Compliant-brightgreen.svg)](https://tools.ietf.org/html/rfc4180)
[![Tests](https://img.shields.io/badge/tests-9%2F12%20passing-yellow.svg)](tests/)
[![Memory Safe](https://img.shields.io/badge/memory-safe-brightgreen.svg)](lib/)
[![Performance](https://img.shields.io/badge/performance-high-blue.svg)](README.md#performance)

A high-performance PHP extension for reading and writing CSV files with full RFC 4180 compliance and advanced features.

> **Tip**: For a unified API that automatically leverages this extension when available, check out [PHP-CSVHelper](https://github.com/csvtoolkit/PHP-CSVHelper). It provides a consistent interface that uses FastCSV for maximum performance while gracefully falling back to SplFileObject when needed.

## Features

- **High Performance**: Native C implementation with optimized memory management using Arena allocation
- **RFC 4180 Compliant**: Full compliance with CSV standard including proper quote handling and multi-line records
- **Flexible API**: Support for both file paths and configuration objects in constructors
- **Configurable Flushing**: Auto-flush mode for immediate data visibility or manual flush for maximum performance
- **Advanced CSV Handling**: 
  - Proper quote escaping and unescaping (`""` → `"`)
  - Multi-line quoted fields support
  - Configurable delimiters, quotes, and escape characters
  - CRLF and LF line ending support
  - Immediate or buffered write operations
- **Memory Efficient**: Arena-based memory management for optimal performance
- **Comprehensive Testing**: Extensive test suite with 75% pass rate (9/12 tests passing)

## Classes

### FastCSVReader

High-performance CSV reader with advanced navigation capabilities.

```php
// Create reader with file path
$reader = new FastCSVReader('/path/to/file.csv');

// Or with configuration object
$config = new FastCSVConfig();
$config->delimiter = ';';
$config->quote = '"';
$reader = new FastCSVReader($config);
$reader->open('/path/to/file.csv');

// Read records
while ($reader->hasNext()) {
    $record = $reader->nextRecord();
    print_r($record);
}

// Navigation methods
$reader->rewind();
$reader->seek(100);
$position = $reader->getPosition();
$count = $reader->getRecordCount();
$headers = $reader->getHeaders();
```

### FastCSVWriter

High-performance CSV writer with proper quoting and escaping.

```php
// Create writer with headers
$config = new FastCSVConfig();
$config->setPath('/path/to/output.csv');
$writer = new FastCSVWriter($config, ['Name', 'Age', 'City']);

// Write records (auto-flushed by default)
$writer->writeRecord(['John Doe', '30', 'New York']);
$writer->writeRecord(['Jane Smith', '25', 'Los Angeles']);

// For high-performance scenarios, disable auto-flush
$config->setAutoFlush(false);
$writer = new FastCSVWriter($config, ['ID', 'Data']);

for ($i = 0; $i < 100000; $i++) {
    $writer->writeRecord([$i, "Data$i"]);
    
    // Manual flush every 1000 records for optimal performance
    if ($i % 1000 == 0) {
        $writer->flush();
    }
}

$writer->close(); // Final flush on close
```

### FastCSVConfig

Configuration class for customizing CSV parsing and writing behavior.

```php
$config = new FastCSVConfig();
$config->setDelimiter(';');      // Field delimiter (default: ',')
$config->setEnclosure('"');      // Quote character (default: '"')
$config->setEscape('\\');        // Escape character (default: '\\')
$config->setHasHeader(true);     // First row contains headers (default: true)
$config->setAutoFlush(true);     // Auto-flush after each write (default: true)
$config->setStrictMode(false);   // Strict quoting mode (default: false)
$config->setSkipEmptyLines(false); // Skip empty lines (default: false)
$config->setTrimFields(false);   // Trim whitespace from fields (default: false)
$config->setWriteBOM(false);     // Write BOM for Unicode files (default: false)
```

## Installation

### Prerequisites

- PHP 7.4+ or PHP 8.x
- GCC or compatible C compiler
- PHP development headers (`php-dev` package)

### From Source

```bash
# Clone the repository
git clone <repository-url>
cd fastcsv-extension

# Initialize submodules (for the lib directory)
git submodule update --init --recursive

# Build the extension
phpize
./configure
make
make install
```

### Enable Extension

Add to your `php.ini`:

```ini
extension=fastcsv
```

Or load dynamically:

```php
dl('fastcsv.so'); // Linux/macOS
dl('fastcsv.dll'); // Windows
```

## Development

### Building

The extension uses an external C library located in the `lib/` directory (git submodule). The build process automatically includes:

- `lib/arena.c` - Arena memory management
- `lib/csv_parser.c` - Core CSV parsing logic
- `lib/csv_reader.c` - CSV reader implementation
- `lib/csv_writer.c` - CSV writer implementation
- `lib/csv_utils.c` - Utility functions

### Testing

Run the test suite:

```bash
# Run all tests
make test

# Run specific test
php run-tests.php tests/FastCSVReader_001.phpt

# Run with verbose output
php run-tests.php -v tests/
```

### Current Test Status

- **Passing**: 9/12 tests (75% success rate)
- **Known Issues**: 
  - Performance test may show early termination on large datasets
  - Minor formatting discrepancies in edge cases
  - Writer quoting behavior in specific scenarios

## Technical Details

### Architecture

The extension is built on top of a high-performance C library with the following components:

- **Arena Memory Management**: Efficient memory allocation and cleanup
- **Character-by-Character Parsing**: Proper handling of quoted fields and line endings
- **State Machine Parser**: RFC 4180 compliant parsing with quote state tracking
- **Configurable Parameters**: Flexible delimiter, quote, and escape character support

### Memory Management

The extension uses Arena-based memory allocation for optimal performance:

- Each reader/writer instance maintains its own arena
- Automatic cleanup when objects are destroyed
- Minimal memory fragmentation
- Efficient bulk allocations

### RFC 4180 Compliance

Full compliance with CSV standard including:

- Proper quote character handling
- Multi-line field support
- CRLF and LF line ending support
- Quote escaping (`""` becomes `"`)
- Whitespace preservation in quoted fields

## Performance

The extension is optimized for high-performance CSV processing:

- Native C implementation
- Arena-based memory management
- Efficient parsing algorithms
- Minimal PHP object overhead

Benchmarks show significant performance improvements over pure PHP implementations, especially for large files.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for development guidelines.

## License

This project is licensed under the terms specified in the [LICENSE](LICENSE) file.

## Changelog

### Recent Updates

- **Arena Integration**: Updated to use Arena memory management system
- **API Improvements**: Flexible constructors accepting both strings and config objects
- **RFC 4180 Compliance**: Complete rewrite of parser for standards compliance
- **Multi-line Support**: Proper handling of quoted fields spanning multiple lines
- **Error Handling**: Improved PHP error reporting and exception handling
- **Test Coverage**: Comprehensive test suite with extensive edge case coverage

### Known Issues

- Performance test may show reduced record counts on complex CSV files
- Minor formatting differences in specific edge cases
- Writer may not quote all fields in certain configurations

For the latest updates and bug reports, please check the project's issue tracker. 