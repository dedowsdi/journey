# my vim key binding

## f1-f12
n   f1                  unused(ubuntu system help
n   f2      cpp         rename
n   f3                  highlight
n   f4                  hover
n   f5                  run
n   f6                  update tags
n   f7                  compile
n   c-f7                lint
n   c-f7    cpp         YcmDiags
n   f8      cpp         switch between function dec and def
n   f9                  break point toggle for c,c++
n   c-f9                single break point for c,c++
n   f10                 cycle virtual edit option( only all and ''
n   f11                 ubuntu fullscreen
n   f12                 YcmCompleter GoToDefinition
n   c-f12               YcmCompleter GotoDeclaration
n   s-f12               find references(need language server)

## the most used overrided shortcuts.
n    Y       y$
n    g6      alternate file
n    a-o     head, source switch
n    c-p     file search, excludes git, hg, symbolic links. (fzf)
n    c-h     history search. (fzf)
n    c-b     buffer search. (fzf)
n    c-j     jump to tag. (fzf)
n    c-k     focus. (fzf)
n    __      edit init.vim or .vimrc
n   <A-h>    goto left wnd
n   <A-j>    goto down wnd
n   <A-k>    goto up wnd
n   <A-l>    goto right wnd
t   <A-h>    leave terminal, goto left wnd
t   <A-j>    leave terminal, goto down wnd
t   <A-k>    leave terminal, goto up wnd
t   <A-l>    leave terminal, goto right wnd
c   %%       current dir
c   %t       current file
x   *        visual search
x   #        visual search

## ctrl-l layout related stuff
:redraw is the same as default ctrl-l
nt  <c-l>l      focus, close everything except normal buffer
n   <c-l>r      redraw
n   <c-l>m      toggle global termial. (there is m in terminal)
n   <c-l>j      close job terms.
n   <c-l>s      smart split
n   <c-l>g      goyo

## ctrl-f fzf related stuff
n  <c-f>p       find everything
n  <c-f>b       find buffer
n  <c-f>i  cpp  find includes
n  <c-f>d  cpp  find derived
n  <c-f>tt      find tags
n  <c-f>td      find class, struct, function prototype for current word

## <leader>d debug related stuff.
n <leader>dd            cpp     start debug
n <leader>db            cpp     break only here
n <leader>dq            cpp     make quickfix from current buffer
n <leader>ds            cpp     open debug script(gdb or lldb)
n <leader>dp            cpp     open project script(project.json)

## <leader>a external application.
n <leader>aa           cpp     apitrace
n <leader>al           cpp     last apitrace
n <leader>ar           cpp     renderdoc
n <leader>an           cpp     nvidia gfx debugger
n <leader>ag                   Google current word
v <leader>ag                   Google current visual selection
n <leader>am           cpp     :Cmake

## misc
n dd   quickfix     delete current item under cursor from current quickfix

##ctrl-e code edit
ctrl-e can be replace by 1<ctrl-d>
i <c-e>e            escape abbreviation, return to normal mode
n <c-e>o     cpp    auto include, works for osg, qt kind stuff
n <c-e>i     cpp    manual include
n <c-e>g     cpp    header guard
n <c-e>de    cpp    define function(cdef)
v <c-e>de    cpp    define function in range(cdef)
n <c-e>df    cpp    define fine(cdef)
n <c-e>du    cpp    update function signature(cdef)
v <c-e>f            clang format
n <c-e>]            shift item
n <c-e>[            shift item
n <c-e>j     vim    join with |
n <c-e>s     vim    source block

#ctrl-s search, substitute, select related
x   <c-s>s              call :s with visual selection
n   <c-s>w              substitute current word
n   <c-s>W              substitute current WORD
v   <c-s>lv             convert visual selection to vim literal, place it at @"
v   <c-s>lg             convert visual selection to grep literal, place it at @"
v   <c-s>lf             convert visual selection to fzf_ag literal, place it at @"
n   <c-s>j              <c-v>j
n   <c-s>k              <c-v>j
v   <c-s>j              vertical select down
v   <c-s>k              vertical select up

## unimpair
n   yo[a-z] toggle optoin
n   ][a-zA-Z]  next or Last
n   [[a-zA-Z]  previous or first

## \c.+  nerdcommenter
## \y.+  ycm
n  \yd         * :YcmShowDetailedDiagnostic<CR>
n  \ygd        * :YcmCompleter GoToDeclaration<CR>
n  \ygi        * :YcmCompleter GoToInclude<CR>
n  \yf         * :YcmCompleter FixIt<CR>

## easy align
default ga can be replaced with ascii
n  ga  <Plug>(EasyAlign)
v  ga  <Plug>(EasyAlign)

## ultisnips
c-s  i  expand
c-n  i  next slot 
c-p  i  previous slot

## language server related
\sh   hover

## unused handy keys:
c-y  n      default can be replaced with 1<c-u>
c-g  n      default can be replaced with :f

