--TEST--
FastCSVWriter - Special characters and formats
--SKIPIF--
<?php
if (!extension_loaded('fastcsv')) die('skip');
?>
--FILE--
<?php
require_once __DIR__ . '/../fastcsv.php';

$config = new FastCSVConfig();
$config->setPath('special_chars.csv');
$config->setEncoding(CSV_ENCODING_UTF8);
$config->setWriteBOM(true);
$config->setStrictMode(true);

$writer = new FastCSVWriter($config, ['id', 'name', 'description', 'notes']);

$writer->writeRecord([
    1,
    'Product "Special"',
    'Contains, commas and "quotes"',
    'Test notes'
]);

$writer->writeRecord([
    2,
    "Multi-line\nProduct",
    "Description with\nnewlines",
    "Another note"
]);

$writer->close();

echo "Testing FastCSVWriter with special characters\n";
echo "Special characters file written\n";
echo "File content preview:\n";
$content = file_get_contents('special_chars.csv');
$lines = explode("\n", $content);
foreach ($lines as $i => $line) {
    if ($line !== '') {  // Skip empty lines
        echo "Line " . ($i + 1) . ": " . $line . "\n";
    }
}

// Check for BOM
$bom = substr($content, 0, 3);
echo "BOM present: " . ($bom === "\xEF\xBB\xBF" ? "Yes" : "No") . "\n\n";

// Test with semicolon delimiter
$config->setDelimiter(';');
$config->setPath('semicolon.csv');
$config->setWriteBOM(false);  // Disable BOM for semicolon test
$writer = new FastCSVWriter($config, ['id', 'name', 'value']);
$writer->writeRecord([1, 'John; Doe', 100]);
$writer->writeRecord([2, "Jane's Product", 200]);
$writer->writeRecord([3, '  Not Trimmed  ', '  Spaces  ']);
$writer->close();

echo "Testing with semicolon delimiter\n";
echo "Semicolon file written\n";
echo "Semicolon file content:\n";
$content = file_get_contents('semicolon.csv');
$lines = explode("\n", $content);
foreach ($lines as $line) {
    if ($line !== '') {  // Skip empty lines
        echo $line . "\n";
    }
}

// Test UTF-16 encoding
$config->setEncoding(CSV_ENCODING_UTF16LE);
$config->setPath('utf16.csv');
$writer = new FastCSVWriter($config, ['id', 'name']);
$writer->writeRecord([1, 'UTF-16 Test']);
$writer->close();

echo "\nTesting UTF-16 encoding\n";
echo "UTF-16 file written\n";
echo "Special characters and formats test completed!\n";
?>
--EXPECT--
Testing FastCSVWriter with special characters
Special characters file written
File content preview:
Line 1: ﻿id,name,description,notes
Line 2: 1,"Product ""Special""","Contains, commas and ""quotes""","Test notes"
Line 3: 2,"Multi-line
Line 4: Product","Description with
Line 5: newlines","Another note"
BOM present: Yes

Testing with semicolon delimiter
Semicolon file written
Semicolon file content:
id;name;value
1;"John; Doe";100
2;"Jane's Product";200
3;"  Not Trimmed  ";"  Spaces  "

Testing UTF-16 encoding
UTF-16 file written
Special characters and formats test completed! 