--TEST--
FastCSVReader - Complete reading test with config
--SKIPIF--
<?php
if (!extension_loaded('fastcsv')) die('skip');
?>
--FILE--
<?php
echo "Testing FastCSVReader with configuration\n";

// Create configuration
$config = new FastCSVConfig();
$config->setPath(__DIR__ . '/data/test.csv')
       ->setHasHeader(true);

// Create reader
$reader = new FastCSVReader($config);
echo "Reader created successfully\n";

// Test getting headers
$headers = $reader->getHeaders();
echo "Headers: ";
if ($headers) {
    echo implode(', ', $headers) . "\n";
} else {
    echo "No headers\n";
}

// Test reading records
$recordCount = 0;
while (($record = $reader->nextRecord()) !== null) {
    $recordCount++;
    echo "Record $recordCount: ";
    if ($record) {
        echo implode(', ', $record) . "\n";
    } else {
        echo "null\n";
    }
}

echo "Total records read: $recordCount\n";

// Test rewind
$reader->rewind();
echo "Rewound successfully\n";

// Read first record again
$record = $reader->nextRecord();
if ($record) {
    echo "First record after rewind: " . implode(', ', $record) . "\n";
}

// Test position and seeking
echo "Current position: " . $reader->getPosition() . "\n";

// Test hasNext
echo "Has next record: " . ($reader->hasNext() ? 'yes' : 'no') . "\n";

$reader->close();
echo "Reader closed successfully\n";
?>
--EXPECT--
Testing FastCSVReader with configuration
Reader created successfully
Headers: id, name, age
Record 1: 1, John Doe, 30
Record 2: 2, Jane Doe, 28
Record 3: 3, Bob Smith, 45
Total records read: 3
Rewound successfully
First record after rewind: 1, John Doe, 30
Current position: 2
Has next record: yes
Reader closed successfully 