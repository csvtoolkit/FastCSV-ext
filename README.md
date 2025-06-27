# FastCSV PHP Extension

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![PHP Extension](https://img.shields.io/badge/PHP-Extension-777BB4.svg?logo=php)](https://www.php.net/)
[![RFC 4180](https://img.shields.io/badge/RFC%204180-Compliant-brightgreen.svg)](https://tools.ietf.org/html/rfc4180)
[![Tests](https://img.shields.io/badge/tests-12%2F12%20passing-brightgreen.svg)](tests/)
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
- **Comprehensive Testing**: Extensive test suite with 100% pass rate (12/12 tests passing)

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

- PHP 8.x
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

- **Passing**: 12/12 tests (100% success rate)
- **Comprehensive Coverage**: Edge cases, performance, memory management, and RFC 4180 compliance
- **Stable**: All tests consistently pass across different environments

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

The extension is optimized for high-performance CSV processing with significant improvements over PHP's native SplFileObject:

### Benchmark Results (PHP 8.4.8, 1GB Memory Limit)

#### Read Operations Performance

| Data Size | Implementation | Median Time (ms) | Throughput (records/sec) | Speed Improvement |
|-----------|----------------|------------------|--------------------------|-------------------|
| Small (1K rows) | FastCSV | 3.67 | 272,410 | **4.1x faster** |
| | SplFileObject | 15.03 | 66,520 | |
| Medium (100K rows) | FastCSV | 176.04 | 568,049 | **3.6x faster** |
| | SplFileObject | 639.51 | 156,370 | |
| Large (1M rows) | FastCSV | 1,987.23 | 503,212 | **4.8x faster** |
| | SplFileObject | 9,468.64 | 105,612 | |

#### Combined Read/Write Operations

| Data Size | Implementation | Median Time (ms) | Throughput (records/sec) | Speed Improvement |
|-----------|----------------|------------------|--------------------------|-------------------|
| Small (1K rows) | FastCSV | 22.76 | 87,870 | **1.6x faster** |
| | SplFileObject | 35.5 | 56,341 | |
| Medium (100K rows) | FastCSV | 590.78 | 338,535 | **2.5x faster** |
| | SplFileObject | 1,469.31 | 136,118 | |
| Large (1M rows) | FastCSV | 7,088.8 | 282,135 | **2.9x faster** |
| | SplFileObject | 20,513.19 | 97,498 | |

### Key Performance Advantages

- **Read Operations**: 3.6x to 4.8x performance improvement over SplFileObject
- **Combined Operations**: 1.6x to 2.9x advantage for read/write operations
- **Scalability**: Performance advantage increases with data size
- **Memory Efficiency**: Constant ~2MB memory usage regardless of file size
- **Consistency**: Lower standard deviation, indicating more predictable performance

### Technical Performance Features

- **Native C Implementation**: Direct memory access and optimized algorithms
- **Arena Memory Management**: Efficient bulk allocations and minimal fragmentation
- **Streaming Operations**: Constant memory usage regardless of file size
- **RFC 4180 Optimized**: Fast parsing with proper quote handling



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


For the latest updates and bug reports, please check the project's issue tracker. 