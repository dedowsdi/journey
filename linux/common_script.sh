
chr () {
  local val
  [ "$1" -lt 256 ] || return 1
  printf -v val %o "$1"; printf "%s" "\\$val"
  # That one requires bash 3.1 or above.
}

# "If the leading character is a single-quote or double-quote, the value shall be the numeric value in the underlying codeset of the character following the
# single-quote or double-quote."
ord() {
  # POSIX
  LC_CTYPE=C printf %d "'$1"
}

# hex() - converts ASCII character to a hexadecimal value
# unhex() - converts a hexadecimal value to an ASCII character

hex() {
   LC_CTYPE=C printf %x "'$1"
}

unhex() {
   printf "%s" "\\x$1"
}

# examples:
chr "$(ord A)"    # -> A
ord "$(chr 65)"   # -> 65
