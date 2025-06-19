--TEST--
FastCSVWriter - Complete writing test with config
--SKIPIF--
<?php
if (!extension_loaded('fastcsv')) die('skip');
?>
--FILE--
<?php
echo "Testing FastCSVWriter with configuration\n";

$outputFile = __DIR__ . '/data/test_output.csv';

// Clean up any existing file
if (file_exists($outputFile)) {
    unlink($outputFile);
}

// Create configuration
$config = new FastCSVConfig();
$config->setPath($outputFile)
       ->setDelimiter(',')
       ->setEnclosure('"')
       ->setHasHeader(true);

// Create writer with headers
$headers = ['id', 'name', 'email', 'age'];
$writer = new FastCSVWriter($config, $headers);
echo "Writer created successfully\n";

// Write some records using writeRecord
$writer->writeRecord(['1', 'John Doe', 'john@example.com', '30']);
$writer->writeRecord(['2', 'Jane Smith', 'jane@example.com', '25']);
echo "Records written using writeRecord\n";

// Write records using writeRecordMap
$writer->writeRecordMap([
    'id' => '3',
    'name' => 'Bob Johnson',
    'email' => 'bob@example.com',
    'age' => '35'
]);

$writer->writeRecordMap([
    'id' => '4',
    'name' => 'Alice Brown',
    'email' => 'alice@example.com',
    'age' => '28'
]);
echo "Records written using writeRecordMap\n";

// Test with different data types
$writer->writeRecord([5, 'Charlie Davis', null, 40]);
echo "Record with mixed types written\n";

$writer->close();
echo "Writer closed successfully\n";

// Verify the output file was created and has content
if (file_exists($outputFile)) {
    echo "Output file created successfully\n";
    $content = file_get_contents($outputFile);
    $lines = explode("\n", trim($content));
    echo "File has " . count($lines) . " lines\n";
    
    // Show first few lines
    for ($i = 0; $i < min(3, count($lines)); $i++) {
        echo "Line " . ($i + 1) . ": " . $lines[$i] . "\n";
    }
} else {
    echo "ERROR: Output file was not created\n";
}

// Clean up
if (file_exists($outputFile)) {
    unlink($outputFile);
}
?>
--EXPECT--
Testing FastCSVWriter with configuration
Writer created successfully
Records written using writeRecord
Records written using writeRecordMap
Record with mixed types written
Writer closed successfully
Output file created successfully
File has 6 lines
Line 1: id,name,email,age
Line 2: 1,John Doe,john@example.com,30
Line 3: 2,Jane Smith,jane@example.com,25 