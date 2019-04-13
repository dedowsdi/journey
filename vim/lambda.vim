let F0 = {-> 0 } " return directly
echo F0()

let s:var = 0
let F1 = {-> s:var + 2 } " return evaluated expression
echo F1()

let F2 = {-> execute('echo "balabala"')} " execute get executed as an ex command ?
call F2()

