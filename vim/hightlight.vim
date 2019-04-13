autocmd VimEnter,WinEnter * :match Error /\v%>80v%<3l.*|%>80v.*%$|%>80v.*(\n.*%$)@=/
