# Installing FastCSV with PIE

FastCSV is available through PIE (PHP Installer for Extensions), which provides an easy way to install PHP extensions.

## Prerequisites

- PHP 8.2 or higher
- PIE installed on your system
- Build tools (for non-Windows systems)

## Installation

### Using PIE

```bash
# Install the latest version
pie install csvtoolkit/fastcsv

# Install a specific version
pie install csvtoolkit/fastcsv:^0.0.1

# Install with custom configuration
pie install csvtoolkit/fastcsv --enable-fastcsv
```

### Verify Installation

After installation, verify that the extension is loaded:

```bash
# Check if extension is loaded
php -m | grep fastcsv

# Or check extension info
php --ri fastcsv
```

### Enable in php.ini

The extension should be automatically enabled, but if needed, add to your `php.ini`:

```ini
extension=fastcsv
```

## Usage

Once installed, you can use FastCSV in your PHP code:

```php
<?php

// Create a configuration
$config = new FastCSVConfig();
$config->setDelimiter(',');
$config->setEnclosure('"');

// Read CSV file
$reader = new FastCSVReader($config);
$reader->open('/path/to/file.csv');

while ($reader->hasNext()) {
    $record = $reader->nextRecord();
    print_r($record);
}

$reader->close();
```

## Troubleshooting

### Build Issues

If you encounter build issues on non-Windows systems:

1. Ensure you have the required build tools:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential php-dev

   # macOS
   brew install php
   ```

2. Check that git submodules are initialized:
   ```bash
   git submodule update --init --recursive
   ```

### Windows Users

Windows users will automatically receive pre-built binaries when installing through PIE. No build tools are required.

## Development Installation

For development purposes, you can install directly from the source:

```bash
# Clone the repository
git clone https://github.com/csvtoolkit/fastcsv.git
cd fastcsv

# Initialize submodules
git submodule update --init --recursive

# Install using PIE from local path
pie repository:add path .
pie install csvtoolkit/fastcsv:*@dev
```

## Uninstalling

To uninstall the extension:

```bash
pie uninstall csvtoolkit/fastcsv
```

## Support

For issues with PIE installation, please check:
- [PIE Documentation](https://github.com/php/pie)
- [FastCSV Issues](https://github.com/csvtoolkit/fastcsv/issues) 