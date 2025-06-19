--TEST--
FastCSV - Performance test with larger dataset
--SKIPIF--
<?php
if (!extension_loaded('fastcsv')) die('skip');
?>
--FILE--
<?php
echo "Testing FastCSV performance with larger dataset\n";

$outputFile = __DIR__ . '/data/performance_test.csv';

// Clean up any existing file
if (file_exists($outputFile)) {
    unlink($outputFile);
}

// Step 1: Create a large CSV file
echo "Step 1: Creating large CSV file with 1000 records\n";

$config = new FastCSVConfig();
$config->setPath($outputFile)
       ->setDelimiter(',')
       ->setEnclosure('"')
       ->setHasHeader(true);

$headers = ['id', 'first_name', 'last_name', 'email', 'age', 'city', 'score'];
$writer = new FastCSVWriter($config, $headers);

$startTime = microtime(true);

// Generate 1000 test records
for ($i = 1; $i <= 1000; $i++) {
    $record = [
        $i,
        'FirstName' . $i,
        'LastName' . $i,
        'user' . $i . '@example.com',
        rand(18, 80),
        'City' . ($i % 50),
        rand(1, 100) / 10
    ];
    $writer->writeRecord($record);
    
    // Show progress every 200 records
    if ($i % 200 == 0) {
        echo "Written $i records\n";
    }
}

$writer->close();
$writeTime = microtime(true) - $startTime;

echo "Write completed in " . number_format($writeTime, 3) . " seconds\n";

// Step 2: Read the large CSV file
echo "\nStep 2: Reading large CSV file\n";

$readConfig = new FastCSVConfig();
$readConfig->setPath($outputFile)->setHasHeader(true);

$reader = new FastCSVReader($readConfig);

$startTime = microtime(true);

$headers = $reader->getHeaders();
echo "Headers: " . implode(', ', $headers) . "\n";

$recordCount = 0;
$totalAge = 0;
$totalScore = 0;

while (($record = $reader->nextRecord()) !== null) {
    $recordCount++;
    
    // Process data (calculate averages)
    $totalAge += (int)$record[4];  // age
    $totalScore += (float)$record[6];  // score
    
    // Show progress every 200 records
    if ($recordCount % 200 == 0) {
        echo "Read $recordCount records\n";
    }
}

$reader->close();
$readTime = microtime(true) - $startTime;

echo "Read completed in " . number_format($readTime, 3) . " seconds\n";
echo "Total records read: $recordCount\n";

if ($recordCount > 0) {
    $avgAge = $totalAge / $recordCount;
    $avgScore = $totalScore / $recordCount;
    echo "Average age: " . number_format($avgAge, 1) . "\n";
    echo "Average score: " . number_format($avgScore, 1) . "\n";
}

// Clean up
if (file_exists($outputFile)) {
    $fileSize = filesize($outputFile);
    echo "File size: " . number_format($fileSize / 1024, 1) . " KB\n";
    unlink($outputFile);
}

$totalTime = $writeTime + $readTime;
echo "Total processing time: " . number_format($totalTime, 3) . " seconds\n";

echo "\nPerformance test completed successfully!\n";
?>
--EXPECTF--
Testing FastCSV performance with larger dataset
Step 1: Creating large CSV file with 1000 records
Written 200 records
Written 400 records
Written 600 records
Written 800 records
Written 1000 records
Write completed in %f seconds

Step 2: Reading large CSV file
Headers: id, first_name, last_name, email, age, city, score
Read 200 records
Read 400 records
Read 600 records
Read 800 records
Read 1000 records
Read completed in %f seconds
Total records read: 1000
Average age: %f
Average score: %f
File size: %f KB
Total processing time: %f seconds

Performance test completed successfully! 