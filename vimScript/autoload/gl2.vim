"write your enums line by line,    make sure there is blank line around the enum
"blocks. 
function! gl2#toAndFromString()
  "to string
  let str = [ 
              \'const char* glToString(GLenum e) {',
              \'  switch (e) {',
              \]

  silent normal! "tyap
  let enums = split(@t)
  for enum in enums
    let str += [
          \ '    case '.enum.':',
          \ '      return "'.enum.'";'
          \ ]
  endfor
  let str += [
        \'    default: ',
        \'      return "";',
        \'  }',
        \'}',
        \''
        \]


  "from string
  let str += ['GLenum glFromString(const char* s) {']
  for enum in enums
    let str += [
        \ '  if (strcmp(s, "'.enum.'") == 0)',
        \ '    return '.enum.';',
        \ ]
  endfor

  let str += [
      \ '  return 0;',
      \ '}',
      \ ''
      \ ]

  call append(line('.')-1, str)
  silent normal! dap
  
endfunction
