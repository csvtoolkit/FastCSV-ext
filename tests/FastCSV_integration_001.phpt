--TEST--
FastCSV - Integration test (read and write)
--SKIPIF--
<?php
if (!extension_loaded('fastcsv')) die('skip');
?>
--FILE--
<?php
echo "Testing FastCSV integration (read + write)\n";

$inputFile = __DIR__ . '/data/test.csv';
$outputFile = __DIR__ . '/data/integration_output.csv';

// Clean up any existing output file
if (file_exists($outputFile)) {
    unlink($outputFile);
}

// Step 1: Read from input file
echo "Step 1: Reading from input file\n";

$readConfig = new FastCSVConfig();
$readConfig->setPath($inputFile)->setHasHeader(true);

$reader = new FastCSVReader($readConfig);

// Get headers from input
$headers = $reader->getHeaders();
echo "Input headers: " . implode(', ', $headers) . "\n";

// Read all records
$records = [];
while (($record = $reader->nextRecord()) !== null) {
    $records[] = $record;
}

echo "Read " . count($records) . " records from input\n";
$reader->close();

// Step 2: Write to output file with modifications
echo "\nStep 2: Writing to output file with modifications\n";

$writeConfig = new FastCSVConfig();
$writeConfig->setPath($outputFile)
           ->setDelimiter(',')
           ->setEnclosure('"')
           ->setHasHeader(true);

// Add a new column
$newHeaders = array_merge($headers, ['status']);
$writer = new FastCSVWriter($writeConfig, $newHeaders);

// Write modified records
foreach ($records as $i => $record) {
    $modifiedRecord = array_merge($record, ['processed']);
    $writer->writeRecord($modifiedRecord);
}

// Add a new record
$writer->writeRecord(['999', 'New User', '99', 'new']);

$writer->close();
echo "Written " . (count($records) + 1) . " records to output\n";

// Step 3: Verify the output by reading it back
echo "\nStep 3: Verifying output file\n";

$verifyConfig = new FastCSVConfig();
$verifyConfig->setPath($outputFile)->setHasHeader(true);

$verifyReader = new FastCSVReader($verifyConfig);

$outputHeaders = $verifyReader->getHeaders();
echo "Output headers: " . implode(', ', $outputHeaders) . "\n";

$outputRecords = [];
while (($record = $verifyReader->nextRecord()) !== null) {
    $outputRecords[] = $record;
}

echo "Verified " . count($outputRecords) . " records in output\n";

// Show first and last record
if (count($outputRecords) > 0) {
    echo "First record: " . implode(', ', $outputRecords[0]) . "\n";
    echo "Last record: " . implode(', ', $outputRecords[count($outputRecords) - 1]) . "\n";
}

$verifyReader->close();

// Clean up
if (file_exists($outputFile)) {
    unlink($outputFile);
}

echo "\nIntegration test completed successfully!\n";
?>
--EXPECT--
Testing FastCSV integration (read + write)
Step 1: Reading from input file
Input headers: id, name, age
Read 3 records from input

Step 2: Writing to output file with modifications
Written 4 records to output

Step 3: Verifying output file
Output headers: id, name, age, status
Verified 4 records in output
First record: 1, John Doe, 30, processed
Last record: 999, New User, 99, new

Integration test completed successfully! 