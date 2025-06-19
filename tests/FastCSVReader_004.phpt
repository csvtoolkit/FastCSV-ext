--TEST--
FastCSVReader - Special characters and formats
--SKIPIF--
<?php
if (!extension_loaded('fastcsv')) die('skip');
?>
--FILE--
<?php
echo "Testing FastCSVReader with special characters\n";

// Test with special characters file
$config = new FastCSVConfig();
$config->setPath(__DIR__ . '/data/special_chars.csv')
       ->setHasHeader(true);

$reader = new FastCSVReader($config);

// Read headers
$headers = $reader->getHeaders();
echo "Headers: " . implode(', ', $headers) . "\n";

// Read all records
$recordCount = 0;
while (($record = $reader->nextRecord()) !== null) {
    $recordCount++;
    echo "Record $recordCount:\n";
    foreach ($record as $i => $field) {
        echo "  " . $headers[$i] . ": '" . $field . "'\n";
    }
    if ($recordCount >= 2) break; // Only show first 2 records
}

$reader->close();

echo "\nTesting with different delimiter and enclosure\n";

// Test with semicolon delimiter and single quote enclosure
$config2 = new FastCSVConfig();
$config2->setPath(__DIR__ . '/data/semicolon.csv')
        ->setDelimiter(';')
        ->setEnclosure("'")
        ->setHasHeader(true);

$reader2 = new FastCSVReader($config2);

$headers2 = $reader2->getHeaders();
echo "Headers: " . implode(', ', $headers2) . "\n";

$record = $reader2->nextRecord();
if ($record) {
    echo "First record: " . implode(', ', $record) . "\n";
}

$reader2->close();

echo "Special characters test completed!\n";
?>
--EXPECT--
Testing FastCSVReader with special characters
Headers: id, name, description, price
Record 1:
  id: '1'
  name: 'Product "A"'
  description: 'Contains, commas and "quotes"'
  price: '29.99'
Record 2:
  id: '2'
  name: 'Product
B'
  description: 'Multi-line
description'
  price: '15.50'

Testing with different delimiter and enclosure
Headers: id, name, value
First record: 1, John; Doe, 100
Special characters test completed! 