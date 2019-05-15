" because, char2nr return code point, not the actural utf8 storage.
echo printf('%x', char2nr('中'))
