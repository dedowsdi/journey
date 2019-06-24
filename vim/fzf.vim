" custom layout
let g:fzf_layout = {'up':'~30%'}

let g:fzf_action = {
      \ 'ctrl-t': 'tab split',
      \ 'ctrl-x': 'split',
      \ 'ctrl-v': 'vertical rightbelow split',
      \ 'ctrl-a': 'argadd',
      \ }

function! s:fzf_open_file(cmd)
  " default sink is :e file
  " source can be string(shell command) or list
  let opts = {
  \ 'source':  a:cmd,
  \ 'options': ['--ansi',
  \             '--multi', '--bind', 'alt-a:select-all,alt-d:deselect-all',
  \             '--color', 'hl:68,hl+:110']
  \}
  call fzf#run(fzf#wrap(opts))
endfunction

" custom source
function! s:fzf()
  " fzf use awk style field separator by default
  let source = [
        \ "1 hello john snow",
        \ "2 hello dany targ",
        \ "3 hello tyrion lanister",
        \ ]

  " use --with-nth to select display items
  " use --nth to limit search fields. (default to 1). note that --nth works on
  " top of --with-nth, not the original items.
  let opts = {
  \ 'source':  source,
  \ 'options': ['--ansi', '--color', 'hl:68,hl+:110',
  \ '--with-nth=2..',
  \ '--nth=2',
  \ '--prompt=last name>'
  \ ],
  \ 'sink': function('s:custom_sink')
  \}

  call fzf#run(fzf#wrap(opts))
endfunction

" custom sink
function! s:custom_sink(line)
  let items = split(a:line, '\t')
  echo items
endfunction

command! -nargs=+ -complete=file FF :call s:fzf_open_file(<q-args>)
command! -nargs=0 -complete=file FS :call s:fzf()
