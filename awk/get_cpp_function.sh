if [[ $# -lt 2 ]]; then
  echo 'require file and line number'
  exit 0
fi

ctags  -f - --excmd=number --sort=no  --fields=e  --kinds-c++=f --language-force=c++  $1 | awk -vline=$2 -f <(cat <<"EOF"
{
  l0=substr($3,1,length($3)-2)+0
  l1=substr($4,5)+0
  if (line >= l0 && line <= l1)
    print l0, l1
}
EOF
)
