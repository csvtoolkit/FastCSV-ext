# FastCSV Extension

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

> ⚠️ **Experimental Status**: This extension is currently in experimental phase. While it works correctly and passes all tests, please use with caution in production environments. We recommend thorough testing in your specific use case before deployment.

A high-performance PHP extension for CSV file handling by CSVToolkit Organization. FastCSV provides significant improvements in speed and memory efficiency compared to PHP's native CSV handling functions.

**Performance Benchmarks (validated):**
- **4-7x faster** than native PHP CSV functions
- **Read Performance**: Up to 383K records/sec vs 82K records/sec (SplFileObject)
- **Write Performance**: Up to 692K records/sec vs 109K records/sec (SplFileObject)
- **Memory Efficient**: Constant memory usage with streaming for datasets of any size

This extension is built on top of [FastCSV-C](https://github.com/csvtoolkit/FastCSV-C), a standalone C library for high-performance CSV processing.

> **Tip**: For a unified API that automatically leverages this extension when available, check out [PHP-CSVHelper](https://github.com/csvtoolkit/PHP-CSVHelper). It provides a consistent interface that uses FastCSV for maximum performance while gracefully falling back to SplFileObject when needed.

## Features

- High-performance CSV reading and writing
- Memory-efficient processing
- Support for custom delimiters and enclosures
- Header row handling
- Robust error handling
- UTF-8 support

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

## Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## Security

If you discover any security related issues, please use the GitHub Security Advisory feature in this repository instead of using the issue tracker.

## Credits

- [CSVToolkit Organization](https://github.com/csvtoolkit)
- [FastCSV-C Library](https://github.com/csvtoolkit/FastCSV-C)
- [All Contributors](../../contributors)
- Built with [php-dev-box](https://github.com/achrafAa/php-dev-box)

## Support the Project

If you find FastCSV useful for your projects, please consider sponsoring the development! Your support helps maintain and improve this high-performance CSV extension while reducing development and infrastructure costs.

[![Sponsor](https://img.shields.io/badge/sponsor-❤️-ff69b4?style=for-the-badge&logo=github-sponsors)](https://github.com/sponsors/achrafAa)

**Why sponsor?**
- 🚀 Accelerate development of new features
- 🐛 Faster bug fixes and improvements  
- 📚 Better documentation and examples
- 🎯 Priority support for feature requests
- 💡 Fund research into even faster CSV processing techniques
- 💰 **Lower development costs** - Reduce your team's time spent on CSV processing optimization
- 🏗️ **Reduce infrastructure costs** - More efficient processing means lower server resources needed

## License

The MIT License (MIT). Please see [License File](LICENSE) for more information. 