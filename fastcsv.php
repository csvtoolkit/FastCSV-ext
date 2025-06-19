<?php

if (!extension_loaded('fastcsv')) {
    die('The fastcsv extension is not loaded');
}

// Encoding constants (only define if not already defined by the extension)
if (!defined('CSV_ENCODING_UTF8')) define('CSV_ENCODING_UTF8', 0);
if (!defined('CSV_ENCODING_UTF16LE')) define('CSV_ENCODING_UTF16LE', 1);
if (!defined('CSV_ENCODING_UTF16BE')) define('CSV_ENCODING_UTF16BE', 2);
if (!defined('CSV_ENCODING_UTF32LE')) define('CSV_ENCODING_UTF32LE', 3);
if (!defined('CSV_ENCODING_UTF32BE')) define('CSV_ENCODING_UTF32BE', 4);
if (!defined('CSV_ENCODING_ASCII')) define('CSV_ENCODING_ASCII', 5);
if (!defined('CSV_ENCODING_LATIN1')) define('CSV_ENCODING_LATIN1', 6); 