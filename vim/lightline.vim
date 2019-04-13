" vim:set showtabline=2 :

function! ListedBufferNames()
  return join(map(filter(getbufinfo(), 'v:val.listed'),
        \ 'fnamemodify(bufname(v:val.bufnr), ":t")'), ' | ')
endfunction

let g:lightline = {
      \ 'colorscheme': 'solarized',
      \ 'active': {
      \   'left': [ [ 'mode', 'paste' ],
      \             [  'readonly', 'filename', 'modified'] ]
      \ },
      \ 'component_function': {
      \   'lbn':'ListedBufferNames'
      \ },
      \ 'component':
      \ {
      \ },
      \ }

let g:lightline.tabline = {
      \ 'left': [ [ 'lbn' ] ],
      \ 'right': [ [ 'close' ] ] }

command! LightlineReload call LightlineReload()

function! LightlineReload()
  call lightline#init()
  call lightline#colorscheme()
  call lightline#update()
endfunction
