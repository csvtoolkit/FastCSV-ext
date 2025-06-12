--TEST--
FastCSVReader class test
--SKIPIF--
<?php
if (!extension_loaded('fastcsv')) die('skip');
?>
--FILE--
<?php
/**
 * @var FastCSVReader $reader
 */
$reader = new FastCSVReader(__DIR__ . '/data/test.csv');

/**
 * @return array
 */
$headers = $reader->getHeaders();
var_dump($headers);

/**
 * @return array|false
 */
$record = $reader->nextRecord();
var_dump($record);

$reader->close();
?>
--EXPECTF--
array(3) {
  [0]=>
  string(2) "id"
  [1]=>
  string(4) "name"
  [2]=>
  string(3) "age"
}
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(8) "John Doe"
  [2]=>
  string(2) "30"
} 