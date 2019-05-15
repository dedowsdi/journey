fun! s:mkbuf()
    "new
    "call setline(1, repeat('a', 1048576))  " 1M
    "silent w! edit_buffer_test
    silent edit ~/journey/gl4/gl43/include/glad/glad.h
endfun

fun! Benchmark_Buf() abort
    call s:mkbuf()
    let l:buf = bufnr('')
    close

    for i in range(0, g:bench_n)
        exe printf('buffer %d', l:buf)
        close
    endfor
endfun

fun! Benchmark_Edit() abort
    call s:mkbuf()
    let l:buf = bufname('')
    close

    for i in range(0, g:bench_n)
        exe printf('edit %s', l:buf)
        close
    endfor
endfun
