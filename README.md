# FastCSV Extension

[![PECL](https://img.shields.io/pecl/v/fastcsv.svg)](https://pecl.php.net/package/fastcsv)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

A high-performance PHP extension for CSV file handling by CSVToolkit Organization. FastCSV provides significant improvements in speed and memory efficiency compared to PHP's native CSV handling functions.

This extension is built on top of [FastCSV-C](https://github.com/csvtoolkit/FastCSV-C), a standalone C library for high-performance CSV processing.

## Features

- High-performance CSV reading and writing
- Memory-efficient processing
- Support for custom delimiters and enclosures
- Header row handling
- Robust error handling
- UTF-8 support

## Performance Highlights

- **Reading**: Up to 2.31x faster than native PHP
- **Writing**: Up to 1.56x faster than native PHP
- **Memory Usage**: 
  - Reading: 94.8% less memory (1.2KB vs 22.8KB)
  - Writing: 99.1% less memory (80B vs 9KB)

## Requirements

- PHP 8.2+
- C compiler (GCC 4.x+, clang, Visual Studio for Windows)
- PHP development headers and tools

## Development Environment

For development, we recommend using [php-dev-box](https://github.com/achrafAa/php-dev-box), a containerized PHP development environment specifically designed for building, testing, and debugging PHP extensions. It provides:

- Pre-configured PHP development environment
- All necessary build tools and dependencies
- Debugging tools (GDB, Valgrind)
- Support for multiple PHP versions (8.2, 8.3, 8.4)
- Cross-platform compatibility

To get started with development:

```bash
# Clone php-dev-box
git clone https://github.com/achrafAa/php-dev-box
cd php-dev-box

# Start the development environment
make up

# Access the container shell
make shell

# Clone the extension with submodules
git clone --recursive https://github.com/csvtoolkit/fastcsv
cd fastcsv

# Build the extension
phpdev build
```

## Installation

### Via PECL

```bash
pecl install fastcsv
```

### Manual Installation

```bash
git clone --recursive https://github.com/csvtoolkit/fastcsv
cd fastcsv
phpize
./configure
make
make install
```

Add to your php.ini:
```ini
extension=fastcsv.so
```

## Quick Start

### Reading CSV Files

```php
$config = new FastCSVConfig();
$config->setFilename('data.csv')
       ->setHasHeader(true)
       ->setDelimiter(',')
       ->setEnclosure('"');

$reader = new FastCSVReader($config);

// Get headers
$headers = $reader->getHeaders();

// Read records
while (($record = $reader->nextRecord()) !== false) {
    // Process record
    print_r($record);
}

$reader->close();
```

### Writing CSV Files

```php
$config = new FastCSVConfig();
$config->setFilename('output.csv')
       ->setHasHeader(true);

$writer = new FastCSVWriter($config);

// Write headers
$writer->setHeaders(['id', 'name', 'email']);

// Write records
$writer->writeRecord(['1', 'John Doe', 'john@example.com']);
$writer->writeRecord(['2', 'Jane Doe', 'jane@example.com']);

$writer->close();
```

## API Reference

### FastCSVConfig

- `setFilename(string $filename): self`
- `setDelimiter(string $delimiter): self`
- `setEnclosure(string $enclosure): self`
- `setEscape(string $escape): self`
- `setHasHeader(bool $hasHeader): self`

### FastCSVReader

- `__construct(FastCSVConfig $config)`
- `getHeaders(): array`
- `nextRecord(): array|false`
- `getRecordCount(): int`
- `rewind(): void`
- `close(): void`

### FastCSVWriter

- `__construct(FastCSVConfig $config)`
- `setHeaders(array $headers): bool`
- `writeRecord(array $record): bool`
- `close(): void`

## Testing

Run the test suite:
```bash
make test
```

## Benchmarking

Run the included benchmark script:
```bash
php -d extension=fastcsv.so dev/benchmark.php <input_csv_file>
```

## Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## Security

If you discover any security related issues, please email security@csvtoolkit.org instead of using the issue tracker.

## Credits

- [CSVToolkit Organization](https://github.com/csvtoolkit)
- [FastCSV-C Library](https://github.com/csvtoolkit/FastCSV-C)
- [All Contributors](../../contributors)
- Built with [php-dev-box](https://github.com/achrafAa/php-dev-box)

## License

The MIT License (MIT). Please see [License File](LICENSE) for more information. 