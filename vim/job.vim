
let ch = job_getchannel(job)
let jobinfo = job_info(job)
let chinfo = ch_info(ch)
"echom string(jobinfo)
"echom string(chinfo)
"call ch_evalraw(job, 'a\nb\nc\nd\n')
"call ch_evalexpr(job, 'a\nb\nc\nd\n')

function! s:async_cat()
  let buf = term_start('cat')
  let job = term_getjob(buf)
  call ch_sendraw(job, "a\nb\nc\nd\n\<c-d>")
endfunction

function! s:out_cb(ch, msg)
  echom a:msg
endfunction

function! s:err_cb(ch, msg)
  echom a:msg
endfunction

function! s:exit_cb(job, exit_status)
  echom a:job
  echom a:exit_status
endfunction

function! s:sync_cat()
  let job = job_start(['bash', '-c', " cat > result"],
        \ { 'out_cb':function('s:out_cb'), 'exit_cb':function("s:exit_cb"), 'err_cb':function('s:err_cb')})
  call ch_sendraw(job, "bala\nbala\n")
  "let job = job_start(' bash -c "echo abc" ',
        "\ {
        "\ 'out_cb':function("s:out_cb"), 'exit_cb':function("s:exit_cb")})
  let g:jb = job
  let g:jbinfo = job_info(job)
  "let g:chinfo = ch_info(g:jbinfo.channel)
endfunction

"call s:sync_cat()

let job = job_start(['bash', '-c', 'cat > result'])
call ch_sendraw(job, "bala\nbala\nbala\n")
