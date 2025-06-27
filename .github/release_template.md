## 🚀 FastCSV Extension vX.Y.Z

A high-performance CSV parsing extension for PHP.

### 📦 Available Builds

#### Linux:
- PHP 8.2: `fastcsv-linux-php8.2.so`
- PHP 8.3: `fastcsv-linux-php8.3.so`
- PHP 8.4: `fastcsv-linux-php8.4.so`

#### macOS:
- PHP 8.2: `fastcsv-macos-php8.2.so`
- PHP 8.3: `fastcsv-macos-php8.3.so`
- PHP 8.4: `fastcsv-macos-php8.4.so`

### 🔧 Installation

1. **Download** the appropriate file for your platform and PHP version
2. **Rename** the downloaded file to `fastcsv.so`
3. **Copy** to your PHP extensions directory:
   ```bash
   # Find your PHP extension directory
   php -i | grep extension_dir
   
   # Copy and rename (example for Linux PHP 8.2)
   cp fastcsv-linux-php8.2.so /path/to/extension_dir/fastcsv.so
   ```
4. **Configure** your php.ini:
   ```ini
   extension=fastcsv.so
   ```
5. **Restart** your PHP service/server

### ✅ Verify Installation
```bash
# Check if the extension is loaded
php -m | grep fastcsv

# Test basic functionality
php -r "var_dump(extension_loaded('fastcsv'));"
```

### 📝 Changes in this Release

<!-- Add your changelog here -->

### 🐛 Issues & Support

If you encounter any issues, please [open an issue](https://github.com/your-repo/issues) on GitHub.

### 🔗 Quick Start

```php
<?php
// Basic usage example
$reader = new FastCSVReader('data.csv');
while ($row = $reader->read()) {
    print_r($row);
}
$reader->close();
?>
``` 