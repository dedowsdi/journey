#!/bin/bash

# generate completion for parameters starts with -
# source this file before you call ./parameter_complete -

_generate_candidates()   #  By convention, the function name
{                        #+ starts with an underscore.
  local cur
  # Pointer to current completion word.
  # By convention, it's named "cur" but this isn't strictly necessary.

  COMPREPLY=()   # Array variable storing the possible completions.
  cur=${COMP_WORDS[COMP_CWORD]}

  case "$cur" in
    -*)
    COMPREPLY=( $( compgen -W '-a -d -f -l -t -h --aoption --debug 
                               --file --log --test --help --' -- "$cur" ) );;
#   Generate the completion matches and load them into $COMPREPLY array.
#   xx) May add more cases here.
#   yy)
#   zz)
  esac

  return 0
}

complete -F _generate_candidates ./parameter_complete.sh
#        ^^ ^^^^^^^^^^^^  Invokes the function to generate completion
