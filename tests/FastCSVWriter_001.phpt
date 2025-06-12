--TEST--
FastCSVWriter class test
--SKIPIF--
<?php
if (!extension_loaded('fastcsv')) die('skip');
?>
--FILE--
<?php
/**
 * @var FastCSVWriter $writer
 */
$headers = ['id', 'name', 'age'];
$writer = new FastCSVWriter(__DIR__ . '/data/output.csv', $headers);

/**
 * @return bool
 */
$result = $writer->writeRecord(['1', 'John Doe', '30']);
var_dump($result);

$writer->close();

// Verify written content
$content = file_get_contents(__DIR__ . '/data/output.csv');
var_dump($content);

unlink(__DIR__ . '/data/output.csv');
?>
--EXPECTF--
bool(true)
string(%d) "id,name,age
1,John Doe,30
" 