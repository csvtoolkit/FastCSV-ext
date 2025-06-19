--TEST--
FastCSVReader - Error handling and edge cases
--SKIPIF--
<?php
if (!extension_loaded('fastcsv')) die('skip');
?>
--FILE--
<?php
echo "Testing FastCSVReader error handling\n";

// Test with invalid file path
$config = new FastCSVConfig();
$config->setPath('/nonexistent/file.csv');

try {
    $reader = new FastCSVReader($config);
    echo "ERROR: Should have thrown exception for nonexistent file\n";
} catch (Exception $e) {
    echo "PASS: Caught exception for nonexistent file: " . $e->getMessage() . "\n";
}

// Test with valid file
$config->setPath(__DIR__ . '/data/test.csv');
$reader = new FastCSVReader($config);

// Test invalid seek position
try {
    $reader->seek(-1);
    echo "ERROR: Should have thrown exception for negative seek position\n";
} catch (Exception $e) {
    echo "PASS: Caught exception for negative seek: " . $e->getMessage() . "\n";
}

// Test operations on closed reader
$reader->close();

try {
    $reader->getHeaders();
    echo "ERROR: Should have thrown exception for closed reader headers\n";
} catch (Exception $e) {
    echo "PASS: Caught exception for closed reader headers: " . $e->getMessage() . "\n";
}

try {
    $reader->nextRecord();
    echo "ERROR: Should have thrown exception for closed reader nextRecord\n";
} catch (Exception $e) {
    echo "PASS: Caught exception for closed reader nextRecord: " . $e->getMessage() . "\n";
}

try {
    $reader->rewind();
    echo "ERROR: Should have thrown exception for closed reader rewind\n";
} catch (Exception $e) {
    echo "PASS: Caught exception for closed reader rewind: " . $e->getMessage() . "\n";
}

echo "All error handling tests passed!\n";
?>
--EXPECT--
Testing FastCSVReader error handling
PASS: Caught exception for nonexistent file: Failed to open CSV file
PASS: Caught exception for negative seek: Position must be non-negative
PASS: Caught exception for closed reader headers: Reader not initialized
PASS: Caught exception for closed reader nextRecord: Reader not initialized
PASS: Caught exception for closed reader rewind: Reader not initialized
All error handling tests passed! 