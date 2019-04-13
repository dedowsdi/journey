" vim: tabstop=4 shiftwidth=4 softtabstop=4 smarttab shiftround noexpandtab noet list listchars=tab\:+.,space\:-

function! SpecialTab() abort
    if (col('.') == 1) || (matchstr(getline('.'), '\%'.(col('.') - 1).'c.') =~ '\t')
        return "\<Tab>"
    else
        return repeat("\<Space>", (&tabstop - (virtcol('.') % &tabstop) + 1)) . "a\<tab>\<backspace>\<backspace>"
    endif
endfunction
inoremap <expr> <Tab> SpecialTab()

