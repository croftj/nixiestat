let SessionLoad = 1
if &cp | set nocp | endif
let s:so_save = &so | let s:siso_save = &siso | set so=0 siso=0
let v:this_session=expand("<sfile>:p")
silent only
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
set shortmess=aoO
badd +17 ~/p4/nixiestat/webui/CMakeLists.txt
badd +0 ~/p4/nixiestat/webui/DBManager.cpp
badd +1 ~/p4/nixiestat/webui/DBManager.h
badd +13 ~/p4/nixiestat/webui/JsonTab.h
badd +9 ~/p4/nixiestat/webui/LocationTab.cpp
badd +19 ~/p4/nixiestat/webui/LocationTab.h
badd +31 ~/p4/nixiestat/webui/LogTopicHandler.cpp
badd +1 ~/p4/nixiestat/webui/LogTopicHandler.h
badd +34 ~/p4/nixiestat/webui/main.cpp
badd +0 ~/p4/nixiestat/webui/MQTT.cpp
badd +0 ~/p4/nixiestat/webui/MQTT.h
badd +0 ~/p4/nixiestat/webui/MQTTHandler.h
badd +55 ~/p4/nixiestat/webui/NixieStatApplication.cpp
badd +32 ~/p4/nixiestat/webui/NixieStatApplication.h
badd +1 ~/p4/nixiestat/webui/PingHandler.cpp
badd +0 ~/p4/nixiestat/webui/PingHandler.h
badd +1 ~/p4/nixiestat/webui/PingTopicHandler.cpp
badd +1 ~/p4/nixiestat/webui/PingTopicHandler.h
badd +0 ~/p4/nixiestat/webui/SubPing.cpp
badd +6 ~/p4/nixiestat/webui/SubPing.h
badd +0 ~/p4/nixiestat/webui/SubTemperatures.cpp
badd +0 ~/p4/nixiestat/webui/SubTemperatures.h
badd +40 ~/p4/nixiestat/webui/ThreadsafeQueue.h
badd +11 ~/p4/nixiestat/webui/WtApplication.cpp
badd +15 ~/p4/nixiestat/webui/WtApplication.h
badd +34 ~/p4/nixiestat/webui/TempTopicHandler.h
badd +24 ~/p4/nixiestat/webui/TempTopicHandler.cpp
argglobal
silent! argdel *
$argadd ~/p4/nixiestat/webui/CMakeLists.txt
$argadd ~/p4/nixiestat/webui/DBManager.cpp
$argadd ~/p4/nixiestat/webui/DBManager.h
$argadd ~/p4/nixiestat/webui/JsonTab.h
$argadd ~/p4/nixiestat/webui/LocationTab.cpp
$argadd ~/p4/nixiestat/webui/LocationTab.h
$argadd ~/p4/nixiestat/webui/LogTopicHandler.cpp
$argadd ~/p4/nixiestat/webui/LogTopicHandler.h
$argadd ~/p4/nixiestat/webui/main.cpp
$argadd ~/p4/nixiestat/webui/MQTT.cpp
$argadd ~/p4/nixiestat/webui/MQTT.h
$argadd ~/p4/nixiestat/webui/MQTTHandler.h
$argadd ~/p4/nixiestat/webui/NixieStatApplication.cpp
$argadd ~/p4/nixiestat/webui/NixieStatApplication.h
$argadd ~/p4/nixiestat/webui/PingHandler.cpp
$argadd ~/p4/nixiestat/webui/PingHandler.h
$argadd ~/p4/nixiestat/webui/PingTopicHandler.cpp
$argadd ~/p4/nixiestat/webui/PingTopicHandler.h
$argadd ~/p4/nixiestat/webui/SubPing.cpp
$argadd ~/p4/nixiestat/webui/SubPing.h
$argadd ~/p4/nixiestat/webui/SubTemperatures.cpp
$argadd ~/p4/nixiestat/webui/SubTemperatures.h
$argadd ~/p4/nixiestat/webui/ThreadsafeQueue.h
$argadd ~/p4/nixiestat/webui/WtApplication.cpp
$argadd ~/p4/nixiestat/webui/WtApplication.h
set lines=25 columns=127
edit ~/p4/nixiestat/webui/WtApplication.cpp
set splitbelow splitright
set nosplitbelow
set nosplitright
wincmd t
set winminheight=1 winheight=1 winminwidth=1 winwidth=1
argglobal
if bufexists('~/p4/nixiestat/webui/WtApplication.cpp') | buffer ~/p4/nixiestat/webui/WtApplication.cpp | else | edit ~/p4/nixiestat/webui/WtApplication.cpp | endif
let s:l = 11 - ((10 * winheight(0) + 11) / 22)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
11
normal! 0
if exists('s:wipebuf')
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20 shortmess=filnxtToO
set winminheight=1 winminwidth=1
let s:sx = expand("<sfile>:p:r")."x.vim"
if file_readable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &so = s:so_save | let &siso = s:siso_save
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
