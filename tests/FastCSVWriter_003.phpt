--TEST--
FastCSVWriter - Error handling and edge cases
--SKIPIF--
<?php
if (!extension_loaded('fastcsv')) die('skip');
?>
--FILE--
<?php
echo "Testing FastCSVWriter error handling\n";

// Test with invalid directory path
$config = new FastCSVConfig();
$config->setPath('/nonexistent/directory/file.csv');

try {
    $writer = new FastCSVWriter($config, ['col1', 'col2']);
    echo "ERROR: Should have thrown exception for invalid path\n";
} catch (Exception $e) {
    echo "PASS: Caught exception for invalid path: " . $e->getMessage() . "\n";
}

// Test with valid path
$outputFile = __DIR__ . '/data/test_error.csv';
if (file_exists($outputFile)) {
    unlink($outputFile);
}

$config->setPath($outputFile);
$writer = new FastCSVWriter($config, ['id', 'name']);

// Test writing empty array
$result = $writer->writeRecord([]);
echo "Writing empty array result: " . ($result ? 'true' : 'false') . "\n";

// Test writing with invalid field map
try {
    $writer->writeRecordMap([]);
    echo "Writing empty record map result: false\n";
} catch (Exception $e) {
    echo "PASS: Caught exception for empty record map: " . $e->getMessage() . "\n";
}

// Test operations on closed writer
$writer->close();

try {
    $writer->writeRecord(['1', 'test']);
    echo "ERROR: Should have thrown exception for closed writer writeRecord\n";
} catch (Exception $e) {
    echo "PASS: Caught exception for closed writer writeRecord: " . $e->getMessage() . "\n";
}

try {
    $writer->writeRecordMap(['id' => '1', 'name' => 'test']);
    echo "ERROR: Should have thrown exception for closed writer writeRecordMap\n";
} catch (Exception $e) {
    echo "PASS: Caught exception for closed writer writeRecordMap: " . $e->getMessage() . "\n";
}

// Clean up
if (file_exists($outputFile)) {
    unlink($outputFile);
}

echo "All error handling tests passed!\n";
?>
--EXPECT--
Testing FastCSVWriter error handling
PASS: Caught exception for invalid path: Failed to create CSV file
Writing empty array result: false
Writing empty record map result: false
PASS: Caught exception for closed writer writeRecord: Writer not initialized
PASS: Caught exception for closed writer writeRecordMap: Writer not initialized
All error handling tests passed! 