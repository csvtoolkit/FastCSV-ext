--TEST--
FastCSVConfig - Basic configuration test
--SKIPIF--
<?php
if (!extension_loaded('fastcsv')) die('skip');
?>
--FILE--
<?php
echo "Testing FastCSVConfig basic functionality\n";

// Test constructor
$config = new FastCSVConfig();
echo "Config created successfully\n";

// Test getters with default values
echo "Default delimiter: '" . $config->getDelimiter() . "'\n";
echo "Default enclosure: '" . $config->getEnclosure() . "'\n";
echo "Default escape: '" . $config->getEscape() . "'\n";
echo "Default path: '" . $config->getPath() . "'\n";
echo "Default offset: " . $config->getOffset() . "\n";
echo "Default hasHeader: " . ($config->hasHeader() ? 'true' : 'false') . "\n";

// Test setters and fluent interface
$config->setDelimiter(';')
       ->setEnclosure("'")
       ->setEscape('\\')
       ->setPath('/tmp/test.csv')
       ->setOffset(10)
       ->setHasHeader(false);

echo "After setting values:\n";
echo "Delimiter: '" . $config->getDelimiter() . "'\n";
echo "Enclosure: '" . $config->getEnclosure() . "'\n";
echo "Escape: '" . $config->getEscape() . "'\n";
echo "Path: '" . $config->getPath() . "'\n";
echo "Offset: " . $config->getOffset() . "\n";
echo "HasHeader: " . ($config->hasHeader() ? 'true' : 'false') . "\n";

echo "All tests passed!\n";
?>
--EXPECT--
Testing FastCSVConfig basic functionality
Config created successfully
Default delimiter: ','
Default enclosure: '"'
Default escape: '"'
Default path: ''
Default offset: 0
Default hasHeader: true
After setting values:
Delimiter: ';'
Enclosure: '''
Escape: '\'
Path: '/tmp/test.csv'
Offset: 10
HasHeader: false
All tests passed! 