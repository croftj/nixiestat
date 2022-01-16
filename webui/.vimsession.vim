let SessionLoad = 1
if &cp | set nocp | endif
let s:so_save = &so | let s:siso_save = &siso | set so=0 siso=0
let v:this_session=expand("<sfile>:p")
silent only
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
set shortmess=aoO
badd +20 ~/p4/GCoder/GCode.cpp
badd +1 ~/p4/GCoder/main.cpp
badd +79 ~/p4/GCoder/mainwindow.cpp
badd +1 ~/p4/GCoder/XSettings.cpp
badd +157 ~/p4/GCoder/GCode.h
badd +30 ~/p4/GCoder/mainwindow.h
badd +1 ~/p4/GCoder/outputdeviceselection.h
badd +1 ~/p4/GCoder/xlineedit.h
badd +1 ~/p4/GCoder/XSettings.h
argglobal
silent! argdel *
argadd ~/p4/GCoder/GCode.cpp
argadd ~/p4/GCoder/main.cpp
argadd ~/p4/GCoder/mainwindow.cpp
argadd ~/p4/GCoder/XSettings.cpp
argadd ~/p4/GCoder/GCode.h
argadd ~/p4/GCoder/mainwindow.h
argadd ~/p4/GCoder/outputdeviceselection.h
argadd ~/p4/GCoder/xlineedit.h
argadd ~/p4/GCoder/XSettings.h
set lines=40 columns=194
edit ~/p4/GCoder/mainwindow.h
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
set nosplitbelow
set nosplitright
wincmd t
set winheight=1 winwidth=1
exe 'vert 1resize ' . ((&columns * 92 + 97) / 194)
exe 'vert 2resize ' . ((&columns * 101 + 97) / 194)
argglobal
edit ~/p4/GCoder/mainwindow.h
let s:l = 36 - ((0 * winheight(0) + 18) / 37)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
36
normal! 04|
wincmd w
argglobal
edit ~/p4/GCoder/mainwindow.cpp
let s:l = 89 - ((17 * winheight(0) + 18) / 37)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
89
normal! 028|
wincmd w
2wincmd w
exe 'vert 1resize ' . ((&columns * 92 + 97) / 194)
exe 'vert 2resize ' . ((&columns * 101 + 97) / 194)
if exists('s:wipebuf')
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20 shortmess=filnxtToO
let s:sx = expand("<sfile>:p:r")."x.vim"
if file_readable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &so = s:so_save | let &siso = s:siso_save
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
