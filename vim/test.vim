" run all test cases, then report

" clear error
let v:errors = []
"call assert_report(0)
call assert_true(0)
call assert_true(0)
call assert_true(0)
echohl WarningMsg
for err in v:errors
  echom err
endfor
echohl None

" run case one by one, stop immediately if test failed
let v:errors = []
if(assert_true(0)) | throw v:errors[-1] | endif
if(assert_true(0)) | throw v:errors[-1] | endif
if(assert_true(0)) | throw v:errors[-1] | endif
