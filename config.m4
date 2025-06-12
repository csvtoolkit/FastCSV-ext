PHP_ARG_ENABLE(fastcsv, whether to enable FastCSV support,
[  --enable-fastcsv      Enable FastCSV support])

if test "$PHP_FASTCSV" != "no"; then
  PHP_NEW_EXTENSION(fastcsv, fastcsv.c lib/csv_config.c lib/csv_reader.c lib/csv_writer.c, $ext_shared)
fi 