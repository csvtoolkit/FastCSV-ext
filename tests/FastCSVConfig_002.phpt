--TEST--
FastCSVConfig - Error handling and validation
--SKIPIF--
<?php
if (!extension_loaded('fastcsv')) die('skip');
?>
--FILE--
<?php
echo "Testing FastCSVConfig error handling\n";

$config = new FastCSVConfig();

// Test invalid delimiter (multiple characters)
try {
    $config->setDelimiter(';;');
    echo "ERROR: Should have thrown exception for multiple character delimiter\n";
} catch (Exception $e) {
    echo "PASS: Caught exception for invalid delimiter: " . $e->getMessage() . "\n";
}

// Test invalid enclosure (multiple characters)
try {
    $config->setEnclosure('""');
    echo "ERROR: Should have thrown exception for multiple character enclosure\n";
} catch (Exception $e) {
    echo "PASS: Caught exception for invalid enclosure: " . $e->getMessage() . "\n";
}

// Test invalid escape (multiple characters)
try {
    $config->setEscape('\\\\');
    echo "ERROR: Should have thrown exception for multiple character escape\n";
} catch (Exception $e) {
    echo "PASS: Caught exception for invalid escape: " . $e->getMessage() . "\n";
}

// Test negative offset
try {
    $config->setOffset(-1);
    echo "ERROR: Should have thrown exception for negative offset\n";
} catch (Exception $e) {
    echo "PASS: Caught exception for negative offset: " . $e->getMessage() . "\n";
}

echo "All error handling tests passed!\n";
?>
--EXPECT--
Testing FastCSVConfig error handling
PASS: Caught exception for invalid delimiter: Delimiter must be exactly one character
PASS: Caught exception for invalid enclosure: Enclosure must be exactly one character
PASS: Caught exception for invalid escape: Escape character must be exactly one character
PASS: Caught exception for negative offset: Offset must be non-negative
All error handling tests passed! 